#!/bin/bash

#PBS -l select=2:ncpus=4:mem=2gb

# define the queue
#PBS -q short_cpuQ

# set the excecution time
#PBS -l walltime=00:10:00

# set user (optional)
#PBS -M andrea.leoni-1@unitn.it

module load mpich-3.2

export CHARM_OPENMP=0
export DATA_PATH=/home/andrea.leoni-1/Charm/data
export DATA_FILE=synthetic_retail.dat
export DATA_CHARACTERS=0
export MIN_SUPPORT=10000
export MAX_TRANSACTIONS=100000

mpiexec -n 8 $(pwd)/Charm/out/charm_parallel