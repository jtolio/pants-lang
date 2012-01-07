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
__all__ = ["Program", "Value", "Variable", "Subexpression", "Integer", "Float",
    "String", "Array", "DictDefinition", "Dict", "Function", "Expression",
    "Assignment", "Application", "Assignee", "FieldAssignee",
    "VariableAssignee", "IndexAssignee", "Term", "ValueModifier", "OpenCall",
    "Index", "Field", "ClosedCall", "InArgument", "OutArgument",
    "KeywordInArgument", "KeywordOutArgument", "ArbitraryInArgument",
    "ArbitraryOutArgument", "DefaultInArgument", "NamedOutArgument",
    "RequiredInArgument", "PositionalOutArgument"]

def formats(things, indent):
  return map(lambda thing: thing.format(indent), things)

class Program(object):
  __slots__ = ["expressions"]
  def __init__(self, expressions):
    self.expressions = expressions
  def format(self, indent=""):
    return (";\n%s" % indent).join(formats(self.expressions, indent))

class Value(object): pass

class Variable(Value):
  __slots__ = ["identifier", "line", "col"]
  def __init__(self, identifier, line, col):
    self.identifier = identifier
    self.line = line
    self.col = col
  def format(self, indent): return str(self.identifier)

class Subexpression(Value):
  __slots__ = ["expressions", "line", "col"]
  def __init__(self, expressions, line, col):
    self.expressions = expressions
    self.line = line
    self.col = col
  def format(self, indent):
    return "(%s)" % ("; ").join(formats(self.expressions, indent))

class Integer(Value):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent): return str(self.value)

class Float(Value):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent): return str(self.value)

class String(Value):
  __slots__ = ["value", "line", "col", "byte_oriented"]
  def __init__(self, byte_oriented, value, line, col):
    self.byte_oriented = byte_oriented
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent):
    if self.byte_oriented:
      return 'b"%s"' % self.value
    return '"%s"' % self.value

class Array(Value):
  __slots__ = ["applications", "line", "col"]
  def __init__(self, applications, line, col):
    self.applications = applications
    self.line = line
    self.col = col
  def format(self, indent):
    return "[%s]" % ", ".join(formats(self.applications, indent))

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
    return "{%s}" % ", ".join(formats(self.definitions, indent))

class Function(Value):
  __slots__ = ["expressions", "left_args", "right_args", "line", "col"]
  def __init__(self, expressions, left_args, right_args, line, col):
    self.expressions = expressions
    self.left_args = left_args
    self.right_args = right_args
    self.line = line
    self.col = col
  def format(self, indent):
    string = ["{"]
    if self.left_args or self.right_args:
      string.append("|")
    if self.left_args:
      string.append(", ".join(formats(self.left_args, indent + "  ")))
      string.append(";")
    if self.left_args and self.right_args:
      string.append(" ")
    if self.right_args:
      string.append(", ".join(formats(self.right_args, indent + "  ")))
    if self.left_args or self.right_args:
      string.append("|")
    for exp in self.expressions:
      string.append("\n%s  %s;" % (indent, exp.format(indent + "  ")))
    if self.expressions:
      string.append("\n%s" % indent)
    string.append("}")
    return "".join(string)

class Expression(object): pass

class Assignment(Expression):
  __slots__ = ["mutation", "assignee", "expression", "line", "col"]
  def __init__(self, mutation, assignee, expression, line, col):
    self.mutation = mutation
    self.assignee = assignee
    self.expression = expression
    self.line = line
    self.col = col
  def format(self, indent):
    return "%s %s %s" % (self.assignee.format(indent),
        self.mutation and ":=" or "=", self.expression.format(indent))

class Application(Expression):
  __slots__ = ["terms", "line", "col"]
  def __init__(self, terms, line, col):
    self.terms = terms
    self.line = line
    self.col = col
  def format(self, indent):
    return " ".join(formats(self.terms, indent))

class Assignee(object): pass

class FieldAssignee(Assignee):
  __slots__ = ["term", "field", "line", "col"]
  def __init__(self, term, field, line, col):
    self.term = term
    self.field = field
    self.line = line
    self.col = col
  def format(self, indent):
    return "%s%s" % (self.term.format(indent), self.field.format(indent))

