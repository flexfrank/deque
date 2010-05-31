require_relative "../ext/deque_core"

class Deque
  alias :add :add_last
  alias :push :add_last
  alias :pop :remove_last
  alias :shift :remove_first
  alias :unshift :add_first
  include Enumerable

end
