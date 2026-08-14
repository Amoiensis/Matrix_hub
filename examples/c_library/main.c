/*
 * MatrixHub v2.0 C-library example.
 *
 * This example is intentionally a little broader than a minimal smoke test:
 * it shows common matrix construction, algebra, transforms, decompositions,
 * linear solving, and the optional LP/MIP solver plugins.
 */

#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"
#include "solver_plugin/plugin_Linear_Solver.h"
#include "solver_plugin/plugin_LP_Sover.h"
#include "solver_plugin/plugin_MIP_Solver.h"

enum {
    EIG_VAL = 0,
    EIG_VEC = 1,
    QR_Q = 0,
    QR_R = 1,
    SVD_U = 0,
    SVD_S = 1,
    SVD_V = 2
};

static void print_section(const char *title) {
    printf("\n==== %s ====\n", title);
}

static void print_matrix(const char *label, Matrix *mat) {
    printf("%s\n", label);
    if (mat == NULL) {
        printf("(null)\n");
        return;
    }
    M_print(mat);
}

static void free_uptri(Uptri_struct *uptri) {
    if (uptri == NULL) {
        return;
    }
    M_free(uptri->trans_matrix);
    M_free(uptri->Uptri_matrix);
    free(uptri);
}

static void free_lowtri(Lowtri_struct *lowtri) {
    if (lowtri == NULL) {
        return;
    }
    M_free(lowtri->trans_matrix);
    M_free(lowtri->Lowtri_matrix);
    free(lowtri);
}

static void free_dia(Dia_struct *dia) {
    if (dia == NULL) {
        return;
    }
    M_free(dia->trans_leftmatrix);
    M_free(dia->Diatri_matrix);
    M_free(dia->trans_rightmatrix);
    free(dia);
}

static void free_matrix_array(Matrix **items, int count) {
    int i;
    if (items == NULL) {
        return;
    }
    for (i = 0; i < count; i++) {
        M_free(items[i]);
    }
    free(items);
}

static void demo_creation(void) {
    MATRIX_TYPE a_data[9] = {
        4, 1, 2,
        1, 3, 0,
        2, 0, 5
    };
    Matrix *A = Matrix_gen(3, 3, a_data);
    Matrix *A_copy = Matrix_copy(A);
    Matrix *I = M_I(3);
    Matrix *Z = M_Zeros(2, 3);
    Matrix *O = M_Ones(2, 3);
    Matrix *H = Hilbert(4);

    print_section("Matrix creation");
    print_matrix("A = Matrix_gen(...)", A);
    print_matrix("Matrix_copy(A)", A_copy);
    print_matrix("M_I(3)", I);
    print_matrix("M_Zeros(2, 3)", Z);
    print_matrix("M_Ones(2, 3)", O);
    print_matrix("Hilbert(4)", H);

    M_free(A);
    M_free(A_copy);
    M_free(I);
    M_free(Z);
    M_free(O);
    M_free(H);
}

static void demo_basic_algebra(void) {
    MATRIX_TYPE a_data[9] = {
        4, 1, 2,
        1, 3, 0,
        2, 0, 5
    };
    MATRIX_TYPE b_data[9] = {
        1, 2, 0,
        0, 1, 2,
        2, 0, 1
    };
    MATRIX_TYPE v_data[3] = {2, -1, 3};
    Matrix *A = Matrix_gen(3, 3, a_data);
    Matrix *B = Matrix_gen(3, 3, b_data);
    Matrix *v = Matrix_gen(3, 1, v_data);
    Matrix *diff = M_add_sub(1, A, 1, B);
    Matrix *sum = M_add_sub(1, A, -1, B);
    Matrix *product = M_mul(A, B);
    Matrix *Av = M_mul(A, v);
    Matrix *scaled = Matrix_copy(A);
    Matrix *hadamard = M_pmuldiv(A, B, _MUL_);
    Matrix *transpose = M_T(A);

    M_numul(scaled, 2.0);

    print_section("Basic algebra");
    print_matrix("A + B, via M_add_sub(1, A, -1, B)", sum);
    print_matrix("A - B, via M_add_sub(1, A, 1, B)", diff);
    print_matrix("A * B", product);
    print_matrix("A * v", Av);
    print_matrix("2 * A", scaled);
    print_matrix("Hadamard product pmul(A, B)", hadamard);
    print_matrix("A^T", transpose);

    M_free(A);
    M_free(B);
    M_free(v);
    M_free(sum);
    M_free(diff);
    M_free(product);
    M_free(Av);
    M_free(scaled);
    M_free(hadamard);
    M_free(transpose);
}

