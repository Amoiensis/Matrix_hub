/*
\ \     __  ___     __      _       __ __     __
 \ \   /  |/  /__ _/ /_____(_)_ __ / // /_ __/ /
 / /  / /|_/ / _ `/ __/ __/ /\ \ // _  / // / _ \
/ /  /_/  /_/\_,_/\__/_/ /_//_\_\/_//_/\_,_/_.__/
* [INFORMATION]
    MATRIX_HUB
    AUTHOR: Xiping.Yu
    E-MAIL:Amoiensis@outlook.com
    GITHUB: https://github.com/Amoiensis/Matrix_hub
    DATE: 2020.02.12-2026.08.14
    VERSION: 2.0.0
    CASE: Matrix Operation (C)
    DETAILS: The plugin-code-file for Matrix_Hub, LP-Solver (Linear Programing).
    LICENSE: Apache-2.0
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#if !defined(_WIN32)
#include <sys/time.h>
#endif
#include "../matrix.h"
#include "./plugin_Linear_Solver.h"

// _SILENT_MODE_ == True(1) -> solver keep silence.
#define _SILENT_MODE_ 1

#define _INTEGER_GAP_ _MATRIX_ABS_EPS_
#define _ZERRO_GAP_ _MATRIX_ABS_EPS_

#define LP_STATUS_OPTIMAL 0
#define LP_STATUS_UNBOUNDED 1
#define LP_STATUS_INFEASIBLE 2
#define LP_STATUS_ITER_LIMIT 3
#define LP_STATUS_INPUT_ERROR 4
#define LP_STATUS_TIME_LIMIT 5
#define LP_STATUS_GAP_LIMIT 6

#define LP_METHOD_REVISED_SIMPLEX 1
#define LP_METHOD_INTERIOR_POINT 2

#define LP_DEGENERATE_PIVOT_LIMIT 20

typedef struct _matrix_LP_result_struct {
    /* Store simplex solver result.
     * 保存单纯形求解结果。返回结构拥有这些矩阵的释放责任。 */
    Matrix *_matrix_A;
    Matrix *_matrix_b;
    Matrix *_matrix_c;
    Matrix *_matrix_base;
    Matrix *_matrix_delta;
    int OPT_STATUS;
    MATRIX_TYPE values_opt;
    int iter_num;
} M_LP_struct;

typedef struct _lp_options_struct {
    /* Runtime controls shared by the standalone LP solvers.
     * Nonpositive limits mean "no explicit limit". */
    int method;
    int progress_interval;
    MATRIX_TYPE time_limit_seconds;
    MATRIX_TYPE gap_limit;
} LP_Options;

LP_Options LP_Default_Options(void);
M_LP_struct * LP_Solver(Matrix* mat_A,Matrix* mat_B,Matrix* mat_C, int LP_method);
M_LP_struct * LP_Solver_With_Progress(Matrix* mat_A,Matrix* mat_B,Matrix* mat_C,
                                      int LP_method, int progress_interval);
M_LP_struct * LP_Solver_With_Options(Matrix* mat_A,Matrix* mat_B,Matrix* mat_C,
                                     const LP_Options *options);
M_LP_struct * LP_simplex_method(Matrix* mat_A,Matrix* mat_B,Matrix* mat_C);
M_LP_struct * LP_simplex_method_with_progress(Matrix* mat_A,Matrix* mat_B,
                                              Matrix* mat_C, int progress_interval);
M_LP_struct * LP_simplex_method_with_options(Matrix* mat_A,Matrix* mat_B,
                                             Matrix* mat_C,
                                             const LP_Options *options,
                                             double start_seconds);
M_LP_struct * LP_interior_point_method(Matrix* mat_A,Matrix* mat_B,Matrix* mat_C);
M_LP_struct * LP_interior_point_method_with_progress(Matrix* mat_A,Matrix* mat_B,
                                                     Matrix* mat_C, int progress_interval);
M_LP_struct * LP_interior_point_method_with_options(Matrix* mat_A,Matrix* mat_B,
                                                    Matrix* mat_C,
                                                    const LP_Options *options,
                                                    double start_seconds);

int LP_free(M_LP_struct* _LP_);
int f_position_list_max(MATRIX_TYPE* list, int len);
int f_position_list_min(MATRIX_TYPE* list, int len);
static MATRIX_TYPE LP_tolerance(Matrix *mat_A, Matrix *mat_b, Matrix *mat_C);
static int LP_validate_input(Matrix *mat_A, Matrix *mat_b, Matrix *mat_C);
static LP_Options LP_normalized_options(const LP_Options *options);
static double LP_now_seconds(void);
static int LP_time_limit_reached(double start_seconds,
                                 MATRIX_TYPE time_limit_seconds);
static MATRIX_TYPE LP_vector_get(Matrix *vector, int index);
static MATRIX_TYPE LP_cost_get(Matrix *mat_C, int index);
static Matrix *LP_vector_to_column(Matrix *vector, int length);
static Matrix *LP_cost_basis(Matrix *mat_C, Matrix *basis);
static int LP_find_identity_basis(Matrix *mat_A, Matrix *basis, MATRIX_TYPE tol);
static int LP_find_pivot_basis(Matrix *mat_A, Matrix *basis, MATRIX_TYPE tol);
static Matrix *LP_sample_columns(Matrix *mat_A, Matrix *basis);
static int LP_basis_is_identity(Matrix *mat_A, Matrix *basis, MATRIX_TYPE tol);
static Matrix *LP_basis_inverse(Matrix *mat_A, Matrix *basis, MATRIX_TYPE tol);
static int LP_select_entering(Matrix *delta, MATRIX_TYPE tol, int use_bland);
static int LP_select_leaving(Matrix *A_bar, Matrix *b_bar, Matrix *basis,
                             int entering, MATRIX_TYPE tol, int use_bland);
static int LP_has_negative_rhs(Matrix *mat_b, int length, MATRIX_TYPE tol);
static Matrix *LP_copy_normalized_problem(Matrix *mat_A, Matrix *mat_b,
                                          Matrix **b_normalized,
                                          MATRIX_TYPE tol);
static Matrix *LP_make_phase_one_basis(Matrix *mat_A, Matrix *mat_b,
                                       MATRIX_TYPE tol,
                                       int progress_interval,
                                       double start_seconds,
                                       MATRIX_TYPE time_limit_seconds,
                                       MATRIX_TYPE gap_limit,
                                       Matrix **start_A_bar,
                                       Matrix **start_b_bar,
                                       int *phase_status);
static int LP_basis_contains(Matrix *basis, int value, int skip_row);
static int LP_pivot_artificial_basis(Matrix *phase_A_bar, Matrix *phase_b_bar,
                                     Matrix *basis, int original_cols,
                                     MATRIX_TYPE tol);
static MATRIX_TYPE LP_l2_norm(const MATRIX_TYPE *data, int len);
static MATRIX_TYPE LP_dot_cost_vector(Matrix *mat_C, const MATRIX_TYPE *x, int len);
static Matrix *LP_array_to_column(const MATRIX_TYPE *data, int len);
static Matrix *LP_array_to_row(const MATRIX_TYPE *data, int len);
static MATRIX_TYPE LP_basis_objective(Matrix *mat_C, Matrix *basis, Matrix *b_bar,
                                      int base_num);
