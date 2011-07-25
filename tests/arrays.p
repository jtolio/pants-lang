output_list = { |x|
  i = 0
  while { < i x.size() } {
    print i ":" x[i]
    i := + i 1
  }

  print.
}

test_list = { |x|
  output_list x

  i = - 1
  while { >= i (- x.size()) } {
    print i ":" x[i]
    i := - i 1
  }

  print.

  try {
    print x[- x.size()]
    print x[- (- x.size()) 1]
  } { |e|
    print "out of range:" e
  }

  print.

  try {
    print x[- x.size() 1]
    print x[x.size()]
  } { |e|
    print "out of range:" e
  }

  print.
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

print x.pop()
print.
output_list(x)
print x.shift()
print.
output_list(x)
