
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

int isPrime(int n, int start, int end);
int main(int argc, char* argv[]) {
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int n;
  int* process_results = (int*)malloc(size * sizeof(int));

  FILE* file;
  double start_time = MPI_Wtime();

  if (rank == 0) {
    if (argc < 2) {
      printf("wrong number of arguments entered.");
    }
    n = atoi(argv[1]);

    file = fopen("prob2_report.out", "a");
    fprintf(file, "Number of processes %d\n-------------\n", size);

    if (n <= 3) {
      fprintf(file, "%d is prime\n", n);
      return 0;
    }
  }

  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int start = rank * (sqrt(n) / size);
  int end = (rank + 1) * (sqrt(n) / size);

  int is_prime = isPrime(n, start, end);

  MPI_Gather(&is_prime, 1, MPI_INT, process_results, 1, MPI_INT, 0,
             MPI_COMM_WORLD);

  if (rank == 0) {
    int is_legit_prime = 1;

    for (int i = 0; i < size; i++) {
      fprintf(file, "Local result of %d is ", i);
      int _res = process_results[i];

      if (_res == 0) {
        fprintf(file, "PRIME.\n");

      } else {
        fprintf(file, "not PRIME because it is divisible by %d.\n", _res);
        is_legit_prime = 0;
      }
    }

    fprintf(file, "%d is %sPRIME\n", n, is_legit_prime == 0 ? "not " : "");

    double end_time = MPI_Wtime();

    fprintf(file,
            "Wallclock time elapsed: %.8lf "
            "seconds\n--------------------------\n\n",
            end_time - start_time);
    fclose(file);
  }

  MPI_Finalize();
  return 0;
}

int isPrime(int n, int start, int end) {
  if (start == 0) {
    start = 2;
  }
  for (int i = start; i < end; i++) {
    if (n % i == 0) {
      return i;
    }
  }
  return 0;
}

// 10

//

// 2 * (rank + 1) - > 2 + n/size * (rank + 1)
// 2 -> 4
// 4 -> 6
// 6 -> 8
// 8 -> 10
