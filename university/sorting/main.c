#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <omp.h>

int nthrds = 0;

void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int* sort(int* a, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            if (a[i] > a[j]) swap(a+i, a+j);
        }
    }

    return a;
}

int* fill(int* a, int n) {
    for (int i = 0; i < n; ++i) {
        a[i] = rand();
    }

    return a;
}

int* print(int* a, int n) {
    printf("[%d", a[0]);
    for (int i = 1; i < n; ++i) {
        printf(", %d", a[i]);
    }
    printf("]");

    return a;
}

long long get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
  
    return tv.tv_usec + tv.tv_sec*1000000LL;
 }

long seconds(long long t) {
    return (t / 1000000);
}

long useconds(long long t) {
    return (t % 1000000);
}

int* partial_print(int* a, int n, int k) {
    printf("[");
    print(a, n/k);
    for (int i = 1; i < k; ++i) {
        printf(", ");
        print(a + i*n/k, (i+1)*n/k - i*n/k);
        // the length of subarray is ((a + (i+1)*n/k) - (a + i*n/k))
    }
    printf("]");

    return a;
}

int* merge_parts(int* a, int n, int* b, int k, int* c) {
    int* a_end = a + n;
    int* b_end = b + k;

    while ((a < a_end) && (b < b_end)) {
        if (*a < *b) {
            *c++ = *a++;
        } else {
            *c++ = *b++;
        }
    }

    if (!(a < a_end) && b < b_end) {
        while (b < b_end) {
            *c++ = *b++;
        }
    }
    if (!(b < b_end) && a < a_end) {
        while (a < a_end) {
            *c++ = *a++;
        }
    }

    return c;
}

int* merge(int* a, int n, int k) {
    int* b = calloc(n, sizeof(int));
    for (int i = 0; i < k; ++i) {
        merge_parts(a, i*n/k, a + i*n/k, (i+1)*n/k - i*n/k, b);
        memcpy(a, b, ((i+1)*n/k)*sizeof(int));
        memset(b, 0, n*sizeof(int));
    }
    free(b);

    return a;
}

int* partial_sort(int* a, int n, int k) {
#pragma omp parallel
    {
        nthrds = omp_get_num_threads();
#pragma omp for
        for (int i = 0; i < k; ++i) {
            sort(a + i*n/k, (i+1)*n/k - i*n/k);
            // the length of subarray is ((a + (i+1)*n/k) - (a + i*n/k))
        }
    }
    printf("n threads = %d\n", nthrds);
    nthrds = 0;

    return a;
}

long long test_sort(int n, int turns) {
    long long time = 0;
    for (int i = 0; i < turns; ++i) {
        int* a = calloc(n, sizeof(int));
        fill(a, n);

        long long t1 = get_time();
        sort(a, n);
        long long t2 = get_time();

        time += t2 - t1;
        free(a);
    }

    return time/turns;
}

long long test_psort(int n, int k, int turns) {
    long long time = 0;
    for (int i = 0; i < turns; ++i) {
        int* a = calloc(n, sizeof(int));
        fill(a, n);

        long long t1 = get_time();
        partial_sort(a, n, k);
        merge(a, n, k);
        long long t2 = get_time();

        // partial_print(a, n, k); printf("\n\n");

        time += t2 - t1;
        free(a);
    }

    return time/turns;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("usage: ./m n k turns\n");
        return 1;
    }

    srand(time(0));

    int n = atoi(argv[1]);
    int k = atoi(argv[2]);
    int turns = atoi(argv[3]);

    if (k > n) {
        k = n;
        printf("k > n => assumed k = n\n");
    }

    long long tpsort = test_psort(n, k, turns);

    printf("time of part: %lldus\n", tpsort);

    return 0;
}



