obj = construct { |o|
  o.field1 = 3
  o.field2 = 4
}

print obj
print obj.field1
print obj.field2
obj.field1 = 5
print obj.field1
print "should make it here"
obj.field3 = 6 # should fail!
print "shouldn't make it here"
