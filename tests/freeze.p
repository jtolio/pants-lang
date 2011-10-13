function_dynamic_var = DynamicVar()
function = {|func|
  {|:(largs); :(rargs), ::(kwargs)|
    function_dynamic_var.call cont { func(:(largs); :(rargs), ::(kwargs)) }
  }
}
return = {|var| function_dynamic_var.get() var}

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
