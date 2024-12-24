#!/bin/bash
#PBS -l select=4:ncpus=8
#PBS -q short_cpuQ
#PBS -l walltime=00:10:00
#PBS -N template_test
#PBS -o template_test.out
#PBS -e template_test.err
#PBS -M your.user@unitn.it
#PBS -m abe
cd ${PBS_O_WORKDIR}
sleep 600