static MATRIX_TYPE LP_max_reduced_cost(Matrix *delta);
static int LP_first_negative_rhs(Matrix *b_bar, MATRIX_TYPE tol);
static int LP_rebuild_tableau(Matrix *mat_A, Matrix *b_col, Matrix *basis,
                              MATRIX_TYPE tol, Matrix **A_bar, Matrix **b_bar);
static int LP_recompute_delta(Matrix *mat_C, Matrix *basis,
                              Matrix *A_bar, Matrix *b_bar,
                              Matrix **delta, MATRIX_TYPE *values_opt,
                              MATRIX_TYPE tol);
static M_LP_struct *LP_make_result(Matrix *A_bar, Matrix *b_bar, Matrix *mat_C,
                                   Matrix *basis, Matrix *delta,
                                   int status, MATRIX_TYPE opt_value, int iter_num);
static M_LP_struct *LP_simplex_core(Matrix *mat_A, Matrix *mat_B, Matrix *mat_C,
                                    Matrix *initial_basis,
                                    Matrix *initial_A_bar,
                                    Matrix *initial_b_bar,
                                    int progress_interval,
                                    double start_seconds,
                                    MATRIX_TYPE time_limit_seconds,
                                    MATRIX_TYPE gap_limit,
                                    const char *progress_name,
                                    int print_basis_error);

int LP_free(M_LP_struct* _LP_){
    if (_LP_ == NULL) {
        return 0;
    }
    if (_LP_->_matrix_A != NULL) {
        M_free(_LP_->_matrix_A);
    }
    if (_LP_->_matrix_b != NULL) {
        M_free(_LP_->_matrix_b);
    }
    if (_LP_->_matrix_c != NULL) {
        M_free(_LP_->_matrix_c);
    }
    if (_LP_->_matrix_base != NULL) {
        M_free(_LP_->_matrix_base);
    }
    if (_LP_->_matrix_delta != NULL) {
        M_free(_LP_->_matrix_delta);
    }
    free(_LP_);
    return 0;
}

int f_position_list_min(MATRIX_TYPE* list, int len){
    int position = 0, i;
    MATRIX_TYPE value_temp = 0, value_temp_1;
    for(i=0;i<len;i++){
        value_temp_1 = list[i];
        value_temp = list[position];
        if ((value_temp_1 < value_temp)){
            position = i;
        }
    }
    return position;
}

int f_position_list_max(MATRIX_TYPE* list, int len){
    int position = 0, i;
    MATRIX_TYPE value_temp = 0, value_temp_1;
    for(i=0;i<len;i++){
        value_temp_1 = list[i];
        value_temp = list[position];
        if (value_temp_1 > value_temp){
            position = i;
        }
    }
    return position;
}

LP_Options LP_Default_Options(void) {
    LP_Options options;
    options.method = LP_METHOD_REVISED_SIMPLEX;
    options.progress_interval = 0;
    options.time_limit_seconds = 0;
    options.gap_limit = 0;
    return options;
}

static LP_Options LP_normalized_options(const LP_Options *options) {
    LP_Options normalized = LP_Default_Options();
    if (options != NULL) {
        normalized = *options;
    }
    if ((normalized.method != LP_METHOD_REVISED_SIMPLEX) &&
        (normalized.method != LP_METHOD_INTERIOR_POINT)) {
        normalized.method = LP_METHOD_REVISED_SIMPLEX;
    }
    if (normalized.progress_interval < 0) {
        normalized.progress_interval = 0;
    }
    if (normalized.time_limit_seconds < 0) {
        normalized.time_limit_seconds = 0;
    }
    if (normalized.gap_limit < 0) {
        normalized.gap_limit = 0;
    }
    return normalized;
}

static double LP_now_seconds(void) {
#if defined(_WIN32)
    return (double)clock() / (double)CLOCKS_PER_SEC;
#else
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == 0) {
        return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
    }
    return (double)clock() / (double)CLOCKS_PER_SEC;
#endif
}

static int LP_time_limit_reached(double start_seconds,
                                 MATRIX_TYPE time_limit_seconds) {
    if (time_limit_seconds <= 0) {
        return 0;
    }
    return (LP_now_seconds() - start_seconds) >= (double)time_limit_seconds;
}

static MATRIX_TYPE LP_tolerance(Matrix *mat_A, Matrix *mat_b, Matrix *mat_C) {
    MATRIX_TYPE scale = M_fro_norm_raw(mat_A);
    MATRIX_TYPE b_scale = M_fro_norm_raw(mat_b);
    MATRIX_TYPE c_scale = M_fro_norm_raw(mat_C);
    if (b_scale > scale) {
        scale = b_scale;
    }
    if (c_scale > scale) {
        scale = c_scale;
    }
    return M_relative_tolerance(scale);
}

static int LP_validate_input(Matrix *mat_A, Matrix *mat_b, Matrix *mat_C) {
    if ((mat_A == NULL) || (mat_b == NULL) || (mat_C == NULL)) {
        return 0;
    }
    if ((mat_b->row != mat_A->row || mat_b->column != 1) &&
        (mat_b->row != 1 || mat_b->column != mat_A->row)) {
        return 0;
    }
    if ((mat_C->row != 1 || mat_C->column != mat_A->column) &&
        (mat_C->row != mat_A->column || mat_C->column != 1)) {
        return 0;
    }
    return mat_A->row <= mat_A->column;
}

static MATRIX_TYPE LP_vector_get(Matrix *vector, int index) {
    return vector->data[index];
}

static MATRIX_TYPE LP_cost_get(Matrix *mat_C, int index) {
    if (mat_C->row == 1) {
        return mat_C->data[index];
    }
    return mat_C->data[index * mat_C->column];
}

static Matrix *LP_vector_to_column(Matrix *vector, int length) {
    Matrix *result = M_Zeros(length, 1);
    int i;
    if (result == NULL) {
        return NULL;
    }
    for (i = 0; i < length; i++) {
        result->data[i] = LP_vector_get(vector, i);
    }
    return result;
}

static Matrix *LP_cost_basis(Matrix *mat_C, Matrix *basis) {
    Matrix *result = M_Zeros(1, basis->column);
    int i;
    if (result == NULL) {
        return NULL;
    }
    for (i = 0; i < basis->column; i++) {
        result->data[i] = LP_cost_get(mat_C, (int)basis->data[i]);
    }
    return result;
}

static int LP_find_identity_basis(Matrix *mat_A, Matrix *basis, MATRIX_TYPE tol) {
    int row, col, other, used;
    for (row = 0; row < mat_A->row; row++) {
        basis->data[row] = -1;
    }
    for (row = 0; row < mat_A->row; row++) {
        for (col = 0; col < mat_A->column; col++) {
            int ok = fabs(mat_A->data[row * mat_A->column + col] - 1) <= tol;
            if (!ok) {
                continue;
            }
            for (other = 0; other < mat_A->row; other++) {
                if ((other != row) && (fabs(mat_A->data[other * mat_A->column + col]) > tol)) {
                    ok = 0;
                    break;
                }
            }
            if (!ok) {
                continue;
            }
            for (used = 0; used < row; used++) {
                if ((int)basis->data[used] == col) {
                    ok = 0;
                    break;
                }
            }
            if (ok) {
                basis->data[row] = col;
                break;
            }
        }
        if (basis->data[row] < 0) {
            return 0;
        }
    }
    return 1;
}

