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

  AST types
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

import itertools

def formats(things, indent):
  return map(lambda thing: thing.format(indent), things)

def references_in_expression_list(expressions, identifier):
  for expression in expressions:
    if expression.references(identifier): return True
    if expression.binds(identifier): return False
  return False

class Program(object):
  __slots__ = ["expressions", "line", "col"]
  def __init__(self, expressions, line=1, col=1):
    self.expressions = expressions
    self.line = line
    self.col = col
  def __repr__(self):
    return "Program(%r, %d, %d)" % (self.expressions, self.line, self.col)
  def references(self, identifier):
    return references_in_expression_list(self.expressions, identifier)
  def format(self, indent=""):
    return (";\n%s" % indent).join(formats(self.expressions, indent))

class Value(object): pass

class Variable(Value):
  __slots__ = ["identifier", "line", "col"]
  def __init__(self, identifier, line, col):
    self.identifier = identifier
    self.line = line
    self.col = col
  def __repr__(self):
    return "Variable(%r, %d, %d)" % (self.identifier, self.line, self.col)
  def references(self, identifier): return self.identifier == identifier
  def format(self, indent): return str(self.identifier)

class Subexpression(Value):
  __slots__ = ["expressions", "line", "col"]
  def __init__(self, expressions, line, col):
    self.expressions = expressions
    self.line = line
    self.col = col
  def references(self, identifier):
    return references_in_expression_list(self.expressions, identifier)
  def binds_anything(self):
    for expression in self.expressions:
      if expression.binds_anything(): return True
    return False
  def format(self, indent):
    return "(%s)" % ("; ").join(formats(self.expressions, indent))
  def __repr__(self):
    return "Subexpression(%r, %d, %d)" % (self.expressions, self.line, self.col)

class Integer(Value):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def references(self, identifier): return False
  def format(self, indent): return str(self.value)
  def __repr__(self):
    return "Integer(%r, %d, %d)" % (self.value, self.line, self.col)

class Float(Value):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def references(self, identifier): return False
  def format(self, indent): return str(self.value)
  def __repr__(self):
    return "Float(%r, %d, %d)" % (self.value, self.line, self.col)

class String(Value):
  __slots__ = ["value", "line", "col", "byte_oriented"]
  SAFE_CHARS = set("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789~`!@#$%^&*()-_=+[{]}|;:'<,>./? ")
  def __init__(self, byte_oriented, value, line, col):
    self.byte_oriented = byte_oriented
    self.value = value
    self.line = line
    self.col = col
  def references(self, identifier): return False
  def safe_format(self, c_style=True):
    string = []
    hex_escape = "\\x"
    if not c_style:
      hex_escape = "\\0x"
    for char in self.value:
      if char in String.SAFE_CHARS:
        string.append(char)
      else:
        string.append(hex_escape)
        string.append("%x" % ord(char))
    if self.byte_oriented:
      return 'b"%s"' % "".join(string)
    return '"%s"' % "".join(string)
  def format(self, indent): return self.safe_format(False)
  def __repr__(self):
    return "String(%r, %r, %d, %d)" % (self.byte_oriented, self.value,
        self.line, self.col)

class Array(Value):
  __slots__ = ["applications", "line", "col"]
  def __init__(self, applications, line, col):
    self.applications = applications
    self.line = line
    self.col = col
  def references(self, identifier):
    for app in self.applications:
      if app.references(identifier): return True
    return False
  def format(self, indent):
    return "[%s]" % ", ".join(formats(self.applications, indent))
  def __repr__(self):
    return "Array(%r, %d, %d)" % (self.applications, self.line, self.col)

class DictDefinition(object):
  __slots__ = ["key", "value", "line", "col"]
  def __init__(self, key, value, line, col):
    self.key = key
    self.value = value
    self.line = line
    self.col = col
  def references(self, identifier):
    return self.key.references(identifier) or self.value.references(identifier)
  def format(self, indent):
    return "%s: %s" % (self.key.format(indent), self.value.format(indent))
  def __repr__(self):
    return "DictDefinition(%r, %r, %d, %d)" % (self.key, self.value, self.line,
        self.col)

class Dict(Value):
  __slots__ = ["definitions", "line", "col"]
  def __init__(self, definitions, line, col):
    self.definitions = definitions
    self.line = line
    self.col = col
  def references(self, identifier):
    for definition in self.definitions:
      if definition.references(identifier): return True
    return False
  def format(self, indent):
    return "{%s}" % ", ".join(formats(self.definitions, indent))
  def __repr__(self):
    return "Dict(%r, %d, %d)" % (self.definitions, self.line, self.col)

class Function(Value):
  __slots__ = ["expressions", "left_args", "right_args", "line", "col"]
  def __init__(self, expressions, left_args, right_args, line, col):
    self.expressions = expressions
    self.left_args = left_args
    self.right_args = right_args
    self.line = line
    self.col = col
  def references(self, identifier):
    is_bound = False
    for arg in itertools.chain(self.left_args, self.right_args):
      if arg.references(identifier): return True
      if arg.binds(identifier): is_bound = True
    if is_bound: return False
    return references_in_expression_list(self.expressions, identifier)
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
  def __repr__(self):
    return "Function(%r, %r, %r, %d, %d)" % (self.expressions, self.left_args,
        self.right_args, self.line, self.col)

