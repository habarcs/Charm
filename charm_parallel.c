#include "charm_functions.h"
#include <mpi.h>
#include <stdio.h>

int main() {

    int rank, size = 0;
    MPI_Init(NULL, NULL);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("My rank is %d/%d\n", rank, size);

    // get the dataset
    // divide dataset ?
    // compute first iteration in serial ??? No, i think that should already be parallelized
    // compute charm_extend() in parallel
    // how to gather the results later? MPI_Gather I suppose ...

    MPI_Finalize();

    return 0;
}