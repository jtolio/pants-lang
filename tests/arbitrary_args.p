func = {|x, *(y)|
  println x "*"
  y @each {|x| println x}
}

func 1 2 3 4

println.

func = {|*(left); *(right)|

  println "left:"
  left @each {|x| println x}

  println "right:"
  right @each {|x| println x}

  println.
}

func.
1 2 func. 3
"hey" func.
"hey" "there" @func "dude" "this" "is" "cool"
func "whoa" "nelly" 1 2 3

func = {|x, y, *(left);|
  println "x:" x
  println "y:" y
  left @each {|z| println z}
}

1 2 3 4 @func

println "================="

func = {|x, y, z|
  println x y z
}

func(*([1, 2, 3]))

list = [4, 5, 6, 7]
{|x, y, *(z)|
  println x y
  z @each {|x| println x}
}(1, *(list))
