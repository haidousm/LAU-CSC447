#include <stdio.h>
#include <stdlib.h>

#include "math.h"
#include "mpi.h"

#define N 1000

int main(int argc, char* argv[]) {
  int size, rank;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int M = N / size;
  double local_sum = 0;
  double global_sum = 0;

  int start = rank * M;
  int end = (rank + 1) * M;

  for (int i = start; i < end; i++) {
    float x = i % 2 == 0 ? 1.0 : -1.0;

    local_sum += x / ((2 * i) - 1);
  }

  MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);

  if (rank == 0) {
    double PI = fabs(global_sum) * 4 - 4;
    printf("PI: %f", PI);
  }

  MPI_Finalize();

  return (0);
}