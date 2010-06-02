require_relative "../ext/deque_core"

class Deque
  def initialize(ary=nil)
   super()
   if ary
     concat(ary.to_a)
   end
  end
  alias :add :add_last
  alias :<< :add_last
  alias :pop :remove_last
  alias :shift :remove_first
  include Enumerable

end