static int LP_find_pivot_basis(Matrix *mat_A, Matrix *basis, MATRIX_TYPE tol) {
    Matrix *work = Matrix_copy(mat_A);
    int row = 0;
    int col, i, j;
    if (work == NULL) {
        return 0;
    }
    for (i = 0; i < mat_A->row; i++) {
        basis->data[i] = -1;
    }
    for (col = 0; (col < mat_A->column) && (row < mat_A->row); col++) {
        int pivot = row;
        MATRIX_TYPE pivot_abs = fabs(work->data[row * work->column + col]);
        for (i = row + 1; i < work->row; i++) {
            MATRIX_TYPE value_abs = fabs(work->data[i * work->column + col]);
            if (value_abs > pivot_abs) {
                pivot_abs = value_abs;
                pivot = i;
            }
        }
        if (pivot_abs <= tol) {
            continue;
        }
        if (pivot != row) {
            M_Swap(work, pivot + 1, row + 1, _ROW_);
        }
        {
            MATRIX_TYPE scale = work->data[row * work->column + col];
            for (j = col; j < work->column; j++) {
                work->data[row * work->column + j] /= scale;
            }
        }
        for (i = 0; i < work->row; i++) {
            MATRIX_TYPE factor;
            if (i == row) {
                continue;
            }
            factor = work->data[i * work->column + col];
            if (fabs(factor) <= tol) {
                continue;
            }
            for (j = col; j < work->column; j++) {
                work->data[i * work->column + j] -= factor * work->data[row * work->column + j];
            }
        }
        basis->data[row] = col;
        row++;
    }
    M_free(work);
    return row == mat_A->row;
}

static Matrix *LP_sample_columns(Matrix *mat_A, Matrix *basis) {
    return M_Sample(mat_A, basis, _COLUMN_);
}

static int LP_basis_is_identity(Matrix *mat_A, Matrix *basis, MATRIX_TYPE tol) {
    int row, other;
    if ((mat_A == NULL) || (basis == NULL)) {
        return 0;
    }
    for (row = 0; row < mat_A->row; row++) {
        int col = (int)basis->data[row];
        if ((col < 0) || (col >= mat_A->column)) {
            return 0;
        }
        if (fabs(mat_A->data[row * mat_A->column + col] - 1) > tol) {
            return 0;
        }
        for (other = 0; other < mat_A->row; other++) {
            if ((other != row) &&
                (fabs(mat_A->data[other * mat_A->column + col]) > tol)) {
                return 0;
            }
        }
    }
    return 1;
}

static Matrix *LP_basis_inverse(Matrix *mat_A, Matrix *basis, MATRIX_TYPE tol) {
    Matrix *B = NULL;
    Matrix *I = NULL;
    Matrix *B_inv = NULL;
    MATRIX_TYPE solve_tol;
    if (LP_basis_is_identity(mat_A, basis, tol)) {
        return M_I(mat_A->row);
    }
    B = LP_sample_columns(mat_A, basis);
    if (B == NULL) {
        return NULL;
    }
    solve_tol = M_matrix_tolerance(B);
    if (tol > solve_tol) {
        solve_tol = tol;
    }
    I = M_I(B->row);
    if (I != NULL) {
        /* Reuse the dense linear solver: B_inv is the solution of B * X = I. */
        B_inv = Linear_Solve_With_Tolerance(B, I, solve_tol);
    }
    M_free(I);
    M_free(B);
    return B_inv;
}

static int LP_select_entering(Matrix *delta, MATRIX_TYPE tol, int use_bland) {
    int col;
    int position = -1;
    MATRIX_TYPE best = tol;
    for (col = 0; col < delta->column; col++) {
        if (use_bland && (delta->data[col] > tol)) {
            return col;
        }
        if (delta->data[col] > best) {
            best = delta->data[col];
            position = col;
        }
    }
    return position;
}

static int LP_select_leaving(Matrix *A_bar, Matrix *b_bar, Matrix *basis,
                             int entering, MATRIX_TYPE tol, int use_bland) {
    int row;
    int position = -1;
    MATRIX_TYPE best_theta = 0;
    for (row = 0; row < A_bar->row; row++) {
        MATRIX_TYPE a = A_bar->data[row * A_bar->column + entering];
        if (a > tol) {
            MATRIX_TYPE theta = b_bar->data[row] / a;
            if ((position < 0) || (theta < best_theta - tol)) {
                position = row;
                best_theta = theta;
            } else if (use_bland && (fabs(theta - best_theta) <= tol) &&
                       ((int)basis->data[row] < (int)basis->data[position])) {
                position = row;
                best_theta = theta;
            }
        }
    }
    return position;
}

static int LP_has_negative_rhs(Matrix *mat_b, int length, MATRIX_TYPE tol) {
    int row;
    if (mat_b == NULL) {
        return 0;
    }
    for (row = 0; row < length; row++) {
        if (LP_vector_get(mat_b, row) < -tol) {
            return 1;
        }
    }
    return 0;
}

static Matrix *LP_copy_normalized_problem(Matrix *mat_A, Matrix *mat_b,
                                          Matrix **b_normalized,
                                          MATRIX_TYPE tol) {
    Matrix *A = Matrix_copy(mat_A);
    Matrix *b = LP_vector_to_column(mat_b, mat_A->row);
    int row, col;
    *b_normalized = NULL;
    if ((A == NULL) || (b == NULL)) {
        M_free(A);
        M_free(b);
        return NULL;
    }
    for (row = 0; row < A->row; row++) {
        if (b->data[row] < -tol) {
            b->data[row] = -b->data[row];
            for (col = 0; col < A->column; col++) {
                A->data[row * A->column + col] =
                    -A->data[row * A->column + col];
            }
        } else if (fabs(b->data[row]) <= tol) {
            b->data[row] = 0;
        }
    }
    *b_normalized = b;
    return A;
}

static int LP_basis_contains(Matrix *basis, int value, int skip_row) {
    int row;
    if (basis == NULL) {
        return 0;
    }
    for (row = 0; row < basis->column; row++) {
        if ((row != skip_row) && ((int)basis->data[row] == value)) {
            return 1;
        }
    }
    return 0;
}

