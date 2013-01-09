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

  Single pass delimited continuation transform:

  T(v, ec, mc)         => (ec v mc)
  T(lam, ec, mc)       => (ec M(lam) mc)
  T(f e, ec, mc)       => T(e, {|ve vm| T(f, {|vf vn| (vf ve ec vn)}, vm)}, mc)
  T(f e, ec, mc)       => T(e, {|ve vm| (f ve ec vm)}, mc)
  T(f e, ec, mc)       => (f e ec mc)

  M({|...| e})         => {|... vc vm| T(e, vc, vm)}

  reset = {|lam ec mc| (lam {|x vm| vm x} {|vd| (ec vd mc)})}
  shift = {|lam ec mc| (lam {|x vc vm| ec x {|vd| vc vd vm}} {|x vm| vm x} mc)}
"""

__author__ = "JT Olds"
__author_email__ = "hello@jtolds.com"
__all__ = ["transform"]

import itertools
import types as cps
import ir.types as ir
from common.errors import TransformationError
from common.errors import assert_source

IDENTITY_CONT = cps.Identifier("identity_cont", False, 0, 0)
HALT_CONT = cps.Identifier("halt_cont", False, 0, 0)
RESET = cps.Identifier("reset", True, 0, 0)
SHIFT = cps.Identifier("shift", True, 0, 0)


class Transformer(object):
  def __init__(self):
    self.varcount = 0

  def gensym(self, line, col):
    self.varcount += 1
    return cps.Identifier("cps_%x" % self.varcount, False, line, col)

  def transform_value(self, value):
    if not isinstance(value, ir.Function): return value
    exp, comp_cont, delim_cont = self.transform(value)
    return cps.Callable(exp, value.left_args, value.right_args, comp_cont,
                        delim_cont, value.line, value.col)

  def transform(self, node):
    comp_cont = self.gensym(node.line, node.col)
    delim_cont = self.gensym(node.line, node.col)
    lastval = node.lastval
    expressions = node.expressions
    if (expressions and isinstance(expressions[-1], ir.ReturnValue)
        and isinstance(lastval, ir.Variable)
        and expressions[-1].assignee == lastval.identifier):
      expressions, ir_exp = expressions[:-1], expressions[-1]
      exp = cps.Call(ir_exp.call, ir_exp.left_args, ir_exp.right_args,
          cps.Variable(comp_cont, comp_cont.line, comp_cont.col),
          cps.Variable(delim_cont, delim_cont.line, delim_cont.col),
          ir_exp.line, ir_exp.col)
    else:
      exp = cps.Call(cps.Variable(comp_cont, comp_cont.line, comp_cont.col),
          [], [cps.PositionalOutArgument(lastval, lastval.line, lastval.col)],
          None, cps.Variable(delim_cont, delim_cont.line, delim_cont.col),
          node.line, node.col)
    for ir_exp in reversed(expressions):
      exp, comp_cont, delim_cont = self.transform_expression(
          ir_exp, exp, comp_cont, delim_cont)

    # this provides access to the undelimited function-specific continuation
    cont = cps.Identifier("cont", True, exp.line, exp.col)
    if exp.references(cont):
        arg = self.gensym(exp.line, exp.col)
        cont_call = cps.Call(
                cps.Variable(comp_cont, comp_cont.line, comp_cont.col), [],
                [cps.PositionalOutArgument(
                        cps.Variable(arg, arg.line, arg.col),
                        arg.line, arg.col)],
                cps.Variable(comp_cont, comp_cont.line, comp_cont.col),
                cps.Variable(delim_cont, delim_cont.line, delim_cont.col),
                exp.line, exp.col)
        cont_callable = cps.Callable(
                cont_call, [],
                [cps.RequiredInArgument(arg, arg.line, arg.col)],
                None, None, exp.line, exp.col)
        exp = cps.Assignment(cont, cont_callable, True, exp, exp.line, exp.col)

    return exp, comp_cont, delim_cont

  def transform_expression(self, ir_exp, next_cps_exp, comp_cont, delim_cont):
    if isinstance(ir_exp, ir.Assignment):
      return (self.transform_assignment(ir_exp, next_cps_exp),
              comp_cont, delim_cont)
    if isinstance(ir_exp, ir.ObjectMutation):
      return (self.transform_object_mutation(ir_exp, next_cps_exp),
              comp_cont, delim_cont)
    assert isinstance(ir_exp, ir.ReturnValue)
    return self.transform_return_value(ir_exp, next_cps_exp, comp_cont,
                                       delim_cont)

  def transform_assignment(self, ir_exp, next_cps_exp):
    return cps.Assignment(ir_exp.assignee, self.transform_value(ir_exp.value),
        ir_exp.local, next_cps_exp, ir_exp.line, ir_exp.col)

  def transform_object_mutation(self, ir_exp, next_cps_exp):
    return cps.ObjectMutation(ir_exp.object, ir_exp.field,
        self.transform_value(ir_exp.value), next_cps_exp, ir_exp.line,
        ir_exp.col)

  def transform_return_value(self, ir_exp, next_cps_exp, comp_cont,
                             delim_cont):
    child_comp_cont = cps.Callable(next_cps_exp, [], [cps.RequiredInArgument(
        ir_exp.assignee, ir_exp.line, ir_exp.col)], None, delim_cont,
        ir_exp.line, ir_exp.col)
    child_comp_cont_sym = self.gensym(ir_exp.line, ir_exp.col)
    new_delim_cont = self.gensym(ir_exp.line, ir_exp.col)
    call = cps.Call(ir_exp.call, ir_exp.left_args, ir_exp.right_args,
        cps.Variable(child_comp_cont_sym, ir_exp.line, ir_exp.col),
        cps.Variable(new_delim_cont, ir_exp.line, ir_exp.col),
        ir_exp.line, ir_exp.col)
    return cps.Assignment(child_comp_cont_sym, child_comp_cont, True, call,
        ir_exp.line, ir_exp.col), comp_cont, new_delim_cont

def transform(ir_root):
  trans = Transformer()
  exp, comp_cont, delim_cont = trans.transform(ir_root)
  predef = CPSPredefines(exp, trans.gensym)

  predef.define_reset()
  predef.define_shift()
  predef.define_delim_cont(delim_cont)
  predef.define_comp_cont(comp_cont)
  predef.define_identity_cont()

  return predef.result()

class CPSPredefines(object):
  def __init__(self, initial_exp, gensym):
    self.exp = initial_exp
    self.gensym = gensym

  def result(self):
    return self.exp

  def define_identity_cont(self):
    val = self.gensym(0, 0)
    delim_cont = self.gensym(0, 0)
    ident_call = cps.Call(
        cps.Variable(delim_cont, delim_cont.line, delim_cont.col),
        [],
        [cps.PositionalOutArgument(
            cps.Variable(val, val.line, val.col), val.line, val.col)],
        None, None, val.line, val.col)
    ident_cont = cps.Callable(ident_call, [],
        [cps.RequiredInArgument(val, val.line, val.col)],
        None, delim_cont, val.line, val.col)
    self.exp = cps.Assignment(IDENTITY_CONT,
        ident_cont, True, self.exp, 0, 0)

  def define_comp_cont(self, comp_cont):
    self.exp = cps.Assignment(
        comp_cont,
        cps.Variable(IDENTITY_CONT, IDENTITY_CONT.line, IDENTITY_CONT.col),
        True, self.exp, 0, 0)

  def define_delim_cont(self, delim_cont):
    self.exp = cps.Assignment(
        delim_cont,
        cps.Variable(HALT_CONT, HALT_CONT.line, HALT_CONT.col),
        True, self.exp, 0, 0)

  def define_reset(self):
    # {|lam ec mc|
    #     reset_delim_sym = {|vd| (ec vd mc)}
    #     (lam ident_cont reset_delim_sym)}
    if not self.exp.references(RESET):
      return
    lam = self.gensym(0, 0)
    ec = self.gensym(0, 0)
    mc = self.gensym(0, 0)
    vd = self.gensym(0, 0)
    reset_delim_sym = self.gensym(0, 0)
    reset_delim_body = cps.Call(
        cps.Variable(ec, ec.line, ec.col), [],
        [cps.PositionalOutArgument(
            cps.Variable(vd, vd.line, vd.col), vd.line, vd.col)],
        None, cps.Variable(mc, mc.line, mc.col), 0, 0)
    reset_delim_cont = cps.Callable(reset_delim_body, [],
        [cps.RequiredInArgument(vd, vd.line, vd.col)], None, None, 0, 0)
    reset_body = cps.Call(
        cps.Variable(lam, lam.line, lam.col),
        [], [],
        cps.Variable(IDENTITY_CONT, IDENTITY_CONT.line, IDENTITY_CONT.col),
        cps.Variable(reset_delim_sym, reset_delim_sym.line,
            reset_delim_sym.col), 0, 0)
    reset_body = cps.Assignment(reset_delim_sym, reset_delim_cont, True,
        reset_body, 0, 0)
    reset_callable = cps.Callable(reset_body, [],
        [cps.RequiredInArgument(lam, lam.line, lam.col)], ec, mc, 0, 0)
    self.exp = cps.Assignment(RESET, reset_callable, True, self.exp, 0, 0)

  def define_shift(self):
    # {|lam ec mc|
    #     user_sym = {|x vc vm|
    #                     reset_delim_sym = {|vd| vc vd vm}
    #                     (ec x reset_delim_sym)}
    #     (lam user_sym identity_cont mc)}
    if not self.exp.references(SHIFT):
      return
    lam = self.gensym(0, 0)
    ec = self.gensym(0, 0)
    mc = self.gensym(0, 0)
    x = self.gensym(0, 0)
    vc = self.gensym(0, 0)
    vm = self.gensym(0, 0)
    vd = self.gensym(0, 0)
    reset_delim_sym = self.gensym(0, 0)
    user_sym = self.gensym(0, 0)
    reset_delim_body = cps.Call(cps.Variable(vc, vc.line, vc.col), [],
        [cps.PositionalOutArgument(
            cps.Variable(vd, vd.line, vd.col), vd.line, vd.col)], None,
        cps.Variable(vm, vm.line, vm.col), 0, 0)
    reset_delim_cont = cps.Callable(reset_delim_body, [],
        [cps.RequiredInArgument(vd, vd.line, vd.col)], None, None, 0, 0)
    user_body = cps.Call(cps.Variable(ec, ec.line, ec.col), [],
        [cps.PositionalOutArgument(
            cps.Variable(x, x.line, x.col), x.line, x.col)], None,
        cps.Variable(
            reset_delim_sym, reset_delim_sym.line, reset_delim_sym.col), 0, 0)
    user_body = cps.Assignment(reset_delim_sym, reset_delim_cont, True,
        user_body, 0, 0)
    user_cont = cps.Callable(user_body, [],
        [cps.RequiredInArgument(x, x.line, x.col)], vc, vm, 0, 0)
    shift_body = cps.Call(cps.Variable(lam, lam.line, lam.col), [],
        [cps.PositionalOutArgument(
            cps.Variable(user_sym, user_sym.line, user_sym.col),
            user_sym.line, user_sym.col)],
        cps.Variable(IDENTITY_CONT, IDENTITY_CONT.line, IDENTITY_CONT.col),
        cps.Variable(mc, mc.line, mc.col), 0, 0)
    shift_body = cps.Assignment(user_sym, user_cont, True, shift_body, 0, 0)
    shift_callable = cps.Callable(shift_body, [],
        [cps.RequiredInArgument(lam, lam.line, lam.col)], ec, mc, 0, 0)
    self.exp = cps.Assignment(SHIFT, shift_callable, True, self.exp, 0, 0)
