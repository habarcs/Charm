#!/bin/bash

#PBS -l select=1:ncpus=1

# define the queue
#PBS -q short_cpuQ

# set the excecution time
#PBS -l walltime=00:00:10

# set user (optional)
#PBS -M andrea.leoni-1@unitn.it

module load mpich-3.2
mpirun.actual -n ./charm_parallel