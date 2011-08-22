x = { 3: 1, "hey": "there"}

println x[3] x[2]

x[3] = 4
x[2] = 5

println x[3] x[2] x["hey"] x["no"]

x[2] = 6

println x[2]
println x["hey"]

println x.has_key?("hey")
println x.has_key?("there")

x.each {|k, v| println "each:" k v }
println x.size()

y = x.copy.

y[10] = "new"
y[3] = 11

println "=="
x.each {|k, v| println "each:" k v }
println x.size()
println "=="
y.each {|k, v| println "each:" k v }
println y.size()

println.

println y.delete(3)
y.each {|k, v| println "each:" k v }
println y.size()
