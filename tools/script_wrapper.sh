#!/bin/sh

set -e

cat $2 | $1 > /tmp/cirth-$$.c
gcc -o /tmp/cirth-$$ /tmp/cirth-$$.c -lgc
rm -f /tmp/cirth-$$.c
/tmp/cirth-$$
rm -f /tmp/cirth-$$
