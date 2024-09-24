#include "mpi.h"
#include <stdio.h>

#define WCOMM MPI_COMM_WORLD
int main(int argc, char **argv){
    int my_rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(WCOMM, &my_rank);

    int pdf_i = 1;
    int cdf_i = 1;

    pdf_i = my_rank;

    MPI_Scan(&pdf_i, &cdf_i, 1, MPI_INT, MPI_SUM, WCOMM);

    printf("process %d: cumulative sum = %d\n", my_rank, cdf_i);

    MPI_Finalize();
}
