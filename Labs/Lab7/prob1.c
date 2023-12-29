#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int totalSum = 0;
int* u;
int* v;
pthread_mutex_t mutex1;

void* slave(void* index) {
  int i = *((int*)index);

  int a = u[i];
  int b = v[i];

  pthread_mutex_lock(&mutex1);
  totalSum += (a * b);
  pthread_mutex_unlock(&mutex1);
  return NULL;
}

int main(int argc, char** argv) {
  int N;
  FILE* file = fopen("Problem1.txt", "r");

  fscanf(file, "%d", &N);

  u = (int*)malloc(N * sizeof(int));
  v = (int*)malloc(N * sizeof(int));

  for (int i = 0; i < N; ++i) {
    fscanf(file, "%d", &u[i]);
  }
  for (int i = 0; i < N; ++i) {
    fscanf(file, "%d", &v[i]);
  }

  fclose(file);
  int totalSum = 0;
  pthread_mutex_init(&mutex1, NULL);

  int index[N];
  pthread_t threads[N];
  for (int i = 0; i < N; i++) {
    printf("done\n");
    index[i] = i;
    printf("In main: creating thread %d\n", i);
    if (pthread_create(&threads[i], NULL, slave, (void*)&index[i]) != 0)
      printf("Pthread_create fails");
    ;
  }

  for (int i = 0; i < N; ++i) {
    pthread_join(threads[i], NULL);
  }
  printf("Scalar Product = %d\n", totalSum);
  pthread_mutex_destroy(&mutex1);
}
