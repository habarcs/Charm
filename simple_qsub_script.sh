#!/bin/bash

#PBS -l select=1:ncpus=2:mem=2gb

# define the queue
#PBS -q short_cpuQ

# set the excecution time
#PBS -l walltime=00:01:00

# set user (optional)
#PBS -M andrea.leoni-1@unitn.it

module load mpich-3.2
module load openmpi-4.0.4
module load glibc-2.32
mpirun.actual -n 2 $(pwd)/Charm/charm_parallel_cluster