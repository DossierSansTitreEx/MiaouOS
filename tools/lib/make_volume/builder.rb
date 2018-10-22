require 'byte_buffer'

module MakeVolume
  SIZE_SUFFIXES = {
    "B" => 0,
    "K" => 1,
    "M" => 2,
    "G" => 3,
    "T" => 4
  }

  CHUNK_SIZE = 4096
  MBR_RESERVED = CHUNK_SIZE * 10
  VBR_RESERVED = CHUNK_SIZE * 10
  SECTION_COUNT = CHUNK_SIZE * 8
  SECTION_SIZE = SECTION_COUNT * CHUNK_SIZE
  VOLUME_START = MBR_RESERVED
  VOLUME_HEADER = VOLUME_START + VBR_RESERVED
  MBR_TABLE = 446

  class Builder
    def initialize(commands, output)
      @commands = commands
      @output_path = output
    end

    def run
      size = @commands.shift[1]
      suffix = size[-1]
      size = size[0..-2].to_i * (1024 ** SIZE_SUFFIXES[suffix])
      # First, we need to round up to a section multiple
      # ignoring MBR space
      section_count = ((size - MBR_RESERVED).to_f / SECTION_SIZE.to_f).ceil
      size = section_count * SECTION_SIZE + MBR_RESERVED

      # We need to round this up for valid CHS values
      @size = ((size.to_f / CHUNK_SIZE.to_f).ceil * CHUNK_SIZE).to_i
      @buffer = ByteBuffer.new(@size)

      # Mark the section FAT themselves as used
      section_count.times {|i| mark_chunk_as_used(i * SECTION_COUNT + SECTION_COUNT - 1)}

      # Mark the VBR + the Header Chunk as used
      reserved_chunks = (VBR_RESERVED.to_f / CHUNK_SIZE.to_f).ceil + 1
      reserved_chunks.times {|i| mark_chunk_as_used(i)}

      @root = create_root
      @commands.each do |com|
        exec_command(com)
      end
      dump_volume
    end

    def exec_command(command)
      action = command.shift
      if action == "FILE"
        dst = command.shift
        src = command.shift
        File.open(src, "rb") do |file|
          data = file.read
          copy_file_to_path(data, dst)
        end
      elsif %w[MBR BOOTLOADER].include? action
        mbr = (action == "MBR")
        size = mbr ? MBR_RESERVED : VBR_RESERVED
        if mbr
          base = 0
        else
          base = VOLUME_START
        end
        filename = command.shift
        File.open(filename, "rb") do |file|
          data, _ = slice_data(file.read, size)
          @buffer.write(base, data)
        end
        fix_mbr if mbr
      end
    end

    def fix_mbr
      start_sector = VOLUME_START / 512
      end_sector = @size / 512 - 1
      mbr = [0x80].pack("C")
      # Dummy start CHS
      mbr += pack_chs(start_sector)
      mbr += [0xcf].pack("C")
      mbr += pack_chs(end_sector)
      mbr += [start_sector].pack("L<")
      mbr += [end_sector - start_sector + 1].pack("L<")
      @buffer.write(MBR_TABLE, mbr)
      nil
    end

    def pack_chs(lba)
      if lba >= 1023 * 255 * 63
        [0xfe, 0xff, 0xff].pack("C3")
      else
        cylinder = lba / (255 * 63)
        tmp = lba % (255 * 63)
        head = tmp / 63
        sector = tmp % 63 + 1
        b0 = head
        b1 = ((cylinder >> 8) << 6) | sector
        b2 = cylinder & 0xff
        [b0, b1, b2].pack("C3")
      end
    end

    def dump_volume
      File.open(@output_path, "wb") do |file|
        file.write(@buffer.data)
      end
    end

    def copy_file_to_path(data, path)
      dirname = File.dirname(path)
      basename = File.basename(path)
      dir_chunk = make_path(dirname)
      file_chunk = create_file(data)
      dir_entry = directory_entry(file_chunk, basename)
      append_file(dir_chunk, dir_entry)
    end

    def make_path(path)
      elements = path_elements(path)
      c = @root
      elements.each do |e|
        c2 = subdir_chunk(c, e)
        if c2 == 0
          c2 = create_file("")
          append_file(c, directory_entry(c2, e))
        end
        c = c2
      end
      c
    end

    def chunk_from_path(path)
      elements = path_elements(path)
      c = @root
      elements.each do |e|
        c = subdir_chunk(c, e)
        return 0 if c == 0
      end
      c
    end

    def path_elements(path)
      path.split('/').select{|e| !e.nil? && !e.empty?}
    end

    def directory_entry(chunk, name)
      size = [name.size].pack("S<")
      chunk = [chunk].pack("Q<")
      size + name + chunk
    end

    def subdir_chunk(chunk, name)
      data = read_file(chunk)
      loop do
        return 0 if (data.nil? || data.size == 0)
        size, data = slice_data(data, 2)
        size = size.unpack("S<")[0]
        str, data = slice_data(data, size)
        c, data = slice_data(data, 8)
        c = c.unpack("Q<")[0]
        return c if name == str
      end
    end

    def create_root
      c = create_file("")
      @buffer.write(VOLUME_HEADER, "MeowMeow")
      @buffer.write(VOLUME_HEADER + 0x18, [c].pack("Q<"))
      c
    end

    def read_file(chunk)
      addr = chunk_addr(chunk)
      size = @buffer.read(addr + 0x38, 8).unpack("Q<")[0]
      data = []
      12.times do |i|
        break if size == 0
        iaddr = @buffer.read(addr + 0x40 + 0x8 * i, 8).unpack('Q<')[0]
        head_size = [size, 4096].min
        head = @buffer.read(chunk_addr(iaddr), head_size)
        data << head
        size -= head_size
      end
      data*''
    end

    def append_file(chunk, data)
      head = read_file(chunk)
      write_file(chunk, head + data)
      nil
    end

    def create_file(data)
      c = find_free_chunk
      write_file(c, data)
      c
    end

    def write_file(chunk, data)
      size = data.size
      addr = chunk_addr(chunk)
      @buffer.write(addr + 0x38, [size].pack("Q<"))
      12.times do |i|
        break if data.nil?
        head, data = slice_data(data, CHUNK_SIZE)
        c2 = @buffer.read(addr + 0x40 + 0x8 * i, 8).unpack("Q<")[0]
        if c2 == 0
          c2 = file_raw_data(head)
        else
          @buffer.write(chunk_addr(c2), head)
        end
        @buffer.write(addr + 0x40 + 0x8 * i, [c2].pack("Q<"))
      end
    end

    def slice_data(data, size)
      first = data[0, size]
      second = data[size..-1]
      second = nil if second && second.empty?
      [first, second]
    end

    def file_raw_data(data)
      c = find_free_chunk
      addr = chunk_addr(c)
      @buffer.write(addr, data)
      c
    end

    def chunk_addr(chunk)
      VOLUME_START + chunk * CHUNK_SIZE
    end

    def find_free_chunk
      c = 0
      loop do
        break if chunk_free?(c)
        c += 1
      end
      mark_chunk_as_used(c)
      c
    end

    def chunk_free?(index)
      info = decompose_chunk(index)
      if info[1] == 0 && info[2] == 0
        false
      else
        byte = @buffer.read(VOLUME_START + info[0] * SECTION_SIZE + (SECTION_SIZE - CHUNK_SIZE) + info[1], 1)
        bit = byte.ord & (1 << info[2])
        bit == 0
      end
    end

    def mark_chunk_as_used(index)
      info = decompose_chunk(index)
      byte = @buffer.read(VOLUME_START + info[0] * SECTION_SIZE + (SECTION_SIZE - CHUNK_SIZE) + info[1], 1)
      byte = (byte.ord | (1 << info[2])).chr
      @buffer.write(VOLUME_START + info[0] * SECTION_SIZE + (SECTION_SIZE - CHUNK_SIZE) + info[1], byte)
    end

    def decompose_chunk(idx)
      section = idx / SECTION_COUNT
      idx = idx % SECTION_COUNT
      byte = idx / 8
      bit = idx % 8
      [section, byte, bit]
    end
  end
end
