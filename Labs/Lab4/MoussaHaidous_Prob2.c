
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

#define PI 3.14159265358979323846
#define N 1e8

int main(int argc, char* argv[]) {
  int rank, size;

  double x, y;

  int process_sum = 0, total_sum = 0;

  double result = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  FILE* file;
  double start_time = MPI_Wtime();

  srand(rank + 1);

  for (int i = rank; i < N; i += size) {
    x = ((double)rand()) / RAND_MAX;
    y = ((double)rand()) / RAND_MAX;

    if (x * x + y * y < 1.0) process_sum++;
  }

  MPI_Reduce(&process_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    result = 4 * total_sum * (1 / N);

    double end_time = MPI_Wtime();
    file = fopen("prob3_report.out", "a");
    fprintf(file, "Number of processes %d\n-------------\n", size);
    fprintf(file, "Approximated PI: %.5f, True PI: %.5f\n", result, PI);

    fprintf(file,
            "Wallclock time elapsed: %.8lf "
            "seconds\n--------------------------\n\n",
            end_time - start_time);
    fclose(file);
  }

  MPI_Finalize();
  return 0;
}
