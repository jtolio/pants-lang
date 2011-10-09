Employee = constructor {|status:"new"; obj, first, last|
  obj.status = status
  obj.first = first
  obj.last = last
}

joe = Employee "Joe" "Schmoe"
println joe.status joe.first joe.last

max = "returning" @Employee "Max" "Schmax"
println max.status max.first max.last

# ==================

println ({1} @unless true)
println ({2} @unless false)
println (unless true {3})
println (unless false {4})

# ===================

try {
  assert true "message1"
  println "message2"
  assert false "message3"
  println "message4"
} {|e|
  println e
}

try {
  assert true
  println "message5"
  assert false
  println "message6"
} {|e|
  println e
}

# ====================

i = 0
while {< i 10} {
  print i
  print " "
  i := + i 1
}
println.

i = 0
while {< i 10} {
  i := + i 1
  continue @if (== (- i 1) 5)
  break @if (== (- i 1) 7)
  print (- i 1)
  print " "
}
println.

loop {
  break @if (== i 20)
  print i
  print " "
  i := + i 1
}
println.

# =====================

func = function {|a; return, b, c|
  if (== a b) {
    return c
  }
  + a b c
}

println (1 @func 1 2)
println (1 @func 2 3)

# ======================

println "throws" throws({joe.status})
println "throws" throws({joe.invalid_field})
println "safe" safe({joe.status})
println "safe" safe({joe.invalid_field})

# ======================

[1, 2, 3] [4, 5, 6, 7] [] [8] @each {|x|
  print x
  print " "
}
println.

[1, 2, 3] [4, 5, 6, 7] [] [8] @each-with-index {|x, i|
  print i
  print ":"
  print x
  print " "
}
println.

# ======================

println ([1] [2,3,4] @fold +)
println (fold +)
println ([] @fold +)
println ([1] @fold +)
println ([1, 2] @fold +)

# ======================

println and(1, 2, 3)
println and(1, 2, null, 3)
println and()
println or(1, 2, 3)
println or(false, null, 4)
println or()

# ======================

catch = {|thunk| try thunk {|e| println e}}

func = binary_function {|x, y| + x y 10}
println (== (func 1 2) 13)
println (== (3 func. 1) 14)
println (== (3 4 @func) 17)
catch {func 1 2 3}
catch {1 func. 1 2 3}
catch {1 func. 1 3}
catch {2 1 func. 1 3}
catch {2 func.}
catch {func 2}

# ======================

println (<= 1 2)
println (<= 1 1)
println (not (<= 1 0))
println (> 2 1)
println (not (> 1 1))
println (> 1 0)
println (not (>= 1 2))
println (>= 1 1)
println (>= 1 0)
println (!= 1 2)
println (not (!= 1 1))
println (!= 1 0)

# ======================

catch { assert-raises { throw "message1" } "message1" "message2" }
catch { assert-raises { + 1 1 } "message2" "message3" }
catch { assert-raises { throw "message4" } "message5" "message6" }
catch { assert-raises { throw "message7" } "message7" }
catch { assert-raises { + 1 1 } "message8" }
catch { assert-raises { throw "message9" } "message10" }
