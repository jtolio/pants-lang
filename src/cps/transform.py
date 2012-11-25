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

  CPS transformation
"""

__author__ = "JT Olds"
__author_email__ = "hello@jtolds.com"
__all__ = ["transform"]

import itertools
import types as cps
import ir.types as ir
from common.errors import TransformationError
from common.errors import assert_source

class Transformer(object):
  def __init__(self):
    self.varcount = 0

  def gensym(self, line, col):
    self.varcount += 1
    return cps.Identifier("cps_%x" % self.varcount, False, line, col)

  def transform_value(self, value):
    if not isinstance(value, ir.Function): return value
    exp = self.transform(value)
    return cps.Callable(exp, value.left_args, value.right_args, True,
        value.line, value.col)

  def transform(self, node, lastval=None):
    if lastval is None: lastval = node.lastval
    expressions = node.expressions
    if (expressions and isinstance(expressions[-1], ir.ReturnValue)
        and isinstance(lastval, ir.Variable)
        and expressions[-1].assignee == lastval.identifier):
      expressions, ir_exp = expressions[:-1], expressions[-1]
      exp = cps.Call(ir_exp.call, ir_exp.left_args, ir_exp.right_args,
          cps.Variable(cps.Identifier("cont", False, ir_exp.line, ir_exp.col),
          ir_exp.line, ir_exp.col), ir_exp.line, ir_exp.col)
    else:
      exp = cps.Call(cps.Variable(cps.Identifier("cont", False, node.line,
          node.col), node.line, node.col), [], [cps.PositionalOutArgument(
          lastval, lastval.line, lastval.col)], None, node.line, node.col)
    for ir_exp in reversed(expressions):
      exp = self.transform_expression(ir_exp, exp)
    return exp

  def transform_expression(self, ir_exp, next_cps_exp):
    if isinstance(ir_exp, ir.Assignment):
      return self.transform_assignment(ir_exp, next_cps_exp)
    if isinstance(ir_exp, ir.ObjectMutation):
      return self.transform_object_mutation(ir_exp, next_cps_exp)
    assert isinstance(ir_exp, ir.ReturnValue)
    return self.transform_return_value(ir_exp, next_cps_exp)

  def transform_assignment(self, ir_exp, next_cps_exp):
    return cps.Assignment(ir_exp.assignee, self.transform_value(ir_exp.value),
        ir_exp.local, next_cps_exp, ir_exp.line, ir_exp.col)

  def transform_object_mutation(self, ir_exp, next_cps_exp):
    return cps.ObjectMutation(ir_exp.object, ir_exp.field,
        self.transform_value(ir_exp.value), next_cps_exp, ir_exp.line,
        ir_exp.col)

  def transform_return_value(self, ir_exp, next_cps_exp):
    continuation = cps.Callable(next_cps_exp, [], [cps.RequiredInArgument(
        ir_exp.assignee, ir_exp.line, ir_exp.col)], False, ir_exp.line,
        ir_exp.col)
    continuation_sym = self.gensym(ir_exp.line, ir_exp.col)
    call = cps.Call(ir_exp.call, ir_exp.left_args, ir_exp.right_args,
        cps.Variable(continuation_sym, ir_exp.line, ir_exp.col), ir_exp.line,
        ir_exp.col)
    return cps.Assignment(continuation_sym, continuation, True, call,
        ir_exp.line, ir_exp.col)

def transform(ir_root, lastval=None, add_predefines=True):
  if add_predefines:
    predefs = [ir.Assignment(ir.Identifier("shift", True, 0, 0),
                             ir.Variable(ir.Identifier("shift", False, 0, 0),
                                         0, 0),
                             True, 0, 0),
               ir.Assignment(ir.Identifier("reset", True, 0, 0),
                             ir.Variable(ir.Identifier("reset", False, 0, 0),
                                         0, 0),
                             True, 0, 0)]
    ir_root = ir.Program(predefs + ir_root.expressions, ir_root.lastval,
                         ir_root.line, ir_root.col)
  return Transformer().transform(ir_root, lastval)
