#!/bin/sh 
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
mkdir -p "${SCRIPTPATH}/out"
gcc -Wall -Wextra -O0 -g3 -std=c99 -pedantic charm.c -o "${SCRIPTPATH}/out/charm.debug"
gdb "${SCRIPTPATH}/out/charm.debug"
