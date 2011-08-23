#!../tools/script_wrapper.sh ../src/pants

input = []; last_line = null
loop {
  new_line = readln()
  break @if (not new_line)
  if (!= new_line last_line) { print new_line }
  last_line := new_line
}
