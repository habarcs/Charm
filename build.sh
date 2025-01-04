#!/bin/sh 
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
mkdir -p "${SCRIPTPATH}/out"
gcc -fopenmp -Wall -Wextra -O2 -std=c99 -pedantic -o "${SCRIPTPATH}/out/charm" charm.c utils.c
