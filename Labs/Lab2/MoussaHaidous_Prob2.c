#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

int main(int argc, char* argv[]) {
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int n;
  int* global_arr;
  int n_elems_process;
  int* process_arr;
  float* avgs_arr = (float*)malloc(size * sizeof(float));

  float partial_avg = 0;

  FILE* file;
  double start_time = MPI_Wtime();

  if (rank == 0) {
    if (argc < 2) {
      printf("wrong number of arguments entered.");
    }
    file = fopen(argv[1], "r");
    fscanf(file, "%d", &n);
    global_arr = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
      fscanf(file, "%d", &global_arr[i]);
    }

    n_elems_process = n / size;
    fclose(file);
    file = fopen("report.out", "a");
    fprintf(file, "Number of processes %d\n-------------\n", size);
  }

  MPI_Bcast(&n_elems_process, 1, MPI_INT, 0, MPI_COMM_WORLD);
  process_arr = (int*)malloc(n_elems_process * sizeof(int));
  MPI_Scatter(global_arr, n_elems_process, MPI_INT, process_arr,
              n_elems_process, MPI_INT, 0, MPI_COMM_WORLD);

  for (int i = 0; i < n_elems_process; i++) {
    partial_avg += (float)process_arr[i] / n_elems_process;
  }

  MPI_Gather(&partial_avg, 1, MPI_FLOAT, avgs_arr, 1, MPI_FLOAT, 0,
             MPI_COMM_WORLD);

  if (rank == 0) {
    float total_avg = 0;

    for (int i = 0; i < size; i++) {
      total_avg += avgs_arr[i] / size;

      fprintf(file, "process %d, local avg %f\n", i, avgs_arr[i]);
    }

    fprintf(file, "global avg %f\n-------------\n", total_avg);
  }

  if (rank == 0) {
    free(global_arr);
  }

  free(process_arr);
  free(avgs_arr);

  if (rank == 0) {
    double end_time = MPI_Wtime();

    fprintf(
        file,
        "Wallclock time elapsed: %.8lf seconds\n--------------------------\n\n",
        end_time - start_time);
    fclose(file);
  }

  MPI_Finalize();
  return 0;
}
