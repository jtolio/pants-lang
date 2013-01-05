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
"""

__author__ = "JT Olds"
__author_email__ = "hello@jtolds.com"
__all__ = ["cleanup"]

import sys
import types as cps


def cleanup(node):
  if isinstance(node, cps.Call): return node
  if isinstance(node, cps.ObjectMutation): return node
  assert isinstance(node, cps.Assignment)
  if isinstance(node.value, cps.Callable):
    node.value.expression = cleanup(node.value.expression)
  node.next_expression = cleanup(node.next_expression)
  if not node.local or node.next_expression.references(node.assignee):
    return node

  print >>sys.stderr, "unnecessary definition: %s, line %d col %d" % (
      node.assignee.format(), node.line, node.col)
  return node.next_expression
