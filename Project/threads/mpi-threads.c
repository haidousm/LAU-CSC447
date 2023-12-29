
#include <mpi.h>
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

int main(int argc, char **argv) {
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

  int rank, size;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // calculates the number of rows each process will receive
  int process_board_height = BOARD_HEIGHT / size;
  if (rank == size - 1) {
    // last process will receive the remainder of rows if board height cant be
    // evenly divided upon processes
    process_board_height += BOARD_HEIGHT % size;
  }

  // sets up state arrays in addition to allowing space for "ghost" rows and
  // columns
  int **current_state =
      (int **)malloc(sizeof(int *) * (process_board_height + 2));
  int **next_state = (int **)malloc(sizeof(int *) * (process_board_height + 2));

  for (int i = 0; i < process_board_height + 2; i++) {
    current_state[i] = (int *)malloc(sizeof(int) * (BOARD_WIDTH + 2));
    next_state[i] = (int *)malloc(sizeof(int) * (BOARD_WIDTH + 2));
  }

  pthread_t *threads = malloc(sizeof(pthread_t) * BOARD_WIDTH);

  // inits the current board state randomly for each process

  for (int i = 1; i <= process_board_height; i++) {
    for (int j = 1; j <= BOARD_WIDTH; j++) {
      current_state[i][j] = rand() % 2;
    }
  }

  // to enable looping around board
  int previous_process = rank == 0 ? size - 1 : rank - 1;
  int next_process = rank == size - 1 ? 0 : rank + 1;

  double start_time, end_time;
  if (rank == 0) {
    start_time = MPI_Wtime();
  }

  for (int i = 0; i < NUMBER_OF_RUNS; i++) {
    /*
     * ghost rows/columns setup
     */

    // sends top row to previous process
    MPI_Send(current_state[1], BOARD_WIDTH + 2, MPI_INT, previous_process, 0,
             MPI_COMM_WORLD);

    // sends bottom row to next process
    MPI_Send(current_state[process_board_height], BOARD_WIDTH + 2, MPI_INT,
             next_process, 0, MPI_COMM_WORLD);

    // receives top ghost row from previous process
    MPI_Recv(current_state[0], BOARD_WIDTH + 2, MPI_INT, previous_process, 0,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // receives bottom ghost row from next process
    MPI_Recv(current_state[process_board_height + 1], BOARD_WIDTH + 2, MPI_INT,
             next_process, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // copies left column to right ghost column and vice versa
    // hence, its a pacman-like (torus) board which loops around
    for (int i = 0; i <= process_board_height + 1; i++) {
      current_state[i][0] = current_state[i][BOARD_WIDTH];
      current_state[i][BOARD_WIDTH + 1] = current_state[i][1];
    }

    for (int i = 1; i <= process_board_height; i++) {
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
    for (int i = 1; i <= process_board_height; i++) {
      for (int j = 1; j <= BOARD_WIDTH; j++) {
        current_state[i][j] = next_state[i][j];
      }
    }
    if (print_output) {
      // todo - if time permits
    }
  }
  if (rank == 0) {
    if (print_output == 0) {
      double time_spent = MPI_Wtime() - start_time;

      printf("simulation took %.2f seconds with a %dx%d board after %d runs\n",
             time_spent, BOARD_HEIGHT, BOARD_WIDTH, NUMBER_OF_RUNS);
    }
  }

  MPI_Finalize();
  return 0;
}