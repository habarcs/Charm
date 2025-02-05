#!/bin/sh 
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
mkdir -p "${SCRIPTPATH}/out"
if [ -f "${SCRIPTPATH}/out/charm" ]; then
  rm "${SCRIPTPATH}/out/charm"
fi
gcc -fopenmp -Wall -Wextra -O2 -std=c99 -pedantic -o "${SCRIPTPATH}/out/charm" charm.c charm_functions.c itarray.c set.c utils.c
