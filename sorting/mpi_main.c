#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <mpi.h>
#include <unistd.h>

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

long long test_sort(int n) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int k;
    MPI_Comm_size(MPI_COMM_WORLD, &k);

    long long t1 = get_time();

    if (rank == 0) {
        int local_n = n/k;
        int* a = calloc(n, sizeof(int));
        fill(a, n);

        // send parts to nodes
        for (int i = 1; i < k; ++i) {
            MPI_Send(a + i*n/k,        // buff
                    (i+1)*n/k - i*n/k, // count
                    MPI_INT,           // type
                    i,                 // dest
                    i,                 // tag
                    MPI_COMM_WORLD);   // comm
        }

        sort(a, local_n);

        // recv parts from nodes
        for (int i = 1; i < k; ++i) {
            // recv part
            MPI_Status stat;
            MPI_Recv(a + i*n/k,        // buff
                    (i+1)*n/k - i*n/k, // count
                    MPI_INT,           // type
                    i,                 // source 
                    i,                 // tag
                    MPI_COMM_WORLD,    // comm    
                    &stat);
        }

        merge(a, n, k);

        free(a);

    } else {
        int local_n = (rank+1)*n/k - rank*n/k;
        int* a = calloc(local_n, sizeof(int));
        
        // recv part
        MPI_Status stat;
        MPI_Recv(a,                     // buff
                local_n,                // count
                MPI_INT,                // type
                0,                      // source 
                rank,                   // tag
                MPI_COMM_WORLD,         // comm    
                &stat);                 // stat

        // usleep(rank*1000);

        sort(a, local_n);

        MPI_Send(a,                // buff
                local_n,           // count
                MPI_INT,           // type
                0,                 // dest
                rank,              // tag
                MPI_COMM_WORLD);   // comm

        free(a);
    }

    long long t2 = get_time();

    return t2 - t1;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(0));

    FILE* f = fopen("data/mpi_sort k = 1.data", "w");
    if (!f) {
        printf("bad file\n");
        return 1;
    }

    for (int i = 0; i < 6; ++i) {
        int n = 50000*(i+1);
        long long t = test_sort(n);
        fprintf(f, "%d %lld\n", n, t);
        if (rank == 0) printf("%f%%\n", ((double)(i+1)/6)*100);
    }

    // if (rank == 0) printf("time on n = %d is %ld s. and %ld us.\n", n, seconds(t), useconds(t));

    fclose(f);

    MPI_Finalize();
    return 0;
}



