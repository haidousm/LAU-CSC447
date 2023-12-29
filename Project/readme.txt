/*
 * Sequential Code
 * How to run
 * gcc seq.c -o seq
 * ./seq -w BOARD_WIDTH -h BOARD_HEIGHT -t NUMBER_OF_RUNS -p
 * all above values of have default values if unentered
 * "-p" option is if you'd like an output of the board as it updates (will
 * slowdown program)
 */

/*
 * Parallel Code
 * How to run
 * mpicc mpi.c -o mpi
 * mpirun mpi -n NUM_PROCESSES -w BOARD_WIDTH -h BOARD_HEIGHT -t NUMBER_OF_RUNS
 * all above values of have default values if unentered
 */
