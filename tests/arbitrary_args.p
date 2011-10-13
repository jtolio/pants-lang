# PANTS OPTIONS: --skip-prelude

while = {|test, body|
  if test() {
    body()
    while test body
  }
}

each = {|:(iterables); func|
  i = 0
  while {< i iterables.size()} {
    j = 0
    while {< j iterables[i].size()} {
      func iterables[i][j]
      j := + j 1
    }
    i := + i 1
  }
}

func = {|x, :(y)|
  println x "*"
  y @each {|x| println x}
}

func 1 2 3 4

println.

func = {|:(left); :(right)|

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

func = {|:(left), x, y;|
  left @each {|z| println z}
  println "x:" x
  println "y:" y
}

1 2 3 4 @func

println "---------------"

func = {|x, y, z|
  println x y z
}

func(:([1, 2, 3]))

list = [4, 5, 6, 7]
{|x, y, :(z)|
  println x y
  z @each {|x| println x}
}(1, :(list))
