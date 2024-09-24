#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int main() {
    /* Local Variables */
    int num;
    int comm_sz, my_rank;
    int *matrix_vector, *vec1, *vec2, *local_vec2;
    
    /* MPI Initialization */
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    /* Input vectors & Print Results */
    if (my_rank == 0) {
        printf("Problem 3.0");
        printf("\n-------------------------------------------------------------------");
        printf("\nEnter the size of the matrix vector: \n");
        scanf("%d", &num);
        
        matrix_vector = (int*)malloc(num * num * sizeof(int));
        vec1 = (int*)malloc(num * sizeof(int));
        
        /* Randomize integer values */
        for (int i = 0; i < num * num; i++) {
            matrix_vector[i] = (rand() % 5) + 1;
        }
        for (int i = 0; i < num; i++) {
            vec1[i] = (rand() % 4) + 1;
        }
        
        printf("\nMatrix Vector:\n");
        for (int i = 0; i < num; i++) {
            for (int j = 0; j < num; j++) {
                printf("%d\t", matrix_vector[i * num + j]);
            }
            printf("\n");
        }
        printf("\nVector 1:\n");
        for (int i = 0; i < num; i++) {
            printf("%d\n", vec1[i]);
        }
    }

    local_vec2 = (int*)malloc(num / comm_sz * sizeof(int));
    
    /* MPI Functions */
    MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(vec1, num / comm_sz, MPI_INT, local_vec2, num / comm_sz, MPI_INT, 0, MPI_COMM_WORLD);
    
    
    /* Matrix Vector Multiplcation Calculations */
    for (int i = 0; i < num / comm_sz; i++) {
        local_vec2[i] = 0;
        for (int j = 0; j < num; j++) {
            local_vec2[i] += matrix_vector[i * num + j] * vec1[j];
        }
    }
    
    if (my_rank == 0) {
        vec2 = (int*)malloc(num * sizeof(int));
    }
    
    /* MPI Functions */
    MPI_Gather(local_vec2, num / comm_sz, MPI_INT, vec2, num / comm_sz, MPI_INT, 0, MPI_COMM_WORLD);
    
    /* Print Results */
    if (my_rank == 0) {
        printf("\nVector 2 Results:\n");
        for (int i = 0; i < num; i++) {
            printf("%d\n", vec2[i]);
        }
        free(matrix_vector);
        free(vec1);
        free(vec2);
    }

    free(local_vec2);
    
    MPI_Finalize();
    return 0;
}   /* main */
