println "hidden object test"

function = {|func|
  {|:(largs); :(rargs), ::(kwargs)|
    func(:(largs); :(rargs), ::(kwargs); return-cont:cont)
  }
}
return = {|var| .return-cont var }

while = {|test, body|
  if test() {
    if {{ body(); true }(;;loop-cont:cont)}() {
      while test body
    }
  }
}
break = { .loop-cont false }
continue = { .loop-cont true }
loop = {|body| while {true} body }

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

println function({ .return-cont 1 })()
println function({ { .return-cont 2 }() })()
println function({|:(l); :(r)| .return-cont 3 })()
println function({|:(l); :(r)| { .return-cont 4 }() })()
println function({|:(l); :(r)| if true { .return-cont 5 } })()
println function({|:(l); :(r)| if true { { .return-cont 6 }() } })()
println function({|:(l); :(r)| { .return-cont 7 } @if true })()
println function({|:(l); :(r)| { { .return-cont 8 }() } @if true })()
println function({|:(l); :(r)| .return-cont 9 })(false, 1, 2)
println function({|:(l); :(r)| { .return-cont 10 }() })(false, 1, 2)
println function({|:(l); :(r)| if true { .return-cont 11 } })(false, 1, 2)
println function({|:(l); :(r)| if true { { .return-cont 12 }() } })(false, 1, 2)
println function({|:(l); :(r)| { .return-cont 13 } @if true })(false, 1, 2)
println function({|:(l); :(r)| { { .return-cont 14 }() } @if true })(false, 1, 2)

println function({|:(l); :(r)|
  func = {|x| {.return-cont x} @if x}
  func false
  func 10
  func 12
})(7, 8, 9)

println function({|:(l); :(r)|
  func = {|x| {.return-cont x} @if x}
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

{
  println .x
}(;;x:3)

println .x # should fail
