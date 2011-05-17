#!/usr/bin/env python

__author__ = "JT Olds <hello@jtolds.com>"

import os, re, subprocess, tempfile, sys, threading, Queue

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

def run_test(compiler, source, output):
  errors = []
  try:
    c_source = translate(source)
    try:
      subprocess.check_call(compiler + ["-o", c_source[:-2], c_source] +
          C_LIBRARIES)
      binary = subprocess.Popen([c_source[:-2]], stdout=subprocess.PIPE)
      binary_output = clean_up(binary.communicate()[0] + ("\nreturn code: %d" %
          binary.returncode))
      expected_output = clean_up(file(output).read())
      if binary_output != expected_output:
        errors.append("\n".join((
            "test %s" % source,
            "============== Expected:",
            expected_output,
            "============== Received:",
            binary_output,
            "",
            "output mismatch for test %s" % source)))
        raise TestError, "output mismatch for test %s" % source
    finally:
      os.unlink(c_source[:-2])
      os.unlink(c_source)
  except Exception, e:
    errors.append("FAILURE: %s" % e)
  return errors

def worker(job_queue, errors):
  while True:
    try: job = job_queue.get_nowait()
    except Queue.Empty, e: return
    job_errors = run_test(*job)
    if job_errors:
      errors.put(1)
      for error in job_errors:
        sys.stdout.write("%s\n" % error)
    sys.stdout.write(".")
    sys.stdout.flush()

def run_tests(explicit_tests, parallelism):
  total_tests = 0
  job_queue = Queue.Queue()
  errors = Queue.Queue()
  for compiler in C_COMPILERS:
    for source, output in find_tests(explicit_tests):
      job_queue.put((compiler, source, output))
      total_tests += 1
  threads = [threading.Thread(target=worker, args=(job_queue, errors))
      for _ in xrange(parallelism)]
  for thread in threads: thread.start()
  alive_threads = True
  while alive_threads:
    alive_threads = False
    for thread in threads:
      if thread.is_alive():
        alive_threads = True
        thread.join(0.1) # don't keep the main thread asleep. python is sad
  failed_tests = errors.qsize()
  sys.stdout.write("\nTotal tests: %d, Successes: %d, Failures: %d\n" % (
      total_tests, (total_tests - failed_tests), failed_tests))

def processor_count():
  processors = {}
  for line in file("/proc/cpuinfo"):
    data = [x.strip() for x in line.strip().split(":")]
    if data[0] == "processor" and len(data) > 1: processors[data[1]] = True
  return max(len(processors), 1)

def main(argv):
  run_tests(argv[1:], processor_count())
  return 0

if __name__ == "__main__": sys.exit(main(sys.argv))
