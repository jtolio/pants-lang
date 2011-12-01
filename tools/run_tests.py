#!/usr/bin/env python

__author__ = "JT Olds <hello@jtolds.com>"

import os, re, subprocess, tempfile, sys, threading, Queue, traceback

TEST_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..",
    "tests"))
PANTS_TEST_EXT = re.compile(r'\.p$')
C_TEST_EXT = re.compile(r'\.c$')
PANTS_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), "..",
    "src", "pants"))
C_COMPILERS = [["gcc"]] # ["clang"] seems broken or something
C_LIBRARIES = ["-lgc"]
PANTS_OPTIONS = []
PANTS_OPTIONS_HEADER = "# PANTS OPTIONS: "

class Error_(Exception): pass
class TestError(Error_): pass

def find_tests(explicit_tests):
  filter_tests = False
  if explicit_tests:
    explicit_tests = set(explicit_tests)
    filter_tests = True
  for filename in os.listdir(TEST_DIR):
    if PANTS_TEST_EXT.search(filename):
      testname = PANTS_TEST_EXT.sub('', filename)
      if os.path.exists(os.path.join(TEST_DIR, "%s.out" % testname)) and (
          not filter_tests or testname in explicit_tests):
        yield (os.path.join(TEST_DIR, filename),
               os.path.join(TEST_DIR, "%s.out" % testname),
               testname,
               True)
    if C_TEST_EXT.search(filename):
      testname = C_TEST_EXT.sub('', filename)
      if not filter_tests or testname in explicit_tests:
        yield (os.path.join(TEST_DIR, filename),
               None,
               testname,
               False)

def translate(source_path):
  in_file = file(source_path)
  first_line = in_file.readline().strip()
  in_file.seek(0, 0)
  file_specific_options = []
  if first_line.find(PANTS_OPTIONS_HEADER) == 0:
    file_specific_options = first_line[len(PANTS_OPTIONS_HEADER):].split(' ')
  fd, path = tempfile.mkstemp(suffix=".c", prefix="test-")
  out_file = os.fdopen(fd, "w")
  compiler = subprocess.Popen([PANTS_PATH] + PANTS_OPTIONS +
      file_specific_options, stdin=in_file, stdout=out_file)
  in_file.close()
  out_file.close()
  compiler.communicate()
  if compiler.returncode != 0:
    raise TestError, "Failed translating %s" % source_path
  return path

def clean_up(output):
  return '\n'.join((x.strip() for x in output.strip().split('\n')))

def run_test(compiler, source, output, testname, needs_translation):
  errors = []
  try:
    if needs_translation: source = translate(source)
    try:
      subprocess.check_call(compiler + ["-o", source[:-2], source] +
          C_LIBRARIES)
      binary = subprocess.Popen([source[:-2]], stdout=subprocess.PIPE)
      binary_output = clean_up(binary.communicate()[0] + ("\nreturn code: %d" %
          binary.returncode))
      if output is not None:
        expected_output = clean_up(file(output).read())
        if binary_output != expected_output:
          errors.append("\n".join((
              "test %s" % source,
              "============== Expected:",
              expected_output,
              "============== Received:",
              binary_output,
              "",
              "output mismatch for test %s" % testname)))
          raise TestError, "output mismatch for test %s" % testname
      else:
        if binary.returncode != 0:
          errors.append("\n".join((
              "test %s" % source,
              "============== Output:",
              binary_output,
              ""
              "unexpected returncode for test %s" % testname)))
          raise TestError, "unexpected returncode for test %s" % testname
    finally:
      os.unlink(source[:-2])
      if needs_translation: os.unlink(source)
  except Exception, e:
    errors.append("FAILURE on test %s: %s\n%s" % (testname, e,
        traceback.format_exc()))
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
    for source, output, testname, needs_translation in \
        find_tests(explicit_tests):
      job_queue.put((compiler, source, output, testname, needs_translation))
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
  if argv[1].find("--parallelism=") == 0:
    parallelism = int(argv[1].split("=")[1])
    run_tests(argv[2:], parallelism)
  else:
    run_tests(argv[1:], processor_count())
  return 0

if __name__ == "__main__": sys.exit(main(sys.argv))
