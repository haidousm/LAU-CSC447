
#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

int main(int argc, char *argv[]) {
  int rank, size, m1, n1, m2, n2;

  int *matrix1_row;
  int *ans_row;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  double start_time = MPI_Wtime();
  if (rank == 0) {
    FILE *file;
    if (argc < 2) {
      printf("wrong number of arguments entered.");
    }

    file = fopen(argv[1], "r");

    fscanf(file, "%d %d", &m1, &n1);
    fclose(file);
  }

  MPI_Bcast(&m1, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&n1, 1, MPI_INT, 0, MPI_COMM_WORLD);
  int(*matrix1)[n1] = malloc(sizeof *matrix1 * m1);

  if (rank == 0) {
    FILE *file;
    if (argc < 2) {
      printf("wrong number of arguments entered.");
    }

    file = fopen(argv[1], "r");
    int _, __;
    fscanf(file, "%d %d", &_, &__);

    for (int i = 0; i < m1; i++) {
      for (int j = 0; j < n1; j++) {
        fscanf(file, "%d", &matrix1[i][j]);
      }
    }

    fscanf(file, "%d %d", &m2, &n2);

    if (n1 != m2) {
      printf("Dimensions Don’t Match");
      MPI_Finalize();
      return 1;
    }
    fclose(file);
  }

  MPI_Bcast(&m2, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&n2, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int(*matrix2)[n2] = malloc(sizeof *matrix2 * m2);

  if (rank == 0) {
    FILE *file;
    if (argc < 2) {
      printf("wrong number of arguments entered.");
    }

    file = fopen(argv[1], "r");

    int _, __;
    fscanf(file, "%d %d", &_, &__);
    int(*___)[m1] = malloc(sizeof *___ * n1);
    for (int i = 0; i < m1; i++) {
      for (int j = 0; j < n1; j++) {
        fscanf(file, "%d", &___[i][j]);
      }
    }

    fscanf(file, "%d %d", &_, &__);
    for (int i = 0; i < m2; i++) {
      for (int j = 0; j < n2; j++) {
        fscanf(file, "%d", &matrix2[i][j]);
      }
    }
  }

  MPI_Bcast(matrix2, m2 * n2, MPI_INT, 0, MPI_COMM_WORLD);

  matrix1_row = (int *)malloc(m1 * sizeof(int));
  ans_row = (int *)malloc(n2 * sizeof(int));

  MPI_Scatter(matrix1, m1 * n1 / size, MPI_INT, matrix1_row, m1 * n1 / size,
              MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  int sum = 0;
  for (int i = 0; i < m1; i++) {
    for (int j = 0; j < n2; j++) {
      sum += matrix1_row[j] * matrix2[j][i];
    }

    ans_row[i] = sum;
    sum = 0;
  }

  int(*ans)[n2] = malloc(sizeof *ans * m1);

  MPI_Gather(ans_row, m1 * n2 / size, MPI_INT, ans, m1 * n2 / size, MPI_INT, 0,
             MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();

  if (rank == 0) {
    FILE *file;
    file = fopen("report.out", "w");
    fprintf(file, "Number of processes %d\n-------------\n", size);
    double end_time = MPI_Wtime();
    fprintf(
        file,
        "Wallclock time elapsed: %.8lf seconds\n--------------------------\n\n",
        end_time - start_time);

    for (int i = 0; i < m1; i++) {
      for (int j = 0; j < n2; j++) {
        fprintf(file, "%d ", ans[i][j]);
      }
      fprintf(file, "\n");
    }

    fclose(file);
  }
}

// Wallclock time elapsed: 0.16758600 seconds
// Wallclock time elapsed: 0.15559700 seconds