// Parallel Programming (English), Spring 2021
// Weifeng Liu, China University of Petroleum-Beijing
// Homework 1. *Use ``#pragma omp parallel for''
//             for accelerating for loops of GEMM in
//             the function gemm_yours().
//             *Try to put the OpenMP directive on
//             top of the three for loops and see
//             the performance.
//             *Also, try to explore better storage
//             methods and faster ways for optimize GEMM.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <openblas_config.h>
#include <generated/cblas.h>

#include <omp.h>

// the reference code of GEMM
void gemm_ref(double *A, double *B, double *C, int m, int k, int n)
{
    // for (int mi = 0; mi < m; mi++)
    // {
    //     for (int ni = 0; ni < n; ni++)
    //     {
    //         for (int ki = 0; ki < k; ki++)
    //             C[mi * n + ni] += A[mi * k + ki] * B[ki * n + ni];
    //     }
    // }
}

// insert your code in this function and run
void gemm_yours(double *A, double *B, double *C, int M, int K, int N)
{
#pragma omp parallel for schedule(dynamic,1) collapse(2)
    for (int m = 0; m < M; m += 4)
    {
        for (int n = 0; n < N; n += 4)
        {
            double temp_m0n0 = C[(m + 0) * N + n + 0];
            double temp_m0n1 = C[(m + 0) * N + n + 1];
            double temp_m0n2 = C[(m + 0) * N + n + 2];
            double temp_m0n3 = C[(m + 0) * N + n + 3];

            double temp_m1n0 = C[(m + 1) * N + n + 0];
            double temp_m1n1 = C[(m + 1) * N + n + 1];
            double temp_m1n2 = C[(m + 1) * N + n + 2];
            double temp_m1n3 = C[(m + 1) * N + n + 3];

            double temp_m2n0 = C[(m + 2) * N + n + 0];
            double temp_m2n1 = C[(m + 2) * N + n + 1];
            double temp_m2n2 = C[(m + 2) * N + n + 2];
            double temp_m2n3 = C[(m + 2) * N + n + 3];

            double temp_m3n0 = C[(m + 3) * N + n + 0];
            double temp_m3n1 = C[(m + 3) * N + n + 1];
            double temp_m3n2 = C[(m + 3) * N + n + 1];
            double temp_m3n3 = C[(m + 3) * N + n + 1];

            for (int k = 0; k < K; k++)
            {
                double temp_m0 = A[(m + 0) * K + k];
                double temp_m1 = A[(m + 1) * K + k];
                double temp_m2 = A[(m + 2) * K + k];
                double temp_m3 = A[(m + 3) * K + k];

                double temp_n0 = B[k * N + n + 0];
                double temp_n1 = B[k * N + n + 1];
                double temp_n2 = B[k * N + n + 2];
                double temp_n3 = B[k * N + n + 3];

                temp_m0n0 += temp_m0 * temp_n0;
                temp_m0n1 += temp_m0 * temp_n1;
                temp_m0n2 += temp_m0 * temp_n2;
                temp_m0n3 += temp_m0 * temp_n3;

                temp_m1n0 += temp_m1 * temp_n0;
                temp_m1n1 += temp_m1 * temp_n1;
                temp_m1n2 += temp_m1 * temp_n2;
                temp_m1n3 += temp_m1 * temp_n3;

                temp_m2n0 += temp_m2 * temp_n0;
                temp_m2n1 += temp_m2 * temp_n1;
                temp_m2n2 += temp_m2 * temp_n2;
                temp_m2n3 += temp_m2 * temp_n3;

                temp_m3n0 += temp_m3 * temp_n0;
                temp_m3n1 += temp_m3 * temp_n1;
                temp_m3n2 += temp_m3 * temp_n2;
                temp_m3n3 += temp_m3 * temp_n3;
            }

            C[(m + 0) * N + n + 0] = temp_m0n0;
            C[(m + 0) * N + n + 1] = temp_m0n1;
            C[(m + 0) * N + n + 2] = temp_m0n2;
            C[(m + 0) * N + n + 3] = temp_m0n3;

            C[(m + 1) * N + n + 0] = temp_m1n0;
            C[(m + 1) * N + n + 1] = temp_m1n1;
            C[(m + 1) * N + n + 2] = temp_m1n2;
            C[(m + 1) * N + n + 3] = temp_m1n3;

            C[(m + 2) * N + n + 0] = temp_m2n0;
            C[(m + 2) * N + n + 1] = temp_m2n1;
            C[(m + 2) * N + n + 2] = temp_m2n2;
            C[(m + 2) * N + n + 3] = temp_m2n3;

            C[(m + 3) * N + n + 0] = temp_m3n0;
            C[(m + 3) * N + n + 1] = temp_m3n1;
            C[(m + 3) * N + n + 2] = temp_m3n2;
            C[(m + 3) * N + n + 3] = temp_m3n3;
        }
    }
}

