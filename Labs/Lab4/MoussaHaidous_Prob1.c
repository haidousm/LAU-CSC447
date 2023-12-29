
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

int isF_x(int n, int start, int end);
int F_x(int J);
int main(int argc, char* argv[]) {
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int A, B, C;
  int* process_results = (int*)malloc(size * sizeof(int));

  FILE* file;
  double start_time = MPI_Wtime();

  if (rank == 0) {
    if (argc < 4) {
      printf("wrong number of arguments entered.");
    }
    A = atoi(argv[1]);
    B = atoi(argv[2]);
    C = atoi(argv[3]);

    file = fopen("prob3_report.out", "a");
    fprintf(file, "Number of processes %d\n-------------\n", size);
  }

  MPI_Bcast(&A, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&B, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&C, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // 250 -> 3070
  // 20 / 4 = 767
  // start -> 767
  // 767 -> 1534
  // 1534 -> 2301
  // 2301 -> end + 1

  // start = rank * B/size

  int start = rank * (B / size);
  int end = (rank + 1) * (B / size);

  if (rank == 0) {
    start = A;
  }

  if (rank == size - 1) {
    end = B + 1;
  }

 
  int _isF_x = isF_x(C, start, end);

  MPI_Gather(&_isF_x, 1, MPI_INT, process_results, 1, MPI_INT, 0,
             MPI_COMM_WORLD);

  if (rank == 0) {
    int ans_exists = 0;
    for (int i = 0; i < size; i++) {
      if (process_results[i] > 0) {
        fprintf(file, "Process %d found J = %d such that F(J) = %d\n", i,
                process_results[i], C);
        ans_exists = 1;
      }
    }

    if (ans_exists == 0) {
      fprintf(file, "There exists no J such that F(J) = %d and %d < J < %d\n",
              C, A, B);
    }

    double end_time = MPI_Wtime();

    fprintf(file,
            "Wallclock time elapsed: %.8lf "
            "seconds\n--------------------------\n\n",
            end_time - start_time);
    fclose(file);
  }
  free(process_results);
  MPI_Finalize();
  return 0;
}

int isF_x(int C, int start, int end) {
  for (int i = start; i < end; i++) {
    if (F_x(i) == C) {
      return i;
    }
  }
  return 0;
}

int F_x(int J) { return (3 * pow(J, 3) + 5 * J + 20); }

// 10

//

// 2 * (rank + 1) - > 2 + n/size * (rank + 1)
// 2 -> 4
// 4 -> 6
// 6 -> 8
// 8 -> 10
