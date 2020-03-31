#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<mpi.h>
#include<string.h>
#include<unistd.h>

void print_m(const double* a, const int n, const int m) {
    printf("---matrix---\n");
    int i, j;
    for (i = 0; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            printf("%4.1f ", *(a + i*n + j));
        }
        printf("\n");
    }
    printf("------------\n");
}

void multiply_m(const double* a, const double* b, double* res, const int n, const int m) {
    int i, j, k;
    
    for (i = 0; i<m; ++i) {
        for (j = 0; j<m; ++j) {
            double sum = 0;
            for (k = 0; k<n; ++k) {
                sum += *(a+i*n+k)**(b+j*n+k);
            }
            *(res+i*n+j) = sum;
        }
    }
}

long long get_useconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_usec + tv.tv_sec*1000000LL;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("argument n is expected\n");
        return 1;
    }

    int n = atoi(argv[1]);

    if (n % 3) {
        printf("condition (3 | n) don't met\n");
        return 1;
    }

    int m = n / 3;

    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        double* a = (double*) malloc(sizeof(double)*n*n);
        double* b = (double*) malloc(sizeof(double)*n*n);

        /// init the matrix
        int i,j;
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                *(a + i*n + j) = i + 1;
                *(b + j*n + i) = 1.0/(j + 1);
            }
        }

        // print_m(a, n, n);
        // print_m(b, n, n);

        long long t1 = get_useconds();

        // divide matrices to 3 parts;
        double* partial_a = (double*) malloc(sizeof(double)*n*m);
        double* partial_b = (double*) malloc(sizeof(double)*n*m);
        double* partial_c = (double*) malloc(sizeof(double)*n*m);
        MPI_Status stat;

   

        // send second part of matrices a to node-1
        // memcpy(partial_a, a + n*m, sizeof(double)*n*m);
        // memcpy(partial_b, b + n*m, sizeof(double)*n*m);
        // MPI_Send(partial_a, n*m, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD);
        // MPI_Send(partial_b, n*m, MPI_DOUBLE, 1, 2, MPI_COMM_WORLD);

        MPI_Send(a + n*m, n*m, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD);
        MPI_Send(b + n*m, n*m, MPI_DOUBLE, 1, 2, MPI_COMM_WORLD);

        //send third part of matrices a to node-2
        // memcpy(partial_a, a + 2*n*m, sizeof(double)*n*m);
        // memcpy(partial_b, b + 2*n*m, sizeof(double)*n*m);
        // MPI_Send(partial_a, n*m, MPI_DOUBLE, 2, 1, MPI_COMM_WORLD);
        // MPI_Send(partial_b, n*m, MPI_DOUBLE, 2, 2, MPI_COMM_WORLD);

        MPI_Send(a + 2*n*m, n*m, MPI_DOUBLE, 2, 1, MPI_COMM_WORLD);
        MPI_Send(b + 2*n*m, n*m, MPI_DOUBLE, 2, 2, MPI_COMM_WORLD);

        // copy first part of matrices to partials
        memcpy(partial_a, a, sizeof(double)*n*m);
        memcpy(partial_b, b, sizeof(double)*n*m);

        free(a);
        free(b);

        long long t2 = get_useconds();

        printf("0: sent parts and free memory in %lld us\n", t2 - t1);

        t1 = get_useconds();

        multiply_m(partial_a, partial_b, partial_c, n, m);

        t2 = get_useconds();
        printf("0: multiply the first part in %lld us\n", t2 - t1);

        t1 = get_useconds();

        MPI_Send(partial_b, n*m, MPI_DOUBLE, 1, 3, MPI_COMM_WORLD);
        printf("0: sent 2 ---------------\n");
        fflush(NULL);
        MPI_Recv(partial_b, n*m, MPI_DOUBLE, 2, 5, MPI_COMM_WORLD, &stat);
        multiply_m(partial_a, partial_b, partial_c + 2*m, n, m);

        t2 = get_useconds();
        printf("0: recv and multiply the second part in %lld us\n", t2 - t1);

        t1 = get_useconds();
        MPI_Send(partial_b, n*m, MPI_DOUBLE, 1, 3, MPI_COMM_WORLD);
        MPI_Recv(partial_b, n*m, MPI_DOUBLE, 2, 5, MPI_COMM_WORLD, &stat);
        multiply_m(partial_a, partial_b, partial_c + m, n, m);

        t2 = get_useconds();
        printf("0: recv and multiply the third part in %lld us\n", t2 - t1);

        t1 = get_useconds();
        double* c = (double*) malloc(sizeof(double)*n*n);
        memcpy(c, partial_c, sizeof(double)*n*m);

        MPI_Recv(partial_c, n*m, MPI_DOUBLE, 1, 6, MPI_COMM_WORLD, &stat);
        memcpy(c + n*m, partial_c, sizeof(double)*n*m);

        MPI_Recv(partial_c, n*m, MPI_DOUBLE, 2, 7, MPI_COMM_WORLD, &stat);
        memcpy(c + 2*n*m, partial_c, sizeof(double)*n*m);

        t2 = get_useconds();
        printf("0: gather all matrix in %lld us\n", t2 - t1);

        printf("c[1][1]   = %e\n", *c);
        printf("c[1][%d]  = %e\n", n, *(c + (n - 1)));
        printf("c[%d][1]  = %e\n", n, *(c + n*(n - 1)));
        printf("c[%d][%d] = %e\n", n, n, *(c + n*(n - 1) + (n - 1)));

        // printf("c[%d][%d] = %e\n", 2605, 6312, *(c + 2605*n + 6312));
        // printf("c[%d][%d] = %e\n", 1412, 8415, *(c + 1412*n + 8415));
        // printf("c[%d][%d] = %e\n", 10072, 11340, *(c + 10072*n + 11340));

        // printf("time = %lld s. %lld us.\n", (t2 - t1) / 1000000, (t2 - t1) % 1000000);

    } else if (rank == 1) {

        double* partial_a = (double*) malloc(sizeof(double)*n*m);
        double* partial_b = (double*) malloc(sizeof(double)*n*m);
        double* partial_c = (double*) malloc(sizeof(double)*n*m);
        MPI_Status stat;

        long long t1 = get_useconds();

        MPI_Recv(partial_a, n*m, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &stat);
        MPI_Recv(partial_b, n*m, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &stat);

        long long t2 = get_useconds();
        printf("1: got parts in %lld us\n", t2 - t1);

        t1 = get_useconds();

        multiply_m(partial_a, partial_b, partial_c + m, n, m);

        t2 = get_useconds();
        printf("1:multiply them in %lld us\n", t2 - t1);

        t1 = get_useconds();

        MPI_Send(partial_b, n*m, MPI_DOUBLE, 2, 4, MPI_COMM_WORLD);
        printf("1: sent 2 ---------------\n");
        fflush(NULL);
        MPI_Recv(partial_b, n*m, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD, &stat);
        multiply_m(partial_a, partial_b, partial_c, n, m);

        t2 = get_useconds();
        printf("1: got 2nd parts and multiply them in %lld us\n", t2 - t1);

        t1 = get_useconds();

        MPI_Send(partial_b, n*m, MPI_DOUBLE, 2, 4, MPI_COMM_WORLD);
        MPI_Recv(partial_b, n*m, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD, &stat);
        multiply_m(partial_a, partial_b, partial_c + 2*m, n, m);

        t2 = get_useconds();
        printf("1: got 3rd parts and multiply them in %lld us\n", t2 - t1);

        MPI_Send(partial_c, n*m, MPI_DOUBLE, 0, 6, MPI_COMM_WORLD);

    } else if (rank == 2) {
        double* partial_a = (double*) malloc(sizeof(double)*n*m);
        double* partial_b = (double*) malloc(sizeof(double)*n*m);
        double* partial_c = (double*) malloc(sizeof(double)*n*m);
        MPI_Status stat;

        long long t1 = get_useconds();

        MPI_Recv(partial_a, n*m, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &stat);
        MPI_Recv(partial_b, n*m, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &stat);

        long long t2 = get_useconds();
        printf("2: got parts in %lld us\n", t2 - t1);

        t1 = get_useconds();

        multiply_m(partial_a, partial_b, partial_c + 2*m, n, m);

        t2 = get_useconds();
        printf("2: multiply them in %lld us\n", t2 - t1);

        t1 = get_useconds();

        MPI_Send(partial_b, n*m, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD);
        printf("2: sent 2 ---------------\n");
        fflush(NULL);
        MPI_Recv(partial_b, n*m, MPI_DOUBLE, 1, 4, MPI_COMM_WORLD, &stat);
        multiply_m(partial_a, partial_b, partial_c + m, n, m);

        t2 = get_useconds();
        printf("2: got 2nd parts and multiply them in %lld us\n", t2 - t1);

        t1 = get_useconds();

        MPI_Send(partial_b, n*m, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD);
        MPI_Recv(partial_b, n*m, MPI_DOUBLE, 1, 4, MPI_COMM_WORLD, &stat);
        multiply_m(partial_a, partial_b, partial_c, n, m);

        t2 = get_useconds();
        printf("2: got 2nd parts and multiply them in %lld us\n", t2 - t1);

        MPI_Send(partial_c, n*m, MPI_DOUBLE, 0, 7, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
