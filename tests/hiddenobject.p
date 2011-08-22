
println "hidden object test"

{
  println .x
}(;;x:3)

println .x # should fail
