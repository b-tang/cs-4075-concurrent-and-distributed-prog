#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h> 

void Read_n(int* n_p, int* local_n_p, int my_rank, int comm_sz, 
      MPI_Comm comm);
void Check_for_error(int local_ok, char fname[], char message[], 
      MPI_Comm comm);
void Read_data(double local_vec1[], double local_vec2[], double* scalar_p,
      int local_n, int my_rank, int comm_sz, MPI_Comm comm);
void Print_vector(double local_vec[], int local_n, int n, char title[], 
      int my_rank, MPI_Comm comm);
double Par_dot_product(double local_vec1[], double local_vec2[], 
      int local_n, MPI_Comm comm);
void Par_vector_scalar_mult(double local_vec[], double scalar, 
      double local_result[], int local_n);

int main(void) {
   int n, local_n;
   double *local_vec1, *local_vec2;
   double scalar;
   double *local_scalar_mult1, *local_scalar_mult2;
   double dot_product;
   int comm_sz, my_rank;
   MPI_Comm comm;
   
   /* MPI Initialization */
   MPI_Init(NULL, NULL);
   comm = MPI_COMM_WORLD;
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

   /* Read_n reads the # of elements of "n" */
   /* Read_data reads local_vec1 & local_vec2 */
   /* Takes both local vectors and scatters both vectors across the processes a & b */
   Read_n(&n, &local_n, my_rank, comm_sz, comm);
   local_vec1 = (double*)malloc(local_n * sizeof(double));
   local_vec2 = (double*)malloc(local_n * sizeof(double));
   local_scalar_mult1 = (double*)malloc(local_n * sizeof(double));
   local_scalar_mult2 = (double*)malloc(local_n * sizeof(double));
   Read_data(local_vec1, local_vec2, &scalar, local_n, my_rank, comm_sz, comm);
   
   /* Print input data */
   if (my_rank == 0) {
      printf("\nScalar_Input: %lf\n", scalar);
   }
   Print_vector(local_vec1, local_n, n, "\nVector_1: ", my_rank, comm);
   Print_vector(local_vec2, local_n, n, "Vector_2: ", my_rank, comm);
   /* Print results */
   
   /* Compute and print dot product */
   dot_product = Par_dot_product(local_vec1, local_vec2, local_n, comm);
   if (my_rank == 0) {
      printf("Dot_Product_Output: %lf\n", dot_product);
      printf("\n");
   }
   
   /* Compute scalar multiplication and print out result */
   Par_vector_scalar_mult(local_vec1, scalar, local_scalar_mult1, local_n);
   Print_vector(local_scalar_mult1, local_n, n, "Scalar_Vector1_Output: ", my_rank, comm);
   Par_vector_scalar_mult(local_vec2, scalar, local_scalar_mult2, local_n);
   Print_vector(local_scalar_mult2, local_n, n, "Scalar_Vector2_Output: ", my_rank, comm);
   
   free(local_scalar_mult2);
   free(local_scalar_mult1);
   free(local_vec2);
   free(local_vec1);
   
   MPI_Finalize();
   return 0;
}

/*-------------------------------------------------------------------*/
void Check_for_error(
                int       local_ok   /* in */, 
                char      fname[]    /* in */,
                char      message[]  /* in */, 
                MPI_Comm  comm       /* in */) {
   int ok;
   
   MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
   if (ok == 0) {
      int my_rank;
      MPI_Comm_rank(comm, &my_rank);
      if (my_rank == 0) {
         fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname, 
               message);
         fflush(stderr);
      }
      MPI_Finalize();
      exit(-1);
   }
}  /* Check_for_error */


/* Get the input of n: size of the vectors, and then calculate local_n according to comm_sz and n */
/* where local_n is the number of elements each process obtains */
/*-------------------------------------------------------------------*/
void Read_n(int* n_p, int* local_n_p, int my_rank, int comm_sz, 
      MPI_Comm comm) {
   if (my_rank == 0) {
      printf("Enter the vector size (n): \n");
      scanf("%d", n_p);
   }

   MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
   *local_n_p = *n_p / comm_sz;
}  /* Read_n */


/* local_vec1 and local_vec2 are the two local vectors of size local_n which the process pertains */
/* process 0 will take the input of the scalar, the two vectors a and b */
/* process 0 will scatter the two vectors a and b across all processes */
/*-------------------------------------------------------------------*/
void Read_data(double local_vec1[], double local_vec2[], double* scalar_p,
      int local_n, int my_rank, int comm_sz, MPI_Comm comm) {
   double* a = NULL;
   int i;
   if (my_rank == 0){
      printf("\nWhat is the scalar? \n");
      scanf("%lf", scalar_p);
   }
   
   MPI_Bcast(scalar_p, 1, MPI_DOUBLE, 0, comm);
   
   /* First Vector Input */
   if (my_rank == 0){
      a = malloc(local_n * comm_sz * sizeof(double));
      printf("\nEnter the first vector: \n");
      for (i = 0; i < local_n * comm_sz; i++) {
         scanf("%lf", &a[i]);
      }
      MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec1, local_n, MPI_DOUBLE, 0, comm);
      free(a);
   } else {
      MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec1, local_n, MPI_DOUBLE, 0, comm);
   }

    /* Second Vector Input */
   if (my_rank == 0){
      a = malloc(local_n * comm_sz * sizeof(double));
      printf("\nEnter the second vector: \n");
      for (i = 0; i < local_n * comm_sz; i++) {
         scanf("%lf", &a[i]);
      }
      MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec2, local_n, MPI_DOUBLE, 0, comm);
      free(a);
   } else {
      MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec2, local_n, MPI_DOUBLE, 0, comm);
   }
}  /* Read_data */

/* The print_vector gathers the local vectors from all processes and print the gathered vector */
/*-------------------------------------------------------------------*/
void Print_vector(double local_vec[], int local_n, int n, char title[], 
      int my_rank, MPI_Comm comm) {
   double* a = NULL;
   int i;
   
   if (my_rank == 0) {
      a = malloc(n * sizeof(double));
      MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm);
      printf("%s\n", title);
      for (i = 0; i < n; i++) {
         printf("%f ", a[i]);
      }
      printf("\n\n");
      free(a);
   } else {
      MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm);
   }
}  /* Print_vector */


/* This function computes and returns the partial dot product of local_vec1 and local_vec2 */
/*-------------------------------------------------------------------*/
double Par_dot_product(double local_vec1[], double local_vec2[], 
      int local_n, MPI_Comm comm) {
   double par_dot_product = 0.0;
   double final_dot_product;
   for (int i = 0; i < local_n; i++) {                
      par_dot_product += local_vec1[i] * local_vec2[i];
   }
   MPI_Reduce(&par_dot_product, &final_dot_product, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
   return final_dot_product;
}  /* Par_dot_product */


/* This function gets the vector which is the scalar times local_vec, and put the vector into local_result */
/*-------------------------------------------------------------------*/
void Par_vector_scalar_mult(double local_vec[], double scalar, 
      double local_result[], int local_n) {
   for (int i = 0; i < local_n; i++) {
      local_result[i] = local_vec[i] * scalar;
   }
   // return local_result[i];
}  /* Par_vector_scalar_mult */
