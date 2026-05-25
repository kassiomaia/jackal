#!/bin/sh
# Bootstrap the autotools build, then run ./configure && make.
set -e
autoreconf -i "$@"
echo "autogen: done — now run ./configure && make"