static void demo_indexing_and_editing(void) {
    MATRIX_TYPE a_data[9] = {
        4, 1, 2,
        1, 3, 0,
        2, 0, 5
    };
    MATRIX_TYPE sample_cols_data[2] = {0, 2};
    MATRIX_TYPE sample_rows_data[2] = {0, 2};
    MATRIX_TYPE set_target_data[5] = {0, 0, 0, 0, 0};
    MATRIX_TYPE set_values_data[2] = {9, 7};
    MATRIX_TYPE set_order_data[2] = {1, 3};
    MATRIX_TYPE row_scale_data[3] = {1, 2, 3};
    Matrix *A = Matrix_gen(3, 3, a_data);
    Matrix *cut = M_Cut(A, 1, 2, 2, 3);
    Matrix *sample_cols = Matrix_gen(1, 2, sample_cols_data);
    Matrix *sample_rows = Matrix_gen(2, 1, sample_rows_data);
    Matrix *cols = M_Sample(A, sample_cols, _COLUMN_);
    Matrix *rows = M_Sample(A, sample_rows, _ROW_);
    Matrix *padded = M_full(A, 1, 1, 1, 1, 0);
    Matrix *target = M_Zeros(4, 4);
    Matrix *patch = M_Ones(2, 2);
    Matrix *set_target = Matrix_gen(1, 5, set_target_data);
    Matrix *set_values = Matrix_gen(1, 2, set_values_data);
    Matrix *set_order = Matrix_gen(1, 2, set_order_data);
    Matrix *swapped = Matrix_copy(A);
    Matrix *row_scaled = Matrix_copy(A);
    Matrix *row_scale = Matrix_gen(3, 1, row_scale_data);

    M_matFull(target, 2, 2, patch);
    M_setval(set_target, set_values, set_order, _ORD4INI_);
    M_Swap(swapped, 1, 3, _ROW_);
    M_numul_m(row_scaled, row_scale);

    print_section("Indexing, sampling, and editing");
    print_matrix("M_Cut(A, 1, 2, 2, 3) uses 1-based inclusive ranges", cut);
    print_matrix("Sample columns 0 and 2 with M_Sample(..., _COLUMN_)", cols);
    print_matrix("Sample rows 0 and 2 with M_Sample(..., _ROW_)", rows);
    print_matrix("M_full(A, 1, 1, 1, 1, 0)", padded);
    print_matrix("M_matFull(target, 2, 2, ones(2,2))", target);
    print_matrix("M_setval target at flat positions 1 and 3", set_target);
    print_matrix("M_Swap(A, 1, 3, _ROW_)", swapped);
    print_matrix("M_numul_m(A, [1;2;3]) scales each row", row_scaled);

    M_free(A);
    M_free(cut);
    M_free(sample_cols);
    M_free(sample_rows);
    M_free(cols);
    M_free(rows);
    M_free(padded);
    M_free(target);
    M_free(patch);
    M_free(set_target);
    M_free(set_values);
    M_free(set_order);
    M_free(swapped);
    M_free(row_scaled);
    M_free(row_scale);
}

static void demo_properties_and_logic(void) {
    MATRIX_TYPE a_data[9] = {
        4, 1, 2,
        1, 3, 0,
        2, 0, 5
    };
    MATRIX_TYPE neg_data[6] = {
        -1, 2, -3,
        4, -5, 6
    };
    MATRIX_TYPE v_data[3] = {2, -1, 3};
    Matrix *A = Matrix_gen(3, 3, a_data);
    Matrix *neg = Matrix_gen(2, 3, neg_data);
    Matrix *v = Matrix_gen(3, 1, v_data);
    Matrix *abs_mat = M_abs(neg);
    Matrix *sum_cols = M_sum(A);
    Matrix *min_pos = M_min(A);
    Matrix *max_pos = M_max(A);
    Matrix *max_val = M_minax_val(A, max_pos);
    Matrix *logic_eq = M_logic_equal(A, 0);
    Matrix *logic_not = M_logic(logic_eq, NULL, _NOT_);
    Matrix *I = M_I(3);
    Matrix *find_ones = M_find(I, 1);

    print_section("Properties, reductions, and logic");
    printf("trace(A) = %.10g\n", M_tr(A));
    printf("det(A)   = %.10g\n", M_det(A));
    printf("rank(A)  = %d\n", M_rank(A));
    printf("norm(v, 2)   = %.10g\n", M_norm(v, 2));
    printf("norm(A, FRO) = %.10g\n", M_norm(A, FRO));
    printf("cond(A, 1)   = %.10g\n", M_cond(A, 1));
    print_matrix("M_abs(neg)", abs_mat);
    print_matrix("M_sum(A), column sums", sum_cols);
    print_matrix("M_min(A), row index of column minimum", min_pos);
    print_matrix("M_max(A), row index of column maximum", max_pos);
    print_matrix("M_minax_val(A, M_max(A)), column maximum values", max_val);
    print_matrix("M_logic_equal(A, 0)", logic_eq);
    print_matrix("M_logic(..., _NOT_), element-wise not", logic_not);
    print_matrix("M_find(M_I(3), 1), column-major positions", find_ones);

    M_free(A);
    M_free(neg);
    M_free(v);
    M_free(abs_mat);
    M_free(sum_cols);
    M_free(min_pos);
    M_free(max_pos);
    M_free(max_val);
    M_free(logic_eq);
    M_free(logic_not);
    M_free(I);
    M_free(find_ones);
}

