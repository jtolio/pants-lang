x = { 3: 1, "hey": "there"}

print x[3] x[2]

x[3] = 4
x[2] = 5

print x[3] x[2] x["hey"] x["no"]

x[2] = 6

print x[2]
print x["hey"]

print x.has_key?("hey")
print x.has_key?("there")

x.each {|k, v| print "each:" k v }
print x.size()

y = x.copy.

y[10] = "new"
y[3] = 11

print "=="
x.each {|k, v| print "each:" k v }
print x.size()
print "=="
y.each {|k, v| print "each:" k v }
print y.size()

print.

print y.delete(3)
y.each {|k, v| print "each:" k v }
print y.size()
