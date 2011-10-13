# PANTS OPTIONS: --skip-prelude

freeze = {|func|
  k = null; sub_k = null; sub_arg = null
  if {k := cont; false}() { sub_k func(sub_arg) }
  {|arg| sub_k := cont; sub_arg := arg; k true }
}

try = {|body, handler|
  k = cont; h = freeze handler
  throw_dynamic_var.call {|e| k h(e)} body
}
throw = {|e| throw_dynamic_var.get() e}

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

not = {|val| if val { false } { true }}
!= = { |left, right| (not (== left right)) }

call_wrapper = {|call|
  {|:(largs);:(rargs),::(kwargs)| call(:(largs); :(rargs),::(kwargs)) }
}

func1 = {|a,b,c|
  println a b c
}

func1(b:1,c:2,a:3)
call_wrapper(func1)(b:1,c:2,a:3)

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
call_wrapper(func2)(3,4,5)
func2(b:1, a:2)
call_wrapper(func2)(b:1, a:2)

func3 = {|::(d)|
  println d.a d.c
}

func3(c:4, a:2)
call_wrapper(func3)(c:4, a:2)

func4 = {|a:3|
  println a
}

func4.
call_wrapper(func4).
func4 1
call_wrapper(func4) 1

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
call_wrapper(func5)(a:1,b:2,c:3,d:4,e:5,f:6,g:7,h:8)
call_wrapper(func5)(1,2,3,4,5,6,7,8)
call_wrapper(func5)(1,2)
call_wrapper(func5)(1,2,3)
call_wrapper(func5)(d:10,a:3,b:1,h:6,f:7)

println.

times = {|count;func|
  i = 0
  while {!= i count} {
    func i
    i := + i 1
  }
}
list = []
100 @times {|i| list.append i }

func2(1000, :(list))
call_wrapper(func2)(1000, :(list))

func6 = {|b,c,d;e,f,g|
  println b c d e f g
}
5 8 9 @call_wrapper(func6) 3 2 7
