#!/bin/sh 
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
"${SCRIPTPATH}/build.sh"
"${SCRIPTPATH}/out/charm"
