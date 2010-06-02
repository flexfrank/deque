require "benchmark"
require_relative "../lib/deque"
begin
require_relative '../ext/CDeque.so'
module Containers end
class Containers::CDeque
  alias push push_back
  alias pop pop_back
  alias shift pop_front
  alias unshift push_front
  def [](a)
  end
end
end
o=Deque.new
#o=RList.new
#o=[]
o=Containers::CDeque.new


TIMES=8000000


Benchmark.bm do|b|
  b.report("push1") do
    TIMES.times do|i|
      o.push(i)
    end
  end
  b.report("shift1") do
    o.size.times do
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

  srand(32057203)
  TIMES.times{|i|o.push i}
  b.report("shiftpop") do
    o.size.times do
      if rand(2)%2==0
        o.shift
      else
        o.pop
      end
    end
  end

  TIMES.times{|i|o.push i}
  o.shift
  b.report("at") do
    TIMES.times do|i|
      o[i]
    end
  end
  b.report("each") do
    x=nil
    o.each do|i|
      x=i
    end
  end 
  b.report("to_a") do
    o.to_a[-1]=-1
  end
  o.clear

  b.report("unshift1") do
    TIMES.times do|i|
      o.unshift(i)
    end
  end
  b.report("pop1") do
    o.size.times do
      o.pop
    end
  end

end
