# PANTS OPTIONS: --skip-prelude

println "dynamic variable test"

function-dynamic-var = DynamicVar()
function = {|func|
  {|:(largs); :(rargs), ::(kwargs)|
    function-dynamic-var.call cont { func(:(largs); :(rargs), ::(kwargs)) }
  }
}
return = {|var| function-dynamic-var.get() var }

while_dynamic_var = DynamicVar()
while = {|test, body|
  if test() {
    if { while_dynamic_var.call cont { body(); true } }() {
      while test body
    }
  }
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

println function({ return 1 })()
println function({ { return 2 }() })()
println function({|:(l); :(r)| return 3 })()
println function({|:(l); :(r)| { return 4 }() })()
println function({|:(l); :(r)| if true { return 5 } })()
println function({|:(l); :(r)| if true { { return 6 }() } })()
println function({|:(l); :(r)| { return 7 } @if true })()
println function({|:(l); :(r)| { { return 8 }() } @if true })()
println function({|:(l); :(r)| return 9 })(false, 1, 2)
println function({|:(l); :(r)| { return 10 }() })(false, 1, 2)
println function({|:(l); :(r)| if true { return 11 } })(false, 1, 2)
println function({|:(l); :(r)| if true { { return 12 }() } })(false, 1, 2)
println function({|:(l); :(r)| { return 13 } @if true })(false, 1, 2)
println function({|:(l); :(r)| { { return 14 }() } @if true })(false, 1, 2)

println function({|:(l); :(r)|
  func = {|x| {return x} @if x}
  func false
  func 10
  func 12
})(7, 8, 9)

println function({|:(l); :(r)|
  func = {|x| {return x} @if x}
  vals = [false, 101, 102]
  i = 0
  while {< i 3} {
    func vals[i]
    i := + i 1
  }
})(17, 18, 19)

println function({|:(l); :(r)|
  l r @each {|thing| { return thing } @if thing }
  false
})(false, 1, 2)

temp_dynamic_var = DynamicVar()
temp_dynamic_var.call 3 {
  println temp_dynamic_var.get()
}

println temp_dynamic_var.get() # should fail
