require "benchmark"
require_relative "../lib/deque"

o=Deque.new
#o=RList.new
#o=[]


TIMES=1000000


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
