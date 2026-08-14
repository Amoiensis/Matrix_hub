/*
\ \     __  ___     __      _       __ __     __
 \ \   /  |/  /__ _/ /_____(_)_ __ / // /_ __/ /
 / /  / /|_/ / _ `/ __/ __/ /\ \ // _  / // / _ \
/ /  /_/  /_/\_,_/\__/_/ /_//_\_\/_//_/\_,_/_.__/
* [INFORMATION]
    MATRIX_HUB
    AUTHOR: Xiping.Yu
    GITHUB: https://github.com/Amoiensis/Matrix_hub
    VERSION: 2.0.0
    CASE: Matrix Solver Plugin (C)
    DETAILS: Dense linear equation solver plugin.
    LICENSE: Apache-2.0
*/

#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../matrix.h"

Matrix *Linear_Solve(Matrix *mat_A, Matrix *mat_B);
Matrix *Linear_Solve_With_Tolerance(Matrix *mat_A, Matrix *mat_B, MATRIX_TYPE tol);

Matrix *Linear_Solve_With_Tolerance(Matrix *mat_A, Matrix *mat_B, MATRIX_TYPE tol) {/*
 * Solve A * X = B by dense Gaussian elimination with partial pivoting.
 * 用带列主元选择的稠密高斯消元求解 A * X = B。
 *
 * This plugin avoids explicitly forming inv(A). It is usually faster and
 * more accurate than M_mul(M_Inverse(A), B) for solving linear equations.
 */
    Matrix *result = NULL;
    MATRIX_TYPE *aug = NULL;
    int n, rhs_cols;
    int row, col, rhs, k;

    if ((mat_A == NULL) || (mat_B == NULL) ||
        (mat_A->row != mat_A->column) || (mat_A->row != mat_B->row)) {
        printf("@ERROR: Linear_Solve Wrong!\n\tDetails: A must be square and A->row must equal B->row.\n");
        return NULL;
    }

    n = mat_A->row;
    rhs_cols = mat_B->column;
    if (tol <= 0) {
        tol = M_matrix_tolerance(mat_A);
    }
    aug = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * n * (n + rhs_cols));
    if (aug == NULL) {
        return NULL;
    }

    for (row = 0; row < n; row++) {
        for (col = 0; col < n; col++) {
            aug[row * (n + rhs_cols) + col] = mat_A->data[row * n + col];
        }
        for (rhs = 0; rhs < rhs_cols; rhs++) {
            aug[row * (n + rhs_cols) + n + rhs] = mat_B->data[row * rhs_cols + rhs];
        }
    }

    for (k = 0; k < n; k++) {
        int pivot = k;
        MATRIX_TYPE pivot_abs = fabs(aug[k * (n + rhs_cols) + k]);
        for (row = k + 1; row < n; row++) {
            MATRIX_TYPE value_abs = fabs(aug[row * (n + rhs_cols) + k]);
            if (value_abs > pivot_abs) {
                pivot_abs = value_abs;
                pivot = row;
            }
        }
        if (pivot_abs <= tol) {
            printf("@ERROR: Linear_Solve Wrong!\n\tDetails: singular or nearly singular matrix.\n");
            free(aug);
            return NULL;
        }
        if (pivot != k) {
            for (col = k; col < n + rhs_cols; col++) {
                MATRIX_TYPE temp = aug[k * (n + rhs_cols) + col];
                aug[k * (n + rhs_cols) + col] = aug[pivot * (n + rhs_cols) + col];
                aug[pivot * (n + rhs_cols) + col] = temp;
            }
        }
        for (row = k + 1; row < n; row++) {
            MATRIX_TYPE factor = aug[row * (n + rhs_cols) + k] / aug[k * (n + rhs_cols) + k];
            aug[row * (n + rhs_cols) + k] = 0;
            for (col = k + 1; col < n + rhs_cols; col++) {
                aug[row * (n + rhs_cols) + col] -= factor * aug[k * (n + rhs_cols) + col];
            }
        }
    }

    result = M_Zeros(n, rhs_cols);
    if (result == NULL) {
        free(aug);
        return NULL;
    }
    for (rhs = 0; rhs < rhs_cols; rhs++) {
        for (row = n - 1; row >= 0; row--) {
            MATRIX_TYPE sum = aug[row * (n + rhs_cols) + n + rhs];
            for (col = row + 1; col < n; col++) {
                sum -= aug[row * (n + rhs_cols) + col] * result->data[col * rhs_cols + rhs];
            }
            result->data[row * rhs_cols + rhs] = sum / aug[row * (n + rhs_cols) + row];
        }
    }

    free(aug);
    return result;
}

Matrix *Linear_Solve(Matrix *mat_A, Matrix *mat_B) {
    return Linear_Solve_With_Tolerance(mat_A, mat_B, 0);
}
