amb_dv = DynamicVar()
amb = {|:(vals);func| return = cont
  vals @each {|val|
    return amb_dv.call(cont, {func val})
  }
  if safe(amb_dv.get) { amb_dv.get() null }
}
require = {|bool| return = cont
  { return bool } .if bool
  amb_dv.get() null
}

########################################

even = {|num| if (type(num) ==. Integer) { == (% num 2) 0 } { false } }

times = {|count; func|
  i = 0
  while {< i count} {
    func.
    i := + i 1
  }
}

^ = binary_function { |left, right|
  accumulator = 1
  right @times {
    accumulator := * accumulator left
  }
  accumulator
}

1 2 3 4 5 6 .amb {|a|
1 2 3 4 5 6 .amb {|b|
1 2 3 4 5 6 .amb {|c|
  require even(a)
  require (((a ^. 2) +. (b ^. 2)) ==. (c ^. 2))
  println a b c
}}}
