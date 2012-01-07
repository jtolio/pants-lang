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
    "KeywordInArgument"]

import functools
from ast import types as ast

class Expression(object): pass

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
  def format(self, indent): return self.name
  def c_name(self):
    if not self.user_provided: return "c_%s" % self.name
    name = ["u_"]
    for char in self.name:
      if char in Identifier.SAFE_C_CHARS:
        name.append(char)
      elif char == "_":
        name.append("__")
      else:
        name.append("_")
        name.append("%x" % ord(char))
    return "".join(name)
  def __eq__(self, other):
    return (self.name, self.user_provided) == (other.name, other.user_provided)
  def __lt__(self, other):
    return (self.name, self.user_provided) < (other.name, other.user_provided)

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

class Variable(Value):
  __slots__ = ["identifier", "line", "col"]
  def __init__(self, identifier, line, col):
    self.identifier = identifier
    self.line = line
    self.col = col
  def format(self, indent): return self.identifier.format(indent)

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

class Array(Value):
  __slots__ = ["values", "line", "col"]
  def __init__(self, values, line, col):
    self.values = values
    self.line = line
    self.col = col
  def format(self, indent):
    return "[%s]" % ", ".join((value.format(indent) for value in self.values))

class DictDefinition(object):
  __slots__ = ["key", "value", "line", "col"]
  def __init__(self, key, value, line, col):
    self.key = key
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent):
    return "%s: %s" % (self.key.format(indent), self.value.format(indent))

class Dict(Value):
  __slots__ = ["definitions", "line", "col"]
  def __init__(self, definitions, line, col):
    self.definitions = definitions
    self.line = line
    self.col = col
  def format(self, indent):
    return "{%s}" % ", ".join((definition.format(indent)
        for definition in self.definitions))

class OutArgument(object): pass

class PositionalOutArgument(OutArgument):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent): return self.value.format(indent)

class NamedOutArgument(OutArgument):
  __slots__ = ["name", "value", "line", "col"]
  def __init__(self, name, value, line, col):
    self.name = name
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent):
    return "%s:%s" % (self.name.format(indent), self.value.format(indent))

class ArbitraryOutArgument(OutArgument):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent): return ":(%s)" % self.value.format(indent)

class KeywordOutArgument(OutArgument):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent): return "::(%s)" % self.value.format(indent)

class InArgument(object): pass

class RequiredInArgument(InArgument):
  __slots__ = ["name", "line", "col"]
  def __init__(self, name, line, col):
    self.name = name
    self.line = line
    self.col = col
  def format(self, indent): return self.name.format(indent)

class DefaultInArgument(InArgument):
  __slots__ = ["name", "value", "line", "col"]
  def __init__(self, name, value, line, col):
    self.name = name
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent):
    return "%s:%s" % (self.name.format(indent), self.value.format(indent))

class ArbitraryInArgument(InArgument):
  __slots__ = ["name", "line", "col"]
  def __init__(self, name, line, col):
    self.name = name
    self.line = line
    self.col = col
  def format(self, indent): return ":(%s)" % self.name.format(indent)

class KeywordInArgument(InArgument):
  __slots__ = ["name", "line", "col"]
  def __init__(self, name, line, col):
    self.name = name
    self.line = line
    self.col = col
  def format(self, indent): return "::(%s)" % self.name.format(indent)
