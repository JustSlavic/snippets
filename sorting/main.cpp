#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct _array {
    int* arr;
    int n;
} array;

array* make_array(int n) {
    array* a = malloc(sizeof(array));
    a->arr = calloc(n, sizeof(int));
    a->n   = n;
}

void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

array* sort(array* a) {
    for (int i = 0; i < a->n; ++i) {
        for (int j = i+1; j < a->n; ++j) {
            if (a->arr[i] > a->arr[j]) swap(a->arr+i, a->arr+j);
        }
    }

    return a;
}

array* fill(array* a) {
    for (int i = 0; i < a->n; ++i) {
        a->arr[i] = rand() % 100;
    }

    return a;
}

array print(array a) {
    printf("[%d", a.arr[0]);
    for (int i = 1; i < a.n; ++i) {
        printf(", %d", a[i]);
    }
    printf("]\n");

    return a;
}

array partial_sort(array a, int k) {
    k = 2;

    return a;
}

int main() {
    srand(time(0));

    array a;
    a.n = 10;
    a.arr = calloc(a.n, sizeof(int));

    print(sort(fill(a)));

    return 0;
}



