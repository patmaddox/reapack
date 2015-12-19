task :default => [:prepare, :build]

desc 'Prepare the source code for building using prepare.rb'
task :prepare do
  sources = FileList['src/*.{h,c}pp']
  ruby 'prepare.rb', *sources, :verbose => false
end

desc 'Build ReaPack (by default both 64-bit and 32-bit builds are generated)'
task :build, [:variants] do |_, args|
  vars = Array(args[:variants]) + args.extras
  vars.reject &:empty?

  sh 'tup', *vars, :verbose => false
end

desc 'Run the test suite for all architectures'
task :test do
  FileList['x{86,64}/bin/test{,.exe}'].each {|exe|
    sh exe
  }
end