static void demo_transforms_and_decompositions(void) {
    MATRIX_TYPE a_data[9] = {
        4, 1, 2,
        1, 3, 0,
        2, 0, 5
    };
    MATRIX_TYPE diag_data[9] = {
        2, 0, 0,
        0, 4, 0,
        0, 0, 5
    };
    MATRIX_TYPE x_data[3] = {4, 3, 0};
    MATRIX_TYPE svd_data[4] = {
        3, 1,
        1, 3
    };
    Matrix *A = Matrix_gen(3, 3, a_data);
    Matrix *D = Matrix_gen(3, 3, diag_data);
    Matrix *x = Matrix_gen(3, 1, x_data);
    Matrix *row_op_mat = Matrix_copy(A);
    Etrans_struct row_op;
    Matrix *row_op_matrix = NULL;
    Uptri_struct *uptri = NULL;
    Lowtri_struct *lowtri = NULL;
    Dia_struct *dia = NULL;
    Matrix *diag_inv = NULL;
    Matrix *H = NULL;
    Matrix *hessenberg = NULL;
    Matrix **qr = NULL;
    Matrix **eigen = NULL;
    M_eigen_struct *eigen_max = NULL;
    Matrix *svd_source = Matrix_gen(2, 2, svd_data);
    Matrix **svd = NULL;
    Matrix *pinv = NULL;

    row_op.minuend_line = 2;
    row_op.subtractor_line = 1;
    row_op.scale = 0.25;
    row_op.forward_E_trans = NULL;
    row_op.next_E_trans = NULL;
    M_E_trans(row_op_mat, &row_op, _ROW_);
    row_op_matrix = Etrans_2_Matrix(&row_op, 3, _ROW_);

    uptri = M_Uptri_(A);
    lowtri = M_Lowtri_(A);
    dia = M_Diatri_(A);
    diag_inv = M_Dia_Inv(D);
    H = householder(x);
    hessenberg = M_householder(A);
    qr = M_QR(A);
    eigen = M_eigen(A);
    eigen_max = M_eigen_max(A);
    svd = M_SVD(svd_source);
    pinv = M_pinv(svd_source, _SVD_);

    print_section("Transforms and decompositions");
    print_matrix("After row2 <- row2 - 0.25 * row1", row_op_mat);
    print_matrix("Etrans_2_Matrix(row_op)", row_op_matrix);
    print_matrix("M_Uptri_(A).Uptri_matrix", uptri ? uptri->Uptri_matrix : NULL);
    print_matrix("M_Lowtri_(A).Lowtri_matrix", lowtri ? lowtri->Lowtri_matrix : NULL);
    print_matrix("M_Diatri_(A).Diatri_matrix", dia ? dia->Diatri_matrix : NULL);
    print_matrix("M_Dia_Inv(diag([2,4,5]))", diag_inv);
    print_matrix("householder([4;3;0])", H);
    print_matrix("M_householder(A), Hessenberg-style transform", hessenberg);
    if (qr != NULL) {
        Matrix *qr_rebuild = M_mul(qr[QR_Q], qr[QR_R]);
        print_matrix("QR: Q", qr[QR_Q]);
        print_matrix("QR: R", qr[QR_R]);
        print_matrix("QR check: Q * R", qr_rebuild);
        M_free(qr_rebuild);
    }
    if (eigen != NULL) {
        print_matrix("M_eigen(A): eigen values", eigen[EIG_VAL]);
        print_matrix("M_eigen(A): eigen vectors", eigen[EIG_VEC]);
    }
    if (eigen_max != NULL) {
        printf("M_eigen_max(A): %.10g\n", eigen_max->eigen_value);
        print_matrix("M_eigen_max(A): eigen vector", eigen_max->eigen_matrix);
    }
    if (svd != NULL) {
        print_matrix("M_SVD(source): U", svd[SVD_U]);
        print_matrix("M_SVD(source): singular values", svd[SVD_S]);
        print_matrix("M_SVD(source): V", svd[SVD_V]);
    }
    print_matrix("M_pinv(source, _SVD_)", pinv);

    M_free(A);
    M_free(D);
    M_free(x);
    M_free(row_op_mat);
    M_free(row_op_matrix);
    free_uptri(uptri);
    free_lowtri(lowtri);
    free_dia(dia);
    M_free(diag_inv);
    M_free(H);
    M_free(hessenberg);
    free_matrix_array(qr, 2);
    free_matrix_array(eigen, 2);
    if (eigen_max != NULL) {
        M_free(eigen_max->eigen_matrix);
        free(eigen_max);
    }
    M_free(svd_source);
    free_matrix_array(svd, 3);
    M_free(pinv);
}

