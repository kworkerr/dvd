#!/bin/sh

if [ "$(id -u)" -ne 0 ] ; then
	echo "install.sh: requires root privileges"
	exit
fi

if [ ! -f "./dvd" ]; then
	echo "install.sh: dvd not found, run './build.sh' first"
	exit
fi

PREFIX=${PREFIX:-/usr}

install -m 755 dvd "$PREFIX/bin/dvd"
