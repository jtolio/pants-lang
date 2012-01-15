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

  Parser module
"""

__author__ = "JT Olds"
__author_email__ = "hello@jtolds.com"
__all__ = ["parse"]

import types as ast
from common.errors import ParserError
from common.errors import assert_source

DEBUG_LEVEL = 0

class Parser(object):

  NON_ID_CHARS = set([" ", "\n", "\r", "\t", ";", ",", "(", ")", "[", "]", "{",
      "}", "|", "'", '"', ".", ":", "@", "#"])
  ESCAPES = { '"': '"',
              "n": "\n",
              "t": "\t",
              "r": "\r",
              "\n": "",
              "\\": "\\",
              "'": "'",
              "a": "\a",
              "b": "\b",
              "f": "\f",
              "v": "\v" }
  INTEGER_BASES = { "b": 2,
                    "o": 8,
                    "d": 12,
                    "x": 16 }
  SAFE_DIGITS = { 2: set("01"),
                  8: set("01234567"),
                  10: set("0123456789"),
                  12: set("0123456789abAB"),
                  16: set("0123456789abcdefABCDEF") }

  __slots__ = ["source", "pos", "line", "col", "_term_memoization",
      "current_char"]

  def __init__(self, io):
    if hasattr(io, "read"):
      self.source = io.read()
    else:
      self.source = io
    self.pos = 0
    self.line = 1
    self.col = 1
    self._term_memoization = {}
    self.current_char = self.source[self.pos]

  def advance(self, distance=1):
    for _ in xrange(distance):
      assert not self.eof()
      if self.current_char == "\n":
        self.line += 1
        self.col = 1
      else:
        self.col += 1
      self.pos += 1
      if self.pos >= len(self.source):
        self.current_char = None
      else:
        self.current_char = self.source[self.pos]

  def checkpoint(self):
    return (self.pos, self.col, self.line)

  def restore(self, checkpoint):
    self.pos, self.col, self.line = checkpoint
    if self.pos >= len(self.source):
      self.current_char = None
    else:
      self.current_char = self.source[self.pos]

  def source_ref(self, checkpoint):
    return checkpoint[2], checkpoint[1]

  def assert_source(self, message, line=None, col=None):
    if line is None: line = self.line
    if col is None: col = self.col
    assert_source(ParserError, message, line, col)

  def eof(self, lookahead=0):
    assert lookahead >= 0
    return self.pos + lookahead >= len(self.source)

  def char(self, lookahead=0, required=False):
    if self.pos + lookahead >= len(self.source) or self.pos + lookahead < 0:
      if not required: return None
      assert lookahead >= 0
      self.assert_source("end of input unexpected")
    return self.source[self.pos + lookahead]

  def parse_string_escape(self):
    if self.current_char != "\\": return None
    self.advance()
    if Parser.ESCAPES.has_key(self.char(required=True)):
      return Parser.ESCAPES[self.char]
    elif self.current_char in Parser.SAFE_DIGITS[10]:
      integer, base, log = self.parse_integer()
      assert integer is not None
      if integer >= 256:
        self.assert_source("byte escape sequence represents more than one byte")
      return chr(integer)
    elif self.current_char in ("U", "u", "N"):
      self.assert_source("TODO: unicode escape sequences unimplemented")
    else:
      self.assert_source("unknown escape sequence")

  def parse_string(self):
    checkpoint = self.checkpoint()
    bytestring = False
    if self.current_char == 'b':
      bytestring = True
      self.advance()
    if self.current_char != '"':
      self.restore(checkpoint)
      return None
    opening_quote_count = 1
    self.advance()
    if self.char(required=True) == '"' and self.char(lookahead=1) != '"':
      self.advance()
      return ast.String(bytestring, "", *self.source_ref(checkpoint))
    while self.char(required=True) == '"':
      opening_quote_count += 1
      self.advance()
    value = []
    while True:
      found_ending = True
      for i in xrange(opening_quote_count):
        if self.char(required=True, lookahead=i) != '"':
          found_ending = False
          break
      if found_ending: break
      val_to_append = self.current_char
      if opening_quote_count == 1:
        string_escape = self.parse_string_escape()
        if string_escape is not None:
          val_to_append = string_escape
      value.append(val_to_append)
      self.advance()
    for _ in xrange(opening_quote_count): self.advance()
    return ast.String(bytestring, "".join(value), *self.source_ref(checkpoint))

  def skip_comment(self):
    if self.current_char != "#": return False
    self.advance()
    if self.parse_string() is not None: return True
    while True:
      if self.eof(): return True
      self.advance()
      if self.current_char == "\n": return True

  def skip_whitespace(self, other_skips=[]):
    if self.eof(): return False
    if self.skip_comment(): return True
    if self.current_char in " \t\r" or self.current_char in other_skips:
      self.advance()
      return True
    return False

  def skip_all_whitespace(self, other_skips=[]):
    any_skipped = False
    while self.skip_whitespace(other_skips): any_skipped = True
    return any_skipped

  def parse_subexpression(self):
    if self.current_char != "(": return None
    checkpoint = self.checkpoint()
    self.advance()
    self.skip_all_whitespace("\n;")
    expressions = self.parse_expression_list(False)
    if self.current_char != ")":
      self.assert_source("unexpected input at end of subexpression")
    if not expressions:
      self.assert_source("expression list expected in subexpression")
    self.advance()
    return ast.Subexpression(expressions, *self.source_ref(checkpoint))

  def parse_function(self):
    if self.current_char != "{": return None
    checkpoint = self.checkpoint()
    self.advance()
    self.skip_all_whitespace("\n")
    if self.current_char == "}":
      self.restore(checkpoint)
      return None # empty dictionary
    maybe_a_dict = True
    left_args = []
    right_args = []
    if self.current_char == "|":
      maybe_a_dict = False
      self.advance()
      self.skip_all_whitespace("\n")
      right_args = self.parse_in_arg_list()
      if self.current_char == ";":
        self.advance()
        self.skip_all_whitespace("\n")
        left_args = right_args
        right_args = self.parse_in_arg_list()
      if self.current_char != "|":
        self.assert_source("unexpected input for argument list")
      self.advance()
      self.check_left_in_args(left_args)
      self.check_right_in_args(right_args)
    self.skip_all_whitespace("\n;")
    expressions = self.parse_expression_list(True)
    if self.current_char != "}":
      if maybe_a_dict:
        self.restore(checkpoint)
        return None
      self.assert_source("unexpected input at close of function")
    self.advance()
    return ast.Function(expressions, left_args, right_args,
        *self.source_ref(checkpoint))

  def parse_keyword_out_arg(self):
    if (self.current_char != ":" or self.char(lookahead=1) != ":" or
        self.char(lookahead=2) != "("):
      return None
    checkpoint = self.checkpoint()
    self.advance(distance=3)
    self.skip_all_whitespace("\n;")
    expressions = self.parse_expression_list(False)
    if self.current_char != ")":
      self.assert_source("unexpected input for keyword argument")
    if not expressions:
      self.assert_source("keyword argument expected")
    self.advance()
    return ast.KeywordOutArgument(expressions, *self.source_ref(checkpoint))

  def parse_arbitrary_out_arg(self):
    if self.current_char != ":" or self.char(lookahead=1) != "(": return None
    checkpoint = self.checkpoint()
    self.advance(distance=2)
    self.skip_all_whitespace("\n;")
    expressions = self.parse_expression_list(False)
    if self.current_char != ")":
      self.assert_source("unexpected input for list argument")
    if not expressions:
      self.assert_source("list argument expected")
    self.advance()
    return ast.ArbitraryOutArgument(expressions, *self.source_ref(checkpoint))

  def parse_named_out_arg(self):
    checkpoint = self.checkpoint()
    field = self.parse_identifier()
    if not field: return None
    self.skip_all_whitespace("\n")
    if self.current_char != ":":
      self.restore(checkpoint)
      return None
    self.advance()
    self.skip_all_whitespace("\n")
    application = self.parse_application(False)
    if not application:
      self.assert_source("expected value for named argument")
    return ast.NamedOutArgument(field, application,
        *self.source_ref(checkpoint))

  def parse_positional_out_arg(self):
    checkpoint = self.checkpoint()
    application = self.parse_application(False)
    if application is None: return None
    return ast.PositionalOutArgument(application, *self.source_ref(checkpoint))

  def parse_keyword_in_arg(self):
    if (self.current_char != ":" or self.char(lookahead=1) != ":" or
        self.char(lookahead=2) != "("):
      return None
    checkpoint = self.checkpoint()
    self.advance(distance=3)
    self.skip_all_whitespace("\n")
    identifier = self.parse_identifier()
    if identifier is None:
      self.assert_source("expected keyword argument identifier")
    self.skip_all_whitespace("\n")
    if self.current_char != ")":
      self.assert_source("unexpected input for keyword argument")
    self.advance()
    return ast.KeywordInArgument(identifier, *self.source_ref(checkpoint))

  def parse_arbitrary_in_arg(self):
    if self.current_char != ":" or self.char(lookahead=1) != "(": return None
    checkpoint = self.checkpoint()
    self.advance(distance=2)
    self.skip_all_whitespace("\n")
    identifier = self.parse_identifier()
    if identifier is None:
      self.assert_source("expected list argument identifier")
    self.skip_all_whitespace("\n")
    if self.current_char != ")":
      self.assert_source("unexpected input for list argument")
    self.advance()
    return ast.ArbitraryInArgument(identifier, *self.source_ref(checkpoint))

  def parse_default_in_arg(self):
    checkpoint = self.checkpoint()
    field = self.parse_identifier()
    if field is None: return None
    self.skip_all_whitespace("\n")
    if self.current_char != ":":
      self.restore(checkpoint)
      return None
    self.advance()
    self.skip_all_whitespace("\n")
    application = self.parse_application(False)
    if application is None:
      self.assert_source("expected value for default argument")
    return ast.DefaultInArgument(field, application,
        *self.source_ref(checkpoint))

  def parse_required_in_arg(self):
    checkpoint = self.checkpoint()
    identifier = self.parse_identifier()
    if identifier is None: return None
    return ast.RequiredInArgument(identifier, *self.source_ref(checkpoint))

  def parse_out_arg(self):
    arg = self.parse_keyword_out_arg()
    if arg is not None: return arg
    arg = self.parse_arbitrary_out_arg()
    if arg is not None: return arg
    arg = self.parse_named_out_arg()
    if arg is not None: return arg
    return self.parse_positional_out_arg()

  def parse_in_arg(self):
    arg = self.parse_keyword_in_arg()
    if arg is not None: return arg
    arg = self.parse_arbitrary_in_arg()
    if arg is not None: return arg
    arg = self.parse_default_in_arg()
    if arg is not None: return arg
    return self.parse_required_in_arg()

  def parse_out_arg_list(self):
    args = []
    while True:
      arg = self.parse_out_arg()
      if arg is None: break
      args.append(arg)
      if self.current_char != ",": break
      self.advance()
      self.skip_all_whitespace("\n")
    return args

  def parse_in_arg_list(self):
    args = []
    while True:
      arg = self.parse_in_arg()
      if arg is None: break
      args.append(arg)
      if self.current_char != ",": break
      self.advance()
      self.skip_all_whitespace("\n")
    return args

  def check_left_out_args(self, left_args):
    for arg in left_args:
      if not isinstance(arg, ast.PositionalOutArgument) and \
          not isinstance(arg, ast.ArbitraryOutArgument):
        self.assert_source("unexpected argument type", arg.line, arg.col)

  def check_left_in_args(self, left_args):
    pos = 0
    if pos < len(left_args) and isinstance(left_args[pos],
        ast.ArbitraryInArgument):
      pos += 1
    while pos < len(left_args) and isinstance(left_args[pos],
        ast.DefaultInArgument):
      pos += 1
    while pos < len(left_args) and isinstance(left_args[pos],
        ast.RequiredInArgument):
      pos += 1
    if pos < len(left_args):
      self.assert_source("unexpected argument type", left_args[pos].line,
          left_args[pos].col)

  def check_right_out_args(self, right_args):
    pos = 0
    while pos < len(right_args) and (isinstance(right_args[pos],
        ast.PositionalOutArgument) or isinstance(right_args[pos],
        ast.ArbitraryOutArgument)):
      pos += 1
    while pos < len(right_args) and isinstance(right_args[pos],
        ast.NamedOutArgument):
      pos += 1
    if pos < len(right_args) and isinstance(right_args[pos],
        ast.KeywordOutArgument):
      pos += 1
    if pos < len(right_args):
      self.assert_source("unexpected argument type", right_args[pos].line,
          right_args[pos].col)

  def check_right_in_args(self, right_args):
    pos = 0
    while pos < len(right_args) and isinstance(right_args[pos],
        ast.RequiredInArgument):
      pos += 1
    while pos < len(right_args) and isinstance(right_args[pos],
        ast.DefaultInArgument):
      pos += 1
    if pos < len(right_args) and isinstance(right_args[pos],
        ast.ArbitraryInArgument):
      pos += 1
    if pos < len(right_args) and isinstance(right_args[pos],
        ast.KeywordInArgument):
      pos += 1
    if pos < len(right_args):
      self.assert_source("unexpected argument type", right_args[pos].line,
          right_args[pos].col)

  def parse_closed_call(self):
    if self.current_char != "(": return None
    checkpoint = self.checkpoint()
    self.advance()
    self.skip_all_whitespace("\n")
    left_args = []
    right_args = self.parse_out_arg_list()
    if self.current_char == ";":
      self.advance()
      self.skip_all_whitespace("\n")
      left_args = right_args
      right_args = self.parse_out_arg_list()
    if self.current_char != ")":
      self.assert_source("unexpected input for function call")
    self.advance()
    self.check_left_out_args(left_args)
    self.check_right_out_args(right_args)
    return ast.ClosedCall(left_args, right_args, *self.source_ref(checkpoint))

  def parse_identifier(self):
    char = self.current_char
    if (char in Parser.SAFE_DIGITS[10] or char in Parser.NON_ID_CHARS or
        not char):
      return None
    if char == "-" and self.char(lookahead=1) in Parser.SAFE_DIGITS[10]:
      return None
    chars = [char]
    while True:
      self.advance()
      char = self.current_char
      if not char or char in Parser.NON_ID_CHARS: break
      chars.append(char)
    return "".join(chars)

  def parse_variable(self):
    checkpoint = self.checkpoint()
    identifier = self.parse_identifier()
    if identifier is None: return None
    return ast.Variable(identifier, *self.source_ref(checkpoint))

  def parse_integer_in_base(self, base):
    char = self.current_char
    if char not in Parser.SAFE_DIGITS[base]: return None, None
    chars = [char]
    while True:
      self.advance()
      char = self.current_char
      if char not in Parser.SAFE_DIGITS[base]:
        if char and char not in Parser.NON_ID_CHARS:
          self.assert_source("invalid value in base %d" % base)
        break
      chars.append(char)
    return int("".join(chars).lower(), base), len(chars)

  def parse_integer(self):
    if self.current_char == "0" and (self.char(lookahead=1) in
        Parser.INTEGER_BASES):
      base = Parser.INTEGER_BASES[self.char(lookahead=1)]
      self.advance(distance=2)
      integer, log = self.parse_integer_in_base(base)
      if integer is None: self.assert_source("invalid value in base %d" % base)
      return integer, base, log
    integer, log = self.parse_integer_in_base(10)
    return integer, 10, log

  def parse_number(self):
    checkpoint = self.checkpoint()
    multiplier = 1
    if self.current_char == "-":
      multiplier = -1
      self.advance()
    integer, base, log = self.parse_integer()
    if integer is None:
      self.restore(checkpoint)
      return None
    if self.current_char != ".":
      return ast.Integer(multiplier * integer, *self.source_ref(checkpoint))
    self.advance()
    fractional, fractional_base, fractional_log = self.parse_integer()
    if fractional is None:
      self.assert_source("invalid floating point number")
    fractional = float(fractional) / int("1" + ("0" * fractional_log),
        fractional_base)
    return ast.Float(multiplier * (integer + fractional),
        *self.source_ref(checkpoint))

  def parse_dict_definition(self):
    checkpoint = self.checkpoint()
    key = self.parse_application(False)
    if key is None: return None
    if self.current_char != ":":
      self.assert_source("unexpected input while parsing dictionary")
    self.advance()
    self.skip_all_whitespace("\n")
    value = self.parse_application(False)
    if value is None:
      self.assert_source("expected dictionary value")
    return ast.DictDefinition(key, value, *self.source_ref(checkpoint))

  def parse_dict(self):
    if self.current_char != "{": return None
    checkpoint = self.checkpoint()
    self.advance()
    definitions = []
    while True:
      self.skip_all_whitespace("\n")
      definition = self.parse_dict_definition()
      if definition is None: break
      definitions.append(definition)
      if self.current_char != ",": break
      self.advance()
    self.skip_all_whitespace("\n,")
    if self.current_char != "}":
      self.assert_source("unexpected input for dictionary close ('}')")
    self.advance()
    return ast.Dict(definitions, *self.source_ref(checkpoint))

  def parse_array(self):
    if self.current_char != "[": return None
    checkpoint = self.checkpoint()
    self.advance()
    applications = []
    while True:
      self.skip_all_whitespace("\n")
      application = self.parse_application(False)
      if application is None: break
      applications.append(application)
      if self.current_char != ",": break
      self.advance()
    self.skip_all_whitespace("\n,")
    if self.current_char != "]":
      self.assert_source("unexpected input for list close (']')")
    self.advance()
    return ast.Array(applications, *self.source_ref(checkpoint))

  def parse_assignee(self):
    term = self.parse_term()
    if term is None: return None
    if term.modifiers:
      modifier = term.modifiers[-1]
      if isinstance(modifier, ast.Index):
        return ast.IndexAssignee(ast.Term(term.value, term.modifiers[:-1],
            term.line, term.col), modifier, term.line, term.col)
      if isinstance(modifier, ast.Field):
        return ast.FieldAssignee(ast.Term(term.value, term.modifiers[:-1],
            term.line, term.col), modifier, term.line, term.col)
      return None
    if isinstance(term.value, ast.Variable):
      return ast.VariableAssignee(term.value, term.line, term.col)
    return None

  def parse_value(self):
    value = self.parse_subexpression()
    if value is not None: return value
    value = self.parse_function()
    if value is not None: return value
    value = self.parse_string()
    if value is not None: return value
    value = self.parse_variable()
    if value is not None: return value
    value = self.parse_number()
    if value is not None: return value
    value = self.parse_dict()
    if value is not None: return value
    return self.parse_array()

  def parse_index(self):
    if self.current_char != "[": return None
    checkpoint = self.checkpoint()
    self.advance()
    self.skip_all_whitespace("\n;")
    expressions = self.parse_expression_list(False)
    if self.current_char != "]":
      self.assert_source("unexpected input for index lookup")
    if not expressions:
      self.assert_source("expected expression for index lookup")
    self.advance()
    return ast.Index(expressions, *self.source_ref(checkpoint))

  def parse_field(self):
    if self.current_char == ".":
      checkpoint = self.checkpoint()
      self.advance()
      identifier = self.parse_identifier()
      if identifier is not None:
        return ast.Field(identifier, *self.source_ref(checkpoint))
      self.restore(checkpoint)
    return None

  def parse_right_open_call(self):
    if self.current_char == "." and (self.char(lookahead=1) in
        Parser.NON_ID_CHARS or self.char(lookahead=1) == None):
      checkpoint = self.checkpoint()
      self.advance()
      return ast.OpenCall(*self.source_ref(checkpoint))
    return None

  def parse_left_open_call(self):
    if self.current_char in ("@", "."):
      checkpoint = self.checkpoint()
      self.advance()
      return ast.OpenCall(*self.source_ref(checkpoint))
    return None

  def parse_header(self):
    return self.parse_left_open_call()

  def parse_trailer(self):
    trailer = self.parse_right_open_call()
    if trailer is not None: return trailer
    trailer = self.parse_index()
    if trailer is not None: return trailer
    trailer = self.parse_field()
    if trailer is not None: return trailer
    return self.parse_closed_call()

  def parse_term(self):
    if self._term_memoization.has_key(self.pos):
      rv, checkpoint = self._term_memoization[self.pos]
      self.restore(checkpoint)
      return rv
    checkpoint = self.checkpoint()
    headers = []
    trailers = []
    while True:
      header = self.parse_header()
      if header is None: break
      headers.append(header)
    headers.reverse()
    value = self.parse_value()
    if value is None:
      self.restore(checkpoint)
      self._term_memoization[self.pos] = None, checkpoint
      return None
    while True:
      trailer = self.parse_trailer()
      if trailer is None: break
      trailers.append(trailer)
    rv = ast.Term(value, trailers + headers, *self.source_ref(checkpoint))
    self._term_memoization[checkpoint[0]] = (rv, self.checkpoint())
    return rv

  def parse_assignment(self, newline_sep):
    if newline_sep: skips = []
    else: skips = ["\n"]
    checkpoint = self.checkpoint()
    assignee = self.parse_assignee()
    if assignee is None:
      self.restore(checkpoint)
      return None
    self.skip_all_whitespace(skips)
    mutation = False
    if self.current_char == ":":
      mutation = True
      self.advance()
    if self.current_char != "=":
      self.restore(checkpoint)
      return None
    self.advance()
    if self.current_char not in Parser.NON_ID_CHARS:
      self.restore(checkpoint)
      return None
    self.skip_all_whitespace(skips)
    exp = self.parse_expression(newline_sep)
    if exp is None: self.assert_source("unexpected input")
    return ast.Assignment(mutation, assignee, exp, *self.source_ref(checkpoint))

  def parse_application(self, newline_sep):
    if newline_sep: skips = []
    else: skips = ["\n"]
    checkpoint = self.checkpoint()
    terms = []
    while True:
      self.skip_all_whitespace(skips)
      term = self.parse_term()
      if term is None: break
      terms.append(term)
    if not terms: return None
    return ast.Application(terms, *self.source_ref(checkpoint))

  def parse_expression(self, newline_sep):
    exp = self.parse_assignment(newline_sep)
    if exp is None: exp = self.parse_application(newline_sep)
    return exp

  def parse_expression_list(self, newline_sep):
    expressions = []
    while True:
      exp = self.parse_expression(newline_sep)
      if exp is None: break
      expressions.append(exp)
      self.skip_all_whitespace(";\n")
    return expressions

  def parse(self):
    checkpoint = self.checkpoint()
    self.skip_all_whitespace(";\n")
    explist = self.parse_expression_list(True)
    if not self.eof(): self.assert_source("unexpected input")
    self.restore(checkpoint)
    return ast.Program(explist)

  if DEBUG_LEVEL > 0:
    _parser_methods = locals()
    _debug_indents = []
    for _parser_field, _parser_function in _parser_methods.copy().iteritems():
      if _parser_field[:6] != "parse_": continue
      def _closure(function=_parser_function, field=_parser_field,
          locals_=_parser_methods, indents=_debug_indents):
        def wrapper(self, *args, **kwargs):
          arg_repr = ", ".join(map(repr, args) +
              ["%s: %r" % item for item in kwargs.iteritems()])
          print "%s-> %s(%s) (L%d:%d)" % ("".join(indents), field[6:], arg_repr,
              self.line, self.col)
          if DEBUG_LEVEL > 1: print repr(self.source[:self.pos])
          indents.append("  ")
          rv = function(self, *args, **kwargs)
          indents.pop()
          print "%s<- %s(%s) => %r" % ("".join(indents), field[6:], arg_repr,
              rv)
          if DEBUG_LEVEL > 1: print repr(self.source[:self.pos])
          return rv
        locals_[field] = wrapper
      _closure()
    del _debug_indents, _parser_methods, _parser_field, _parser_function
    del _closure

def parse(io):
  return Parser(io).parse()

if __name__ == "__main__":
  import sys
  if DEBUG_LEVEL > 0:
    print parse(sys.stdin)
  else:
    try: parse(sys.stdin)
    except ParserError, e: print e
