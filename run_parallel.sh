#!/bin/sh 
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
"${SCRIPTPATH}/build_parallel.sh"
mpiexec -n 4 "${SCRIPTPATH}/out/charm_parallel"
