#!/bin/sh
#
libtoolize --copy --force
aclocal -I m4 --force
automake -f --add-missing --copy
autoconf

# Run configure for this platform
#./configure $*

