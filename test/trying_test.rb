require 'test_helper'
require 'trying'

describe Trying do

  it "have a version" do
    refute_nil ::Trying::VERSION
  end

  it "should have a Trie class on it" do
    Trying.class.must_equal Module
  end

end
