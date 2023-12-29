
#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

void mergeSort(int arr[], int l, int r);
void merge(int arr[], int l, int mid, int r);

int main(int argc, char* argv[]) {
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int n;
  int* unsorted_arr;
  int* sorted_arr;

  int n_elems_process;
  int* process_arr;

  FILE* file;
  double start_time = MPI_Wtime();

  if (rank == 0) {
    if (argc < 2) {
      printf("wrong number of arguments entered.");
    }
    file = fopen(argv[1], "r");
    fscanf(file, "%d", &n);

    if (n % size != 0) {
      printf("Number of elements cannot be evenly divided to all processes");
    }

    unsorted_arr = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
      fscanf(file, "%d", &unsorted_arr[i]);
    }

    n_elems_process = n / size;
    fclose(file);
    file = fopen("prob1_report.out", "a");
    fprintf(file, "Number of processes %d\n-------------\n", size);

    sorted_arr = (int*)malloc(n * sizeof(int));
  }

  MPI_Bcast(&n_elems_process, 1, MPI_INT, 0, MPI_COMM_WORLD);
  process_arr = (int*)malloc(n_elems_process * sizeof(int));
  MPI_Scatter(unsorted_arr, n_elems_process, MPI_INT, process_arr,
              n_elems_process, MPI_INT, 0, MPI_COMM_WORLD);

  mergeSort(process_arr, 0, n_elems_process - 1);

  MPI_Gather(process_arr, n_elems_process, MPI_INT, sorted_arr, n_elems_process,
             MPI_INT, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    mergeSort(sorted_arr, 0, n - 1);
    for (int i = 0; i < n; i++) {
      printf("%d ", sorted_arr[i]);
    }

    double end_time = MPI_Wtime();

    fprintf(
        file,
        "Wallclock time elapsed: %.8lf seconds\n--------------------------\n\n",
        end_time - start_time);
    fclose(file);
  }

  free(unsorted_arr);
  free(sorted_arr);
  free(process_arr);

  MPI_Finalize();
  return 0;
}

void mergeSort(int arr[], int l, int r) {
  if (l < r) {
    int mid = l + (r - l) / 2;
    mergeSort(arr, l, mid);
    mergeSort(arr, mid + 1, r);
    merge(arr, l, mid, r);
  }
}

void merge(int arr[], int l, int mid, int r) {
  int n1 = mid - l + 1;
  int n2 = r - mid;
  int leftHalf[n1], rightHalf[n2];
  for (int i = 0; i < n1; i++) {
    leftHalf[i] = arr[l + i];
  }
  for (int j = 0; j < n2; j++) {
    rightHalf[j] = arr[mid + 1 + j];
  }

  int i = 0;
  int j = 0;
  int k = l;

  while (i < n1 && j < n2) {
    if (leftHalf[i] <= rightHalf[j]) {
      arr[k] = leftHalf[i];
      i++;

    } else {
      arr[k] = rightHalf[j];
      j++;
    }

    k++;
  }

  while (i < n1) {
    arr[k] = leftHalf[i];
    i++;
    k++;
  }
  while (j < n2) {
    arr[k] = rightHalf[j];
    j++;
    k++;
  }
}