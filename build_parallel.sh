#!/bin/sh
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
mkdir -p "${SCRIPTPATH}/out"
if [ -f "${SCRIPTPATH}/out/charm_parallel" ]; then
  rm "${SCRIPTPATH}/out/charm_parallel"
fi
mpicc -std=c99 -fopenmp -g -Wall -Wextra -O2 -pedantic -o "${SCRIPTPATH}/out/charm_parallel" charm_parallel.c utils.c charm_functions.c set.c itarray.c
