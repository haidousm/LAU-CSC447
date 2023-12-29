
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "pthread_barrier.h"

float *vector_X;
int T, N;
int thread_turn;

pthread_mutex_t mutex;
pthread_cond_t *cond;
pthread_barrier_t barrier;

void *update_component(void *args) {
  int tid = *((int *)args);

  for (int i = 0; i <= T; i++) {
    pthread_mutex_lock(&mutex);
    while (thread_turn != tid) {
      pthread_cond_wait(&cond[tid], &mutex);
    }

    vector_X[tid] =
        (vector_X[tid - 1] + 2 * vector_X[tid] + vector_X[tid + 1]) / 4.0;

    if (thread_turn == N - 2) {
      thread_turn = 0;
    }
    thread_turn++;

    pthread_cond_signal(&cond[thread_turn]);
    pthread_mutex_unlock(&mutex);

    pthread_barrier_wait(&barrier);
  }

  return NULL;
}

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    printf("Please enter an input file name\n");
    return 1;
  }

  FILE *file = fopen(argv[1], "r");
  fscanf(file, "%d %d", &T, &N);

  vector_X = (float *)malloc(N * sizeof(float));

  for (int i = 0; i < N; i++) {
    fscanf(file, "%f", &vector_X[i]);
  }

  fclose(file);

  pthread_t threads[N];
  int thread_args[N];
  cond = (pthread_cond_t *)malloc(N * sizeof(pthread_cond_t));

  pthread_barrier_init(&barrier, NULL, N - 2);
  pthread_mutex_init(&mutex, NULL);

  thread_turn = 1;

  for (int i = 1; i < N - 1; i++) {
    thread_args[i] = i;
    pthread_cond_init(&cond[i], NULL);
    pthread_create(&threads[i], NULL, update_component,
                   (void *)&thread_args[i]);
  }

  pthread_cond_signal(&cond[1]);
  for (int i = 1; i < N - 1; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("Updated values after %d iterations:\n", T);
  for (int i = 0; i < N; i++) {
    printf("%.2f ", vector_X[i]);
    if (i % 10 == 0 && i != 0) {
      printf("\n");
    }
  }

  free(vector_X);
  pthread_barrier_destroy(&barrier);
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(cond);

  return 0;
}
