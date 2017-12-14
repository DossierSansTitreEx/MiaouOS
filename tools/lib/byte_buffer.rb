class ByteBuffer
  attr_reader :data

  def initialize(size)
    @data = "\x00" * size
  end

  def read(index, size)
    @data[index, size]
  end

  def write(index, data)
    @data[index, data.size] = data
  end
end
