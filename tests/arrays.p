output_list = { |x|
  i = 0
  while { < i x.size() } {
    println i ":" x[i]
    i := + i 1
  }

  println.
}

test_list = { |x|
  output_list x

  i = - 1
  while { >= i (- x.size()) } {
    println i ":" x[i]
    i := - i 1
  }

  println.

  try {
    println x[- x.size()]
    println x[- (- x.size()) 1]
  } { |e|
    println "out of range:" e
  }

  println.

  try {
    println x[- x.size() 1]
    println x[x.size()]
  } { |e|
    println "out of range:" e
  }

  println.
}

test_list ["a", "b", "c"]
test_list ["d", "e"]

v = ["hello", "mr", "sam"]
output_list v
v[2] = "fox"
output_list v
v[2] = "poopypants"
output_list v

x = []
x.append(3)
x.append(4)
x.unshift(5)
output_list(x)

println x.pop()
println.
output_list(x)
println x.shift()
println.
output_list(x)
