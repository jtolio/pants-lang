#!/usr/bin/env python

__author__ = "JT Olds <hello@jtolds.com>"

import os, re, subprocess, tempfile, sys

TEST_EXT = re.compile(r'\.cth$')
COMPILER_PATH = "../src/cirth"
C_COMPILERS = [["clang"], ["gcc"]]
C_LIBRARIES = ["-lgc"]

class Error_(Exception): pass
class TestError(Error_): pass

def find_tests(explicit_tests):
  filter_tests = False
  if explicit_tests:
    explicit_tests = set(explicit_tests)
    filter_tests = True
  for filename in os.listdir("."):
    if not TEST_EXT.search(filename): continue
    testname = TEST_EXT.sub('', filename)
    if os.path.exists("%s.out" % testname) and (not filter_tests or
        testname in explicit_tests):
      yield (filename, "%s.out" % testname)

def translate(source_path):
  in_file = file(source_path)
  fd, path = tempfile.mkstemp(suffix=".c", prefix="test-")
  out_file = os.fdopen(fd, "w")
  compiler = subprocess.Popen([COMPILER_PATH], stdin=in_file, stdout=out_file)
  in_file.close()
  out_file.close()
  compiler.communicate()
  if compiler.returncode != 0:
    raise TestError, "Failed translating %s" % source_path
  return path

def clean_up(output):
  return '\n'.join((x.strip() for x in output.strip().split('\n')))

def run_tests(explicit_tests):
  failed_tests = 0
  total_tests = 0
  for compiler in C_COMPILERS:
    for source, output in find_tests(explicit_tests):
      try:
        c_source = translate(source)
        try:
          subprocess.check_call(compiler + ["-o", c_source[:-2], c_source] +
              C_LIBRARIES)
          binary = subprocess.Popen([c_source[:-2]], stdout=subprocess.PIPE)
          binary_output = clean_up(binary.communicate()[0] +
              ("\nreturn code: %d" % binary.returncode))
          expected_output = clean_up(file(output).read())
          if binary_output != expected_output:
            sys.stdout.write("test %s\n============== Expected:\n" % source)
            sys.stdout.write(expected_output)
            sys.stdout.write("\n============== Received:\n")
            sys.stdout.write(binary_output)
            sys.stdout.write("\n")
            raise TestError, "output mismatch for test %s" % source
        finally:
          subprocess.call(["rm", "-f", c_source[:-2], c_source])
      except Exception, e:
        sys.stdout.write("FAILURE: %s\n" % e)
        failed_tests += 1
      total_tests += 1
      sys.stdout.write(".")
      sys.stdout.flush()

  sys.stdout.write("\nTotal tests: %d, Successes: %d, Failures: %d\n" % (
      total_tests, (total_tests - failed_tests), failed_tests))

if __name__ == "__main__": run_tests(sys.argv[1:])
