#!/bin/sh

CC=${CC:-cc}

TARGET="dvd"

$CC src/* -o $TARGET
