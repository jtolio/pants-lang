func = {|x, *(y)|
  print x "*"
  y @each {|x| print x}
}

func 1 2 3 4

print.

func = {|*(left); *(right)|

  print "left:"
  left @each {|x| print x}

  print "right:"
  right @each {|x| print x}

  print.
}

func.
1 2 func. 3
"hey" func.
"hey" "there" @func "dude" "this" "is" "cool"
func "whoa" "nelly" 1 2 3

func = {|x, y, *(left);|
  print "x:" x
  print "y:" y
  left @each {|z| print z}
}

1 2 3 4 @func
