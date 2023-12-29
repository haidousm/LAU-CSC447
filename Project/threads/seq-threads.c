#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int BOARD_HEIGHT, BOARD_WIDTH, NUMBER_OF_RUNS;

struct thread_args {
  int i;
  int j;
  int **current_state;
  int **next_state;
};

void *update_state(void *arguments) {
  struct thread_args *args = arguments;
  int i = args->i;
  int j = args->j;
  int **current_state = args->current_state;

  int **next_state = args->next_state;
  int alive_neighbors = 0;
  // calculates number of alive neighbors in 3x3 grid
  for (int k = i - 1; k <= i + 1; k++) {
    for (int l = j - 1; l <= j + 1; l++) {
      if ((k != i || l != j) && (current_state[k][l] == 1)) {
        alive_neighbors++;
      }
    }
  }

  // updates next state accordingly
  switch (alive_neighbors) {
    case 0:
    case 1:
      next_state[i][j] = 0;
      break;
    case 2:
      next_state[i][j] = current_state[i][j];
      break;
    case 3:
      next_state[i][j] = 1;
      break;
    default:
      next_state[i][j] = 0;
      break;
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  // default simulation param. values
  BOARD_HEIGHT = 10;
  BOARD_WIDTH = 10;
  NUMBER_OF_RUNS = 5;
  // flag to display the output
  int print_output = 0;
  // reads in cmd line options such as width, height, etc.
  int opt;
  while ((opt = getopt(argc, argv, ":h:w:t:p")) != -1) {
    switch (opt) {
      case 'h':
        BOARD_HEIGHT = atoi(optarg);
        break;
      case 'w':
        BOARD_WIDTH = atoi(optarg);
        break;
      case 't':
        NUMBER_OF_RUNS = atoi(optarg);
        break;
      case 'p':
        print_output = 1;
        break;
      default:
        abort();
    }
  }
  srand(time(0));
  // inits the current board state randomly
  int **current_state = (int **)malloc(sizeof(int *) * (BOARD_HEIGHT + 2));
  int **next_state = (int **)malloc(sizeof(int *) * (BOARD_HEIGHT + 2));
  pthread_t *threads = malloc(sizeof(pthread_t) * BOARD_WIDTH);

  for (int i = 0; i < BOARD_HEIGHT + 2; i++) {
    current_state[i] = (int *)malloc(sizeof(int) * (BOARD_WIDTH + 2));
    next_state[i] = (int *)malloc(sizeof(int) * (BOARD_WIDTH + 2));
  }

  for (int i = 1; i <= BOARD_HEIGHT; i++) {
    for (int j = 1; j <= BOARD_WIDTH; j++) {
      current_state[i][j] = rand() % (2);
    }
  }

  // copies top row to bottom row and vice versa &
  // copies left column to right ghost column and vice versa
  // hence, its a pacman-like (torus) board which loops around
  for (int i = 1; i <= BOARD_WIDTH; i++) {
    current_state[0][i] = current_state[BOARD_WIDTH - 1][i];
  }

  for (int i = 1; i <= BOARD_HEIGHT; i++) {
    current_state[i][0] = current_state[i][BOARD_HEIGHT - 1];
  }

  clock_t begin = clock();

  for (int i = 0; i < NUMBER_OF_RUNS; i++) {
    // copies top row to bottom row and vice versa &
    // copies left column to right ghost column and vice versa
    // hence, its a pacman-like (torus) board which loops around
    for (int i = 1; i <= BOARD_WIDTH; i++) {
      current_state[0][i] = current_state[BOARD_WIDTH - 1][i];
    }

    for (int i = 1; i <= BOARD_HEIGHT; i++) {
      current_state[i][0] = current_state[i][BOARD_HEIGHT - 1];
    }

    // calculates number of alive neighbors in 3x3 grid
    for (int i = 1; i <= BOARD_HEIGHT; i++) {
      for (int j = 1; j <= BOARD_WIDTH; j++) {
        /* threads setup */
        struct thread_args args;
        args.current_state = current_state;
        args.next_state = next_state;
        args.i = i;
        args.j = j;
        pthread_create(&threads[j - 1], NULL, update_state, (void *)&args);
      }

      for (int j = 1; j <= BOARD_WIDTH; j++) {
        pthread_join(threads[j - 1], NULL);
      }
    }
    // copies next state to current state
    for (int i = 1; i <= BOARD_HEIGHT; i++) {
      for (int j = 1; j <= BOARD_WIDTH; j++) {
        current_state[i][j] = next_state[i][j];
      }
    }

    if (print_output) {
      // clears board for in-place updates on cmd line
      puts("\033[H\033[J");
      for (int i = 1; i <= BOARD_HEIGHT; i++) {
        for (int j = 1; j <= BOARD_WIDTH; j++) {
          printf("%c ", current_state[i][j] == 1 ? 'X' : ' ');
        }
        printf("\n");
      }

      // sleeps so that updates are noticable
      sleep(1);
    }
  }

  if (!print_output) {
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("simulation took %.2f seconds with a %dx%d board after %d runs\n",
           time_spent, BOARD_HEIGHT, BOARD_WIDTH, NUMBER_OF_RUNS);
  }
}
