func = { |x|
 k = cont
 if (< x 10) {
   print(x)
   func (+ x 1)
 } {
   f = { |x|
     print (1000 +. x)
   }
   print 70
   f 70
   f := k
   f 100
   print 80
 }
}
{ print func(1) } @if true
{ print func(5) } @if false