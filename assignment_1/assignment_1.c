#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h> 

void Read_input_trapezoid(int my_rank, int comm_sz, double* a, double* b, int* n);
double f(double x);
double Trapezoid(double left_endpoint, double right_endpoint, int trapezoid_count, double base_length);
void Read_input_histogram(int my_rank, int comm_sz, int* data_count, double* a, double* b, int* n);

int main(void) {
    /* Local Variables */
    int my_rank, comm_sz;
    double a, b, h;
    double local_int, total_int;
    int n, data_count;

    /* MPI Initialization */
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    Read_input_trapezoid(my_rank, comm_sz, &a, &b, &n);
    h = (b - a) / n;
    local_int = Trapezoid(a, b, n, h);

    /* #2: Use MPI_Reduce to rewrite the collective communication part of Problem 1. */
    MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    /* Read_input_trapezoid */
    if (my_rank == 0) {
        printf("\nSubintervals (n) = %d (trapezoids)", n);
        printf("\nThe trapezoid of the integral is from %f to %f which equals to %.15e\n", a, b, total_int);
    }

    /* Task #3: Implementing Histogram */
    // Get user input for histogram 
    Read_input_histogram(my_rank, comm_sz, &data_count, &a, &b, &n);
    // Generate random measurements
    double* measurements = (double*)malloc(data_count * sizeof(double));
    // double last_measurement;
    for (int i = 0; i < data_count; i++) {
        measurements[i] = a + (b - a) * rand() / RAND_MAX;
    }
    // Calculate the range for each bin
    double bin_range = (b - a) / n;
    int* bin_counts = (int*)calloc(n, sizeof(int));
    // Count measurements in each bin
    for (int i = 0; i < data_count; i++) {
        int bin_index = (int)((measurements[i] - a) / bin_range);
        if (bin_index == n) {
            bin_index--;
        }
        bin_counts[bin_index]++;
    }
    // Gather bin counts to process 0
    int* total_bin_counts = (int*)malloc(n * sizeof(int));
    MPI_Reduce(bin_counts, total_bin_counts, n, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (my_rank == 0) {
        printf("\nMeasurements:\n");
        for (int i = 0; i < data_count; i++) {
            printf("%.2f, \n", measurements[i]);
            // if (*last_measurement == measurements[data_count]) {
            //     printf("%f", measurements[i]);
            // }
        }
        printf("Measurement Range  | Number of Measurements in Each Bin\n");
        for (int i = 0; i < n; i++) {
            double bin_start = a + i * bin_range;
            double bin_end = bin_start + bin_range;
            printf("%.2lf - %.2lf        | %d\n", bin_start, bin_end, total_bin_counts[i]);
        }
    }

    free(measurements);
    free(bin_counts);
    free(total_bin_counts);

    MPI_Finalize();
    return 0;
}   /* Main */

double f(double x) {
    return x * x; 
}

/* #1: Trapezoid */
/* Calculates using the equation of program 3.2, the Trapezoid Rule */
/*-------------------------------------------------------------------*/
double Trapezoid(double left_endpoint, double right_endpoint, int trapezoid_count, double base_length) {
    int i;
    double final_trapezoid, x;

    final_trapezoid = (f(left_endpoint) + f(right_endpoint)) / 2.0;
    for (i = 1; i <= trapezoid_count - 1; i++) {
        x = left_endpoint + i * base_length;
        if (i == 0 || i == trapezoid_count) {
            final_trapezoid += f(x) / 2.0;
        } else {
            final_trapezoid += f(x);
        }
    }

    final_trapezoid = final_trapezoid * base_length;
    return final_trapezoid;
}   /* Trapezoid */

/* Gets the input of b, a, and n: size of the upper and lower bound */
/* Also the amount of trapezoids */
void Read_input_trapezoid(
                int     my_rank  /* in */,
                int     comm_sz  /* in */,
                double* a        /* out */,
                double* b        /* out */,
                int*    n        /* out */) {
    if(my_rank == 0) {
        printf("Trapezoid");
        printf("\n-------------------------------------------------------------------");
        printf("\nEnter the upper-bound of the interval (b): \n");
        scanf("%lf", b);
        printf("\nEnter the lower-bound of the interval (a): \n");
        scanf("%lf", a);
        printf("\nEnter the number of subintervals (trapezoids) (n): \n");
        scanf("%d", n);
        if (n <= 0) {
            printf("\nThe number of subintervals must be greater than 0. Please try again: \n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    MPI_Bcast(a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(n, 1, MPI_INT, 0, MPI_COMM_WORLD);
}   /* Read_input_trapezoid */

/* #3: Histogram */
/*  */
/*-------------------------------------------------------------------*/
void Read_input_histogram(
                int     my_rank     /* in */, 
                int     comm_sz     /* in */, 
                int*    data_count  /* out */, 
                double* a           /* out */, 
                double* b           /* out */, 
                int*    n           /* out */) {
    if(my_rank == 0) {
        printf("\n\nHistogram");
        printf("\n-------------------------------------------------------------------");
        printf("\nEnter the number of the measurements (data_count): \n");
        scanf("%d", data_count);
        printf("\nEnter the upper-bound for the measurements (b): \n");
        scanf("%lf", b);
        printf("\nEnter the lower-bound for the measurements (a): \n");
        scanf("%lf", a);
        printf("\nEnter the number of the bins (n): \n");
        scanf("%d", n);
        if (n <= 0) {
            printf("\nThe number of bins must be greater than 0. Please try again: \n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    MPI_Bcast(&data_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
}   /* Read_input_histogram */
