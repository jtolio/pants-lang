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

  AST tests
"""

__author__ = "JT Olds"
__author_email__ = "hello@jtolds.com"

from unittest import TestCase
from ast.parse import parse
from common.errors import ParserError

class ParsingTests(TestCase):

  def testSimpleParse(self):
    self.assertEqual(repr(parse("hey. there")),
        "Program(["
          "Application(["
            "Term(Variable('hey', 1, 1), [OpenCall(1, 4)], 1, 1), "
            "Term(Variable('there', 1, 6), [], 1, 6)"
          "], 1, 1)"
        "], 1, 1)")

  def testArrayVsIndex(self):
    self.assertEqual(repr(parse("call. thing [value]")),
        "Program(["
          "Application(["
            "Term(Variable('call', 1, 1), [OpenCall(1, 5)], 1, 1), "
            "Term(Variable('thing', 1, 7), [], 1, 7), "
            "Term(Array(["
              "Application(["
                "Term(Variable('value', 1, 14), [], 1, 14)"
              "], 1, 14)"
            "], 1, 13), [], 1, 13)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("call. thing[key]")),
        "Program(["
          "Application(["
            "Term(Variable('call', 1, 1), [OpenCall(1, 5)], 1, 1), "
            "Term(Variable('thing', 1, 7), [Index(["
              "Application(["
                "Term(Variable('key', 1, 13), [], 1, 13)"
              "], 1, 13)"
            "], 1, 12)], 1, 7)"
          "], 1, 1)"
        "], 1, 1)")

  def testCallVsField(self):
    self.assertEqual(repr(parse("call. thing1. notafield")),
        "Program(["
          "Application(["
            "Term(Variable('call', 1, 1), [OpenCall(1, 5)], 1, 1), "
            "Term(Variable('thing1', 1, 7), [OpenCall(1, 13)], 1, 7), "
            "Term(Variable('notafield', 1, 15), [], 1, 15)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("call. thing1.afield")),
        "Program(["
          "Application(["
            "Term(Variable('call', 1, 1), [OpenCall(1, 5)], 1, 1), "
            "Term(Variable('thing1', 1, 7), [Field('afield', 1, 13)], 1, 7)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("call. function..afield")),
        "Program(["
          "Application(["
            "Term(Variable('call', 1, 1), [OpenCall(1, 5)], 1, 1), "
            "Term(Variable('function', 1, 7), ["
              "OpenCall(1, 15), "
              "Field('afield', 1, 16)"
            "], 1, 7)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("call. function..afuncfield.")),
        "Program(["
          "Application(["
            "Term(Variable('call', 1, 1), [OpenCall(1, 5)], 1, 1), "
            "Term(Variable('function', 1, 7), ["
              "OpenCall(1, 15), "
              "Field('afuncfield', 1, 16), "
              "OpenCall(1, 27)"
            "], 1, 7)"
          "], 1, 1)"
        "], 1, 1)")

  def testClosedCall(self):
    self.assertEqual(repr(parse("f(arg)")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), ["
              "ClosedCall([], ["
                "PositionalOutArgument(Application(["
                  "Term(Variable('arg', 1, 3), [], 1, 3)"
                "], 1, 3), 1, 3)"
              "], 1, 2)"
            "], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("f(arg1, arg2)")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), ["
              "ClosedCall([], ["
                "PositionalOutArgument(Application(["
                  "Term(Variable('arg1', 1, 3), [], 1, 3)"
                "], 1, 3), 1, 3), "
                "PositionalOutArgument(Application(["
                  "Term(Variable('arg2', 1, 9), [], 1, 9)"
                "], 1, 9), 1, 9)"
              "], 1, 2)"
            "], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("f (arg)")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), [], 1, 1), "
            "Term(Subexpression(["
              "Application(["
                "Term(Variable('arg', 1, 4), [], 1, 4)"
              "], 1, 4)"
            "], 1, 3), [], 1, 3)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("f.(arg)")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), ["
              "OpenCall(1, 2), "
              "ClosedCall([], ["
                "PositionalOutArgument(Application(["
                  "Term(Variable('arg', 1, 4), [], 1, 4)"
                "], 1, 4), 1, 4)"
              "], 1, 3)"
            "], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("f. (arg)")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), [OpenCall(1, 2)], 1, 1), "
            "Term(Subexpression(["
              "Application(["
                "Term(Variable('arg', 1, 5), [], 1, 5)"
              "], 1, 5)"
            "], 1, 4), [], 1, 4)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("f(arg1;arg2)")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), ["
              "ClosedCall(["
                "PositionalOutArgument(Application(["
                  "Term(Variable('arg1', 1, 3), [], 1, 3)"
                "], 1, 3), 1, 3)"
              "], ["
                "PositionalOutArgument(Application(["
                  "Term(Variable('arg2', 1, 8), [], 1, 8)"
                "], 1, 8), 1, 8)"
              "], 1, 2)"
            "], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("f(arg1,arg2;arg3)")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), ["
              "ClosedCall(["
                "PositionalOutArgument(Application(["
                  "Term(Variable('arg1', 1, 3), [], 1, 3)"
                "], 1, 3), 1, 3), "
                "PositionalOutArgument(Application(["
                  "Term(Variable('arg2', 1, 8), [], 1, 8)"
                "], 1, 8), 1, 8)"
              "], ["
                "PositionalOutArgument(Application(["
                  "Term(Variable('arg3', 1, 13), [], 1, 13)"
                "], 1, 13), 1, 13)"
              "], 1, 2)"
            "], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("f(arg1,arg2,;arg3)")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), ["
              "ClosedCall(["
                "PositionalOutArgument(Application(["
                  "Term(Variable('arg1', 1, 3), [], 1, 3)"
                "], 1, 3), 1, 3), "
                "PositionalOutArgument(Application(["
                  "Term(Variable('arg2', 1, 8), [], 1, 8)"
                "], 1, 8), 1, 8)"
              "], ["
                "PositionalOutArgument(Application(["
                  "Term(Variable('arg3', 1, 14), [], 1, 14)"
                "], 1, 14), 1, 14)"
              "], 1, 2)"
            "], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")

  def testSingleLists(self):
    self.assertEqual(repr(parse("[z,]")),
        "Program(["
          "Application(["
            "Term(Array(["
              "Application(["
                "Term(Variable('z', 1, 2), [], 1, 2)"
              "], 1, 2)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{test1:test2,}")),
        "Program(["
          "Application(["
            "Term(Dict(["
              "DictDefinition(Application(["
                "Term(Variable('test1', 1, 2), [], 1, 2)"
              "], 1, 2), Application(["
                "Term(Variable('test2', 1, 8), [], 1, 8)"
              "], 1, 8), 1, 2)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("f(arg1,)")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), ["
              "ClosedCall([], ["
                "PositionalOutArgument(Application(["
                  "Term(Variable('arg1', 1, 3), [], 1, 3)"
                "], 1, 3), 1, 3)"
              "], 1, 2)"
            "], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|a,;| null}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Variable('null', 1, 8), [], 1, 8)"
              "], 1, 8)"
            "], ["
              "RequiredInArgument('a', 1, 3)"
            "], [], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|b,| null}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Variable('null', 1, 7), [], 1, 7)"
              "], 1, 7)"
            "], [], ["
              "RequiredInArgument('b', 1, 3)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")

  def testFunctions(self):
    self.assertEqual(repr(parse("{}")),
        "Program(["
          "Application(["
            "Term(Dict([], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertRaisesRegexp(ParserError,
        "Error at line 1, column 10: unexpected input at close of function",
        parse, "{|| thing:thing}")
    self.assertEqual(repr(parse("{thing1:thing2}")),
        "Program(["
          "Application(["
            "Term(Dict(["
              "DictDefinition(Application(["
                "Term(Variable('thing1', 1, 2), [], 1, 2)"
              "], 1, 2), Application(["
                "Term(Variable('thing2', 1, 9), [], 1, 9)"
              "], 1, 9), 1, 2)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{null}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Variable('null', 1, 2), [], 1, 2)"
              "], 1, 2)"
            "], [], [], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{{thing1:thing2}}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Dict(["
                  "DictDefinition(Application(["
                    "Term(Variable('thing1', 1, 3), [], 1, 3)"
                  "], 1, 3), Application(["
                    "Term(Variable('thing2', 1, 10), [], 1, 10)"
                  "], 1, 10), 1, 3)"
                "], 1, 2), [], 1, 2)"
              "], 1, 2)"
            "], [], [], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|a| print(\"hi\"); null}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Variable('print', 1, 6), ["
                  "ClosedCall([], ["
                    "PositionalOutArgument(Application(["
                      "Term(String(False, 'hi', 1, 12), [], 1, 12)"
                    "], 1, 12), 1, 12)"
                  "], 1, 11)"
                "], 1, 6)"
              "], 1, 6), "
              "Application(["
                "Term(Variable('null', 1, 19), [], 1, 19)"
              "], 1, 19)"
            "], [], ["
              "RequiredInArgument('a', 1, 3)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|a,| null}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Variable('null', 1, 7), [], 1, 7)"
              "], 1, 7)"
            "], [], ["
              "RequiredInArgument('a', 1, 3)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|a,b| 0}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Integer(0, 1, 8), [], 1, 8)"
              "], 1, 8)"
            "], [], ["
              "RequiredInArgument('a', 1, 3), "
              "RequiredInArgument('b', 1, 5)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|a,b,| 0}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Integer(0, 1, 9), [], 1, 9)"
              "], 1, 9)"
            "], [], ["
              "RequiredInArgument('a', 1, 3), "
              "RequiredInArgument('b', 1, 5)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|a,b,c:3| 0}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Integer(0, 1, 12), [], 1, 12)"
              "], 1, 12)"
            "], [], ["
              "RequiredInArgument('a', 1, 3), "
              "RequiredInArgument('b', 1, 5), "
              "DefaultInArgument('c', Application(["
                "Term(Integer(3, 1, 9), [], 1, 9)"
              "], 1, 9), 1, 7)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|a,b,c:3,d:4,:(opt)| 0}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Integer(0, 1, 23), [], 1, 23)"
              "], 1, 23)"
            "], [], ["
              "RequiredInArgument('a', 1, 3), "
              "RequiredInArgument('b', 1, 5), "
              "DefaultInArgument('c', Application(["
                "Term(Integer(3, 1, 9), [], 1, 9)"
              "], 1, 9), 1, 7), "
              "DefaultInArgument('d', Application(["
                "Term(Integer(4, 1, 13), [], 1, 13)"
              "], 1, 13), 1, 11), "
              "SplatInArgument('opt', 1, 15)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertRaisesRegexp(ParserError,
        r"Error at line 1, column 16: unexpected input for argument list",
        parse, "{|a,b,c:3,d:4,q(opt)| 0}")
    self.assertEqual(repr(parse("{|| 0}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Integer(0, 1, 5), [], 1, 5)"
              "], 1, 5)"
            "], [], [], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{||}")),
        "Program(["
          "Application(["
            "Term(Function([], [], [], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|;| 0}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Integer(0, 1, 6), [], 1, 6)"
              "], 1, 6)"
            "], [], [], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|a;| 0}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Integer(0, 1, 7), [], 1, 7)"
              "], 1, 7)"
            "], ["
              "RequiredInArgument('a', 1, 3)"
            "], [], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|a,b;| 0}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Integer(0, 1, 9), [], 1, 9)"
              "], 1, 9)"
            "], ["
              "RequiredInArgument('a', 1, 3), "
              "RequiredInArgument('b', 1, 5)"
            "], [], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|a,b,;| 0}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Integer(0, 1, 10), [], 1, 10)"
              "], 1, 10)"
            "], ["
              "RequiredInArgument('a', 1, 3), "
              "RequiredInArgument('b', 1, 5)"
            "], [], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|:(var),a,b,;| 0}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Integer(0, 1, 17), [], 1, 17)"
              "], 1, 17)"
            "], ["
              "SplatInArgument('var', 1, 3), "
              "RequiredInArgument('a', 1, 10), "
              "RequiredInArgument('b', 1, 12)"
            "], [], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|:(var),a,b,d;e,f,g:5,h:7,:(j)| 0}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Integer(0, 1, 34), [], 1, 34)"
              "], 1, 34)"
            "], ["
              "SplatInArgument('var', 1, 3), "
              "RequiredInArgument('a', 1, 10), "
              "RequiredInArgument('b', 1, 12), "
              "RequiredInArgument('d', 1, 14)"
            "], ["
              "RequiredInArgument('e', 1, 16), "
              "RequiredInArgument('f', 1, 18), "
              "DefaultInArgument('g', Application(["
                "Term(Integer(5, 1, 22), [], 1, 22)"
              "], 1, 22), 1, 20), "
              "DefaultInArgument('h', Application(["
                "Term(Integer(7, 1, 26), [], 1, 26)"
              "], 1, 26), 1, 24), "
              "SplatInArgument('j', 1, 28)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|:(a);:(b)| 0}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Integer(0, 1, 14), [], 1, 14)"
              "], 1, 14)"
            "], ["
              "SplatInArgument('a', 1, 3)"
            "], ["
              "SplatInArgument('b', 1, 8)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("{|;::(b)| 0}")),
        "Program(["
          "Application(["
            "Term(Function(["
              "Application(["
                "Term(Integer(0, 1, 11), [], 1, 11)"
              "], 1, 11)"
            "], [], ["
              "KeywordInArgument('b', 1, 4)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")

  def testListExpansion(self):
    self.assertEqual(repr(parse("f(thing, :(thing))")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), ["
              "ClosedCall([], ["
                "PositionalOutArgument(Application(["
                  "Term(Variable('thing', 1, 3), [], 1, 3)"
                "], 1, 3), 1, 3), "
                "SplatOutArgument(["
                  "Application(["
                    "Term(Variable('thing', 1, 12), [], 1, 12)"
                  "], 1, 12)"
                "], 1, 10)"
              "], 1, 2)"
            "], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertRaisesRegexp(ParserError,
        "Error at line 1, column 3: unexpected input for function call",
        parse, "f(:,(thing),thing)")
    self.assertEqual(repr(parse("f(*,(thing),thing)")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), ["
              "ClosedCall([], ["
                "PositionalOutArgument(Application(["
                  "Term(Variable('*', 1, 3), [], 1, 3)"
                "], 1, 3), 1, 3), "
                "PositionalOutArgument(Application(["
                  "Term(Subexpression(["
                    "Application(["
                      "Term(Variable('thing', 1, 6), [], 1, 6)"
                    "], 1, 6)"
                  "], 1, 5), [], 1, 5)"
                "], 1, 5), 1, 5), "
                "PositionalOutArgument(Application(["
                  "Term(Variable('thing', 1, 13), [], 1, 13)"
                "], 1, 13), 1, 13)"
              "], 1, 2)"
            "], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")

  def testByteString(self):
    self.assertEqual(repr(parse("f. b\"thing\"")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), ["
              "OpenCall(1, 2)"
            "], 1, 1), "
            "Term(String(True, 'thing', 1, 4), [], 1, 4)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("f. b \"thing\"")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), [OpenCall(1, 2)], 1, 1), "
            "Term(Variable('b', 1, 4), [], 1, 4), "
            "Term(String(False, 'thing', 1, 6), [], 1, 6)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("f. b\"\"")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), [OpenCall(1, 2)], 1, 1), "
            "Term(String(True, '', 1, 4), [], 1, 4)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("f. b \"\"")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), [OpenCall(1, 2)], 1, 1), "
            "Term(Variable('b', 1, 4), [], 1, 4), "
            "Term(String(False, '', 1, 6), [], 1, 6)"
          "], 1, 1)"
        "], 1, 1)")

  def testParses(self):
    self.assertRaisesRegexp(ParserError,
        r"Error at line 1, column 4: unexpected input",
        parse, ".x := 3")
    self.assertEqual(repr(parse("x := 1")),
        "Program(["
          "Assignment(True, VariableAssignee(Variable('x', 1, 1), 1, 1), "
            "Application(["
              "Term(Integer(1, 1, 6), [], 1, 6)"
            "], 1, 6), 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("(a b).thing")),
        "Program(["
          "Application(["
            "Term(Subexpression(["
              "Application(["
                "Term(Variable('a', 1, 2), [], 1, 2), "
                "Term(Variable('b', 1, 4), [], 1, 4)"
              "], 1, 2)"
            "], 1, 1), ["
              "Field('thing', 1, 6)"
            "], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("(a b).thing := 3")),
        "Program(["
          "Assignment(True, FieldAssignee(Term(Subexpression(["
            "Application(["
              "Term(Variable('a', 1, 2), [], 1, 2), "
              "Term(Variable('b', 1, 4), [], 1, 4)"
            "], 1, 2)"
          "], 1, 1), [], 1, 1), Field('thing', 1, 6), 1, 1), "
            "Application(["
              "Term(Integer(3, 1, 16), [], 1, 16)"
            "], 1, 16), 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("z.thing := 3; x[4] := 5")),
        "Program(["
          "Assignment("
            "True, "
            "FieldAssignee("
              "Term(Variable('z', 1, 1), [], 1, 1), "
              "Field('thing', 1, 2), 1, 1), "
            "Application(["
              "Term(Integer(3, 1, 12), [], 1, 12)"
            "], 1, 12), 1, 1), "
          "Assignment("
            "True, "
            "IndexAssignee("
              "Term(Variable('x', 1, 15), [], 1, 15), "
              "Index(["
                "Application(["
                  "Term(Integer(4, 1, 17), [], 1, 17)"
                "], 1, 17)"
              "], 1, 16), 1, 15), "
            "Application(["
              "Term(Integer(5, 1, 23), [], 1, 23)"
            "], 1, 23), 1, 15)"
        "], 1, 1)")
    self.assertEqual(repr(parse("x = 1")),
        "Program(["
          "Assignment("
            "False, "
            "VariableAssignee(Variable('x', 1, 1), 1, 1), "
            "Application(["
              "Term(Integer(1, 1, 5), [], 1, 5)"
            "], 1, 5), 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("(a b).thing = 3")),
        "Program(["
          "Assignment("
            "False, "
            "FieldAssignee("
              "Term(Subexpression(["
                "Application(["
                  "Term(Variable('a', 1, 2), [], 1, 2), "
                  "Term(Variable('b', 1, 4), [], 1, 4)"
                "], 1, 2)"
              "], 1, 1), [], 1, 1), "
              "Field('thing', 1, 6), 1, 1), "
            "Application(["
              "Term(Integer(3, 1, 15), [], 1, 15)"
            "], 1, 15), 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("z.thing = 3; x[4] = 5")),
        "Program(["
          "Assignment("
            "False, "
            "FieldAssignee("
              "Term(Variable('z', 1, 1), [], 1, 1), "
              "Field('thing', 1, 2), 1, 1), "
            "Application(["
              "Term(Integer(3, 1, 11), [], 1, 11)"
            "], 1, 11), 1, 1), "
          "Assignment("
            "False, "
            "IndexAssignee("
              "Term(Variable('x', 1, 14), [], 1, 14), "
              "Index(["
                "Application(["
                  "Term(Integer(4, 1, 16), [], 1, 16)"
                "], 1, 16)"
              "], 1, 15), 1, 14), "
            "Application(["
              "Term(Integer(5, 1, 21), [], 1, 21)"
            "], 1, 21), 1, 14)"
        "], 1, 1)")

  def testNewlines(self):
    self.assertEqual(repr(parse("f; f")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), [], 1, 1)"
          "], 1, 1), "
          "Application(["
            "Term(Variable('f', 1, 4), [], 1, 4)"
          "], 1, 4)"
        "], 1, 1)")
    self.assertEqual(repr(parse("f\nf")),
        "Program(["
          "Application(["
            "Term(Variable('f', 1, 1), [], 1, 1)"
          "], 1, 1), "
          "Application(["
            "Term(Variable('f', 2, 1), [], 2, 1)"
          "], 2, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("hey there; how are you; ")),
        "Program(["
          "Application(["
            "Term(Variable('hey', 1, 1), [], 1, 1), "
            "Term(Variable('there', 1, 5), [], 1, 5)"
          "], 1, 1), "
          "Application(["
            "Term(Variable('how', 1, 12), [], 1, 12), "
            "Term(Variable('are', 1, 16), [], 1, 16), "
            "Term(Variable('you', 1, 20), [], 1, 20)"
          "], 1, 12)"
        "], 1, 1)")
    self.assertEqual(repr(parse("hey there\nhow are you\n")),
        "Program(["
          "Application(["
            "Term(Variable('hey', 1, 1), [], 1, 1), "
            "Term(Variable('there', 1, 5), [], 1, 5)"
          "], 1, 1), "
          "Application(["
            "Term(Variable('how', 2, 1), [], 2, 1), "
            "Term(Variable('are', 2, 5), [], 2, 5), "
            "Term(Variable('you', 2, 9), [], 2, 9)"
          "], 2, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("(f\nf)")),
        "Program(["
          "Application(["
            "Term(Subexpression(["
              "Application(["
                "Term(Variable('f', 1, 2), [], 1, 2), "
                "Term(Variable('f', 2, 1), [], 2, 1)"
              "], 1, 2)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("(f;f)")),
        "Program(["
          "Application(["
            "Term(Subexpression(["
              "Application(["
                "Term(Variable('f', 1, 2), [], 1, 2)"
              "], 1, 2), "
              "Application(["
                "Term(Variable('f', 1, 4), [], 1, 4)"
              "], 1, 4)"
            "], 1, 1), [], 1, 1)"
          "], 1, 1)"
        "], 1, 1)")

  def testFloatingPoint(self):
    self.assertEqual(repr(parse("x = 3")),
        "Program(["
          "Assignment("
            "False, "
            "VariableAssignee(Variable('x', 1, 1), 1, 1), "
            "Application(["
              "Term(Integer(3, 1, 5), [], 1, 5)"
            "], 1, 5), 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("x = 3.0")),
        "Program(["
          "Assignment("
            "False, "
            "VariableAssignee(Variable('x', 1, 1), 1, 1), "
            "Application(["
              "Term(Float(3.0, 1, 5), [], 1, 5)"
            "], 1, 5), 1, 1)"
        "], 1, 1)")

  def testEquality(self):
    self.assertEqual(repr(parse("x <. 3")),
        "Program(["
          "Application(["
            "Term(Variable('x', 1, 1), [], 1, 1), "
            "Term(Variable('<', 1, 3), [OpenCall(1, 4)], 1, 3), "
            "Term(Integer(3, 1, 6), [], 1, 6)"
          "], 1, 1)"
        "], 1, 1)")
    self.assertEqual(repr(parse("x ==. 3")),
        "Program(["
          "Application(["
            "Term(Variable('x', 1, 1), [], 1, 1), "
            "Term(Variable('==', 1, 3), [OpenCall(1, 5)], 1, 3), "
            "Term(Integer(3, 1, 7), [], 1, 7)"
          "], 1, 1)"
        "], 1, 1)")

  def testComments(self):
    self.assertEqual(repr(parse("# ||||| this comment shouldn't fail { \n 1 "
        "\n")),
        "Program(["
          "Application(["
            "Term(Integer(1, 2, 2), [], 2, 2)"
          "], 2, 2)"
        "], 1, 1)")
    self.assertEqual(repr(parse("# ||||| this comment shouldn't fail { \n "
        "# ||||| this comment shouldn't fail { \n 1 \n")),
        "Program(["
          "Application(["
            "Term(Integer(1, 3, 2), [], 3, 2)"
          "], 3, 2)"
        "], 1, 1)")
    self.assertEqual(repr(parse("\n   1 # ||||| this comment shouldn't fail { "
        "\n # ||||| this comment shouldn't fail { \n \n")),
        "Program(["
          "Application(["
            "Term(Integer(1, 2, 4), [], 2, 4)"
          "], 2, 4)"
        "], 1, 1)")
    self.assertEqual(repr(parse("\n   1 # ||||| this comment shouldn't fail { "
        "\n 2 # ||||| this comment shouldn't fail { \n \n")),
        "Program(["
          "Application(["
            "Term(Integer(1, 2, 4), [], 2, 4)"
          "], 2, 4), "
          "Application(["
            "Term(Integer(2, 3, 2), [], 3, 2)"
          "], 3, 2)"
        "], 1, 1)")
