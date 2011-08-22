obj = construct { |o|
  o.field1 = 3
  o.field2 = 4
}

println obj
println obj.field1
println obj.field2
obj.field1 = 5
println obj.field1
println "should make it here"
obj.field3 = 6 # should fail!
println "shouldn't make it here"
