/* File:     
 *     4.3_pth_producer_comsumer.c
 *
 * Purpose:  
 *     Implement producer-consumer synchronization with two threads using 
 *        a mutex
 *
 * Input:
 *     None
 *
 * Output:
 *     message
 *
 * Compile:  gcc -g -Wall -o 4.3_pth_producer_consumer 
 *              4.3_pth_producer_consumer.c -lpthread
 * Usage:
 *     4.3_pth_producer_consumer
 *
 * Notes:  
 *	- rank 1 create a message
 *	- rank 0 print out message
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX_STRING = 99;

int thread_count;
int msg = 0;
char* message;
pthread_mutex_t mutex;

void Get_args(int argc, char* argv[]);
void Usage(char* progname);

/* Thread Function */
void* Thread_work(void* rank);

/*-----------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	long thread;
	pthread_t* thread_handles;
	message = malloc(MAX_STRING*sizeof(char));

	/* Get number of threads from command line */
	Get_args(argc, argv);

	/* allocate array for threads */
	thread_handles = malloc(thread_count * sizeof(pthread_t));

	/* initialize mutex */
	pthread_mutex_init(&mutex, NULL);

	/* start threads */
	long temp_is[thread_count];
	pthread_mutex_init(&mutex, NULL);
	for (long i = 0; i < thread_count; ++i) {
		temp_is[i] = i;
		pthread_create(&(thread_handles[i]), NULL, Thread_work, &temp_is[i]);
	}
	// Wait for the threads to finish.
	for (long j = 0; j < thread_count; ++j) {
		pthread_join(thread_handles[j], NULL);
	}

	pthread_mutex_destroy(&mutex);

	free(message);
	free(thread_handles);
	return 0;
}	/* main */

/*-------------------------------------------------------------------
 * Function:    Thread_work
 * Purpose:     Producer rank 1: create msg
 * Consumer rank 0: print out msg
 * In arg:      rank
 * Global var:  mutex, msg, message
 */
void* Thread_work(void* rank) {
	long my_rank = *(long *)rank;
	while (1) {
		pthread_mutex_lock(&mutex);
		if (my_rank % 2 != 0) {
			if (msg) {
				printf("Th %ld > message: %s\n", my_rank, message);
				msg = 0;
				pthread_mutex_unlock(&mutex);
				break;
			}
		}
		else {
			if (!msg) {
				sprintf(message, "hello from %ld", my_rank);
				msg = 1;
				pthread_mutex_unlock(&mutex);
				break;
			}
		}
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
} 	/*Thread_work*/

/*--------------------------------------------------------------------
 * Function:    Usage
 * Purpose:     Print command line for function and terminate
 * In arg:      prog_name
 */
void Usage(char *prog_name) {
	fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
	exit(0);
}	/* Usage */

/*------------------------------------------------------------------
 * Function:    Get_args
 * Purpose:     Get the command line args
 * In args:     argc, argv
 * Globals out: thread_count
 */
void Get_args(int argc, char *argv[]) {
	if (argc != 2)
		Usage(argv[0]);
	thread_count = strtol(argv[1], NULL, 10);
	if (thread_count <= 0)
		Usage(argv[0]);
}	/* Get_args */
