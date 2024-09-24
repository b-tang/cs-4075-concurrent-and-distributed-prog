#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

/* struct func: holds specific thread data */
typedef struct {
    int thread_id;
    int thread_num;
    int num;
    int* A;
    int* x;
    int* local_y;
} ThreadData;

/* func: matrix-vector multiplication
 * for loop executes matrix-vec multiplication
 */
/*-------------------------------------------------------------------*/
void* matrixVectorMult(void* thread_data) {
    ThreadData* data = (ThreadData*)thread_data;
    int start = data -> thread_id * (data -> num / data -> thread_num);
    int end = (data -> thread_id + 1) * (data -> num / data -> thread_num);

    for (int i = start; i < end; i++) {
        data -> local_y[i - start] = 0;
        for (int j = 0; j < data -> num; j++) {
            data -> local_y[i - start] += data -> A[i * data -> num + j] * data->x[j];
        }
    }

    pthread_exit(NULL);
} /* matrixVectorMult */

/* func: executes parallel matrix-vector multiplication
 * use variables threads/thread_data to allocate memory
 * then initialize/create thread and thread data in a for loop
 * join threads and allocate the results to get the final y
*/
/*-------------------------------------------------------------------*/
void parallelMatrixVectorMult(int num, int thread_num, int* A, int* x, int* y, int* local_y) {
    pthread_t* threads;
    ThreadData* thread_data;

    threads = (pthread_t*)malloc(thread_num * sizeof(pthread_t));
    thread_data = (ThreadData*)malloc(thread_num * sizeof(ThreadData));

    for (int i = 0; i < thread_num; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].thread_num = thread_num;
        thread_data[i].num = num;
        thread_data[i].A = A;
        thread_data[i].x = x;
        thread_data[i].local_y = local_y + i * (num / thread_num);
        pthread_create(&threads[i], NULL, matrixVectorMult, (void *)&thread_data[i]);
    }

    for (int i = 0; i < thread_num; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < num; i++) {
        y[i] = 0;
        for (int j = 0; j < thread_num; j++) {
            int start = j * (num / thread_num) + (j < num % thread_num ? j : num % thread_num);
            int end = start + (num / thread_num) + (j < num % thread_num ? 1 : 0);
            if (i >= start && i < end) {
                y[i] += local_y[i - start];
            }
        }
    }

    free(threads);
    free(thread_data);
} /* parallelMatrixVectorMult */

/* Problem 5.1 */
/*-------------------------------------------------------------------*/
int main() {
    int num, thread_num;
    int* A, *x, *y, *local_y;

    /* input the size of the matrix-vector and threads */
    printf("Problem 5.1");
    printf("\n-------------------------------------------------------------------");
    printf("\nPlease input the size (num) for the matrix vector: \n");
    scanf("%d", &num);
    printf("Please input the number of threads: \n");
    scanf("%d", &thread_num);

    if (num <= 0 || thread_num <= 0) {
        printf("Invalid input. Please enter positive values for n and thread_num.\n");
        return 1;
    }

    A = (int *)malloc(num * num * sizeof(int));
    x = (int *)malloc(num * sizeof(int));
    y = (int *)malloc(num * sizeof(int));
    local_y = (int *)malloc(num * sizeof(int));

    /* initialize random int */
    srand(time(NULL));
    for (int i = 0; i < num * num; i++) {
        A[i] = (rand() % 5) + 1;
    }
    for (int i = 0; i < num; i++) {
        x[i] = (rand() % 4) + 1;
    }

    printf("\nMatrix A:\n");
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            printf("%d\t", A[i * num + j]);
        }
        printf("\n");
    }
    printf("\nVector x:\n");
    for (int i = 0; i < num; i++) {
        printf("%d\n", x[i]);
    }

    parallelMatrixVectorMult(num, thread_num, A, x, y, local_y);

    /* print vector y results */
    printf("\nResult vector y:\n");
    for (int i = 0; i < num; i++) {
        printf("%d\n", y[i]);
    }

    free(A);
    free(x);
    free(y);
    free(local_y);

    return 0;
} /* main */
