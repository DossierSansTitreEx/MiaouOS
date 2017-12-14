require 'make_volume/parser'
require 'make_volume/builder'

module MakeVolume
  module CLI
    def self.run(args)
      volume_file = args.shift
      output_path = args.shift

      parser = Parser.new(volume_file)
      commands = parser.parse
      builder = Builder.new(commands, output_path)
      builder.run
    end
  end
end
