#!./script_wrapper.sh ../src/pants

hello = { |target|
  println "Hello" (+ target "!")
}

hello "world"
