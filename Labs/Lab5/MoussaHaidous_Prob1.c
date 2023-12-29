#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

int qSortComparator(const void* a, const void* b);

int main(int argc, char* argv[]) {
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  FILE* file;

  int a, b, n;
  int *arr, *small_buckets;
  int* processors_offset = (int*)malloc(size * sizeof(int));

  double start_time = MPI_Wtime();

  if (rank == 0) {
    if (argc < 2) {
      printf("wrong number of arguments entered.");
    }
    file = fopen(argv[1], "r");

    fscanf(file, "%d", &a);
    fscanf(file, "%d", &b);
    fscanf(file, "%d", &n);
  }

  MPI_Bcast(&a, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&b, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  arr = (int*)malloc(n * sizeof(int));

  if (rank == 0) {
    for (int i = 0; i < n; i++) {
      fscanf(file, "%d", &arr[i]);
    }

    fclose(file);
    file = fopen("report_1.txt", "a");
    fprintf(file, "Number of processes %d\n-------------\n", size);
  }

  MPI_Bcast(arr, n, MPI_INT, 0, MPI_COMM_WORLD);

  float magic_number = ceil((float)(b - a + 0.0) / size);

  int start = rank * (n / size);
  int end = (rank + 1) * (n / size);

  for (int i = 0; i < size; i++) {
    processors_offset[i] = i * (int)n / pow(size, 2.0);
  }

  small_buckets = (int*)malloc((n / size) * sizeof(int));
  for (int i = start; i < end; i++) {
    int bucket_index = floor((float)(arr[i] + 0.0) / magic_number);

    int elem_index = processors_offset[bucket_index];

    if (elem_index < (bucket_index + 1) * (int)n / pow(size, 2.0)) {
      small_buckets[elem_index] = arr[i];
      processors_offset[bucket_index]++;
    }
  }

  MPI_Alltoall(small_buckets, (int)n / pow(size, 2.0), MPI_INT, small_buckets,
               (int)n / pow(size, 2.0), MPI_INT, MPI_COMM_WORLD);

  if (rank > 0) {
    for (int i = 0; i < n / size; i++) {
      if (small_buckets[i] == 0) {
        small_buckets[i] = -1;
      }
    }
  }

  qsort(small_buckets, n / size, sizeof(int), qSortComparator);

  for (int i = 0; i < n; i++) {
    arr[i] = -1;
  }

  MPI_Gather(small_buckets, n / size, MPI_INT, arr, n / size, MPI_INT, 0,
             MPI_COMM_WORLD);

  if (rank == 0) {
    double end_time = MPI_Wtime();
    // fprintf(file,
    //         "Wallclock time elapsed: %.8lf "
    //         "seconds\n--------------------------\n\n ",
    //         end_time - start_time);

    for (int i = 0; i < n; i++) {
      if (arr[i] == -1) {
        continue;
      }
      fprintf(file, "%d ", arr[i]);
    }
    fclose(file);
  }

  free(arr);
  free(small_buckets);
  free(processors_offset);

  MPI_Finalize();
  return 0;
}

int qSortComparator(const void* a, const void* b) {
  int _a = *(int*)a;
  int _b = *(int*)b;
  if (_a > _b) {
    return 1;
  } else if (_a < _b) {
    return -1;
  }

  return 0;
}
