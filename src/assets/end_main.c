
  FATAL_ERROR("fell off the end of main?", globals.c_null);
  return 1;
}

int main(int argc, char **argv) {
  GC_INIT();
  return gc_main(argc, argv);
}
