#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define DATA_SIZE 1000
#define NUM_BINS 10
#define NUM_THREADS 4

/* Global Variables */
double a, b;
int data_count, num;
int histogram[NUM_BINS] = {0};

/* Initialize Mutex */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* struct func */
struct ThreadData {
    int thread_id;
    int start_index;
    int end_index;
    double* measurements;
};

void* calc_Histogram(void* thread_data) {
    struct ThreadData* data = (struct ThreadData*)thread_data;
    // Calculate the range for each bin
    double range = (b - a) / num;
    // Count measurements in each local bin
    for (int i = data -> start_index; i < data -> end_index; i++) {
        int index = (int)((data->measurements[i] - a) / range);
        if (index >= 0 && index < num) {
            // Increment bin count using mutex to avoid race conditions
            pthread_mutex_lock(&mutex);
            histogram[index]++;
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_exit(NULL);
}   /* calc_Histogram */

/* Problem 5.2 */
/*-------------------------------------------------------------------*/
int main() {
    pthread_t threads[NUM_THREADS];
    struct ThreadData thread_data[NUM_THREADS];

    /* input user histogram data */
    printf("Problem 5.2");
    printf("\n-------------------------------------------------------------------");
    printf("\nEnter the number of measurements (data_count): \n");
    scanf("%d", &data_count);
    printf("Enter the lower bound for measurements (a): \n");
    scanf("%lf", &a);
    printf("Enter the upper bound for measurements (b): \n");
    scanf("%lf", &b);
    printf("Enter the amount of bins (num): \n");
    scanf("%d", &num);

    if (num <= 0) {
        printf("Number of bins must be greater than 0.\n");
        exit(-1);
    }

    /* generates random mesurements using time func within the range of [a, b] */
    double* measurements = (double*)malloc(data_count * sizeof(double));
    srand(time(NULL));
    for (int i = 0; i < data_count; i++) {
        measurements[i] = a + (b - a) * ((double)rand() / RAND_MAX); 
    }

    printf("\nMeasurements:\n");
    for (int i = 0; i < data_count; i++) {
        printf("%.2f", measurements[i]);
        if (i < data_count - 1) {
            printf(", "); 
        }
    }
    printf("\n\n");

    int data_per_thread = data_count / NUM_THREADS;
    int remaining_data = data_count % NUM_THREADS;

    int start_index = 0;
    for (int t = 0; t < NUM_THREADS; t++) {
        thread_data[t].thread_id = t;
        thread_data[t].start_index = start_index;
        thread_data[t].end_index = start_index + data_per_thread + (t < remaining_data ? 1 : 0);
        thread_data[t].measurements = measurements;
        start_index = thread_data[t].end_index;
        int rc = pthread_create(&threads[t], NULL, calc_Histogram, (void*)&thread_data[t]);
        if (rc) {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    /* prints histrogram results */
    printf("Display Histogram Data:\n");
    printf("Measurement Range | Count\n");
    for (int i = 0; i < num; i++) {
        double bin_start = a + i * (b - a) / num;
        double bin_end = bin_start + (b - a) / num;
        printf("%.2lf - %.2lf   | %d\n", bin_start, bin_end, histogram[i]);
    }

    free(measurements);
    pthread_mutex_destroy(&mutex);

    return 0;
}   /* main */
