#!/usr/bin/env python
#
# Copyright (c) 2011, JT Olds <hello@jtolds.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

"""
  Pants
  http://www.pants-lang.org/

  AST module
"""

__author__ = "JT Olds"
__author_email__ = "hello@jtolds.com"

class Program(object):
  __slots__ = ["expressions"]
  def __init__(self, expressions):
    self.expressions = expressions
  def __str__(self):
    return "; ".join(map(str, self.expressions))

class Value(object): pass

class Variable(Value):
  __slots__ = ["identifier", "line", "col"]
  def __init__(self, identifier, line, col):
    self.identifier = identifier
    self.line = line
    self.col = col
  def __str__(self):
    return str(self.identifier)

class Subexpression(Value):
  __slots__ = ["expressions", "line", "col"]
  def __init__(self, expressions, line, col):
    self.expressions = expressions
    self.line = line
    self.col = col
  def __str__(self):
    return "(%s)" % "; ".join(map(str, self.expressions))

class Integer(Value):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def __str__(self): return str(self.value)

class Float(Value):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def __str__(self): return str(self.value)

class String(Value):
  __slots__ = ["value", "line", "col", "byte_oriented"]
  def __init__(self, byte_oriented, value, line, col):
    self.byte_oriented = byte_oriented
    self.value = value
    self.line = line
    self.col = col
  def __str__(self):
    if self.byte_oriented:
      return 'b"%s"' % self.value
    return '"%s"' % self.value

class Array(Value):
  __slots__ = ["applications", "line", "col"]
  def __init__(self, applications, line, col):
    self.applications = applications
    self.line = line
    self.col = col
  def __str__(self): return "[%s]" % ", ".join(map(str, self.applications))

class DictDefinition(object):
  __slots__ = ["key", "value", "line", "col"]
  def __init__(self, key, value, line, col):
    self.key = key
    self.value = value
    self.line = line
    self.col = col
  def __str__(self): return "%s: %s" % (self.key, self.value)

class Dict(Value):
  __slots__ = ["definitions", "line", "col"]
  def __init__(self, definitions, line, col):
    self.definitions = definitions
    self.line = line
    self.col = col
  def __str__(self): return "{%s}" % ", ".join(map(str, self.definitions))

class Function(Value):
  __slots__ = ["expressions", "left_args", "right_args", "line", "col"]
  def __init__(self, expressions, left_args, right_args, line, col):
    self.expressions = expressions
    self.left_args = left_args
    self.right_args = right_args
    self.line = line
    self.col = col
  def __str__(self):
    if self.left_args:
      return "{|%s; %s| %s}" % (", ".join(map(str, self.left_args)),
          ", ".join(map(str, self.right_args)),
          "; ".join(map(str, self.expressions)))
    if self.right_args:
      return "{|%s| %s}" % (", ".join(map(str, self.right_args)),
          "; ".join(map(str, self.expressions)))
    return "{%s}" % "; ".join(map(str, self.expressions))

class Expression(object): pass

class Assignment(Expression):
  __slots__ = ["mutation", "assignee", "expression", "line", "col"]
  def __init__(self, mutation, assignee, expression, line, col):
    self.mutation = mutation
    self.assignee = assignee
    self.expression = expression
    self.line = line
    self.col = col
  def __str__(self):
    return "%s %s %s" % (self.assignee, self.mutation and ":=" or "=",
        self.expression)

class Application(Expression):
  __slots__ = ["terms", "line", "col"]
  def __init__(self, terms, line, col):
    self.terms = terms
    self.line = line
    self.col = col
  def __str__(self):
    return " ".join(map(str, self.terms))

class Assignee(object): pass

class FieldAssignee(Assignee):
  __slots__ = ["term", "field", "line", "col"]
  def __init__(self, term, field, line, col):
    self.term = term
    self.field = field
    self.line = line
    self.col = col
  def __str__(self):
    return "%s%s" % (self.term, self.field)

