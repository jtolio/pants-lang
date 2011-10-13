# PANTS OPTIONS: --skip-prelude

function = {|func|
  {|:(largs); :(rargs), ::(kwargs)| func(:(largs); cont, :(rargs), ::(kwargs))}
}

myfunc = function { |return, x, y|
  println "x" x
  if (< x 10) {
    return y
  }
  println "y"
}

println "return" myfunc(1,2)
println.
println "return" myfunc(10,2)
