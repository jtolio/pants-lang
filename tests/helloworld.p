# PANTS OPTIONS: --skip-prelude

hello = { |target|
  println "Hello" (+ target "!")
}

hello "world"
