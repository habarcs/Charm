#!/bin/bash

#PBS -l select=4:ncpus=8:mem=4gb

# define the queue
#PBS -q short_cpuQ

# set the excecution time
#PBS -l walltime=00:10:00

# set user (optional)
#PBS -M andrea.leoni-1@unitn.it

module load mpich-3.2

export CHARM_OPENMP=0
# export OMP_NUM_THREADS=2
export DATA_PATH=/home/andrea.leoni-1/Charm/data
export DATA_FILE=synthetic_retail.dat
export DATA_CHARACTERS=0
export MIN_SUPPORT=2500
export MAX_TRANSACTIONS=300000
# export OUT_PATH=stdout

mpiexec -n 32 $(pwd)/Charm/out/charm_parallel