class VariableAssignee(Assignee):
  __slots__ = ["variable", "line", "col"]
  def __init__(self, variable, line, col):
    self.variable = variable
    self.line = line
    self.col = col
  def format(self, indent):
    return self.variable.format(indent)

class IndexAssignee(Assignee):
  __slots__ = ["term", "index", "line", "col"]
  def __init__(self, term, index, line, col):
    self.term = term
    self.index = index
    self.line = line
    self.col = col
  def format(self, indent):
    return "%s%s" % (self.term.format(indent), self.index.format(indent))

class Term(object):
  __slots__ = ["value", "modifiers", "line", "col"]
  def __init__(self, value, modifiers, line, col):
    self.value = value
    self.modifiers = modifiers
    self.line = line
    self.col = col
  def format(self, indent):
    return "%s%s" % (self.value.format(indent),
        "".join(formats(self.modifiers, indent)))

class ValueModifier(object): pass

class OpenCall(ValueModifier):
  __slots__ = ["line", "col"]
  def __init__(self, line, col):
    self.line = line
    self.col = col
  def format(self, indent): return "."

class Index(ValueModifier):
  __slots__ = ["expressions", "line", "col"]
  def __init__(self, expressions, line, col):
    self.expressions = expressions
    self.line = line
    self.col = col
  def format(self, indent):
    return "[%s]" % "; ".join(formats(self.expressions, indent))

class Field(ValueModifier):
  __slots__ = ["identifier", "line", "col"]
  def __init__(self, identifier, line, col):
    self.identifier = identifier
    self.line = line
    self.col = col
  def format(self, indent): return ".%s" % self.identifier

class ClosedCall(ValueModifier):
  __slots__ = ["left_args", "right_args", "line", "col"]
  def __init__(self, left_args, right_args, line, col):
    self.left_args = left_args
    self.right_args = right_args
    self.line = line
    self.col = col
  def format(self, indent):
    if self.left_args:
      return "(%s; %s)" % (", ".join(formats(self.left_args, indent)),
          ", ".join(formats(self.right_args, indent)))
    return "(%s)" % ", ".join(formats(self.right_args, indent))

class InArgument(object): pass
class OutArgument(object): pass

class KeywordOutArgument(OutArgument):
  __slots__ = ["expressions", "line", "col"]
  def __init__(self, expressions, line, col):
    self.expressions = expressions
    self.line = line
    self.col = col
  def format(self, indent):
    return "::(%s)" % "; ".join(formats(self.expressions, indent))

class KeywordInArgument(InArgument):
  __slots__ = ["identifier", "line", "col"]
  def __init__(self, identifier, line, col):
    self.identifier = identifier
    self.line = line
    self.col = col
  def format(self, indent): return "::(%s)" % self.identifier

class ArbitraryOutArgument(OutArgument):
  __slots__ = ["expressions", "line", "col"]
  def __init__(self, expressions, line, col):
    self.expressions = expressions
    self.line = line
    self.col = col
  def format(self, indent):
    return ":(%s)" % "; ".join(formats(self.expressions, indent))

class ArbitraryInArgument(InArgument):
  __slots__ = ["identifier", "line", "col"]
  def __init__(self, identifier, line, col):
    self.identifier = identifier
    self.line = line
    self.col = col
  def format(self, indent): return ":(%s)" % self.identifier

class NamedOutArgument(OutArgument):
  __slots__ = ["name", "value", "line", "col"]
  def __init__(self, name, value, line, col):
    self.name = name
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent):
    return "%s:%s" % (self.name, self.value.format(indent))

class DefaultInArgument(InArgument):
  __slots__ = ["name", "value", "line", "col"]
  def __init__(self, name, value, line, col):
    self.name = name
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent):
    return "%s:%s" % (self.name, self.value.format(indent))

class PositionalOutArgument(OutArgument):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def format(self, indent): return self.value.format(indent)

class RequiredInArgument(InArgument):
  __slots__ = ["name", "line", "col"]
  def __init__(self, name, line, col):
    self.name = name
    self.line = line
    self.col = col
  def format(self, indent): return self.name
