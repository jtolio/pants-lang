# PANTS OPTIONS: --skip-prelude

func = { |x|
 k = cont
 if (< x 10) {
   println(x)
   func (+ x 1)
 } {
   f = { |x|
     println (1000 +. x)
   }
   println 70
   f 70
   f := k
   f 100
   println 80
 }
}
{ println func(1) } @if true
{ println func(5) } @if false
