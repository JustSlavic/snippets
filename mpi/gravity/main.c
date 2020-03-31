#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>

#ifdef MPI
#include <mpi.h>
#endif

const double pi = 3.14159265359;
const double G  = 10;

double uniform_distr(double a, double b) {
    return rand()*(b - a)/RAND_MAX + a;
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

size_t array_size(int rank, size_t n) {
    int comm_size = 1;
#ifdef MPI
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
#endif
    return (rank+1)*n/comm_size - rank*n/comm_size;
}

void output(FILE* f, double* z, size_t n) {
    int comm_size = 1;
#ifdef MPI
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
#endif

    int rank = 0;
#ifdef MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

    size_t local_amount_points = array_size(rank, n);
    double* x = z;
    double* y = x + local_amount_points;
    double* vx = y + local_amount_points;
    double* vy = vx + local_amount_points;
    double* m = vy + local_amount_points;

    if (rank == 0) {
        for (int i = 0; i < local_amount_points; ++i) {
            fprintf(f, "%f %f\n", x[i], y[i]);
        }

        for (int node = 1; node < comm_size; ++node) {
            size_t node_size = array_size(node, n);
            double* buffer = calloc(5*node_size, sizeof(double));
            x = buffer;
            y = x + node_size;
            vx = y + node_size;
            vy = vx + node_size;
            m = vy + node_size;

#ifdef MPI
            MPI_Status stat;
            MPI_Recv(buffer,     // buff
                    (int) 5*node_size, // count
                    MPI_DOUBLE,  // type
                    node,        // source 
                    node,        // tag
                    MPI_COMM_WORLD, // comm    
                    &stat);
#endif

            for (int i = 0; i < node_size; ++i) {
                fprintf(f, "%f %f\n", x[i], y[i]);
            }

            free(buffer);
        }
    } else {
#ifdef MPI
        MPI_Send(z,          // buff
            (int) 5*local_amount_points, // count
            MPI_DOUBLE, // type
            0,          // dest
            rank,       // tag
            MPI_COMM_WORLD); // comm
#endif
    }
}

void F(double* z, double* res, size_t n) {
    int comm_size = 1;
#ifdef MPI
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
#endif

    int rank = 0;
#ifdef MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

    size_t local_amount_points = array_size(rank, n);

    double* x = z;
    double* y = x + local_amount_points;
    double* vx = y + local_amount_points;
    double* vy = vx + local_amount_points;
    double* m = vy + local_amount_points;

    double* rx = res;
    double* ry = rx + local_amount_points;
    double* rvx = ry + local_amount_points;
    double* rvy = rvx + local_amount_points;
    double* rm = rvy + local_amount_points;

    for (int node = 0; node < comm_size; ++node) {
        size_t node_size = array_size(node, n);
        double* buffer = calloc(5*node_size, sizeof(double));

        double* bx = buffer;
        double* by = bx + node_size;
        double* bvx = by + node_size;
        double* bvy = bvx + node_size;
        double* bm = bvy + node_size;

        if (rank == node) {
            memcpy(buffer, z, 5*node_size*sizeof(double));
        }

#ifdef MPI
        MPI_Bcast(buffer, //buffer
            (int) 5*node_size, //count of elements
            MPI_DOUBLE, //type of elements
            node,       //source
            MPI_COMM_WORLD); //network
#endif

        for (int i = 0; i < local_amount_points; ++i) {
            rx[i] = vx[i];
            ry[i] = vy[i];

#pragma omp parallel for firstprivate(i, rank, node, node_size, x, y, bx, by, bm, rvx, rvy, G)
            for (int j = 0; j < node_size; ++j) {
                if (rank == node && i == j) continue;

                double dx = bx[j] - x[i];
                double dy = by[j] - y[i];

                double dr_2 = dx*dx + dy*dy;
                double dr_1 = sqrt(dr_2);

                double F = G*bm[j]/(dr_2*dr_1);

                rvx[i] += dx*F;
                rvy[i] += dy*F;
            }
        }
        
        free(buffer);
    }
}

void runge_kutta_step(double* z, size_t n, double h) {
    int comm_size = 1;
#ifdef MPI
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
#endif

    int rank = 0;
#ifdef MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

    size_t local_amount_points = array_size(rank, n);

    double* k_1 = calloc(5*local_amount_points, sizeof(double));
    double* k_2 = calloc(5*local_amount_points, sizeof(double));
    double* k_3 = calloc(5*local_amount_points, sizeof(double));
    double* k_4 = calloc(5*local_amount_points, sizeof(double));

    double* arg = calloc(5*local_amount_points, sizeof(double));

    F(z, k_1, n);

    for (int i = 0; i < 5*local_amount_points; ++i)
        arg[i] = z[i] + h*k_1[i]/2;

    F(arg, k_2, n);

    for (int i = 0; i < 5*local_amount_points; ++i)
        arg[i] = z[i] + h*k_2[i]/2;

    F(arg, k_3, n);

    for (int i = 0; i < 5*local_amount_points; ++i)
        arg[i] = z[i] + h*k_3[i];

    F(arg, k_4, n);

    for (int i = 0; i < 5*local_amount_points; ++i)
        z[i] += (k_1[i] + 2*k_2[i] + 2*k_3[i] + k_4[i])*h/6;

    free(k_1);
    free(k_2);
    free(k_3);
    free(k_4);

    free(arg);
}


int main(int argc, char** argv) {
    if (argc < 3) {
        printf("usage: ./m init_file turns h\n");
        return 1;
    }

#ifdef MPI
    MPI_Init(&argc, &argv);
#endif

    int comm_size = 1;
#ifdef MPI
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
#endif

    int rank = 0;
#ifdef MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

    int turns = atoi(argv[2]);
    double h = atof(argv[3]);

    FILE* init = fopen(argv[1], "r");
    if (init == NULL) {
        printf("bad file\n");
        return 1;
    }

    size_t n = 0;
    fscanf(init, "%ld", &n);

    size_t local_amount_points = array_size(rank, n);
    double* z = calloc(5*local_amount_points, sizeof(double));

    double* x = z;
    double* y = x + local_amount_points;
    double* vx = y + local_amount_points;
    double* vy = vx + local_amount_points;
    double* m = vy + local_amount_points;

    printf("%d initial data: n = %ld, h = %3.1lf, lap = %ld\n",
        rank, n, h, local_amount_points);

    sleep(1);

    for (int node = 0; node < comm_size; ++node) {
        size_t node_size = array_size(node, n);

        if (rank == 0) {
            double* buffer = calloc(5*node_size, sizeof(double));
            double* _x = buffer;
            double* _y = _x + node_size;
            double* _vx = _y + node_size;
            double* _vy = _vx + node_size;
            double* _m = _vy + node_size;
            
            for (int p = 0; p < node_size; ++p) {
                if (node == 0)
                    fscanf(init, "%lf %lf %lf %lf %lf", 
                        x + p, y + p, vx + p, vy + p, m + p);
                else
                    fscanf(init, "%lf %lf %lf %lf %lf", 
                        _x + p, _y + p, _vx + p, _vy + p, _m + p);
            }

            if (node != 0) {
#ifdef MPI
                MPI_Send(buffer, // buff
                    (int) 5*node_size, // count
                    MPI_DOUBLE,  // type
                    node,        // dest
                    node,        // tag
                    MPI_COMM_WORLD); // comm
#endif
            }

            free(buffer);
        } else if (rank == node) {
#ifdef MPI
            MPI_Status stat;
            MPI_Recv(z,      // buff
                (int) 5*node_size, // count
                MPI_DOUBLE,  // type
                0,           // source 
                rank,        // tag
                MPI_COMM_WORLD, // comm    
                &stat);
#endif
        }
    }

    fclose(init);


    FILE* timeline_file;
     
    if (rank == 0) {
        timeline_file = fopen("data/timeline.data", "w");
        if (timeline_file == NULL) {
            printf("can't open timeline file\n");
            return 1;
        }

        fprintf(timeline_file, "%ld\n", n);
    }

    long long t1 = get_time();

    output(timeline_file, z, n);
    for (int k = 0; k < turns; ++k) {
        runge_kutta_step(z, n, h);
        output(timeline_file, z, n);
    }

    long long t2 = get_time();

    printf("%d done work in %lld us.\n", rank, t2 - t1);

    if (rank == 0) fclose(timeline_file);


    free(z);

#ifdef MPI
    MPI_Finalize();
#endif

    return 0;
}

