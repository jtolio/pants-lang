constructor = {|func|
  class = {|:(largs); :(rargs), ::(kwargs)|
    obj = new_object.
    obj.class = class
    func(:(largs); obj, :(rargs), ::(kwargs))
    seal_object obj
    obj
  }
  class
}
# TODO: actually clear this out of the scope or something
new_object = null
seal_object = null

freeze = {|func|
  k = null; sub_k = null; sub_largs = null; sub_rargs = null; sub_kwargs = null;
  if {k := cont; false}() {
    sub_k func(:(sub_largs); :(sub_rargs), ::(sub_kwargs))
  }
  {|:(largs); :(rargs), ::(kwargs)|
    sub_k := cont
    sub_largs := largs
    sub_rargs := rargs
    sub_kwargs := kwargs
    k true
  }
}

try = {|body, handler|
  k = cont; h = freeze handler
  throw_dynamic_var.call {|e| k h(e)} body
}
throw = {|e| throw_dynamic_var.get() e}
# TODO: actually clear this out of the scope or something
throw_dynamic_var = null

unless = {|lblock:null; test, rblock:null|
  if (== lblock null) { if test { null } rblock
  } { if test { null } lblock }
}

while_dynamic_var = DynamicVar()
while = {|test, body|
  if test() {
    if { while_dynamic_var.call cont { body(); true } }() {
      while test body
    }
  }
}
break = { while_dynamic_var.get() false }
continue = { while_dynamic_var.get() true }
# TODO: actually clear this out of the scope or something
while_dynamic_var = null
loop = {|body| while {true} body }

function = {|func|
  {|:(largs); :(rargs), ::(kwargs)| func(:(largs); cont, :(rargs), ::(kwargs))}
}

assert = {|test, message:"assertion failed"|
  { throw message } @unless test
}
assert-raises = function {|return, thunk, expected, message:"assertion failed"|
  try thunk {|e| assert (== e expected) message; return null}
  throw message
}

each = {|:(iterables); func|
  i = 0
  while {< i iterables.size()} {
    j = 0
    while {< j iterables[i].size()} {
      func iterables[i][j]
      j := + j 1
    }
    i := + i 1
  }
}
each-with-index = {|:(iterables); func|
  i = 0; each(:(iterables); {|thing| func thing i; i := + i 1 })
}

fold = {|:(iterables); func|
  result = null
  one = false
  each(:(iterables); {|thing|
    if one {
      result := func result thing
    } {
      result := thing
      one := true
    }
  })
  result
}

# TODO: 'and' and 'or' currently do not support short circuiting.
and = function {|:(l); return, :(r)|
  [true] l r @fold {|x, y| if x {y} {return x}}}
or = function {|:(l); return, :(r)|
  [false] l r @fold {|x, y| if x {return x} {y}}}
not = {|val| if val { false } { true }}

throws = function {|return, thunk| try thunk {|e| return true}; false }
safe = {|thunk| not throws(thunk)}

binary_function = {|func|
  {|:(largs); :(rargs)|
    # TODO: array concatenation plz
    args = []
    largs rargs @each {|x| args.append x}
    assert (== args.size() 2) "exactly two arguments expected"
    func args[0] args[1]
  }
}

<= = binary_function { |left, right| (or (< left right) (== left right)) }
> = binary_function { |left, right| (not (<= left right)) }
>= = binary_function { |left, right| (or (> left right) (== left right)) }
!= = binary_function { |left, right| (not (== left right)) }

Dictionary = constructor {|obj|

  new_tree_node = constructor {|o, k, v|
    o.key = k
    o.value = v
    o.left = null
    o.right = null
  }

  tree_root = null

  update = function {|return, n, k, v|
    if (== k n.key) {
      n.value = v
      return 0
    }
    if (< k n.key) {
      if (== n.left null) {
        n.left = new_tree_node k v
        return 0
      }
      update n.left k v
      return 0
    }
    if (== n.right null) {
      n.right = new_tree_node k v
      return 0
    }
    update n.right k v
  }

  index = function {|return, n, k|
    { return null } @if (== n null)
    { return n.value } @if (== k n.key)
    { return (index n.left k) } @if (< k n.key)
    return (index n.right k)
  }

  has_key? = function {|return, n, k|
    { return false } @if (== n null)
    { return true } @if (== k n.key)
    { return (has_key? n.left k) } @if (< k n.key)
    return (has_key? n.right k)
  }

  each = function {|return, n, f|
    { return 0 } @if (== n null)
    each n.left f
    f n.key n.value
    each n.right f
  }

  # whoa, deletes are complicated
  delete = function {|return, n, k|
    old_node = null
    if (< k n.key) {
      { return null } @if (== n.left null)
      { return (delete n.left k) } @unless (== n.left.key k)
      old_node := n.left
      n.left = null
    } {
      { return null } @if (== n.right null)
      { return (delete n.right k) } @unless (== n.right.key k)
      old_node := n.right
      n.right = null
    }
    each old_node.left {|k, v| update n k v }
    each old_node.right {|k, v| update n k v }
    return old_node.value
  }

  obj.~update = {|k, v|
    if (== tree_root null) {
      tree_root := new_tree_node k v
    } {
      update tree_root k v
    }
  }
  obj.~index = {|i| index tree_root i }
  obj.has_key? = {|i| has_key? tree_root i }
  obj.each = {|f| each tree_root f }
  obj.delete = function {|return, k|
    { return null } @if (== tree_root null)
    { return (delete tree_root k) } @unless (== tree_root.key k)
    old_root = tree_root
    tree_root := null
    each old_root.left {|k, v| obj[k] = v }
    each old_root.right {|k, v| obj[k] = v }
    old_root.value
  }

  # this is terrible. let's make lookups O(n)! :(
  obj.copy = {
    new_obj = Dictionary.
    each tree_root {|k, v| new_obj[k] = v }
    new_obj
  }

  # this is also terrible. fold anyone? or even better, cached size?
  obj.size = { sum = 0; each tree_root {|k, v| sum := + sum 1 }; sum }
}
