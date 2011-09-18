func1 = {|a,b,c|
  println a b c
}

func1(b:1,c:2,a:3)

func2 = {|a,b,:(c)|
  print a b
  print " "
  c @each {|i|
    print i
    print " "
  }
  println.
}

func2(3,4,5)
func2(b:1, a:2)

func3 = {|::(d)|
  println d.a d.c
}

func3(c:4, a:2)

func4 = {|a:3|
  println a
}

func4.
func4 1

safe_println = {|:(thunks)|
  thunks @each {|thunk|
    try {
      print thunk()
    } { |e|
      print "(error:" (e @+ ")")
    }
    print " "
  }
  println.
}

func5 = {|a,b,c:4,d:5,:(e),::(f)|
  println.
  println a b c d
  print "e:"
  e @each {|i|
    print " "
    print i
  }
  println.
  safe_println {f.e} {f.f} {f.g} {f.h}
}

func5(a:1,b:2,c:3,d:4,e:5,f:6,g:7,h:8)
func5(1,2,3,4,5,6,7,8)
func5(1,2)
func5(1,2,3)
func5(d:10,a:3,b:1,h:6,f:7)
