#!/bin/sh 
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
mkdir -p "${SCRIPTPATH}/out"
gcc -Wall -Wextra -Werror -O2 -std=c99 -pedantic charm.c -o "${SCRIPTPATH}/out/charm"
