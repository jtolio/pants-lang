#!./script_wrapper.sh ../src/pants

hello = { |target|
  print "Hello" (+ target "!")
}

hello "world"
