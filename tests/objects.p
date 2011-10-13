# PANTS OPTIONS: --skip-prelude

constructor = {|func|
  {|:(largs); :(rargs), ::(kwargs)|
    obj = new_object.
    func(:(largs); obj, :(rargs), ::(kwargs))
    seal_object obj
    obj
  }
}
# TODO: actually clear this out of the scope or something
new_object = null
seal_object = null

obj = (constructor { |o|
  o.field1 = 3
  o.field2 = 4
})()

println obj
println obj.field1
println obj.field2
obj.field1 = 5
println obj.field1
println "should make it here"
obj.field3 = 6 # should fail!
println "shouldn't make it here"