static void demo_linear_solver(void) {
    MATRIX_TYPE a_data[4] = {
        3, 1,
        1, 2
    };
    MATRIX_TYPE b_data[2] = {9, 8};
    Matrix *A = Matrix_gen(2, 2, a_data);
    Matrix *b = Matrix_gen(2, 1, b_data);
    Matrix *x = Linear_Solve(A, b);
    Matrix *Ax = M_mul(A, x);
    Matrix *residual = M_add_sub(1, Ax, 1, b);

    print_section("Dense linear solver plugin");
    print_matrix("Linear_Solve(A, b), where A * x = b", x);
    print_matrix("Residual A*x - b", residual);

    M_free(A);
    M_free(b);
    M_free(x);
    M_free(Ax);
    M_free(residual);
}

static void demo_lp_solver(void) {
    MATRIX_TYPE lp_a_data[15] = {
        1, 1, 1, 0, 0,
        1, 0, 0, 1, 0,
        0, 1, 0, 0, 1
    };
    MATRIX_TYPE lp_b_data[3] = {4, 2, 3};
    MATRIX_TYPE lp_c_data[5] = {3, 2, 0, 0, 0};
    Matrix *lp_A = Matrix_gen(3, 5, lp_a_data);
    Matrix *lp_b = Matrix_gen(3, 1, lp_b_data);
    Matrix *lp_c = Matrix_gen(1, 5, lp_c_data);
    M_LP_struct *lp = LP_Solver(lp_A, lp_b, lp_c, LP_METHOD_REVISED_SIMPLEX);

    print_section("LP solver plugin");
    if (lp != NULL) {
        printf("LP status=%d objective=%.10g iterations=%d\n",
               lp->OPT_STATUS, lp->values_opt, lp->iter_num);
        print_matrix("LP basis", lp->_matrix_base);
        print_matrix("LP solution-like b_bar", lp->_matrix_b);
    } else {
        printf("LP solver returned NULL.\n");
    }

    LP_free(lp);
    M_free(lp_A);
    M_free(lp_b);
    M_free(lp_c);
}

static void demo_mip_solver(void) {
    MATRIX_TYPE mip_a_data[6] = {
        1, 1,
        1, 0,
        0, 1
    };
    MATRIX_TYPE mip_b_data[3] = {4, 2, 3};
    MATRIX_TYPE mip_c_data[2] = {3, 2};
    MATRIX_TYPE integer_data[2] = {1, 1};
    Matrix *mip_A = Matrix_gen(3, 2, mip_a_data);
    Matrix *mip_b = Matrix_gen(3, 1, mip_b_data);
    Matrix *mip_c = Matrix_gen(1, 2, mip_c_data);
    Matrix *integer_mask = Matrix_gen(1, 2, integer_data);
    M_MIP_struct *mip = MIP_Solver(mip_A, mip_b, mip_c, integer_mask,
                                   MIP_METHOD_CUT_BRANCH_BOUND);

    print_section("MIP solver plugin");
    if (mip != NULL) {
        printf("MIP status=%d objective=%.10g nodes=%d cuts=%d\n",
               mip->OPT_STATUS, mip->values_opt, mip->node_num, mip->cut_num);
        print_matrix("MIP incumbent x", mip->_matrix_x);
    } else {
        printf("MIP solver returned NULL.\n");
    }

    MIP_free(mip);
    M_free(mip_A);
    M_free(mip_b);
    M_free(mip_c);
    M_free(integer_mask);
}

int main(void) {
    printf("MatrixHub v2.0 C-library example\n");
    printf("Project: https://github.com/Amoiensis/Matrix_hub\n");

    demo_creation();
    demo_basic_algebra();
    demo_indexing_and_editing();
    demo_properties_and_logic();
    demo_transforms_and_decompositions();
    demo_linear_solver();
    demo_lp_solver();
    demo_mip_solver();

    return 0;
}
