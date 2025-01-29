#!/bin/sh 
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
mkdir -p "${SCRIPTPATH}/out"
gcc -fopenmp -Wall -Wextra -O0 -g3 -std=c99 -pedantic -o "${SCRIPTPATH}/out/charm.debug" charm.c utils.c charm_functions.c