class Expression(object): pass

class Assignment(Expression):
  __slots__ = ["mutation", "assignee", "expression", "line", "col"]
  def __init__(self, mutation, assignee, expression, line, col):
    self.mutation = mutation
    self.assignee = assignee
    self.expression = expression
    self.line = line
    self.col = col
  def binds_anything(self):
    return not self.mutation or self.expression.binds_anything()
  def references(self, identifier):
    return self.assignee.references(identifier, self)
  def binds(self, identifier):
    return self.assignee.binds(identifier, self)
  def format(self, indent):
    return "%s %s %s" % (self.assignee.format(indent),
        self.mutation and ":=" or "=", self.expression.format(indent))
  def __repr__(self):
    return "Assignment(%r, %r, %r, %d, %d)" % (self.mutation, self.assignee,
        self.expression, self.line, self.col)

class Application(Expression):
  __slots__ = ["terms", "line", "col"]
  def __init__(self, terms, line, col):
    self.terms = terms
    self.line = line
    self.col = col
  def binds_anything(self): return False
  def references(self, identifier):
    for term in self.terms:
      if term.references(identifier): return True
    return False
  def binds(self, identifier): return False
  def format(self, indent):
    return " ".join(formats(self.terms, indent))
  def __repr__(self):
    return "Application(%r, %d, %d)" % (self.terms, self.line, self.col)

class Assignee(object): pass

class FieldAssignee(Assignee):
  __slots__ = ["term", "field", "line", "col"]
  def __init__(self, term, field, line, col):
    self.term = term
    self.field = field
    self.line = line
    self.col = col
  def references(self, identifier, assignment):
    if assignment.expression.references(identifier): return True
    if self.term.references(identifier): return True
    return False
  def binds(self, identifier, assignment): return False
  def format(self, indent):
    return "%s%s" % (self.term.format(indent), self.field.format(indent))
  def __repr__(self):
    return "FieldAssignee(%r, %r, %d, %d)" % (self.term, self.field, self.line,
        self.col)

class VariableAssignee(Assignee):
  __slots__ = ["variable", "line", "col"]
  def __init__(self, variable, line, col):
    self.variable = variable
    self.line = line
    self.col = col
  def references(self, identifier, assignment):
    if not assignment.mutation:
      if self.variable.references(identifier): return False
      return assignment.expression.references(identifier)
    if assignment.expression.references(identifier): return True
    if assignment.expression.binds(identifier): return False
    return self.variable.references(identifier)
  def binds(self, identifier, assignment):
    if not assignment.mutation and self.variable.references(identifier):
      return True
    if assignment.expression.binds(identifier): return True
    return False
  def format(self, indent):
    return self.variable.format(indent)
  def __repr__(self):
    return "VariableAssignee(%r, %d, %d)" % (self.variable, self.line, self.col)

class IndexAssignee(Assignee):
  __slots__ = ["term", "index", "line", "col"]
  def __init__(self, term, index, line, col):
    self.term = term
    self.index = index
    self.line = line
    self.col = col
  def references(self, identifier, assignment):
    if assignment.expression.references(identifier): return True
    if self.term.references(identifier): return True
    if self.index.references(identifier): return True
    return False
  def binds(self, identifier, assignment): return False
  def format(self, indent):
    return "%s%s" % (self.term.format(indent), self.index.format(indent))
  def __repr__(self):
    return "IndexAssignee(%r, %r, %d, %d)" % (self.term, self.index, self.line,
        self.col)

class Term(object):
  __slots__ = ["value", "modifiers", "line", "col"]
  def __init__(self, value, modifiers, line, col):
    self.value = value
    self.modifiers = modifiers
    self.line = line
    self.col = col
  def references(self, identifier):
    if self.value.references(identifier): return True
    for modifier in self.modifiers:
      if modifier.references(identifier): return True
  def format(self, indent):
    return "%s%s" % (self.value.format(indent),
        "".join(formats(self.modifiers, indent)))
  def __repr__(self):
    return "Term(%r, %r, %d, %d)" % (self.value, self.modifiers, self.line,
        self.col)

class ValueModifier(object): pass

class OpenCall(ValueModifier):
  __slots__ = ["line", "col"]
  def __init__(self, line, col):
    self.line = line
    self.col = col
  def references(self, identifier): return False
  def format(self, indent): return "."
  def __repr__(self): return "OpenCall(%d, %d)" % (self.line, self.col)

class Index(ValueModifier):
  __slots__ = ["expressions", "line", "col"]
  def __init__(self, expressions, line, col):
    self.expressions = expressions
    self.line = line
    self.col = col
  def references(self, identifier):
    return references_in_expression_list(self.expressions, identifier)
  def format(self, indent):
    return "[%s]" % "; ".join(formats(self.expressions, indent))
  def __repr__(self):
    return "Index(%r, %d, %d)" % (self.expressions, self.line, self.col)

