#!/bin/sh

set -e

compilation_needed=false
if [ -x "${2}c" ]; then
  if [ "$2" -nt "${2}c" ]; then
    compilation_needed=true
  fi
  if [ "$1" -nt "${2}c" ]; then
    compilation_needed=true
  fi
else
  compilation_needed=true
fi

if $compilation_needed; then
  cat $2 | $1 > /tmp/pants-$$.c
  gcc -o "${2}c" /tmp/pants-$$.c -lgc
  rm -f /tmp/pants-$$.c
fi

${2}c
