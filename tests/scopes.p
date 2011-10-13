# PANTS OPTIONS: --skip-prelude

x = (3)
println x
{ |x| println x; x := 4; println x }(2)
println x

println.

x = 1
y = { println x }
y.
println x
x := 2
y.
println x

println.

x = 1
y = { println x }
y.
println x
x = 2
y.
println x

println.

x = 1
y = ( z = x; { println z } )
y.
println x
x = 2
y.
println x

println.

z = {
  (
    cont 3
  )
  cont 4
}

println z()

z = {
  cont 4
  (
    cont 3
  )
}

println z()

println.

x = 10
(
  x = 11
)
println x

x = 10
(
  x := 11
)
println x

println.

var = 100
get = { println var }
set = { |val| var := val }
var = 101
println var
get.
set 102
println var
get.

println.

var2 = 200
get = { println var2 }
set = { |val| var2 := val }
var2 := 201
println var2
get.
set 202
println var2
get.

println.

recursion = {|x|
  if (< x 10) {
    recursion (+ x 1)
  }
  println x
}
recursion 1

println.

recursion = {|x, y|
  x := y
  println x
  if (< y 10) {
    recursion 1 (+ y 1)
  }
  println x
}
recursion 1 1

println.

x = 3
test = {
  println x
  x = 4
  println x
}
test.
test.

println.
