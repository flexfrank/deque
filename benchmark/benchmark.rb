require "benchmark"
require_relative "../lib/deque"
=begin
require 'algorithms'
module Containers
class Containers::Deque
  alias push push_back
  alias pop pop_back
  alias shift pop_front
  alias unshift push_front
  def [](a)
  end
end
=end
o=Deque.new
#o=RList.new
#o=[]
#o=Containers::Deque.new


TIMES=10000000


Benchmark.bm do|b|
  b.report("push1") do
    TIMES.times do|i|
      o.push(i)
    end
  end
  b.report("shift1") do
    while o.size>0
      o.shift
    end
  end

  10000.times{|i|o.push(i)}
  b.report("pushshift") do
    TIMES.times do|i|
      o.push(i)
      o.shift
    end
  end
  o.clear



  TIMES.times do|i|
    o.push i
  end
  o.shift
  b.report("at") do
    TIMES.times do|i|
      o[i]
    end
  end
  while o.size>0
    o.pop
  end

  b.report("unshift1") do
    TIMES.times do|i|
      o.unshift(i)
    end
  end
  b.report("pop1") do
    while o.size>0
      o.pop
    end
  end

end
