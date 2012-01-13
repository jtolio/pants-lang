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

  IR types
"""

__author__ = "JT Olds"
__author_email__ = "hello@jtolds.com"
__all__ = ["Identifier", "Expression", "Assignment", "ObjectMutation",
    "ReturnValue", "Value", "Field", "Variable", "Integer", "String", "Float",
    "Function", "OutArgument", "PositionalOutArgument", "NamedOutArgument",
    "ArbitraryOutArgument", "KeywordOutArgument", "InArgument",
    "RequiredInArgument", "DefaultInArgument", "ArbitraryInArgument",
    "KeywordInArgument", "Program"]

import functools
from ast import types as ast

class Expression(object): pass

class Program(object):
  __slots__ = ["expressions", "lastval", "line", "col"]
  def __init__(self, expressions, lastval, line, col):
    self.expressions = expressions
    self.lastval = lastval
    self.line = line
    self.col = col
  def format(self, indent=""):
    return ";\n".join((exp.format(indent) for exp in self.expressions))
  def __repr__(self):
    return "Program(%r, %r, %d, %d)" % (self.expressions, self.lastval,
        self.line, self.col)

@functools.total_ordering
class Identifier(object):
  SAFE_C_CHARS = set("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789")
  __slots__ = ["name", "user_provided", "line", "col"]
  def __init__(self, name, user_provided, line, col):
    self.name = name
    self.user_provided = user_provided
    self.line = line
    self.col = col
  def format(self, indent):
    return self.c_name(False)
  def c_name(self, escape=True):
    if not self.user_provided:
      assert self.name[-1] != "_"
      for char in self.name:
        assert char == "_" or char in Identifier.SAFE_C_CHARS
      return "%s_" % self.name
    if escape:
      name = []
      for char in self.name:
        if char in Identifier.SAFE_C_CHARS:
          name.append(char)
        elif char == "_":
          name.append("__")
        else:
          name.append("_")
          name.append("%x" % ord(char))
      return "".join(name)
    else:
      if self.name[-1] == "_": return "%s_" % self.name
      return self.name
  def __eq__(self, other):
    return (self.name, self.user_provided) == (other.name, other.user_provided)
  def __lt__(self, other):
    return (self.name, self.user_provided) < (other.name, other.user_provided)
  def __repr__(self):
    return "Identifier(%r, %r, %d, %d)" % (self.name, self.user_provided,
        self.line, self.col)

class Assignment(Expression):
  __slots__ = ["assignee", "value", "local", "line", "col"]
  def __init__(self, assignee, value, local, line, col):
    self.assignee = assignee
    self.value = value
    self.local = local
    self.line = line
    self.col = col
  def format(self, indent):
    return "%s %s %s" % (self.assignee.format(indent),
        self.local and "=" or ":=", self.value.format(indent))
  def __repr__(self):
    return "Assignment(%r, %r, %r, %d, %d)" % (self.assignee, self.value,
        self.local, self.line, self.col)

class ObjectMutation(Expression):
  __slots__ = ["object", "field", "value", "line", "col"]
  def __init__(self, object_, field, value, line, col):
    self.object = object_
    self.field = field
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent):
    return "%s.%s = %s" % (self.object.format(indent),
        self.field.format(indent), self.value.format(indent))
  def __repr__(self):
    return "ObjectMutation(%r, %r, %r, %d, %d)" % (self.object, self.field,
        self.value, self.line, self.col)

class ReturnValue(Expression):
  __slots__ = ["assignee", "call", "left_args", "right_args", "line", "col"]
  def __init__(self, assignee, call, left_args, right_args, line, col):
    self.assignee = assignee
    self.call = call
    self.left_args = left_args
    self.right_args = right_args
    self.line = line
    self.col = col
  def format(self, indent):
    call = ["%s = %s(" % (self.assignee.format(indent),
        self.call.format(indent))]
    call.append(", ".join((arg.format(indent) for arg in self.left_args)))
    if self.left_args:
      if self.right_args: call.append("; ")
      else: call.append(";")
    if self.right_args:
      call.append(", ".join((arg.format(indent) for arg in self.right_args)))
    call.append(")")
    return "".join(call)
  def __repr__(self):
    return "ReturnValue(%r, %r, %r, %r, %d, %d)" % (self.assignee, self.call,
        self.left_args, self.right_args, self.line, self.col)

class Value(object): pass

class Field(Value):
  __slots__ = ["object", "field", "line", "col"]
  def __init__(self, object_, field, line, col):
    self.object = object_
    self.field = field
    self.line = line
    self.col = col
  def format(self, indent):
    return "%s.%s" % (self.object.format(indent), self.field.format(indent))
  def __repr__(self):
    return "Field(%r, %r, %d, %d)" % (self.object, self.field, self.line,
        self.col)

class Variable(Value):
  __slots__ = ["identifier", "line", "col"]
  def __init__(self, identifier, line, col):
    self.identifier = identifier
    self.line = line
    self.col = col
  def format(self, indent): return self.identifier.format(indent)
  def __repr__(self):
    return "Variable(%r, %d, %d)" % (self.identifier, self.line, self.col)

Integer = ast.Integer
Float = ast.Float
String = ast.String

class Function(Value):
  __slots__ = ["expressions", "lastval", "left_args", "right_args",
      "cont_defined", "line", "col"]
  def __init__(self, expressions, lastval, left_args, right_args, cont_defined,
      line, col):
    self.expressions = expressions
    self.lastval = lastval
    self.left_args = left_args
    self.right_args = right_args
    self.cont_defined = cont_defined
    self.line = line
    self.col = col
  def format(self, indent):
    string = ["{"]
    if self.left_args or self.right_args:
      string.append("|")
    if self.left_args:
      string.append(", ".join((arg.format(indent + "  ")
                                for arg in self.left_args)))
      string.append(";")
    if self.left_args and self.right_args:
      string.append(" ")
    if self.right_args:
      string.append(", ".join((arg.format(indent + "  ")
                                for arg in self.right_args)))
    if self.left_args or self.right_args:
      string.append("|")
    for exp in self.expressions:
      string.append("\n%s  %s;" % (indent, exp.format(indent + "  ")))
    if self.expressions:
      string.append("\n%s  %s;\n%s}" % (indent,
          self.lastval.format(indent + "  "), indent))
    else:
      string.append("%s}" % self.lastval.format(indent))
    return "".join(string)
  def __repr__(self):
    return "Function(%r, %r, %r, %r, %r, %d, %d)" % (self.expressions,
        self.lastval, self.left_args, self.right_args, self.cont_defined,
        self.line, self.col)

class OutArgument(object): pass

class PositionalOutArgument(OutArgument):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent): return self.value.format(indent)
  def __repr__(self):
    return "PositionalOutArgument(%r, %d, %d)" % (self.value, self.line,
        self.col)

class NamedOutArgument(OutArgument):
  __slots__ = ["name", "value", "line", "col"]
  def __init__(self, name, value, line, col):
    self.name = name
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent):
    return "%s:%s" % (self.name.format(indent), self.value.format(indent))
  def __repr__(self):
    return "NamedOutArgument(%r, %r, %d, %d)" % (self.name, self.value,
        self.line, self.col)

class ArbitraryOutArgument(OutArgument):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent): return ":(%s)" % self.value.format(indent)
  def __repr__(self):
    return "ArbitraryOutArgument(%r, %d, %d)" % (self.value, self.line,
        self.col)

class KeywordOutArgument(OutArgument):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent): return "::(%s)" % self.value.format(indent)
  def __repr__(self):
    return "KeywordOutArgument(%r, %d, %d)" % (self.value, self.line,
        self.col)

class InArgument(object): pass

class RequiredInArgument(InArgument):
  __slots__ = ["name", "line", "col"]
  def __init__(self, name, line, col):
    self.name = name
    self.line = line
    self.col = col
  def format(self, indent): return self.name.format(indent)
  def __repr__(self):
    return "RequiredInArgument(%r, %d, %d)" % (self.name, self.line, self.col)

class DefaultInArgument(InArgument):
  __slots__ = ["name", "value", "line", "col"]
  def __init__(self, name, value, line, col):
    self.name = name
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent):
    return "%s:%s" % (self.name.format(indent), self.value.format(indent))
  def __repr__(self):
    return "DefaultInArgument(%r, %r, %d, %d)" % (self.name, self.value,
        self.line, self.col)

class ArbitraryInArgument(InArgument):
  __slots__ = ["name", "line", "col"]
  def __init__(self, name, line, col):
    self.name = name
    self.line = line
    self.col = col
  def format(self, indent): return ":(%s)" % self.name.format(indent)
  def __repr__(self):
    return "ArbitraryInArgument(%r, %d, %d)" % (self.name, self.line, self.col)

class KeywordInArgument(InArgument):
  __slots__ = ["name", "line", "col"]
  def __init__(self, name, line, col):
    self.name = name
    self.line = line
    self.col = col
  def format(self, indent): return "::(%s)" % self.name.format(indent)
  def __repr__(self):
    return "KeywordInArgument(%r, %d, %d)" % (self.name, self.line, self.col)
