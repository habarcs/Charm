#!/bin/bash

#PBS -l select=1:ncpus=2:mem=2gb

# define the queue
#PBS -q short_cpuQ

# set the excecution time
#PBS -l walltime=00:10:00

# set user (optional)
#PBS -M andrea.leoni-1@unitn.it

module load mpich-3.2

export CHARM_OPENMP=1
export DATA_PATH=/home/andrea.leoni-1/Charm/data
export DATA_FILE=retail.dat
export DATA_CHARACTERS=0
export MIN_SUPPORT=5000

mpiexec -n 2 $(pwd)/Charm/out/charm_parallel