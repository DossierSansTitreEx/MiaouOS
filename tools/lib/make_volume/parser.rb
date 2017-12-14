module MakeVolume
  class ParserError < StandardError; end

  class Parser
    def initialize(path)
      @path = path
    end

    def parse
      commands = []
      File.open(@path, "r") do |file|
        file.each do |line|
          l = line.strip.split(" ")
          next if l.empty?
          commands << l
        end
      end
      if commands[0][0] != "SIZE"
        raise ParserError, "SIZE missing"
      end
      if commands.count {|x| x[0] == "SIZE"} != 1
        raise ParserError, "Only one SIZE expected"
      end
      commands
    end
  end
end