static int LP_pivot_artificial_basis(Matrix *phase_A_bar, Matrix *phase_b_bar,
                                     Matrix *basis, int original_cols,
                                     MATRIX_TYPE tol) {
    int row, col, k;
    if ((phase_A_bar == NULL) || (phase_b_bar == NULL) || (basis == NULL)) {
        return 0;
    }
    for (row = 0; row < basis->column; row++) {
        int pivot_col = -1;
        if ((int)basis->data[row] < original_cols) {
            continue;
        }
        if (fabs(phase_b_bar->data[row]) > 100 * tol) {
            return 0;
        }
        for (col = 0; col < original_cols; col++) {
            MATRIX_TYPE value = phase_A_bar->data[row * phase_A_bar->column + col];
            if ((fabs(value) > tol) && !LP_basis_contains(basis, col, row)) {
                pivot_col = col;
                break;
            }
        }
        if (pivot_col < 0) {
            return 0;
        }
        {
            MATRIX_TYPE pivot = phase_A_bar->data[row * phase_A_bar->column + pivot_col];
            if (fabs(pivot) <= tol) {
                return 0;
            }
            for (col = 0; col < phase_A_bar->column; col++) {
                phase_A_bar->data[row * phase_A_bar->column + col] /= pivot;
            }
            phase_b_bar->data[row] /= pivot;
            for (k = 0; k < phase_A_bar->row; k++) {
                MATRIX_TYPE factor;
                if (k == row) {
                    continue;
                }
                factor = phase_A_bar->data[k * phase_A_bar->column + pivot_col];
                if (fabs(factor) <= tol) {
                    phase_A_bar->data[k * phase_A_bar->column + pivot_col] = 0;
                    continue;
                }
                for (col = 0; col < phase_A_bar->column; col++) {
                    phase_A_bar->data[k * phase_A_bar->column + col] -=
                        factor * phase_A_bar->data[row * phase_A_bar->column + col];
                    if (fabs(phase_A_bar->data[k * phase_A_bar->column + col]) <= tol) {
                        phase_A_bar->data[k * phase_A_bar->column + col] = 0;
                    }
                }
                phase_b_bar->data[k] -= factor * phase_b_bar->data[row];
                if (fabs(phase_b_bar->data[k]) <= tol) {
                    phase_b_bar->data[k] = 0;
                }
            }
        }
        basis->data[row] = pivot_col;
    }
    return 1;
}

static Matrix *LP_make_phase_one_basis(Matrix *mat_A, Matrix *mat_b,
                                       MATRIX_TYPE tol,
                                       int progress_interval,
                                       double start_seconds,
                                       MATRIX_TYPE time_limit_seconds,
                                       MATRIX_TYPE gap_limit,
                                       Matrix **start_A_bar,
                                       Matrix **start_b_bar,
                                       int *phase_status) {
    int row, col;
    int m = mat_A->row;
    int n = mat_A->column;
    Matrix *phase_A = NULL;
    Matrix *phase_c = NULL;
    Matrix *basis = NULL;
    Matrix *phase_basis = NULL;
    M_LP_struct *phase_lp = NULL;
    MATRIX_TYPE phase_tol;

    *start_A_bar = NULL;
    *start_b_bar = NULL;
    *phase_status = LP_STATUS_INPUT_ERROR;
    phase_A = M_Zeros(m, n + m);
    phase_c = M_Zeros(1, n + m);
    basis = M_Zeros(1, m);
    if ((phase_A == NULL) || (phase_c == NULL) || (basis == NULL)) {
        goto PHASE_ONE_END;
    }
    for (row = 0; row < m; row++) {
        for (col = 0; col < n; col++) {
            phase_A->data[row * phase_A->column + col] =
                mat_A->data[row * n + col];
        }
        phase_A->data[row * phase_A->column + n + row] = 1;
        phase_c->data[n + row] = -1;
        basis->data[row] = n + row;
    }

    phase_lp = LP_simplex_core(phase_A, mat_b, phase_c, basis,
                               NULL, NULL,
                               progress_interval, start_seconds,
                               time_limit_seconds, gap_limit,
                               "LP-PHASE-I", 0);
    if (phase_lp == NULL) {
        goto PHASE_ONE_END;
    }
    *phase_status = phase_lp->OPT_STATUS;
    phase_tol = 100 * LP_tolerance(phase_A, mat_b, phase_c);
    if (phase_tol < 1e-8) {
        phase_tol = 1e-8;
    }
    if (phase_lp->OPT_STATUS != LP_STATUS_OPTIMAL) {
        goto PHASE_ONE_END;
    }
    if (phase_lp->values_opt < -phase_tol) {
        *phase_status = LP_STATUS_INFEASIBLE;
        goto PHASE_ONE_END;
    }
    phase_basis = Matrix_copy(phase_lp->_matrix_base);
    if ((phase_basis == NULL) ||
        !LP_pivot_artificial_basis(phase_lp->_matrix_A, phase_lp->_matrix_b,
                                   phase_basis, n, tol)) {
        M_free(phase_basis);
        phase_basis = NULL;
        *phase_status = LP_STATUS_INPUT_ERROR;
        goto PHASE_ONE_END;
    }
    *start_A_bar = M_Zeros(m, n);
    *start_b_bar = Matrix_copy(phase_lp->_matrix_b);
    if ((*start_A_bar == NULL) || (*start_b_bar == NULL)) {
        M_free(phase_basis);
        M_free(*start_A_bar);
        M_free(*start_b_bar);
        *start_A_bar = NULL;
        *start_b_bar = NULL;
        phase_basis = NULL;
        *phase_status = LP_STATUS_INPUT_ERROR;
        goto PHASE_ONE_END;
    }
    for (row = 0; row < m; row++) {
        for (col = 0; col < n; col++) {
            (*start_A_bar)->data[row * n + col] =
                phase_lp->_matrix_A->data[row * phase_lp->_matrix_A->column + col];
        }
    }
    *phase_status = LP_STATUS_OPTIMAL;

    PHASE_ONE_END:
    LP_free(phase_lp);
    M_free(phase_A);
    M_free(phase_c);
    M_free(basis);
    return phase_basis;
}

static MATRIX_TYPE LP_l2_norm(const MATRIX_TYPE *data, int len) {
    MATRIX_TYPE norm = 0;
    int i;
    for (i = 0; i < len; i++) {
        norm = hypot(norm, data[i]);
    }
    return norm;
}

static MATRIX_TYPE LP_dot_cost_vector(Matrix *mat_C, const MATRIX_TYPE *x, int len) {
    MATRIX_TYPE value = 0;
    int i;
    for (i = 0; i < len; i++) {
        value += LP_cost_get(mat_C, i) * x[i];
    }
    return value;
}

static Matrix *LP_array_to_column(const MATRIX_TYPE *data, int len) {
    Matrix *result = M_Zeros(len, 1);
    int i;
    if (result == NULL) {
        return NULL;
    }
    for (i = 0; i < len; i++) {
        result->data[i] = data[i];
    }
    return result;
}

static Matrix *LP_array_to_row(const MATRIX_TYPE *data, int len) {
    Matrix *result = M_Zeros(1, len);
    int i;
    if (result == NULL) {
        return NULL;
    }
    for (i = 0; i < len; i++) {
        result->data[i] = data[i];
    }
    return result;
}

static MATRIX_TYPE LP_basis_objective(Matrix *mat_C, Matrix *basis, Matrix *b_bar,
                                      int base_num) {
    MATRIX_TYPE value = 0;
    int row;
    if ((basis == NULL) || (b_bar == NULL)) {
        return 0;
    }
    for (row = 0; row < base_num; row++) {
        value += LP_cost_get(mat_C, (int)basis->data[row]) * b_bar->data[row];
    }
    return value;
}

static MATRIX_TYPE LP_max_reduced_cost(Matrix *delta) {
    MATRIX_TYPE value = 0;
    int col;
    if (delta == NULL) {
        return 0;
    }
    for (col = 0; col < delta->column; col++) {
        if (delta->data[col] > value) {
            value = delta->data[col];
        }
    }
    return value;
}

