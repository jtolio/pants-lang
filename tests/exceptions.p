try {
  println "hello"
  try {
    println "there"
    throw "ex1"
    println "notreached"
  } { |e|
    println "inner-handler" e
    throw (+ e "-2")
  }
  println "notreached"
} { |e|
  println "outer-handler" e
}

println (try {
  3
} { |e|
  4
})

println (try {
  3
  throw 4
} { |e|
  e
})

println (try {
  3
  throw "error"
} { |e|
  5
})

try {
  3(4)
} { |e|
  println "caught exception! it was:" e
}

try {
  (+ 4 null)
} { |e|
  println "tried adding 4 to null:" e
}

try {
  (+ { "function" } 4.0 )
} { |e|
  println "tried adding closure to float:" e
}

throw "leave program"
println "notreached"
