myfunc = { |x, y| function {
  println "x" x
  if (< x 10) {
    return y
  }
  println "y"
}}

println "return" myfunc(1,2)
println.
println "return" myfunc(10,2)
