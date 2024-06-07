/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k;
    int tmp_1, tmp_2, tmp_3, tmp_4, tmp_5, tmp_6, tmp_7, tmp_8;

    for (i = 0; i < N; i += 8) {
        for (j = 0; j < M; j += 8) {  
            for (k = 0; k < 4; i++, k++) {
                tmp_1 = A[i][j];
                tmp_2 = A[i][j+1];
                tmp_3 = A[i][j+2];
                tmp_4 = A[i][j+3];
                tmp_5 = A[i][j+4];
                tmp_6 = A[i][j+5];
                tmp_7 = A[i][j+6];
                tmp_8 = A[i][j+7];

                B[j][i] = tmp_1;
                B[j+1][i] = tmp_2;
                B[j+2][i] = tmp_3;
                B[j+3][i] = tmp_4;
                B[j][i+4] = tmp_5;
                B[j+1][i+4] = tmp_6;
                B[j+2][i+4] = tmp_7;
                B[j+3][i+4] = tmp_8;
            }
            i -= 4;

            for (k = 0; k < 4; j++, k++) {
                tmp_1 = A[i+4][j];
                tmp_2 = A[i+5][j];
                tmp_3 = A[i+6][j];
                tmp_4 = A[i+7][j];
                tmp_5 = B[j][i+4];
                tmp_6 = B[j][i+5];
                tmp_7 = B[j][i+6];
                tmp_8 = B[j][i+7];

                B[j][i+4] = tmp_1; 
                B[j][i+5] = tmp_2;
                B[j][i+6] = tmp_3;
                B[j][i+7] = tmp_4;
                B[j+4][i] = tmp_5;
                B[j+4][i+1] = tmp_6;
                B[j+4][i+2] = tmp_7;
                B[j+4][i+3] = tmp_8;
            }
            j -= 4;
            j += 4;

            for (i += 4, k = 0; k < 4; i++, k++) {
                tmp_1 = A[i][j];
                tmp_2 = A[i][j+1];
                tmp_3 = A[i][j+2];
                tmp_4 = A[i][j+3];

                B[j][i] = tmp_1;
                B[j+1][i] = tmp_2;
                B[j+2][i] = tmp_3;
                B[j+3][i] = tmp_4;
            }
            i -= 8;
            j -= 4;
        }
    }
}





/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

