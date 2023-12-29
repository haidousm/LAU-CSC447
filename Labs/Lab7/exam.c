#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define T 16
int global_sum;
int N;
int* arr;

pthread_mutex_t mutex;

void* add_values(void* args) {
    int tid = *((int*)args);

    int start = ceil(N / T) * tid;
    int end = ceil(N / T) * (tid + 1);


    for (int i = start; i < end; i++)
    {
        if (i < 0 || i > N) {

            break; // makes sure we dont access memory that is not ours since we are ceiling above

        }

        pthread_mutex_lock(&mutex); // makes sure no one is accessing the global sum as we add to it
        global_sum += arr[start];
        pthread_mutex_unlock(&mutex); // allows others to add since we're done with it

    }


    return NULL;
}

int main(int argc, char const* argv[]) {

    if (argc < 2) {
        printf("Please enter an input file name\n");
        return 1;
    }

    N = 0;
    FILE* file = fopen(argv[1], "r");
    fscanf(file, "%d", &N);

    arr = (int*)malloc(sizeof(int) * N);

    for (int i = 0; i < N; i++) {
        fscanf(file, "%d", &arr[i]);
    }
    fclose(file);

    global_sum = 0;

    pthread_t threads[T];
    int thread_args[T];
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < N; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, add_values,
            (void*)&thread_args[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("array sum = %d\n", global_sum);
    free(arr);
    pthread_mutex_destroy(&mutex);

    return 0;
}

// b. the critical section in the code is when adding to global sum 
// (not when accessing array since each thread accesses its unique index calculated)
// to avoid that, i used a mutex lock that prevents other threads from accessing the global_sum variable 
// when it is being accessed from some other thread
// as such there will be no inconsistency with the actual value of global_sum

// c. gcc -pthreads code.c -o code
// to run: ./code file_name
