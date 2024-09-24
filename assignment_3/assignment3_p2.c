#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#include <omp.h>

/* Problem 3.1 */
/* This program parallelizes the for loop and reduction clause to 
count darts within the circle using OpenMP. The amount of circle hits
and throws are specified as long long int to handle huge numbers, as asked. 
Rand() function creates -1 to 1 random numbers. After linking all threads, 
pi is approximated and printed. */
/*-------------------------------------------------------------------*/
int main(void) {
    /* Local Variables */
    long long int number_of_tosses, number_in_circle = 0;

    /* Input */
    printf("Problem 3.1");
    printf("\n-------------------------------------------------------------------");
    printf("\nEnter the number of tosses: ");
    scanf("%lld", &number_of_tosses);

    /* Random seed generate new sequences of psuedo random num */
    srand(time(0));

    /* Toss variable goes through for loop to rand gen sequences */
    #pragma omp parallel for reduction(+:number_in_circle)
    for (long long int toss = 0; toss < number_of_tosses; toss++) {
        /* Random num between -1 and 1 */
        double x = (double)rand() / RAND_MAX * 2 - 1; 
        double y = (double)rand() / RAND_MAX * 2 - 1; 
        double distance_squared = x * x + y * y;
        if (distance_squared <= 1) {
            number_in_circle++;
        }
    }

    /* Print Results */
    double pi_estimate = 4 * number_in_circle / ((double)number_of_tosses);
    printf("\nThe estimated value of PI is: %lf\n", pi_estimate);

    return 0;
}   /* main */