static int LP_first_negative_rhs(Matrix *b_bar, MATRIX_TYPE tol) {
    int row;
    if (b_bar == NULL) {
        return -1;
    }
    for (row = 0; row < b_bar->row; row++) {
        if (b_bar->data[row] < -tol) {
            return row;
        }
        if (fabs(b_bar->data[row]) <= tol) {
            b_bar->data[row] = 0;
        }
    }
    return -1;
}

static int LP_rebuild_tableau(Matrix *mat_A, Matrix *b_col, Matrix *basis,
                              MATRIX_TYPE tol, Matrix **A_bar, Matrix **b_bar) {
    Matrix *B_inv = NULL;
    Matrix *new_A = NULL;
    Matrix *new_b = NULL;
    if (LP_basis_is_identity(mat_A, basis, tol)) {
        new_A = Matrix_copy(mat_A);
        new_b = Matrix_copy(b_col);
    } else {
        B_inv = LP_basis_inverse(mat_A, basis, tol);
        if (B_inv == NULL) {
            return 0;
        }
        new_A = M_mul(B_inv, mat_A);
        new_b = M_mul(B_inv, b_col);
        M_free(B_inv);
    }
    if ((new_A == NULL) || (new_b == NULL)) {
        M_free(new_A);
        M_free(new_b);
        return 0;
    }
    M_free(*A_bar);
    M_free(*b_bar);
    *A_bar = new_A;
    *b_bar = new_b;
    return 1;
}

static int LP_recompute_delta(Matrix *mat_C, Matrix *basis,
                              Matrix *A_bar, Matrix *b_bar,
                              Matrix **delta, MATRIX_TYPE *values_opt,
                              MATRIX_TYPE tol) {
    Matrix *c_base = NULL;
    Matrix *c_base_A = NULL;
    Matrix *new_delta = NULL;
    int col;
    c_base = LP_cost_basis(mat_C, basis);
    if (c_base == NULL) {
        return 0;
    }
    c_base_A = M_mul(c_base, A_bar);
    new_delta = M_Zeros(1, A_bar->column);
    if ((c_base == NULL) || (c_base_A == NULL) || (new_delta == NULL)) {
        M_free(c_base);
        M_free(c_base_A);
        M_free(new_delta);
        return 0;
    }
    for (col = 0; col < A_bar->column; col++) {
        new_delta->data[col] = LP_cost_get(mat_C, col) - c_base_A->data[col];
        if (fabs(new_delta->data[col]) <= tol) {
            new_delta->data[col] = 0;
        }
    }
    *values_opt = LP_basis_objective(mat_C, basis, b_bar, b_bar->row);
    M_free(*delta);
    *delta = new_delta;
    M_free(c_base);
    M_free(c_base_A);
    return 1;
}

static M_LP_struct *LP_make_result(Matrix *A_bar, Matrix *b_bar, Matrix *mat_C,
                                   Matrix *basis, Matrix *delta,
                                   int status, MATRIX_TYPE opt_value, int iter_num) {
    M_LP_struct *result = (M_LP_struct*)malloc(sizeof(M_LP_struct));
    if (result == NULL) {
        return NULL;
    }
    result->_matrix_A = A_bar;
    result->_matrix_b = b_bar;
    result->_matrix_c = Matrix_copy(mat_C);
    result->_matrix_base = basis;
    result->_matrix_delta = delta;
    result->OPT_STATUS = status;
    result->values_opt = opt_value;
    result->iter_num = iter_num;
    if (result->_matrix_c == NULL) {
        LP_free(result);
        return NULL;
    }
    return result;
}

M_LP_struct * LP_Solver(Matrix* mat_A,Matrix* mat_B,Matrix* mat_C, int LP_method){
    return LP_Solver_With_Progress(mat_A, mat_B, mat_C, LP_method, 0);
}

M_LP_struct * LP_Solver_With_Progress(Matrix* mat_A,Matrix* mat_B,Matrix* mat_C,
                                      int LP_method, int progress_interval){
    LP_Options options = LP_Default_Options();
    options.method = LP_method;
    options.progress_interval = progress_interval;
    return LP_Solver_With_Options(mat_A, mat_B, mat_C, &options);
}

M_LP_struct * LP_Solver_With_Options(Matrix* mat_A,Matrix* mat_B,Matrix* mat_C,
                                     const LP_Options *options_in){
    LP_Options options = LP_normalized_options(options_in);
    double start_seconds = LP_now_seconds();
    if (options.method == LP_METHOD_INTERIOR_POINT) {
        return LP_interior_point_method_with_options(mat_A, mat_B, mat_C,
                                                     &options, start_seconds);
    }
    return LP_simplex_method_with_options(mat_A, mat_B, mat_C, &options,
                                          start_seconds);
}

M_LP_struct * LP_interior_point_method(Matrix* mat_A,Matrix* mat_B,Matrix* mat_C){
    return LP_interior_point_method_with_progress(mat_A, mat_B, mat_C, 0);
}

M_LP_struct * LP_interior_point_method_with_progress(Matrix* mat_A,Matrix* mat_B,
                                                     Matrix* mat_C, int progress_interval){
    LP_Options options = LP_Default_Options();
    options.method = LP_METHOD_INTERIOR_POINT;
    options.progress_interval = progress_interval;
    return LP_interior_point_method_with_options(mat_A, mat_B, mat_C,
                                                 &options, LP_now_seconds());
}

