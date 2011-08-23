#!../tools/script_wrapper.sh ../src/pants

quicksort = function { |list|
  if (<= list.size() 1) { return list }
  left_side = []
  right_side = []
  list @each_with_index { |item, i|
    {(if (< item list[0]) {left_side} {right_side}).append item} @if (> i 0)
  }
  result = quicksort left_side
  result.append list[0]
  quicksort(right_side) @each {|item| result.append item}
  result
}

input = []; line = null
while { line := readln() } { input.append line }
quicksort(input) @each {|line| print line}
