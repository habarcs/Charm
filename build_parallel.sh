#!/bin/sh
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
mkdir -p "${SCRIPTPATH}/out"
mpicc -std=c99 -fopenmp -g -Wall -o "${SCRIPTPATH}/out/charm_parallel_cluster" charm_parallel.c utils.c charm_functions.c set.c itarray.c
