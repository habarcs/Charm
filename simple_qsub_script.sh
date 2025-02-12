#!/bin/bash

#PBS -l select=1:ncpus=2:mem=2gb

# define the queue
#PBS -q short_cpuQ

# set the excecution time
#PBS -l walltime=00:00:10

# set user (optional)
#PBS -M andrea.leoni-1@unitn.it

module purge
module load mpich-3.2

export OMP_NUM_THREADS=2

echo "Starting execution..."
mpiexec -n 2 $(pwd)/Charm/out/charm_parallel_cluster