class VariableAssignee(Assignee):
  __slots__ = ["variable", "line", "col"]
  def __init__(self, variable, line, col):
    self.variable = variable
    self.line = line
    self.col = col
  def __str__(self):
    return str(self.variable)

class IndexAssignee(Assignee):
  __slots__ = ["term", "index", "line", "col"]
  def __init__(self, term, index, line, col):
    self.term = term
    self.index = index
    self.line = line
    self.col = col
  def __str__(self): return "%s%s" % (self.term, self.index)

class Term(object):
  __slots__ = ["value", "modifiers", "line", "col"]
  def __init__(self, value, modifiers, line, col):
    self.value = value
    self.modifiers = modifiers
    self.line = line
    self.col = col
  def __str__(self):
    return "%s%s" % (self.value, "".join(map(str, self.modifiers)))

class ValueModifier(object): pass

class OpenCall(ValueModifier):
  __slots__ = ["line", "col"]
  def __init__(self, line, col):
    self.line = line
    self.col = col
  def __str__(self): return "."

class Index(ValueModifier):
  __slots__ = ["expressions", "line", "col"]
  def __init__(self, expressions, line, col):
    self.expressions = expressions
    self.line = line
    self.col = col
  def __str__(self): return "[%s]" % "; ".join(map(str, self.expressions))

class Field(ValueModifier):
  __slots__ = ["identifier", "line", "col"]
  def __init__(self, identifier, line, col):
    self.identifier = identifier
    self.line = line
    self.col = col
  def __str__(self): return ".%s" % self.identifier

class ClosedCall(ValueModifier):
  __slots__ = ["left_args", "right_args", "line", "col"]
  def __init__(self, left_args, right_args, line, col):
    self.left_args = left_args
    self.right_args = right_args
    self.line = line
    self.col = col
  def __str__(self):
    if self.left_args:
      return "(%s; %s)" % (", ".join(map(str, self.left_args)),
          ", ".join(map(str, self.right_args)))
    return "(%s)" % ", ".join(map(str, self.right_args))

class InArgument(object): pass
class OutArgument(object): pass

class KeywordOutArgument(OutArgument):
  __slots__ = ["expressions", "line", "col"]
  def __init__(self, expressions, line, col):
    self.expressions = expressions
    self.line = line
    self.col = col
  def __str__(self): return "::(%s)" % "; ".join(map(str, self.expressions))

class KeywordInArgument(InArgument):
  __slots__ = ["identifier", "line", "col"]
  def __init__(self, identifier, line, col):
    self.identifier = identifier
    self.line = line
    self.col = col
  def __str__(self): return "::(%s)" % self.identifier

class ArbitraryOutArgument(OutArgument):
  __slots__ = ["expressions", "line", "col"]
  def __init__(self, expressions, line, col):
    self.expressions = expressions
    self.line = line
    self.col = col
  def __str__(self): return ":(%s)" % "; ".join(map(str, self.expressions))

class ArbitraryInArgument(InArgument):
  __slots__ = ["identifier", "line", "col"]
  def __init__(self, identifier, line, col):
    self.identifier = identifier
    self.line = line
    self.col = col
  def __str__(self): return ":(%s)" % self.identifier

class NamedOutArgument(OutArgument):
  __slots__ = ["name", "value", "line", "col"]
  def __init__(self, name, value, line, col):
    self.name = name
    self.value = value
    self.line = line
    self.col = col
  def __str__(self): return "%s: %s" % (self.name, self.value)

class DefaultInArgument(InArgument):
  __slots__ = ["name", "value", "line", "col"]
  def __init__(self, name, value, line, col):
    self.name = name
    self.value = value
    self.line = line
    self.col = col
  def __str__(self): return "%s: %s" % (self.name, self.value)

class PositionalOutArgument(OutArgument):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def __str__(self): return str(self.value)

class RequiredInArgument(InArgument):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def __str__(self): return str(self.value)