M_LP_struct * LP_interior_point_method_with_options(Matrix* mat_A,Matrix* mat_B,
                                                    Matrix* mat_C,
                                                    const LP_Options *options_in,
                                                    double start_seconds){/*
 * Compact primal-dual interior-point method for:
 *     max c*x
 *     s.t. A*x = b, x >= 0
 * This compact dense implementation uses infeasible-start Newton steps and
 * solves the normal equation with plugin_Linear_Solver.h.
 */
    LP_Options options = LP_normalized_options(options_in);
    int progress_interval = options.progress_interval;
    MATRIX_TYPE time_limit_seconds = options.time_limit_seconds;
    MATRIX_TYPE gap_limit = options.gap_limit;
    int m, n;
    int i, j, k, iter = 0;
    int status = LP_STATUS_ITER_LIMIT;
    MATRIX_TYPE tol;
    MATRIX_TYPE stop_tol;
    MATRIX_TYPE alpha_p, alpha_d;
    MATRIX_TYPE values_opt = 0;
    MATRIX_TYPE no_basis_data = -1;
    MATRIX_TYPE *x = NULL, *s = NULL, *y = NULL;
    MATRIX_TYPE *dx = NULL, *ds = NULL, *dy = NULL;
    MATRIX_TYPE *rp = NULL, *rd = NULL, *rc = NULL;
    MATRIX_TYPE *d = NULL, *h = NULL, *rhs = NULL, *atdy = NULL;
    Matrix *b_col = NULL;
    Matrix *H = NULL, *rhs_mat = NULL, *dy_mat = NULL;
    Matrix *x_result = NULL, *s_result = NULL, *basis = NULL, *A_result = NULL;

    if (!LP_validate_input(mat_A, mat_B, mat_C)) {
        printf("[ERROR] LP input should be A(m,n), b(m,1), c(1,n), with m <= n.\n");
        return NULL;
    }

    m = mat_A->row;
    n = mat_A->column;
    tol = LP_tolerance(mat_A, mat_B, mat_C);
    stop_tol = 100 * tol;
    if (stop_tol < 1e-8) {
        stop_tol = 1e-8;
    }

    b_col = LP_vector_to_column(mat_B, m);
    x = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * n);
    s = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * n);
    y = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * m);
    dx = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * n);
    ds = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * n);
    dy = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * m);
    rp = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * m);
    rd = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * n);
    rc = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * n);
    d = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * n);
    h = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * m * m);
    rhs = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * m);
    atdy = (MATRIX_TYPE *)malloc(sizeof(MATRIX_TYPE) * n);
    if ((b_col == NULL) || (x == NULL) || (s == NULL) || (y == NULL) ||
        (dx == NULL) || (ds == NULL) || (dy == NULL) || (rp == NULL) ||
        (rd == NULL) || (rc == NULL) || (d == NULL) || (h == NULL) ||
        (rhs == NULL) || (atdy == NULL)) {
        status = LP_STATUS_INPUT_ERROR;
        goto IPM_END;
    }

    for (j = 0; j < n; j++) {
        x[j] = 1;
        s[j] = 1;
    }
    for (i = 0; i < m; i++) {
        y[i] = 0;
    }

    for (iter = 0; iter < 100; iter++) {
        MATRIX_TYPE mu = 0;
        MATRIX_TYPE rp_norm;
        MATRIX_TYPE rd_norm;
        MATRIX_TYPE obj_scale;

        if (LP_time_limit_reached(start_seconds, time_limit_seconds)) {
            status = LP_STATUS_TIME_LIMIT;
            if (progress_interval > 0) {
                printf("[LP-IPM] iter=%d obj=%.10g status=time_limit\n",
                       iter, values_opt);
            }
            break;
        }

        for (i = 0; i < m; i++) {
            MATRIX_TYPE ax = 0;
            for (j = 0; j < n; j++) {
                ax += mat_A->data[i * n + j] * x[j];
            }
            rp[i] = b_col->data[i] - ax;
        }
        for (j = 0; j < n; j++) {
            MATRIX_TYPE aty = 0;
            for (i = 0; i < m; i++) {
                aty += mat_A->data[i * n + j] * y[i];
            }
            /* Solve max c*x by applying the primal-dual equations to
             * the equivalent minimization problem min -c*x. */
            rd[j] = -LP_cost_get(mat_C, j) - aty - s[j];
            mu += x[j] * s[j];
        }
        mu /= n;
        values_opt = LP_dot_cost_vector(mat_C, x, n);
        rp_norm = LP_l2_norm(rp, m);
        rd_norm = LP_l2_norm(rd, n);
        obj_scale = fabs(values_opt);
        if (obj_scale < 1) {
            obj_scale = 1;
        }
        if ((progress_interval > 0) &&
            ((iter == 0) || (iter % progress_interval == 0))) {
            printf("[LP-IPM] iter=%d obj=%.10g primal=%.3g dual=%.3g mu=%.3g\n",
                   iter, values_opt, rp_norm, rd_norm, mu);
        }
        if ((rp_norm <= stop_tol * (1 + M_fro_norm_raw(b_col))) &&
            (rd_norm <= stop_tol * (1 + M_fro_norm_raw(mat_C))) &&
            (mu <= stop_tol * obj_scale)) {
            status = LP_STATUS_OPTIMAL;
            if (progress_interval > 0) {
                printf("[LP-IPM] iter=%d obj=%.10g status=optimal\n", iter, values_opt);
            }
            break;
        }
        if ((gap_limit > 0) && ((mu / obj_scale) <= gap_limit)) {
            status = LP_STATUS_GAP_LIMIT;
            if (progress_interval > 0) {
                printf("[LP-IPM] iter=%d obj=%.10g gap=%.3g status=gap_limit\n",
                       iter, values_opt, mu / obj_scale);
            }
            break;
        }

        for (j = 0; j < n; j++) {
            rc[j] = 0.1 * mu - x[j] * s[j];
            d[j] = (rc[j] - x[j] * rd[j]) / s[j];
        }

        for (i = 0; i < m; i++) {
            rhs[i] = rp[i];
            for (j = 0; j < n; j++) {
                rhs[i] -= mat_A->data[i * n + j] * d[j];
            }
            for (k = 0; k < m; k++) {
                MATRIX_TYPE value = 0;
                for (j = 0; j < n; j++) {
                    value += mat_A->data[i * n + j] * (x[j] / s[j]) * mat_A->data[k * n + j];
                }
                h[i * m + k] = value;
            }
            h[i * m + i] += stop_tol;
        }

        H = Matrix_gen(m, m, h);
        rhs_mat = Matrix_gen(m, 1, rhs);
        dy_mat = Linear_Solve_With_Tolerance(H, rhs_mat, _MATRIX_ABS_EPS_);
        if (dy_mat == NULL) {
            status = LP_STATUS_INPUT_ERROR;
            goto IPM_END;
        }
        for (i = 0; i < m; i++) {
            dy[i] = dy_mat->data[i];
        }
        M_free(H);
        M_free(rhs_mat);
        M_free(dy_mat);
        H = NULL;
        rhs_mat = NULL;
        dy_mat = NULL;

        for (j = 0; j < n; j++) {
            atdy[j] = 0;
            for (i = 0; i < m; i++) {
                atdy[j] += mat_A->data[i * n + j] * dy[i];
            }
            dx[j] = d[j] + (x[j] / s[j]) * atdy[j];
            ds[j] = rd[j] - atdy[j];
        }

        alpha_p = 1;
        alpha_d = 1;
        for (j = 0; j < n; j++) {
            if (dx[j] < 0) {
                MATRIX_TYPE candidate = -0.99 * x[j] / dx[j];
                if (candidate < alpha_p) {
                    alpha_p = candidate;
                }
            }
            if (ds[j] < 0) {
                MATRIX_TYPE candidate = -0.99 * s[j] / ds[j];
                if (candidate < alpha_d) {
                    alpha_d = candidate;
                }
            }
        }

        for (j = 0; j < n; j++) {
            x[j] += alpha_p * dx[j];
            s[j] += alpha_d * ds[j];
        }
        for (i = 0; i < m; i++) {
            y[i] += alpha_d * dy[i];
        }
    }

    IPM_END:
    values_opt = (x != NULL) ? LP_dot_cost_vector(mat_C, x, n) : 0;
    x_result = (x != NULL) ? LP_array_to_column(x, n) : NULL;
    s_result = (s != NULL) ? LP_array_to_row(s, n) : NULL;
    basis = Matrix_gen(1, 1, &no_basis_data);
    A_result = Matrix_copy(mat_A);

    if (H != NULL) {
        M_free(H);
    }
    if (rhs_mat != NULL) {
        M_free(rhs_mat);
    }
    if (dy_mat != NULL) {
        M_free(dy_mat);
    }
    if (b_col != NULL) {
        M_free(b_col);
    }
    free(x);
    free(s);
    free(y);
    free(dx);
    free(ds);
    free(dy);
    free(rp);
    free(rd);
    free(rc);
    free(d);
    free(h);
    free(rhs);
    free(atdy);

    if ((progress_interval > 0) && (status != LP_STATUS_OPTIMAL)) {
        printf("[LP-IPM] iter=%d obj=%.10g status=%d\n", iter, values_opt, status);
    }
    return LP_make_result(A_result, x_result, mat_C, basis, s_result, status, values_opt, iter);
}


