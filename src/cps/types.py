#!/usr/bin/env python
#
# Copyright (c) 2012, JT Olds <hello@jtolds.com>
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

  CPS types
"""

__author__ = "JT Olds"
__author_email__ = "hello@jtolds.com"
__all__ = ["Identifier", "Field", "Variable", "Integer", "Float", "String",
    "OutArgument", "PositionalOutArgument", "NamedOutArgument",
    "SplatOutArgument", "KeywordOutArgument", "InArgument",
    "RequiredInArgument", "DefaultInArgument", "SplatInArgument",
    "KeywordInArgument", "Expression", "Call", "Assignment", "ObjectMutation",
    "Value", "Callable"]

import itertools
from ir import types as ir

# most of this is just the same

Identifier = ir.Identifier
Field = ir.Field
Variable = ir.Variable
Integer = ir.Integer
Float = ir.Float
String = ir.String

OutArgument = ir.OutArgument
PositionalOutArgument = ir.PositionalOutArgument
NamedOutArgument = ir.NamedOutArgument
SplatOutArgument = ir.SplatOutArgument
KeywordOutArgument = ir.KeywordOutArgument

InArgument = ir.InArgument
RequiredInArgument = ir.RequiredInArgument
DefaultInArgument = ir.DefaultInArgument
SplatInArgument = ir.SplatInArgument
KeywordInArgument = ir.KeywordInArgument

class Expression(object): pass

class Call(Expression):
  __slots__ = ["call", "left_args", "right_args", "continuation", "line", "col"]
  def __init__(self, call, left_args, right_args, continuation, line, col):
    self.call = call
    self.left_args = left_args
    self.right_args = right_args
    self.continuation = continuation
    self.line = line
    self.col = col
  def __repr__(self):
    return "Call(%r, %r, %r, %r, %d, %d)" % (self.call, self.left_args,
        self.right_args, self.continuation, self.line, self.col)
  def references(self, identifier):
    if self.call.references(identifier): return True
    for arg in itertools.chain(self.left_args, self.right_args):
      if arg.references(identifier): return True
    if self.continuation and self.continuation.references(identifier):
      return True
    return False
  def format(self, indent=""):
    return "%s(%s; %s; %s)" % (self.call.format(indent),
              ", ".join((arg.format(indent + "  ") for arg in self.left_args)),
              ", ".join((arg.format(indent + "  ") for arg in self.right_args)),
              self.continuation is not None and
              self.continuation.format(indent + "  ") or "")

class Assignment(Expression):
  __slots__ = ["assignee", "value", "local", "next_expression", "line", "col"]
  def __init__(self, assignee, value, local, next_expression, line, col):
    self.assignee = assignee
    self.value = value
    self.local = local
    self.next_expression = next_expression
    self.line = line
    self.col = col
  def __repr__(self):
    return "Assignment(%r, %r, %r, %r, %d, %d)" % (self.assignee, self.value,
        self.local, self.next_expression, self.line, self.col)
  def references(self, identifier):
    if not self.local and self.assignee.references(identifier): return True
    if self.value.references(identifier): return True
    if self.next_expression.references(identifier): return True
    return False
  def format(self, indent=""):
    return "%s %s %s\n%s%s" % (self.assignee.format(indent),
                               self.local and "=" or ":=",
                               self.value.format(indent),
                               indent,
                               self.next_expression.format(indent))

class ObjectMutation(Expression):
  __slots__ = ["object", "field", "value", "next_expression", "line", "col"]
  def __init__(self, object_, field, value, next_expression, line, col):
    self.object = object_
    self.field = field
    self.value = value
    self.next_expression = next_expression
    self.line = line
    self.col = col
  def __repr__(self):
    return "ObjectMutation(%r, %r, %r, %r, %d, %d)" % (self.object, self.field,
        self.value, self.next_expression, self.line, self.col)
  def references(self, identifier):
    if self.object.references(identifier): return True
    if self.field.references(identifier): return True
    if self.value.references(identifier): return True
    if self.next_expression.references(identifier): return True
  def format(self, indent=""):
    return "%s.%s = %s\n%s%s" % (self.object.format(indent),
                               self.field.format(indent),
                               self.value.format(indent),
                               indent,
                               self.next_expression.format(indent))

class Value(object): pass

class Callable(Value):
  __slots__ = ["expression", "left_args", "right_args", "receives_cont", "line",
      "col"]
  def __init__(self, expression, left_args, right_args, receives_cont, line, col):
    self.expression = expression
    self.left_args = left_args
    self.right_args = right_args
    self.receives_cont = receives_cont
    self.line = line
    self.col = col
  def __repr__(self):
    return "Callable(%r, %r, %r, %r, %d, %d)" % (self.expression,
        self.left_args, self.right_args, self.receives_cont,
        self.line, self.col)
  def format(self, indent=""):
    return "{|%s; %s; %s|\n%s  %s\n%s}" % (
        ", ".join((arg.format(indent) for arg in self.left_args)),
        ", ".join((arg.format(indent) for arg in self.right_args)),
        self.receives_cont and Identifier("cont", False, 0, 0).format("") or "",
        indent,
        self.expression.format(indent + "  "),
        indent)
  def references(self, identifier):
    for arg in itertools.chain(self.left_args, self.right_args):
      if arg.references(identifier): return True
    for arg in itertools.chain(self.left_args, self.right_args):
      if arg.binds(identifier): return False
    if self.receives_cont and identifier == Identifier("null", False, 0, 0):
      return False
    if self.expression.references(identifier): return True
    return False
