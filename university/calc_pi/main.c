#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<sys/time.h>
#include<omp.h>


double get_integral(unsigned n) {
    double sum = 0;

    int i;
#pragma omp parallel
{
    int num_threads = omp_get_num_threads();
#pragma omp for reduction(+:sum)
    for (i = 0; i<n; ++i) {
        double tmp = (i+0.5)/n;

        sum += 1/(1+tmp*tmp);
    }
}

    return 4 * sum / n;
}

long long get_useconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_usec + tv.tv_sec*1000000LL;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: ./pi N\n");
        return 1;
    }

    unsigned N = atoi(argv[1]);

    long long t1 = get_useconds();
    double my_pi = get_integral(N);
    long long t2 = get_useconds();

    printf("pi = %e\napi = %e\nerr = %e\ntime = %llds %lldus.\n",
            M_PI, my_pi, fabs(M_PI - my_pi), (t2 - t1) / 1000000, (t2 - t1) % 1000000);

    return 0;
}
