#include <stdio.h>
#include <stdlib.h>

void mergeSort(int arr[], int l, int r);
void merge(int arr[], int l, int mid, int r);
int main(int argc, char const *argv[]) {
  FILE *file;
  if (argc < 2) {
    printf("wrong number of arguments entered.");
  }

  file = fopen(argv[1], "r");

  int n;
  fscanf(file, "%d", &n);
  int *arr = (int *)malloc(n * sizeof(int));
  for (int i = 0; i < n; i++) {
    fscanf(file, "%d", &arr[i]);
  }

  mergeSort(arr, 0, n - 1);
  file = fopen("prob2.out", "w");
  for (int i = 0; i < n; i++) {
    fprintf(file, "%d ", arr[i]);
  }

  free(arr);

  fclose(file);
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