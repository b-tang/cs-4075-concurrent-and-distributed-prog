#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h> 

int main(void) {
    /* Local Variables */
    int amount;
    int comm_sz, my_rank;

    /* MPI Initialization */
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Input vectors */
    if (my_rank == 0) {
        printf("Problem 2.0/2.1");
        printf("\n-------------------------------------------------------------------");
        printf("\nEnter the amount of the vectors: \n");
        scanf("%d", &amount);
    }

    MPI_Bcast(&amount, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* Take both x and y to calculate the results */
    double* x = (double*)malloc(amount * sizeof(double));
    double* y = (double*)malloc(amount * sizeof(double));
    double* results = (double*)malloc(amount * sizeof(double));

    /* Read inputs */
    Read_vector(x, amount, my_rank);
    Read_vector(y, amount, my_rank);

    /* Single Element datatype */
    MPI_Datatype one_element_type;
    MPI_Type_contiguous(1, MPI_DOUBLE, &one_element_type);
    MPI_Type_commit(&one_element_type);

    /* Print results */
    results[my_rank] = x[my_rank] + y[my_rank];
    Print_vector(x, amount, my_rank, "x");
    Print_vector(y, amount, my_rank, "y");

    /* process 0 calculates the final vector */
    /* Prints final results */
    if (my_rank == 0) {
        for (int i = 0; i < amount; i++) {
            x[i] = x[i] + y[i];
        }
    }
    if (my_rank == 0) {
        Print_vector(x, amount, my_rank, "(x+y)");
    }

    free(x);
    free(y);
    free(results);

    MPI_Type_free(&one_element_type);
    MPI_Finalize();
    return 0;
}   /* main */

/* The read_vector will read the vector sizes */
/* process 0 will take the input of the elements, the vector */
/*-------------------------------------------------------------------*/
void Read_vector(double* vec, int n, int my_rank) {
    if (my_rank == 0) {
        printf("\nEnter the %d elements for the vector:\n", n);
        for (int i = 0; i < n; i++) {
            scanf("%lf", &vec[i]);
        }
    }

    MPI_Bcast(vec, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}   /* Read_vector */

/* The print_vector will be use to gather all vectors and print the gathered vector */
/*-------------------------------------------------------------------*/
void Print_vector(double* vec, int n, int my_rank, const char* temp) {
    if (my_rank == 0) {
        printf("\nVector %s: ", temp);
        for (int i = 0; i < n; i++) {
            printf("%.2lf ", vec[i]);
        }
        printf("\n");
    }
}   /* Print_vector */