void gemm_OpenBlas(double *A, double *B, double *C, int m, int k, int n)
{
    enum CBLAS_ORDER order = CblasRowMajor;
    enum CBLAS_TRANSPOSE transposeA = CblasNoTrans;
    enum CBLAS_TRANSPOSE transposeB = CblasNoTrans;
    double alpha = 1;
    double beta = 1;
    cblas_dgemm(order, transposeA, transposeB, m, n, k, alpha, A, k, B, n, beta, C, n);
}

int main(int argc, char **argv)
{
    struct timeval t1, t2;

    // set matrix size
    int m = atoi(argv[1]);
    int k = atoi(argv[2]);
    int n = atoi(argv[3]);
    printf("Matrix A is %i x %i, matrix B is %i x %i\n", m, k, k, n);
    double gflop = 2.0 * (double)m * (double)n * (double)k / 1000000000.0;

    // malloc A, B and Cs
    double *A = (double *)malloc(sizeof(double) * m * k);
    double *B = (double *)malloc(sizeof(double) * k * n);
    double *C_golden = (double *)malloc(sizeof(double) * m * n);
    double *C_ref = (double *)malloc(sizeof(double) * m * n);
    double *C_yours = (double *)malloc(sizeof(double) * m * n);

    // randomly give values to elements in A and B
    srand(0);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < k; j++)
            A[i * k + j] = rand() % m;
    for (int i = 0; i < k; i++)
        for (int j = 0; j < n; j++)
            B[i * n + j] = rand() % k;

    // compute C_golden for validation
    memset(C_golden, 0, sizeof(double) * m * n);
    for (int mi = 0; mi < m; mi++)
    {
        for (int ni = 0; ni < n; ni++)
        {
            for (int ki = 0; ki < k; ki++)
                C_golden[mi * n + ni] += A[mi * k + ki] * B[ki * n + ni];
        }
    }

    // the reference row-col method for GEMM, A in row-major, B in row-major
    memset(C_ref, 0, sizeof(double) * m * n);
    gettimeofday(&t1, NULL);
    gemm_ref(A, B, C_ref, m, k, n);
    gettimeofday(&t2, NULL);
    double time_rowrow1 = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0;
    printf("\nGEMM (row-col, A and B are in row-major)) used %4.5f s, %4.2f GFlop/s\n", time_rowrow1, gflop / time_rowrow1);

    // check results
    int count1 = 0;
    for (int i = 0; i < m * n; i++)
        if (C_golden[i] != C_ref[i])
            count1++;
    if (count1 == 0)
        printf("GEMM (row-col, A and B are in row-major) PASS!\n\n");
    else
        printf("GEMM (row-col, A and B are in row-major) NOT PASS!\n\n");

    // the your method for GEMM
    memset(C_yours, 0, sizeof(double) * m * n);
    gettimeofday(&t1, NULL);
    gemm_yours(A, B, C_yours, m, k, n);
    gettimeofday(&t2, NULL);
    double time_yours = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0;
    printf("\nGEMM (your method)) used %4.5f s, %4.2f GFlop/s\n", time_yours, gflop / time_yours);

    // check results
    int count2 = 0;
    for (int i = 0; i < m * n; i++)
        if (C_golden[i] != C_yours[i])
            count2++;
    if (count2 == 0)
        printf("GEMM (your method) PASS!\n\n");
    else
        printf("GEMM (your method) NOT PASS!\n\n");

    memset(C_yours, 0, sizeof(double) * m * n);
    gettimeofday(&t1, NULL);
    gemm_OpenBlas(A, B, C_yours, m, k, n);
    gettimeofday(&t2, NULL);
    time_yours = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0;
    printf("\nGEMM (OpenBLAS)) used %4.5f s, %4.2f GFlop/s\n", time_yours, gflop / time_yours);

    // check results
    count2 = 0;
    for (int i = 0; i < m * n; i++)
        if (C_golden[i] != C_yours[i])
            count2++;
    if (count2 == 0)
        printf("GEMM (OpenBLAS) PASS!\n\n");
    else
        printf("GEMM (OpenBLAS) NOT PASS!\n\n");

    // free memory
    free(A);
    free(B);
    free(C_golden);
    free(C_ref);
    free(C_yours);
}
