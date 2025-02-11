#!/bin/sh
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
mkdir -p "${SCRIPTPATH}/out"
if [ -f "${SCRIPTPATH}/out/charm_parallel" ]; then
  rm "${SCRIPTPATH}/out/charm_parallel"
fi
mpicc -fopenmp -Wall -Wextra -O2 -std=c99 -pedantic -o "${SCRIPTPATH}/out/charm_parallel" charm_parallel.c utils.c charm_functions.c set.c itarray.c
