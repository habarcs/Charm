#!/bin/sh 
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
"${SCRIPTPATH}/build_parallel.sh"
mpiexec -n 2 "${SCRIPTPATH}/out/charm_parallel"
