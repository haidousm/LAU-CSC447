
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  FILE *file;
  if (argc < 2) {
    printf("wrong number of arguments entered.");
  }

  file = fopen(argv[1], "r");

  int m1, n1, m2, n2;
  fscanf(file, "%d %d", &m1, &n1);
  int **matrix1 = (int **)malloc(m1 * sizeof(int *));
  for (int i = 0; i < m1; i++) {
    matrix1[i] = (int *)malloc(n1 * sizeof(int));
  }

  for (int i = 0; i < m1; i++) {
    for (int j = 0; j < n1; j++) {
      fscanf(file, "%d", &matrix1[i][j]);
    }
  }

  fscanf(file, "%d %d", &m2, &n2);

  if (n1 != m2) {
    printf("Dimensions Don’t Match");
    return 1;
  }

  int **matrix2 = (int **)malloc(m2 * sizeof(int *));
  for (int i = 0; i < m2; i++) {
    matrix2[i] = (int *)malloc(n2 * sizeof(int));
  }

  for (int i = 0; i < m2; i++) {
    for (int j = 0; j < n2; j++) {
      fscanf(file, "%d", &matrix2[i][j]);
    }
  }

  int **ans = (int **)malloc(m1 * sizeof(int *));
  for (int i = 0; i < m1; i++) {
    ans[i] = (int *)malloc(n2 * sizeof(int));
  }

  for (int i = 0; i < m1; i++) {
    for (int j = 0; j < n2; j++) {
      ans[i][j] = 0;
      for (int k = 0; k < m2; k++) {
        ans[i][j] += matrix1[i][k] * matrix2[k][j];
      }
    }
  }

  file = fopen("prob1.out", "w");
  for (int i = 0; i < m1; i++) {
    for (int j = 0; j < n2; j++) {
      fprintf(file, "%d ", ans[i][j]);
    }
    fprintf(file, "\n");
  }

  for (int i = 0; i < m1; i++) {
    free(matrix1[i]);
  }

  free(matrix1);

  for (int i = 0; i < m2; i++) {
    free(matrix2[i]);
  }

  free(matrix2);

  for (int i = 0; i < m1; i++) {
    free(ans[i]);
  }

  free(ans);

  fclose(file);
  return 0;
}
