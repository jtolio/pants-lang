function = {|func|
  {|:(largs); :(rargs), ::(kwargs)|
    func(:(largs); :(rargs), ::(kwargs); return-cont:cont)
  }
}
return = {|var| .return-cont var}

outside_func = function { |x|
  outside_return = return
  inside_func = function { |y|
    outside_return y
  }
  inside_func 3
  2
}

println outside_func(1)

println.

outside_func = function { |x|
  outside_return = freeze(return)
  inside_func = function { |y|
    outside_return y
  }
  inside_func 3
  2
}

println outside_func(1)
