
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

#define NUM_OF_PRIMES 168

int main(int argc, char* argv[]) {
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int n;

  FILE* file;
  double start_time = MPI_Wtime();

  if (rank == 0) {
    if (argc < 2) {
      printf("wrong number of arguments entered.");
    }

    n = atoi(argv[1]);
    file = fopen("prob1_report.txt", "a");
    fprintf(file, "Number of processes %d\n-------------\n", size);
  }

  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int L = floor((double)NUM_OF_PRIMES / size);

  if (rank == size - 1) {
    L = NUM_OF_PRIMES - L * (size - 1);
  }

  int* process_primes = (int*)malloc(L * sizeof(int));

  if (rank == 0) {
    int arr_index = 0;
    process_primes[arr_index++] = 2;

    for (int i = 3; i <= n; i++) {
      int divisible = 1;
      for (int j = 0; j < arr_index; j++) {
        if (i % process_primes[j] != 0) {
          divisible = 0;
        } else {
          divisible = 1;
          break;
        }
      }
      if (!divisible) {
        if (arr_index == L) {
          if (size == 1) {
            break;
          }
          MPI_Send(&i, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
          continue;
        }
        process_primes[arr_index++] = i;
      }
    }

    int terminator = -1;
    if (size != 1) {
      MPI_Send(&terminator, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }

  } else if (rank < size - 1) {
    MPI_Status status;
    int arr_index = 0;
    int num = -1;
    MPI_Recv(&num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
    process_primes[arr_index++] = num;

    int terminator = -1;

    while (num != terminator) {
      MPI_Recv(&num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
      int divisible = 1;
      for (int i = 0; i < arr_index; i++) {
        if (num % process_primes[i] != 0) {
          divisible = 0;
        } else {
          divisible = 1;
          break;
        }
      }

      if (!divisible) {
        if (arr_index == L) {
          MPI_Send(&num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
          continue;
        }
        process_primes[arr_index++] = num;
      }
    }
    MPI_Send(&terminator, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
  } else if (rank == size - 1) {
    MPI_Status status;
    int arr_index = 0;
    int num = -1;
    MPI_Recv(&num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
    process_primes[arr_index++] = num;

    int terminator = -1;

    while (num != terminator) {
      MPI_Recv(&num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);

      int divisible = 1;
      for (int i = 0; i < arr_index; i++) {
        if (num % process_primes[i] != 0) {
          divisible = 0;
        } else {
          divisible = 1;
          break;
        }
      }

      if (!divisible) {
        if (arr_index == L) {
          break;
        }
        process_primes[arr_index++] = num;
      }
    }
  }

  int* all_primes = (int*)malloc(size * L * sizeof(int));

  MPI_Gather(process_primes, L, MPI_INT, all_primes, L, MPI_INT, 0,
             MPI_COMM_WORLD);

  double end_time = MPI_Wtime();

  if (rank == 0) {
    fprintf(file, "Primes Found: ");
    for (int i = 0; i < size * L; i++) {
      fprintf(file, "%d ", all_primes[i]);
    }
    fprintf(file, "\n");
    fprintf(file,
            "Wallclock time elapsed: %.8lf "
            "seconds\n--------------------------\n\n",
            end_time - start_time);
    fclose(file);
  }

  MPI_Finalize();
  return 0;
}
