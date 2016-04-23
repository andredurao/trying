require "bundler/gem_tasks"
require "rake/testtask"
require 'rake/extensiontask'

Rake::TestTask.new(:test) do |t|
  t.libs << "test"
  t.libs << "lib"
  t.test_files = FileList['test/**/*_test.rb']
end

Rake::ExtensionTask.new 'my_malloc' do |ext|

  # This causes the shared object to be placed in lib/my_malloc/my_malloc.so
  #
  # It allows lib/trie.rb to load different versions if you ship a
  # precompiled extension that supports multiple ruby versions.

  ext.lib_dir = 'lib'
end

task :default => :test


Rake::ExtensionTask.new('trie')
