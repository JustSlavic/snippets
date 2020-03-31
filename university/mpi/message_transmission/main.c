#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int comm_size;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    int i = 0;
    int k = 1;
    int min_size = 8;

    while (k*min_size <= 80000000) {
        char* message = (char*) malloc((size_t) k*min_size);

        if (rank==0) {
            MPI_Status stat;

            double t1 = MPI_Wtime();

            MPI_Send(message,
                    k*min_size,
                    MPI_CHAR,
                    1,
                    k,
                    MPI_COMM_WORLD);

            MPI_Recv(message,
                    k*min_size,
                    MPI_CHAR,
                    1,
                    k,
                    MPI_COMM_WORLD,
                    &stat);

            double t2 = MPI_Wtime();

            printf("%d: got message\n\ttotal size = %d\n\ttime = %f s\n\n", i, k*min_size, (t2-t1)/2);

        } else if (rank==1) {
            MPI_Status stat;

            MPI_Recv(message,
                    k*min_size,
                    MPI_CHAR,
                    0,
                    k,
                    MPI_COMM_WORLD,
                    &stat);

            MPI_Send(message,
                    k*min_size,
                    MPI_CHAR,
                    0,
                    k,
                    MPI_COMM_WORLD);
        }

        k *= 4;
        ++i;
    }

    MPI_Finalize();

    return 0;
}

