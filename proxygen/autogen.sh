#!/bin/sh
set -x
aclocal
autoconf
autoheader      #产生config.h.in供configure使用
automake --foreign --add-missing --copy   #产生各级目录的Makefile.in供configure使用
