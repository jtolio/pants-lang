#!/bin/sh

set -e

cat $2 | $1 > /tmp/pants-$$.c
gcc -o /tmp/pants-$$ /tmp/pants-$$.c -lgc
rm -f /tmp/pants-$$.c
/tmp/pants-$$
rm -f /tmp/pants-$$