class Field(ValueModifier):
  __slots__ = ["identifier", "line", "col"]
  def __init__(self, identifier, line, col):
    self.identifier = identifier
    self.line = line
    self.col = col
  def references(self, identifier): return False
  def format(self, indent): return ".%s" % self.identifier
  def __repr__(self):
    return "Field(%r, %d, %d)" % (self.identifier, self.line, self.col)

class ClosedCall(ValueModifier):
  __slots__ = ["left_args", "right_args", "line", "col"]
  def __init__(self, left_args, right_args, line, col):
    self.left_args = left_args
    self.right_args = right_args
    self.line = line
    self.col = col
  def references(self, identifier):
    for arg in itertools.chain(self.left_args, self.right_args):
      if arg.references(identifier): return True
  def format(self, indent):
    if self.left_args:
      return "(%s; %s)" % (", ".join(formats(self.left_args, indent)),
          ", ".join(formats(self.right_args, indent)))
    return "(%s)" % ", ".join(formats(self.right_args, indent))
  def __repr__(self):
    return "ClosedCall(%r, %r, %d, %d)" % (self.left_args, self.right_args,
        self.line, self.col)

class InArgument(object): pass
class OutArgument(object): pass

class KeywordOutArgument(OutArgument):
  __slots__ = ["expressions", "line", "col"]
  def __init__(self, expressions, line, col):
    self.expressions = expressions
    self.line = line
    self.col = col
  def references(self, identifier):
    return references_in_expression_list(self.expressions, identifier)
  def format(self, indent):
    return "::(%s)" % "; ".join(formats(self.expressions, indent))
  def __repr__(self):
    return "KeywordOutArgument(%r, %d, %d)" % (self.expressions, self.line,
        self.col)

class KeywordInArgument(InArgument):
  __slots__ = ["identifier", "line", "col"]
  def __init__(self, identifier, line, col):
    self.identifier = identifier
    self.line = line
    self.col = col
  def references(self, identifier): return False
  def binds(self, identifier): return identifier == self.identifier
  def format(self, indent): return "::(%s)" % self.identifier
  def __repr__(self):
    return "KeywordInArgument(%r, %d, %d)" % (self.identifier, self.line,
        self.col)

class ArbitraryOutArgument(OutArgument):
  __slots__ = ["expressions", "line", "col"]
  def __init__(self, expressions, line, col):
    self.expressions = expressions
    self.line = line
    self.col = col
  def references(self, identifier):
    return references_in_expression_list(self.expressions, identifier)
  def format(self, indent):
    return ":(%s)" % "; ".join(formats(self.expressions, indent))
  def __repr__(self):
    return "ArbitraryOutArgument(%r, %d, %d)" % (self.expressions, self.line,
        self.col)

class ArbitraryInArgument(InArgument):
  __slots__ = ["identifier", "line", "col"]
  def __init__(self, identifier, line, col):
    self.identifier = identifier
    self.line = line
    self.col = col
  def references(self, identifier): return False
  def binds(self, identifier): return identifier == self.identifier
  def format(self, indent): return ":(%s)" % self.identifier
  def __repr__(self):
    return "ArbitraryInArgument(%r, %d, %d)" % (self.identifier, self.line,
        self.col)

class NamedOutArgument(OutArgument):
  __slots__ = ["name", "value", "line", "col"]
  def __init__(self, name, value, line, col):
    self.name = name
    self.value = value
    self.line = line
    self.col = col
  def references(self, identifier): return self.value.references(identifier)
  def format(self, indent):
    return "%s:%s" % (self.name, self.value.format(indent))
  def __repr__(self):
    return "NamedOutArgument(%r, %r, %d, %d)" % (self.name, self.value,
        self.line, self.col)

class DefaultInArgument(InArgument):
  __slots__ = ["name", "value", "line", "col"]
  def __init__(self, name, value, line, col):
    self.name = name
    self.value = value
    self.line = line
    self.col = col
  def references(self, identifier): return self.value.references(identifier)
  def binds(self, identifier): return identifier == self.name
  def format(self, indent):
    return "%s:%s" % (self.name, self.value.format(indent))
  def __repr__(self):
    return "DefaultInArgument(%r, %r, %d, %d)" % (self.name, self.value,
        self.line, self.col)

class PositionalOutArgument(OutArgument):
  __slots__ = ["value", "line", "col"]
  def __init__(self, value, line, col):
    self.value = value
    self.line = line
    self.col = col
  def references(self, identifier): return self.value.references(identifier)
  def format(self, indent): return self.value.format(indent)
  def __repr__(self):
    return "PositionalOutArgument(%r, %d, %d)" % (self.value, self.line,
        self.col)

class RequiredInArgument(InArgument):
  __slots__ = ["name", "line", "col"]
  def __init__(self, name, line, col):
    self.name = name
    self.line = line
    self.col = col
  def references(self, identifier): return False
  def binds(self, identifier): return identifier == self.name
  def format(self, indent): return self.name
  def __repr__(self):
    return "RequiredInArgument(%r, %d, %d)" % (self.name, self.line, self.col)
