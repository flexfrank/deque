require 'helper'
require "deque"

class TestDeque < MiniTest::Unit::TestCase
  def setup 
    @d=Deque.new
  end

  def test_class_exist
    assert(Deque.new.class,Deque)
  end
  

  def test_deque_last_last
    @d.add_last(0)
    @d.add_last(1)
    @d.add_last(2)
    assert_equal(3,@d.size)
    assert_equal(2,@d.remove_last)
    assert_equal(1,@d.remove_last)
    assert_equal(0,@d.remove_last)
    assert_equal(nil,@d.remove_last)
  end

  def test_deque_last_first
    @d.add_last(0)
    @d.add_last(1)
    @d.add_last(2)
    assert_equal(0,@d.remove_first)
    assert_equal(1,@d.remove_first)
    assert_equal(2,@d.remove_first)
    assert_equal(nil,@d.remove_first)
  end


  def test_deque_first_first
    @d.add_first(0)
    @d.add_first(1)
    @d.add_first(2)
    assert_equal(2,@d.remove_first)
    assert_equal(1,@d.remove_first)
    assert_equal(0,@d.remove_first)
    assert_equal(nil,@d.remove_first)
  end
  def test_deque_first_last
    @d.add_first(0)
    @d.add_first(1)
    @d.add_first(2)
    assert_equal(0,@d.remove_last)
    assert_equal(1,@d.remove_last)
    assert_equal(2,@d.remove_last)
    assert_equal(nil,@d.remove_last)
  end


  def test_deque_last_last_big
    (0..10000).each do|i|
      @d.add_last(i)
    end
    (0..10000).to_a.reverse.each do|i|
      assert_equal(i,@d.remove_last,@d.debug)
    end
  end
  def test_deque_last_first_big
    (0..10000).each do|i|
      @d.add_last(i)
    end
    (0..10000).each do|i|
      assert_equal(i,@d.remove_first,@d.debug)
    end
  end

  def test_deque_first_last_big
    (0..10000).each do|i|
      @d.add_first(i)
    end
    (0..10000).each do|i|
      assert_equal(i,@d.remove_last,@d.debug)
    end
  end

  def test_deque_first_first_big
    (0..10000).each do|i|
      @d.add_first(i)
    end
    (0..10000).to_a.reverse.each do|i|
      assert_equal(i,@d.remove_first,@d.debug)
    end
  end
  
  def test_gc
    
    @d.add_last("a"*100)
    GC.start
    assert_equal("a"*100,@d.remove_last)
    @d.add_first("a"*100)
    GC.start
    assert_equal("a"*100,@d.remove_first)
  end


  def test_at
    @d.add_first(1)
    @d.add_first(0)
    @d.add_last(2)
    @d.add_last(3)
    assert_equal(0,@d.at(0))
    assert_equal(1,@d.at(1))
    assert_equal(2,@d.at(2))
    assert_equal(3,@d.at(3))


    assert_equal(0,@d.at(-4))
    assert_equal(1,@d.at(-3))
    assert_equal(2,@d.at(-2))
    assert_equal(3,@d.at(-1))
  end

  def test_get    
    @d.add_first(1)
    @d.add_first(0)
    @d.add_last(2)
    @d.add_last(3)
    assert_equal(0,@d[0])
    assert_equal(1,@d[1])
    assert_equal(2,@d[2])
    assert_equal(3,@d[3])


    assert_equal(0,@d[-4])
    assert_equal(1,@d[-3])
    assert_equal(2,@d[-2])
    assert_equal(3,@d[-1])
  end

  def test_overrun
    @d.add_first(0)
    assert_nil @d.at(1)
  end

  def test_store
    @d.add_first(0)
    @d.add_last(1)
    @d.store(0,"a")
    @d.store(1,"b")
    @d.store(4,[])
    assert_equal("a",@d.at(0))
    assert_equal("b",@d.at(1))
    assert_equal(nil,@d.at(2))
    assert_equal(nil,@d.at(3))
    assert_equal([],@d.at(4))
  end

  def test_each
    @d.add_first(-1)
    @d.add_first(-2)
    @d.add_first(-3)
    @d.add_last(0)
    @d.add_last(1)
    @d.add_last(2)
    ary=[-3,-2,-1,0,1,2]
    i=0
    @d.each do|obj|
      assert_equal(ary[i],obj)
      i+=1
    end
    @d.each.with_index do|obj,j|
      assert_equal(ary[j],obj)
    end
    assert_equal(ary,@d.to_a)
  end

  def test_slice
    @d.add_first(-1)
    @d.add_first(-2)
    @d.add_first(-3)
    @d.add_last(0)
    @d.add_last(1)
    @d.add_last(2)
    assert_equal([-3,-2],@d[0,2].to_a)
    assert_equal([-2,-1],@d[1,2].to_a)
    assert_equal([-2,-1,0],@d[1,3].to_a)
    assert_equal([0,1],@d[3,2].to_a)
    assert_equal([0,1,2],@d[3,3].to_a)
    assert_equal([1,2],@d[4,2].to_a)
    assert_equal([1,2],@d[4,3].to_a)
    assert_equal([-2,-1,0,1,2],@d[1,8].to_a)
    assert_equal([2],@d[-1,1].to_a)
    assert_equal([1,2],@d[-2,2].to_a)
  end

  def test_range_slice
    @d.add_first(-1)
    @d.add_first(-2)
    @d.add_first(-3)
    @d.add_last(0)
    @d.add_last(1)
    @d.add_last(2)
    assert_equal([-3,-2],@d[0..1].to_a)
    assert_equal([-2,-1],@d[1..2].to_a)
    assert_equal([-2,-1,0],@d[1..3].to_a)
    assert_equal([0,1],@d[3..4].to_a)
    assert_equal([0,1,2],@d[3..5].to_a)
    assert_equal([1,2],@d[4..5].to_a)
    assert_equal([1,2],@d[4..6].to_a)
    assert_equal([-2,-1,0,1,2],@d[1..9].to_a)
  end

  def test_splice1
    @d.add_first(-1)
    @d.add_first(-2)
    @d.add_first(-3)
    @d.add_last(0)
    @d.add_last(1)
    @d.add_last(2)

    @d[1,4]=:a
    assert_equal(:a,@d[1])
    assert_equal([-3,:a,2],@d.to_a)
    assert_equal(3,@d.size)
  end
  def test_splice2
    @d.add_first(-1)
    @d.add_first(-2)
    @d.add_first(-3)
    @d.add_first(-4)
    @d.add_last(0)
    @d.add_last(1)
    @d.add_last(2)

    @d[1,2]=:a
    assert_equal(:a,@d[1])
    assert_equal([-4,:a,-1,0,1,2],@d.to_a)
    assert_equal(6,@d.size)
  end
  def test_splice4
    @d.add_first(-1)
    @d.add_first(-2)
    @d.add_last(0)
    @d.add_last(1)
    @d.add_last(2)
    @d.add_last(3)
    @d.add_last(4)

    @d[3,3]=:a
    assert_equal(:a,@d[3])
    assert_equal([-2,-1,0,:a,4],@d.to_a)
    assert_equal(5,@d.size)

    @d[30,1]=:b
    assert_equal(31,@d.size)
    assert_equal(:b,@d[30])
    assert_equal(nil,@d[29])
  end


  def test_clear
    100.times do
      @d.add_first(1)
      @d.add_last(0)
    end
    assert_equal(200,@d.size)
    @d.clear
    assert_equal(0,@d.size)
    assert_equal([],@d.to_a)
  end

  def test_hash
    d=Deque.new
    e=Deque.new
    o1=["a",1,[2],Object.new]
    o2=[3.9,10,"fff",:a]
    o1.each{|i| d.add_first(i);e.add_first(i)}
    o2.each{|i| d.add_last(i);e.add_last(i)}

    assert_equal(d.hash,e.hash)
    d.remove_last
    assert(d.hash!=e.hash)
  end

  def test_hash_rec
    @d.add_last(@d)
    @d.add_last(1)
    @d.add_first([@d])
    assert_block{@d.hash}
  end
end
