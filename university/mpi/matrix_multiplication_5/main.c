#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<unistd.h>
#include<mpi.h>

void print_m(const double* a, const int n, const int m) {
    printf("---matrix---\n");
    int i, j;
    for (i = 0; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            printf("%6.3f ", *(a + i*n + j));
        }
        printf("\n");
    }
    printf("------------\n");
}

void multiply_m(const double* a, const double* b, double* res, const int n, 
const int m) {
    int i, j, k;
    
    for (i = 0; i<m; ++i) {
        for (j = 0; j<m; ++j) {
            for (k = 0; k<n; ++k) {
                *(res+i*n+j) += *(a+i*n+k)**(b+j*n+k);
            }
        }
    }
}

long long get_useconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_usec + tv.tv_sec*1000000LL;
}

void init_m(double* a, double* b, int n, int m, int rank) {
    int i, j;
    for (i = 0; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            *(a + i*n + j) = i + rank*m + 1;
            *(b + i*n + j) = 1.0/(i + rank*m + 1);
        }
    }
}

void sendrecv(double* a, int n, int m, int rank) {
    MPI_Status stat;
    
    MPI_Sendrecv_replace(
        a,
        n*m/2,
        MPI_DOUBLE,
        (rank + 1) % 3,
        (rank + 1) % 3,
        (rank + 2) % 3,
        rank,
        MPI_COMM_WORLD,
        &stat);

    MPI_Sendrecv_replace(
        a + n*m/2,
        n*m/2,
        MPI_DOUBLE,
        (rank + 1) % 3,
        (rank + 1) % 3,
        (rank + 2) % 3,
        rank,
        MPI_COMM_WORLD,
        &stat);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("argument n is expected\n");
        return 1;
    }

    int n = atoi(argv[1]);

    if (n % 3) {
        printf("condition (3 | n) doesn't met\n");
        return 1;
    }

    int m = n / 3;

    MPI_Init(&argc, &argv);

    long long t1 = get_useconds();

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double* a = (double*) malloc(sizeof(double)*n*m);
    double* b = (double*) malloc(sizeof(double)*n*m);
    double* c = (double*) calloc(n*m, sizeof(double));

    init_m(a, b, n, m, rank);

    multiply_m(a, b, c + rank*m, n, m);

    sendrecv(b, n, m, rank);

    multiply_m(a, b, c + ((rank+2) % 3)*m, n, m);

    sendrecv(b, n, m, rank);

    multiply_m(a, b, c + ((rank+1) % 3)*m, n, m);

    if (rank == 0) {
        printf("c[1][1] = %e\n", *c);
        printf("c[1][%d] = %e\n", n, *(c + (n-1)));

        printf("c[1412][8415] = %e\n", *(c + (1412 - 1)*n + (8415-1)));
        printf("c[2605][6312] = %e\n", *(c + (2605 - 1)*n + (6312-1)));
    } else if (rank == 2) {
        printf("c[%d][1] = %e\n", n, *(c + (m-1)*n));
        printf("c[%d][%d] = %e\n",n, n, *(c + (m-1)*n + (n-1)));
        
        printf("c[10072][11340] = %e\n", *(c + (72 - 1)*n + (11340-1)));
    }

    free(a);
    free(b);
    free(c);

    long long t2 = get_useconds();

    if (rank == 0)
        printf("time = %llds., %lldus.\n", (t2 - t1)/1000000, (t2 - t1)%1000000);

    MPI_Finalize();

    return 0;
}
