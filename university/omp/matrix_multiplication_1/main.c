#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<omp.h>

int num_threads = 0;

void multiply_m(const double* a, const double* b, double* res, const int n) {
    int i, j, k;
#pragma omp parallel
    {
#pragma omp single
        num_threads = omp_get_num_threads();

#pragma omp for private(j, k) schedule(runtime)
        for (i = 0; i<n; ++i) {
            for (j = 0; j<n; ++j) {
                double sum = 0;
                for (k = 0; k<n; ++k) {
                    sum += *(a+i*n+k)**(b+j*n+k);
                }
                *(res+i*n+j) = sum;
            }
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

    double* a = (double*) malloc(sizeof(double)*n*n);
    double* b = (double*) malloc(sizeof(double)*n*n);
    double* c = (double*) malloc(sizeof(double)*n*n);

    /// fill the matrix
    int i,j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            *(a + i*n + j) = i + 1;
            *(b + j*n + i) = 1.0/(j + 1);
        }
    }

    long long t1 = get_useconds();
    multiply_m(a, b, c, n);
    long long t2 = get_useconds();

    printf("c[1][1] = %e\n", *c);
    printf("c[1][%d] = %e\n", n, *(c + (n - 1)));
    printf("c[%d][1] = %e\n", n, *(c + n*(n - 1)));
    printf("c[%d][%d] = %e\n", n, n, *(c + n*(n - 1) + (n - 1)));

    printf("time = %lld s. %lld us.\n", (t2 - t1) / 1000000, (t2 - t1) % 1000000);
    printf("n threads = %d\n", num_threads);

    free(a);
    free(b);
    free(c);

    return 0;
}