M_LP_struct * LP_simplex_method(Matrix* mat_A,Matrix* mat_B,Matrix* mat_C){
    return LP_simplex_method_with_progress(mat_A, mat_B, mat_C, 0);
}

M_LP_struct * LP_simplex_method_with_progress(Matrix* mat_A,Matrix* mat_B,
                                              Matrix* mat_C, int progress_interval){
    LP_Options options = LP_Default_Options();
    options.method = LP_METHOD_REVISED_SIMPLEX;
    options.progress_interval = progress_interval;
    return LP_simplex_method_with_options(mat_A, mat_B, mat_C, &options,
                                          LP_now_seconds());
}

M_LP_struct * LP_simplex_method_with_options(Matrix* mat_A,Matrix* mat_B,
                                             Matrix* mat_C,
                                             const LP_Options *options_in,
                                             double start_seconds){
    /*
     * Two-phase revised simplex wrapper for:
     *     max c*x
     *     s.t. A*x = b, x >= 0
     *
     * The original MatrixHub examples usually pass slack columns, so the
     * identity-basis fast path is still used first. If that basis is not
     * feasible, Phase-I adds artificial variables, drives their sum to zero,
     * and reuses the recovered basis for the real objective.
     */
    LP_Options options = LP_normalized_options(options_in);
    int progress_interval = options.progress_interval;
    MATRIX_TYPE time_limit_seconds = options.time_limit_seconds;
    MATRIX_TYPE gap_limit = options.gap_limit;
    MATRIX_TYPE tol;
    int phase_status = LP_STATUS_INPUT_ERROR;
    int needs_phase_one = 0;
    Matrix *A_normalized = NULL;
    Matrix *b_normalized = NULL;
    Matrix *phase_basis = NULL;
    Matrix *phase_A_bar = NULL;
    Matrix *phase_b_bar = NULL;
    M_LP_struct *result = NULL;

    if (!LP_validate_input(mat_A, mat_B, mat_C)) {
        printf("[ERROR] LP input should be A(m,n), b(m,1), c(1,n), with m <= n.\n");
        return NULL;
    }

    tol = LP_tolerance(mat_A, mat_B, mat_C);
    needs_phase_one = LP_has_negative_rhs(mat_B, mat_A->row, tol);
    A_normalized = LP_copy_normalized_problem(mat_A, mat_B, &b_normalized, tol);
    if ((A_normalized == NULL) || (b_normalized == NULL)) {
        M_free(A_normalized);
        M_free(b_normalized);
        return NULL;
    }

    if (!needs_phase_one) {
        result = LP_simplex_core(A_normalized, b_normalized, mat_C, NULL,
                                 NULL, NULL,
                                 progress_interval, start_seconds,
                                 time_limit_seconds, gap_limit,
                                 "LP-SIMPLEX", 0);
        if ((result != NULL) && (result->OPT_STATUS != LP_STATUS_INFEASIBLE)) {
            M_free(A_normalized);
            M_free(b_normalized);
            return result;
        }
        LP_free(result);
        result = NULL;
    }

    if (progress_interval > 0) {
        printf("[LP-PHASE-I] building feasible initial basis\n");
    }
    phase_basis = LP_make_phase_one_basis(A_normalized, b_normalized, tol,
                                          progress_interval,
                                          start_seconds, time_limit_seconds,
                                          0,
                                          &phase_A_bar, &phase_b_bar,
                                          &phase_status);
    if (phase_basis == NULL) {
        Matrix *basis = M_Zeros(1, A_normalized->row);
        Matrix *delta = M_Zeros(1, A_normalized->column);
        int i;
        if (basis != NULL) {
            for (i = 0; i < basis->column; i++) {
                basis->data[i] = -1;
            }
        }
        result = LP_make_result(Matrix_copy(A_normalized),
                                Matrix_copy(b_normalized),
                                mat_C, basis, delta, phase_status, 0, 0);
        M_free(phase_A_bar);
        M_free(phase_b_bar);
        M_free(A_normalized);
        M_free(b_normalized);
        return result;
    }

    result = LP_simplex_core(A_normalized, b_normalized, mat_C, phase_basis,
                             phase_A_bar, phase_b_bar,
                             progress_interval, start_seconds,
                             time_limit_seconds, gap_limit,
                             "LP-SIMPLEX", 1);
    M_free(phase_basis);
    M_free(phase_A_bar);
    M_free(phase_b_bar);
    M_free(A_normalized);
    M_free(b_normalized);
    return result;
}

