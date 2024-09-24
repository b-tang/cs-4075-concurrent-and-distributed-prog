#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

int main() {
    /* Local Variables */
    int comm_sz, my_rank;

    /* MPI Initialization */
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    /* Takes both local_arr and prefix_sum to find the arr size */
    int num = 4;
    int* local_arr = (int*)malloc(num * sizeof(int));
    int* prefix_sum = (int*)malloc(num * sizeof(int));

    /* Generate a randomize seed num with my_rank */
    srand(time(NULL) + my_rank);
    for (int i = 0; i < num; i++) {
        local_arr[i] = rand() % 10;
    }

    MPI_Scan(local_arr, prefix_sum, num, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    /* Print results */
    /*-------------------------------------------------------------------*/
    printf("Original array on my_rank %d: ", my_rank);
    for (int i = 0; i < num; i++) {
        printf("%d ", local_arr[i]);
    }
    printf("\n");
    printf("Prefix sum on my_rank %d: ", my_rank);
    for (int i = 0; i < num; i++) {
        printf("%d ", prefix_sum[i]);
    }
    printf("\n\n");

    free(local_arr);
    free(prefix_sum);

    MPI_Finalize();
    return 0;
}   /* main */
