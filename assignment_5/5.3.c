#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUMBER_OF_THREADS 4

/* Global Variables */
long long int number_in_circle = 0;
long long int number_of_tosses;

/* Initialize Mutex */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

/* monteCarlo method */
void* monteCarlo(void* arg) {
    long long int local_num_in_circle = 0;
    unsigned int seed = (unsigned int)time(NULL);
    for (long long int toss = 0; toss < number_of_tosses; toss++) {
        double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        double distance_squared = x * x + y * y;
        if (distance_squared <= 1.0) {
            local_num_in_circle++;
        }
    }
    pthread_mutex_lock(&mutex);
    number_in_circle += local_num_in_circle;
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}   /* monteCarlo */

/* Problem 5.3 */
/*-------------------------------------------------------------------*/
int main() {
    pthread_t threads[NUMBER_OF_THREADS];

    /* Input */
    printf("Problem 5.3");
    printf("\n-------------------------------------------------------------------");
    printf("\nEnter the number of tosses: ");
    scanf("%lld", &number_of_tosses);

    /* pthread create/join */
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_create(&threads[i], NULL, monteCarlo, NULL);
    }
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Print estimate of PI results */
    double pi_estimate = 4.0 * number_in_circle / (double)(NUMBER_OF_THREADS * number_of_tosses);
    printf("The estimated value of PI is: %lf\n", pi_estimate);

    return 0;
}   /* main */