static M_LP_struct *LP_simplex_core(Matrix *mat_A, Matrix *mat_B, Matrix *mat_C,
                                    Matrix *initial_basis,
                                    Matrix *initial_A_bar,
                                    Matrix *initial_b_bar,
                                    int progress_interval,
                                    double start_seconds,
                                    MATRIX_TYPE time_limit_seconds,
                                    MATRIX_TYPE gap_limit,
                                    const char *progress_name,
                                    int print_basis_error){/*
 * Simplex method for:
 *     max c*x
 *     s.t. A*x = b, x >= 0
 * If initial_basis is NULL, the core tries the compact basis search used by
 * the original solver. The wrapper above supplies a Phase-I basis when that
 * direct start is infeasible.
 */
    int base_num, variable_num, row, col;
    int Counter = 0;
    int degenerate_counter = 0;
    int status = LP_STATUS_INPUT_ERROR;
    int entering = -1;
    int leaving = -1;
    MATRIX_TYPE tol;
    MATRIX_TYPE values_opt = 0;
    Matrix *basis = NULL;
    Matrix *b_col = NULL;
    Matrix *B_inv = NULL;
    Matrix *A_bar = NULL;
    Matrix *b_bar = NULL;
    Matrix *delta = NULL;

    if (progress_name == NULL) {
        progress_name = "LP-SIMPLEX";
    }
    if (!LP_validate_input(mat_A, mat_B, mat_C)) {
        if (print_basis_error) {
            printf("[ERROR] LP input should be A(m,n), b(m,1), c(1,n), with m <= n.\n");
        }
        return NULL;
    }

    base_num = mat_A->row;
    variable_num = mat_A->column;
    tol = LP_tolerance(mat_A, mat_B, mat_C);
    if (initial_basis != NULL) {
        if ((initial_basis->row != 1) || (initial_basis->column != base_num)) {
            return NULL;
        }
        basis = Matrix_copy(initial_basis);
    } else {
        basis = M_Zeros(1, base_num);
    }
    b_col = LP_vector_to_column(mat_B, base_num);
    if ((basis == NULL) || (b_col == NULL)) {
        M_free(basis);
        M_free(b_col);
        return NULL;
    }

    if ((initial_basis == NULL) &&
        !LP_find_identity_basis(mat_A, basis, tol) &&
        !LP_find_pivot_basis(mat_A, basis, tol)) {
        if (print_basis_error) {
            printf("[ERROR] No initial basis was found. Phase-I artificial columns are needed.\n");
        }
        M_free(basis);
        M_free(b_col);
        return NULL;
    }

    if ((initial_A_bar != NULL) || (initial_b_bar != NULL)) {
        if ((initial_A_bar == NULL) || (initial_b_bar == NULL) ||
            (initial_A_bar->row != base_num) ||
            (initial_A_bar->column != variable_num) ||
            (initial_b_bar->row != base_num) ||
            (initial_b_bar->column != 1)) {
            status = LP_STATUS_INPUT_ERROR;
            goto END_OPT;
        }
        A_bar = Matrix_copy(initial_A_bar);
        b_bar = Matrix_copy(initial_b_bar);
    } else if (LP_basis_is_identity(mat_A, basis, tol)) {
        A_bar = Matrix_copy(mat_A);
        b_bar = Matrix_copy(b_col);
    } else {
        B_inv = LP_basis_inverse(mat_A, basis, tol);
        if (B_inv == NULL) {
            status = LP_STATUS_INPUT_ERROR;
            goto END_OPT;
        }
        A_bar = M_mul(B_inv, mat_A);
        b_bar = M_mul(B_inv, b_col);
        M_free(B_inv);
        B_inv = NULL;
    }
    if ((A_bar == NULL) || (b_bar == NULL)) {
        status = LP_STATUS_INPUT_ERROR;
        goto END_OPT;
    }
    if (!LP_recompute_delta(mat_C, basis, A_bar, b_bar,
                            &delta, &values_opt, tol)) {
        status = LP_STATUS_INPUT_ERROR;
        goto END_OPT;
    }

    while(Counter <= _MAX_LOOP_NUM_) {
        if (LP_time_limit_reached(start_seconds, time_limit_seconds)) {
            status = LP_STATUS_TIME_LIMIT;
            if (progress_interval > 0) {
                printf("[%s] iter=%d obj=%.10g status=time_limit\n",
                       progress_name, Counter, values_opt);
            }
            break;
        }

        if (LP_first_negative_rhs(b_bar, tol) >= 0) {
            if (!LP_rebuild_tableau(mat_A, b_col, basis, tol, &A_bar, &b_bar) ||
                !LP_recompute_delta(mat_C, basis, A_bar, b_bar,
                                    &delta, &values_opt, tol)) {
                status = LP_STATUS_INPUT_ERROR;
                goto END_OPT;
            }
            if (LP_first_negative_rhs(b_bar, tol) >= 0) {
                status = LP_STATUS_INFEASIBLE;
                goto END_OPT;
            }
        }

        if(!_SILENT_MODE_){
            printf("[BASE]\n");
            M_print(basis);
            printf("[TABLE] A/b\n");
            M_print(A_bar);
            M_print(b_bar);
            printf("[DELTA-OPT]\n");
            M_print(delta);
        }

        {
            MATRIX_TYPE max_delta = LP_max_reduced_cost(delta);
            entering = LP_select_entering(delta, tol,
                                          degenerate_counter >= LP_DEGENERATE_PIVOT_LIMIT);
            if ((progress_interval > 0) &&
                ((Counter == 0) || (Counter % progress_interval == 0))) {
                printf("[%s] iter=%d obj=%.10g max_delta=%.3g entering=%d pricing=%s\n",
                       progress_name, Counter, values_opt,
                       max_delta, entering,
                       (degenerate_counter >= LP_DEGENERATE_PIVOT_LIMIT) ? "bland" : "dantzig");
            }
            if (entering < 0) {
                status = LP_STATUS_OPTIMAL;
                if (progress_interval > 0) {
                    printf("[%s] iter=%d obj=%.10g status=optimal\n",
                           progress_name, Counter, values_opt);
                }
                break;
            }
            if ((gap_limit > 0) && (max_delta <= gap_limit)) {
                status = LP_STATUS_GAP_LIMIT;
                if (progress_interval > 0) {
                    printf("[%s] iter=%d obj=%.10g gap=%.3g status=gap_limit\n",
                           progress_name, Counter, values_opt, max_delta);
                }
                break;
            }
        }

        leaving = LP_select_leaving(A_bar, b_bar, basis, entering, tol,
                                    degenerate_counter >= LP_DEGENERATE_PIVOT_LIMIT);
        if (leaving < 0) {
            status = LP_STATUS_UNBOUNDED;
            if (progress_interval > 0) {
                printf("[%s] iter=%d obj=%.10g status=unbounded\n",
                       progress_name, Counter, values_opt);
            }
            break;
        }

        {
            MATRIX_TYPE objective_step;
            MATRIX_TYPE pivot = A_bar->data[leaving * variable_num + entering];
            if (fabs(pivot) <= tol) {
                status = LP_STATUS_INPUT_ERROR;
                break;
            }
            objective_step = delta->data[entering] * b_bar->data[leaving] / pivot;
            if (fabs(objective_step) <= 100 * tol) {
                degenerate_counter++;
            } else {
                degenerate_counter = 0;
            }
            for (col = 0; col < variable_num; col++) {
                A_bar->data[leaving * variable_num + col] /= pivot;
            }
            b_bar->data[leaving] /= pivot;
            {
                MATRIX_TYPE delta_entering = delta->data[entering];
                for (col = 0; col < variable_num; col++) {
                    delta->data[col] -= delta_entering *
                        A_bar->data[leaving * variable_num + col];
                    if (fabs(delta->data[col]) <= tol) {
                        delta->data[col] = 0;
                    }
                }
                delta->data[entering] = 0;
                values_opt += objective_step;
            }
            for (row = 0; row < base_num; row++) {
                MATRIX_TYPE factor;
                if (row == leaving) {
                    continue;
                }
                factor = A_bar->data[row * variable_num + entering];
                if (fabs(factor) <= tol) {
                    A_bar->data[row * variable_num + entering] = 0;
                    continue;
                }
                for (col = 0; col < variable_num; col++) {
                    A_bar->data[row * variable_num + col] -=
                        factor * A_bar->data[leaving * variable_num + col];
                    if (fabs(A_bar->data[row * variable_num + col]) <= tol) {
                        A_bar->data[row * variable_num + col] = 0;
                    }
                }
                b_bar->data[row] -= factor * b_bar->data[leaving];
                if (fabs(b_bar->data[row]) <= tol) {
                    b_bar->data[row] = 0;
                }
            }
        }
        basis->data[leaving] = entering;
        Counter += 1;
    }

    if (Counter > _MAX_LOOP_NUM_) {
        status = LP_STATUS_ITER_LIMIT;
        if (progress_interval > 0) {
            printf("[%s] iter=%d obj=%.10g status=iter_limit\n",
                   progress_name, Counter, values_opt);
        }
    }

    END_OPT:
    if (b_bar != NULL) {
        values_opt = LP_basis_objective(mat_C, basis, b_bar, base_num);
    }

    if (B_inv != NULL) {
        M_free(B_inv);
    }
    if (b_col != NULL) {
        M_free(b_col);
    }
    /* Result owns A_bar, b_bar, basis, delta, and a copy of c. */
    return LP_make_result(A_bar, b_bar, mat_C, basis, delta, status, values_opt, Counter);
}
