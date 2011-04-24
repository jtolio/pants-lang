#!/usr/bin/env python

__author__ = "JT Olds <hello@jtolds.com>"

import os, re, subprocess, tempfile

TEST_EXT = re.compile(r'\.cth$')
COMPILER_PATH = "../src/cirth"

class Error_(Exception): pass
class TestError(Error_): pass

def find_tests():
  for filename in os.listdir("."):
    if not TEST_EXT.search(filename): continue
    testname = TEST_EXT.sub('', filename)
    if os.path.exists("%s.out" % testname):
      yield (filename, "%s.out" % testname)

def translate(source_path):
  in_file = file(source_path)
  fd, path = tempfile.mkstemp(suffix=".c")
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

def run_tests():
  failed_tests = 0
  total_tests = 0
  for source, output in find_tests():
    try:
      c_source = translate(source)
      subprocess.check_call(["gcc", "-o", c_source[:-2], c_source, "-lgc"])
      binary = subprocess.Popen([c_source[:-2]], stdout=subprocess.PIPE)
      binary_output = clean_up(binary.communicate()[0])
      if binary.returncode != 0:
        raise TestError, "Failed running binary %s" % source
      expected_output = clean_up(file(output).read())
      if binary_output != expected_output:
        print "test %s" % source
        print "============== Expected:"
        print expected_output
        print "============== Received:"
        print binary_output
        print
        raise TestError, "output mismatch for test %s" % source
    except Exception, e:
      print "FAILURE: %s" % e
      failed_tests += 1
    total_tests += 1

  print
  print "Total tests: %d, Successes: %d, Failures: %d" % (total_tests,
      (total_tests - failed_tests), failed_tests)

if __name__ == "__main__": run_tests()
