i = 0
while {< i 10} {
  println i
  i := + i 1
  < i 5
}

println.

j = 0
while {true} {
  j := j +. 1
  if (j <. 2) { continue() }
  println j
  if (3 <. j) { break() }
}
