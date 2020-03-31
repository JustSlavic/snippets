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

void F(double* z, double* res, size_t n) {
    int comm_size = 1;
    int rank = 0;

    // sets pointers in their positions in continuous array
    // [x1, x2, y1, y2, vx1, vx2, vy1, vy2, m1, m2]
    double* x = z;
    double* y = x + n;
    double* vx = y + n;
    double* vy = vx + n;
    double* m = vy + n;

    double* rx = res;
    double* ry = rx + n;
    double* rvx = ry + n;
    double* rvy = rvx + n;
    double* rm = rvy + n;

    // apply force onto i-th body
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;

            double dx = x[j] - x[i];
            double dy = y[j] - y[i];

            double r_2 = dx*dx + dy*dy;  // square of distance
            double r_1 = sqrt(dr_2);     // distance between bodies

            double F = G*m[j]/(r_2 * r_1); // G * m / r^3

            rvx[i] += dx*F;
            rvy[i] += dy*F;
        }
    }

    for (int node = 0; node < comm_size; ++node) {
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
    }
}

int main() {
    return 0;
}
