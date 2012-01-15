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

  IR tests
"""

__author__ = "JT Olds"
__author_email__ = "hello@jtolds.com"

from unittest import TestCase
from ast.parse import parse
from ir.convert import convert

class ConversionTests(TestCase):
  def convert(self, code):
    return convert(parse(code)).expressions

  def testSimple(self):
    self.assertEquals(repr(self.convert("x = { 3 }\nx.\n")), "["
        "Assignment("
          "Identifier('x', True, 1, 1), "
          "Function([], Integer(3, 1, 7), [], [], True, 1, 5), "
          "True, 1, 5), "
        "ReturnValue("
          "Identifier('ir_1', False, 2, 2), "
          "Variable(Identifier('x', True, 2, 1), 2, 1), "
          "[], [], 2, 2)"
      "]")
    self.assertEquals(repr(self.convert("x = { 3 }\nx()\n")), "["
        "Assignment("
          "Identifier('x', True, 1, 1), "
          "Function([], Integer(3, 1, 7), [], [], True, 1, 5), "
          "True, 1, 5), "
        "ReturnValue("
          "Identifier('ir_1', False, 2, 2), "
          "Variable(Identifier('x', True, 2, 1), 2, 1), "
          "[], [], 2, 2)"
      "]")
    self.assertEquals(repr(self.convert("x = { 3 }\nx()\n")), "["
        "Assignment("
          "Identifier('x', True, 1, 1), "
          "Function([], Integer(3, 1, 7), [], [], True, 1, 5), "
          "True, 1, 5), "
        "ReturnValue("
          "Identifier('ir_1', False, 2, 2), "
          "Variable(Identifier('x', True, 2, 1), 2, 1), "
          "[], [], 2, 2)"
      "]")
    self.assertEquals(repr(self.convert("x = { 3 }\nx.")), "["
        "Assignment("
          "Identifier('x', True, 1, 1), "
          "Function([], Integer(3, 1, 7), [], [], True, 1, 5), "
          "True, 1, 5), "
        "ReturnValue("
          "Identifier('ir_1', False, 2, 2), "
          "Variable(Identifier('x', True, 2, 1), 2, 1), "
        "[], [], 2, 2)"
      "]")
    self.assertEquals(repr(self.convert("x.x := 3\n")), "["
        "ObjectMutation("
          "Variable(Identifier('x', True, 1, 1), 1, 1), "
          "Identifier('x', True, 1, 1), "
          "Integer(3, 1, 8), 1, 1)"
      "]")
    self.assertEquals(repr(self.convert("{||}.")), "["
        "Assignment("
          "Identifier('ir_1', False, 1, 1), "
          "Function("
            "[], "
            "Variable(Identifier('null', False, 1, 1), 1, 1), "
            "[], [], True, 1, 1), "
          "True, 1, 1), "
        "ReturnValue("
          "Identifier('ir_2', False, 1, 5), "
          "Variable(Identifier('ir_1', False, 1, 1), 1, 1), "
          "[], [], 1, 5)"
      "]")
