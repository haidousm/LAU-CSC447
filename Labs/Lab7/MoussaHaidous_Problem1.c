#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int scalar_product;
int *vector_A;
int *vector_B;

pthread_mutex_t mutex;

void *multiply_components(void *args) {
  int tid = *((int *)args);
  pthread_mutex_lock(&mutex);
  scalar_product += vector_A[tid] * vector_B[tid];
  pthread_mutex_unlock(&mutex);
  return NULL;
}

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    printf("Please enter an input file name\n");
    return 1;
  }
  int N;
  FILE *file = fopen(argv[1], "r");
  fscanf(file, "%d", &N);

  scalar_product = 0;
  vector_A = (int *)malloc(N * sizeof(int));
  vector_B = (int *)malloc(N * sizeof(int));

  for (int i = 0; i < N; i++) {
    fscanf(file, "%d", &vector_A[i]);
  }
  for (int i = 0; i < N; i++) {
    fscanf(file, "%d", &vector_B[i]);
  }

  fclose(file);

  pthread_t threads[N];
  int thread_args[N];
  pthread_mutex_init(&mutex, NULL);

  for (int i = 0; i < N; i++) {
    thread_args[i] = i;
    pthread_create(&threads[i], NULL, multiply_components,
                   (void *)&thread_args[i]);
  }

  for (int i = 0; i < N; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("S = A.B = %d\n", scalar_product);
  free(vector_A);
  pthread_mutex_destroy(&mutex);

  return 0;
}
