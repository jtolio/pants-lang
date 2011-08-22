#!./script_wrapper.sh ../src/pants

while {true} {
  try {
    print readln()
  } {|e|
    if (e ==. "end of file") { break. } { throw e }
  }
}
