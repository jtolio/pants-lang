while = { |test, body|
  if test() {
    if { {body(); true}(;;loop-cont:cont) }() {
      while test body
    }
  }
}
break = { .loop-cont false }
continue = { .loop-cont true }

each = {|iterable; func|
  i = 0; while {< i iterable.size()} { func iterable[i]; i := + i 1 }
}

function = { |func| func(;;return-cont:cont) }
return = { |var| .return-cont var }


# TODO: 'and' and 'or' currently do not support short circuiting.
and = { |left, right| if left { right } { left } }
or = { |left, right| if left { left } { right } }
not = { |val| if val { false } { true } }

<= = { |left, right| (or (< left right) (== left right)) }
> = { |left, right| (not (<= left right)) }
>= = { |left, right| (or (> left right) (== left right)) }

construct = { |func|
  obj = new_object.
  rv = func obj
  seal_object obj
}
# TODO: actually clear this out of the scope or something
new_object = null
seal_object = null


freeze = { |macro|
  k = null; sub_k = null; sub_arg = null
  if { k := cont; false }() { sub_k macro(sub_arg) }
  { |arg| sub_k := cont; sub_arg := arg; k true }
}


try = { |body, handler|
  exception-cont = cont
  exception-handler = freeze handler
  { body() }(;;throw:{|e| exception-cont exception-handler(e)})
}
throw = { |e| .throw e }


Dictionary = {

  new_tree_node = {|k, v| construct {|o|
    o.key = k
    o.value = v
    o.left = null
    o.right = null
  }}

  tree_root = null

  update = {|n, k, v| function {
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
  }}

  index = {|n, k| function {
    if (== n null) { return null }
    if (== k n.key) { return n.value }
    if (< k n.key) { return (index n.left k) }
    return (index n.right k)
  }}

  has_key? = {|n, k| function {
    if (== n null) { return false }
    if (== k n.key) { return true }
    if (< k n.key) { return (has_key? n.left k) }
    return (has_key? n.right k)
  }}

  each = {|n, f| function {
    if (== n null) { return 0 }
    each n.left f
    f n.key n.value
    each n.right f
  }}

  # whoa, deletes are complicated
  delete = {|n, k| return = cont
    old_node = null
    if (< k n.key) {
      { return null } @if (n.left ==. null)
      { return (delete n.left k) } @if (not (== n.left.key k))
      old_node := n.left
      n.left = null
    } {
      { return null } @if (n.right ==. null)
      { return (delete n.right k) } @if (not (== n.right.key k))
      old_node := n.right
      n.right = null
    }
    each old_node.left { |k, v| update n k v }
    each old_node.right { |k, v| update n k v }
    return old_node.value
  }

  construct {|obj|
    obj.~update = {|k,v|
      if (tree_root ==. null) {
        tree_root := new_tree_node k v
      } {
        update tree_root k v
      }
    }
    obj.~index = {|i| index tree_root i }
    obj.has_key? = {|i| has_key? tree_root i }
    obj.each = {|f| each tree_root f }
    obj.delete = {|k| return = cont
      { return null } @if (== tree_root null)
      { return (delete tree_root k) } @if (not (== tree_root.key k))
      old_root = tree_root
      tree_root := null
      each old_root.left { |k, v| obj[k] = v }
      each old_root.right { |k, v| obj[k] = v }
      old_root.value
    }

    # this is terrible. let's make lookups O(n)! :(
    obj.copy = {
      new_obj = {}
      each tree_root { |k, v| new_obj[k] = v }
      new_obj
    }

    # this is also terrible. fold anyone? or even better, cached size?
    obj.size = { sum = 0; each tree_root { |k, v| sum := + sum 1 }; sum }
  }
}