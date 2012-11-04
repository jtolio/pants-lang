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

  IR conversion
"""

__author__ = "JT Olds"
__author_email__ = "hello@jtolds.com"
__all__ = ["convert"]

import itertools
import types as ir
import ast.types as ast
from common.errors import ConversionError
from common.errors import assert_source

class ConversionGlobals(object):
  def __init__(self):
    self.varcount = 0
  def gensym(self, line, col):
    self.varcount += 1
    return ir.Identifier("ir_%x" % self.varcount, False, line, col)

class Converter(object):

  __slots__ = ["globals", "ir"]
  LOOKUP_FIELD = "~index"
  UPDATE_FIELD = "~update"
  ARRAY_CONSTRUCTOR = "Array"
  DICT_CONSTRUCTOR = "Dictionary"

  def __init__(self, globals_):
    self.globals = globals_
    self.ir = []

  def pop_target(self, targets, line, col):
    if targets: return targets.pop()
    return self.globals.gensym(line, col)

  def sync_targets(self, target, other_targets):
    value = ir.Variable(target, target.line, target.col)
    return self.set_targets(value, other_targets)

  def set_targets(self, value, targets):
    for target in targets:
      self.ir.append(ir.Assignment(target, value, True, target.line,
          target.col))
    return value

  def convert(self, expressions_obj):
    self.ir = []
    lastval = ir.null_val(expressions_obj.line, expressions_obj.col)
    for expression in expressions_obj.expressions:
      lastval = self.convert_expression(expression)
    return self.ir, lastval

  def convert_expression(self, exp, targets=[]):
    if isinstance(exp, ast.Application):
      return self.convert_application(exp, targets)
    assert isinstance(exp, ast.Assignment)
    return self.convert_assignment(exp, targets)

  def convert_application(self, app, targets=[]):
    assert len(app.terms) >= 1
    if len(app.terms) > 1:
      target = self.pop_target(targets, app.line, app.col)
      opencall_idx = 0
      found = False
      for i, term in enumerate(app.terms):
        if term.modifiers and isinstance(term.modifiers[-1], ast.OpenCall):
          if found:
            assert_source(ConversionError,
                "application has more than one open calls", term.line, term.col)
          opencall_idx = i
          term.modifiers.pop()
          found = True
      left_args = [ir.PositionalOutArgument(self.convert_term(app.terms[i]),
                                          app.terms[i].line, app.terms[i].col)
                   for i in xrange(opencall_idx)]
      call = self.convert_term(app.terms[opencall_idx])
      right_args = [ir.PositionalOutArgument(self.convert_term(app.terms[i]),
                                          app.terms[i].line, app.terms[i].col)
                    for i in xrange(opencall_idx + 1, len(app.terms))]
      self.ir.append(ir.ReturnValue(target, call, left_args, right_args,
          app.line, app.col))
      return self.sync_targets(target, targets)
    return self.convert_term(app.terms[0], targets)

  def convert_variable_assignment(self, assignment, targets=[]):
    new_target = ir.Identifier(assignment.assignee.variable.identifier,
        True, assignment.line, assignment.col)
    if not assignment.mutation:
      if not assignment.expression.references(
          assignment.assignee.variable.identifier):
        return self.convert_expression(assignment.expression,
            targets + [new_target])
      self.ir.append(ir.Assignment(new_target, ir.null_val(assignment.line,
          assignment.col), True, assignment.line, assignment.col))
    real_val = self.convert_expression(assignment.expression, targets)
    self.ir.append(ir.Assignment(new_target, real_val, False,
        assignment.line, assignment.col))
    return real_val

  def convert_index_assignment(self, assignment, targets=[]):
    rhs = self.convert_expression(assignment.expression, targets)
    assignment.assignee.term.modifiers.append(ast.Field(Converter.UPDATE_FIELD,
        assignment.assignee.index.line, assignment.assignee.index.col))
    call = self.convert_term(assignment.assignee.term)
    loc = self.convert_subexpression(ast.Subexpression(
        assignment.assignee.index.expressions, assignment.assignee.index.line,
        assignment.assignee.index.col))
    self.ir.append(ir.ReturnValue(self.globals.gensym(assignment.line,
        assignment.col), call, [], [ir.PositionalOutArgument(loc,
        assignment.assignee.index.line, assignment.assignee.index.col),
        ir.PositionalOutArgument(rhs, assignment.expression.line,
        assignment.expression.col)], assignment.line, assignment.col))
    return rhs

  def convert_field_assignment(self, assignment, targets=[]):
    rhs = self.convert_expression(assignment.expression, targets)
    term_val = self.convert_term(assignment.assignee.term)
    self.ir.append(ir.ObjectMutation(term_val, ir.Identifier(
        assignment.assignee.field.identifier, True, assignment.assignee.line,
        assignment.assignee.col), rhs, assignment.line, assignment.col))
    return rhs

  def convert_assignment(self, assignment, targets=[]):
    if isinstance(assignment.assignee, ast.VariableAssignee):
      return self.convert_variable_assignment(assignment, targets)
    if isinstance(assignment.assignee, ast.FieldAssignee):
      return self.convert_field_assignment(assignment, targets)
    assert isinstance(assignment.assignee, ast.IndexAssignee)
    return self.convert_index_assignment(assignment, targets)

  def convert_term(self, term, targets=[]):
    if not term.modifiers: return self.convert_value(term.value, targets)
    value = self.convert_value(term.value)
    modifiers = list(reversed(term.modifiers))
    while modifiers:
      modifier = modifiers.pop()
      if not modifiers:
        sub_targets = targets
      else:
        sub_targets = []
      if isinstance(modifier, ast.OpenCall):
        value = self.convert_closed_call(value, ast.ClosedCall([], [],
            modifier.line, modifier.col), sub_targets)
      elif isinstance(modifier, ast.Index):
        value = self.convert_index(value, modifier, sub_targets)
      elif isinstance(modifier, ast.Field):
        value = self.convert_field(value, modifier, sub_targets)
      else:
        assert isinstance(modifier, ast.ClosedCall)
        value = self.convert_closed_call(value, modifier, sub_targets)
    return value

  def convert_closed_call(self, value, modifier, targets=[]):
    target = self.pop_target(targets, modifier.line, modifier.col)
    left_args = [self.translate_out_arg(arg) for arg in modifier.left_args]
    right_args = [self.translate_out_arg(arg) for arg in modifier.right_args]
    self.ir.append(ir.ReturnValue(target, value, left_args, right_args,
        modifier.line, modifier.col))
    return self.sync_targets(target, targets)

  def convert_index(self, value, modifier, targets=[]):
    target = self.pop_target(targets, modifier.line, modifier.col)
    call = self.convert_field(value, ast.Field(Converter.LOOKUP_FIELD,
        modifier.line, modifier.col))
    loc = self.convert_subexpression(ast.Subexpression(modifier.expressions,
        modifier.line, modifier.col))
    self.ir.append(ir.ReturnValue(target, call, [], [ir.PositionalOutArgument(
        loc, modifier.line, modifier.col)], modifier.line, modifier.col))
    return self.sync_targets(target, targets)

  def convert_field(self, object_, modifier, targets=[]):
    target = self.pop_target(targets, modifier.line, modifier.col)
    self.ir.append(ir.Assignment(target, ir.Field(object_, ir.Identifier(
        modifier.identifier, True, modifier.line, modifier.col), modifier.line,
        modifier.col), True, modifier.line, modifier.col))
    return self.sync_targets(target, targets)

  def translate_out_arg(self, arg):
    if isinstance(arg, ast.PositionalOutArgument):
      return ir.PositionalOutArgument(self.convert_application(arg.value),
          arg.line, arg.col)
    if isinstance(arg, ast.NamedOutArgument):
      return ir.NamedOutArgument(ir.Identifier(arg.name, True, arg.line,
          arg.col), self.convert_application(arg.value), arg.line, arg.col)
    obj = self.convert_subexpression(ast.Subexpression(arg.expressions,
        arg.line, arg.col))
    if isinstance(arg, ast.SplatOutArgument):
      return ir.SplatOutArgument(obj, arg.line, arg.col)
    assert isinstance(arg, ast.KeywordOutArgument)
    return ir.KeywordOutArgument(obj, arg.line, arg.col)

  def convert_value(self, value, targets=[]):
    if isinstance(value, ast.Variable):
      return self.set_targets(ir.Variable(ir.Identifier(value.identifier, True,
          value.line, value.col), value.line, value.col), targets)
    if isinstance(value, ast.Function):
      return self.convert_function(value, targets)
    if isinstance(value, ast.Subexpression):
      return self.convert_subexpression(value, targets)
    if isinstance(value, ast.Integer): return self.set_targets(value, targets)
    if isinstance(value, ast.Float): return self.set_targets(value, targets)
    if isinstance(value, ast.String):
      target = self.pop_target(targets, value.line, value.col)
      self.ir.append(ir.Assignment(target, value, True, value.line, value.col))
      return self.sync_targets(target, targets)
    if isinstance(value, ast.Array): return self.convert_array(value, targets)
    assert isinstance(value, ast.Dict)
    return self.convert_dict(value, targets)

  def convert_array(self, array, targets=[]):
    target = self.pop_target(targets, array.line, array.col)
    values = [ir.PositionalOutArgument(self.convert_application(app), app.line,
        app.col) for app in array.applications]
    self.ir.append(ir.ReturnValue(target, ir.Variable(ir.Identifier(
        Converter.ARRAY_CONSTRUCTOR, True, array.line, array.col), array.line,
        array.col), [], values, array.line, array.col))
    return self.sync_targets(target, targets)

  def convert_dict(self, dict_, targets=[]):
    target = self.pop_target(targets, dict_.line, dict_.col)
    self.ir.append(ir.ReturnValue(target, ir.Variable(ir.Identifier(
        Converter.DICT_CONSTRUCTOR, True, dict_.line, dict_.col), dict_.line,
        dict_.col), [], [], dict_.line, dict_.col))
    new_dict = self.sync_targets(target, targets)
    if dict_.definitions:
      update_field = ir.Variable(self.globals.gensym(dict_.line, dict_.col),
          dict_.line, dict_.col)
      self.ir.append(ir.Assignment(update_field.identifier, ir.Field(new_dict,
          ir.Identifier(Converter.UPDATE_FIELD, True, dict_.line, dict_.col),
          dict_.line, dict_.col), True, dict_.line, dict_.col))
      result_target = self.globals.gensym(dict_.line, dict_.col)
      for definition in dict_.definitions:
        self.ir.append(ir.ReturnValue(result_target, update_field, [],
            [ir.PositionalOutArgument(self.convert_application(definition.key),
            definition.key.line, definition.key.col),
            ir.PositionalOutArgument(self.convert_application(definition.value),
            definition.value.line, definition.value.col)], definition.line,
            definition.col))
    return new_dict

  def convert_subexpression(self, subexp, targets=[]):
    assert subexp.expressions
    if not subexp.binds_anything():
      lastval = ir.null_val(subexp.line, subexp.col)
      expressions = list(reversed(subexp.expressions))
      while expressions:
        expression = expressions.pop()
        if expressions: sub_targets = []
        else: sub_targets = targets
        lastval = self.convert_expression(expression, sub_targets)
      return lastval
    func = self.globals.gensym(subexp.line, subexp.col)
    expressions, lastval = Converter(self.globals).convert(subexp)
    self.ir.append(ir.Assignment(func, ir.Function(expressions, lastval, [], [],
        False, subexp.line, subexp.col), True, subexp.line, subexp.col))
    target = self.pop_target(targets, subexp.line, subexp.col)
    self.ir.append(ir.ReturnValue(target, ir.Variable(func, subexp.line,
        subexp.col), [], [], subexp.line, subexp.col))
    return self.sync_targets(target, targets)

  def convert_function(self, func, targets=[]):
    target = self.pop_target(targets, func.line, func.col)
    expressions, lastval = Converter(self.globals).convert(func)
    left_args = [self.translate_in_arg(arg) for arg in func.left_args]
    right_args = [self.translate_in_arg(arg) for arg in func.right_args]
    self.ir.append(ir.Assignment(target, ir.Function(expressions, lastval,
        left_args, right_args, func.line, func.col), True, func.line,
        func.col))
    return self.sync_targets(target, targets)

  def translate_in_arg(self, arg):
    if isinstance(arg, ast.RequiredInArgument):
      return ir.RequiredInArgument(ir.Identifier(arg.name, True, arg.line,
          arg.col), arg.line, arg.col)
    if isinstance(arg, ast.DefaultInArgument):
      return ir.DefaultInArgument(ir.Identifier(arg.name, True, arg.line,
          arg.col), self.convert_application(arg.value), arg.line, arg.col)
    if isinstance(arg, ast.SplatInArgument):
      return ir.SplatInArgument(ir.Identifier(arg.identifier, True,
          arg.line, arg.col), arg.line, arg.col)
    assert isinstance(arg, ast.KeywordInArgument)
    return ir.KeywordInArgument(ir.Identifier(arg.identifier, True,
        arg.line, arg.col), arg.line, arg.col)

def convert(ast_root):
  if not isinstance(ast_root, ast.Program):
    raise ConversionError, "unknown ast type"
  converter = Converter(ConversionGlobals())
  exps, lastval = converter.convert(ast_root)
  return ir.Program(exps, lastval, ast_root.line, ast_root.col)
