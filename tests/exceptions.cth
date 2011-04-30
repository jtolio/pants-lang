try {
  print "hello"
  try {
    print "there"
    throw "ex1"
    print "notreached"
  } { |e|
    print "inner-handler" e
    throw (+ e "-2")
  }
  print "notreached"
} { |e|
  print "outer-handler" e
}

print (try {
  3
} { |e|
  4
})

print (try {
  3
  throw 4
} { |e|
  e
})

print (try {
  3
  throw "error"
} { |e|
  5
})

try {
  3(4)
} { |e|
  print "caught exception! it was:" e
}

try {
  (+ 4 null)
} { |e|
  print "tried adding 4 to null:" e
}

try {
  (+ { "function" } 4.0 )
} { |e|
  print "tried adding closure to float:" e
}

throw "leave program"
print "notreached"
