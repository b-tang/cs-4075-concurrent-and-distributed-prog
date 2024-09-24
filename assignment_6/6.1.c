#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

double a, b, h, sum = 0.0;
int num, thread_num;
pthread_mutex_t mutex;

/* func */
double f(double x) {
    return x * x;
}

/* func: thread func executes each thread */
void* thread_func(void* rank) {
    long my_rank = (long) rank;
    double local_a = a + my_rank * h * (num / thread_num);
    double local_b = local_a + h * (num / thread_num);
    double local_sum = 0.0;

    for (int i = 0; i < num / thread_num; i++) {
        double x_i = local_a + i * h;
        local_sum += (f(x_i) + f(x_i + h)) / 2.0;
    }

    local_sum *= h;

    pthread_mutex_lock(&mutex);
    sum += local_sum;
    pthread_mutex_unlock(&mutex);

    return NULL;
}   /* thread_func */

int main() {
    /* User Input */
    printf("Problem 6.1");
    printf("\n-------------------------------------------------------------------\n");
    printf("Input the lower bound: ");
    scanf("%lf", &a);
    printf("Input the upper bound: ");
    scanf("%lf", &b);
    printf("Input the number of threads: ");
    scanf("%d", &thread_num);
    printf("Input the value of num (trapezoids): ");
    scanf("%d", &num);

    h = (b - a) / num;

    /* Initialization */
    pthread_mutex_init(&mutex, NULL);

    /* Create/Join threads */
    pthread_t threads[thread_num];
    long thread;
    for (thread = 0; thread < thread_num; thread++) {
        pthread_create(&threads[thread], NULL, thread_func, (void*) thread);
    }
    for (thread = 0; thread < thread_num; thread++) {
        pthread_join(threads[thread], NULL);
    }

    /* Close Mutex */
    pthread_mutex_destroy(&mutex);

    /* Output */
    printf("\nSum: %f\n", sum);
    return 0;
}   /* main */
