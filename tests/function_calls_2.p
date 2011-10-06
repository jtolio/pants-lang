call_wrapper = {|call|
  {|:(largs);:(rargs),::(kwargs)| call(:(largs); :(rargs),::(kwargs)) }
}

function = {|la:3;ra1,ra2:4|
  println la ra1 ra2
}

function 1
call_wrapper(function) 1
function 1 2
call_wrapper(function) 1 2
1 @function 2
1 @call_wrapper(function) 2
1 @function 2 3
1 @call_wrapper(function) 2 3

function = {|la1:3,la2;|
  println la1 la2
}

1 @function
1 @call_wrapper(function)
1 2 @function
1 2 @call_wrapper(function)

function = {|:(las),la1:3,la2;|
  las @each {|la|
    print la
    print " "
  }
  println la1 la2
}

1 @function
1 @call_wrapper(function)
1 2 @function
1 2 @call_wrapper(function)
1 2 3 4 @function
1 2 3 4 @call_wrapper(function)
function(:([1,2,3]), 4;)
call_wrapper(function)(:([1,2,3]), 4;)

function = {|ra1,ra2:3,:(ras)|
  print ra1 ra2
  ras @each {|ra|
    print " "
    print ra
  }
  println.
}

function 1
call_wrapper(function) 1
function 1 2
call_wrapper(function) 1 2
function 1 2 3 4
call_wrapper(function) 1 2 3 4
function(1, :([2,3,4]))
call_wrapper(function)(1, :([2,3,4]))

times = {|count;func|
  i = 0
  while {!= i count} {
    func i
    i := + i 1
  }
}
list = []
100 @times {|i| list.append i}

function = {|:(las), la1, la2;|
  las @each {|la|
    print la
    print " "
  }
  println la1 la2
}

function(:(list), 1000;)
call_wrapper(function)(:(list), 1000;)

function = {|la; ra|
  println la ra
}
function(la:1, ra:2)
call_wrapper(function)(la:1, ra:2)
