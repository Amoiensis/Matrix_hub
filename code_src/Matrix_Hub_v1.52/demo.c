/*
\ \     __  ___     __      _       __ __     __
 \ \   /  |/  /__ _/ /_____(_)_ __ / // /_ __/ /
 / /  / /|_/ / _ `/ __/ __/ /\ \ // _  / // / _ \
/ /  /_/  /_/\_,_/\__/_/ /_//_\_\/_//_/\_,_/_.__/
* [INFORMATION]
    MATRIX_HUB
    AUTHOR: Xiping.Yu
    E-MAIL: Amoiensis@outlook.com
    GITHUB: https://github.com/Amoiensis/Matrix_hub
    DATE: 2020.02.12-2022.08.21
    VERSION: 1.5.2
    CASE: Matrix Operation (C)
    DETAILS: The demo-code for Matrix_Hub.
    LICENSE: Apache-2.0
*/

#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
# include "./solver_plugin/plugin_LP_Sover.h"


int main(int argc, char *argv[]) {
    system("color 0F");

/* [Setting Matrix]*/
    //	Mat_1
    MATRIX_TYPE _mat_1[3][5] = {2, 2, 2, 1, 1,
                                4, 1, 1, 2, 1,
                                1, 5, 1, 1, 2};
    int row = sizeof(_mat_1) / sizeof(_mat_1[0]);
    int column = sizeof(_mat_1[0]) / sizeof(_mat_1[0][0]);
    Matrix *mat_1 = Matrix_gen(row, column, _mat_1);
    M_print(mat_1);
    //	Mat_2
    MATRIX_TYPE _mat_2[5][3] = {1, 2, 3, 4, 5, 6, 7, 8, 7, 10, 11, 12, 11, 14, 15};
    //	MATRIX_TYPE _mat_2[5][2] = { 1,2,3,4,5,6,7,8,9,10};
    row = sizeof(_mat_2) / sizeof(_mat_2[0]);
    column = sizeof(_mat_2[0]) / sizeof(_mat_2[0][0]);
    Matrix *mat_2 = Matrix_gen(row, column, _mat_2);
    // M_print(mat_2);
    // Mat_A
    MATRIX_TYPE _mat_A[5][5] = {1, 0, 7, 0, 9, 0, 7, 0, 9, 0, 0, 0, -5, 0, 4, 2, 0, 0, 2, 0, 0, 3, 8, 0, 1};
    row = sizeof(_mat_A) / sizeof(_mat_A[0]);
    column = sizeof(_mat_A[0]) / sizeof(_mat_A[0][0]);
    Matrix *mat_A = Matrix_gen(row, column, _mat_A);
    // M_print(mat_A);
    // A_10
    MATRIX_TYPE _mat_A10[10][10] = {0.221746734017240, 0.928854139478045, 0.488897743920167, 0.0987122786555743,
                                    0.715037078400694, 0.904722238067363, 0.489901388512224, 0.521649842464284,
                                    0.800330575352402, 0.0604711791698936, 0.117417650855806, 0.730330862855453,
                                    0.624060088173690, 0.261871183870716, 0.903720560556316, 0.609866648422558,
                                    0.167927145682257, 0.0967300257808670, 0.453797708726920, 0.399257770613576,
                                    0.296675873218327, 0.488608973803579, 0.679135540865748, 0.335356839962797,
                                    0.890922504330789, 0.617666389588455, 0.978680649641159, 0.818148553859625,
                                    0.432391503783462, 0.526875830508296, 0.318778301925882, 0.578525061023439,
                                    0.395515215668593, 0.679727951377338, 0.334163052737496, 0.859442305646212,
                                    0.712694471678914, 0.817547092079286, 0.825313795402046, 0.416799467930787,
                                    0.424166759713807, 0.237283579771521, 0.367436648544477, 0.136553137355370,
                                    0.698745832334795, 0.805489424529686, 0.500471624154843, 0.722439592366842,
                                    0.0834698148589140, 0.656859890973707, 0.507858284661118, 0.458848828179931,
                                    0.987982003161633, 0.721227498581740, 0.197809826685929, 0.576721515614685,
                                    0.471088374541939, 0.149865442477967, 0.133171007607162, 0.627973359190104,
                                    0.0855157970900440, 0.963088539286913, 0.0377388662395521, 0.106761861607241,
                                    0.0305409463046367, 0.182922469414914, 0.0596188675796392, 0.659605252908307,
                                    0.173388613119006, 0.291984079961715, 0.262482234698333, 0.546805718738968,
                                    0.885168008202475, 0.653757348668560, 0.744074260367462, 0.239932010568717,
                                    0.681971904149063, 0.518594942510538, 0.390937802323736, 0.431651170248720,
                                    0.801014622769739, 0.521135830804002, 0.913286827639239, 0.494173936639270,
                                    0.500022435590201, 0.886511933076101, 0.0424311375007417, 0.972974554763863,
                                    0.831379742839070, 0.0154871256360190, 0.0292202775621463, 0.231594386708524,
                                    0.796183873585212, 0.779051723231275, 0.479922141146060, 0.0286741524641061,
                                    0.0714454646006424, 0.648991492712356, 0.803364391602440, 0.984063724379154};
    row = sizeof(_mat_A10) / sizeof(_mat_A10[0]);
    column = sizeof(_mat_A10[0]) / sizeof(_mat_A10[0][0]);
    Matrix *mat_A10 = Matrix_gen(row, column, _mat_A10);
    // M_print(mat_A10);
    //	Mat_21
    MATRIX_TYPE _mat_21[3][3] = {0, 2, 3, 2, 1, 2, 4, 1, 3};
    row = sizeof(_mat_21) / sizeof(_mat_21[0]);
    column = sizeof(_mat_21[0]) / sizeof(_mat_21[0][0]);
    Matrix *mat_21 = Matrix_gen(row, column, _mat_21);
    // M_print(mat_21);
    //	Mat_21b
    MATRIX_TYPE _mat_21b[3][3] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
    row = sizeof(_mat_21b) / sizeof(_mat_21b[0]);
    column = sizeof(_mat_21b[0]) / sizeof(_mat_21b[0][0]);
    Matrix *mat_21b = Matrix_gen(row, column, _mat_21b);
    // M_print(mat_21b);
    // Mat_b
    MATRIX_TYPE _mat_b[5][1] = {1, 2, 7, 2, 4};
    row = sizeof(_mat_b) / sizeof(_mat_b[0]);
    column = sizeof(_mat_b[0]) / sizeof(_mat_b[0][0]);
    Matrix *mat_b = Matrix_gen(row, column, _mat_b);
    // M_print(mat_b);
    // B_10
    MATRIX_TYPE _mat_b10[10][1] = {0.167168409914656, 0.106216344928664, 0.372409740055537, 0.198118402542975,
                                   0.489687638016024, 0.339493413390758, 0.951630464777727, 0.920332039836564,
                                   0.0526769976807926, 0.737858095516997};
    row = sizeof(_mat_b10) / sizeof(_mat_b10[0]);
    column = sizeof(_mat_b10[0]) / sizeof(_mat_b10[0][0]);
    Matrix *mat_b10 = Matrix_gen(row, column, _mat_b10);
    // M_print(mat_b10);
    //	Mat_eigen_test
    MATRIX_TYPE _mat_eigen_test[3][3] = {
            3.000,2.000,3.000,
            4.000,5.000,6.000,
            2.000,8.000,9.000,
    };
    row = sizeof(_mat_eigen_test) / sizeof(_mat_eigen_test[0]);
    column = sizeof(_mat_eigen_test[0]) / sizeof(_mat_eigen_test[0][0]);
    Matrix *mat_eigen_test = Matrix_gen(row, column, _mat_eigen_test);
    // M_print(mat_eigen_test);
    //	Mat_test
    MATRIX_TYPE _mat_inv_test[5][5] = {
            1900.687134, 1774.058105, 1474.243774, 1084.511719, 680.7639771,
            1506.677734, 1474.243774, 1348.486084, 1052.940186, 679.0471191,
            1087.057983, 1084.511719, 1052.940186, 931.4074707, 651.5393677,
            680.8083496, 680.7639771, 679.0471191, 651.5393677, 545.0874634,
            319.0212097, 319.1206055, 319.8133545, 321.7086487, 307.6086426
    };
    row = sizeof(_mat_inv_test) / sizeof(_mat_inv_test[0]);
    column = sizeof(_mat_inv_test[0]) / sizeof(_mat_inv_test[0][0]);
    Matrix *mat_inv_test = Matrix_gen(row, column, _mat_inv_test);
    // M_print(mat_inv_test);

    MATRIX_TYPE _mat_A_lp[4][7]  = {
            1,0,0,0,1,1,1,
            0,1,0,0,1,0,0,
            0,0,1,0,0,0,1,
            0,0,0,1,0,3,1
    };
    MATRIX_TYPE _mat_B_lp[4][1]  =
            {4,2,3,6};
    MATRIX_TYPE _mat_C_lp [1][7]  =
            {0,0,0,0,1,14,6,};
    row = sizeof(_mat_A_lp) / sizeof(_mat_A_lp[0]);
    column = sizeof(_mat_A_lp[0]) / sizeof(_mat_A_lp[0][0]);
    Matrix*  mat_A_lp = Matrix_gen(row,column,_mat_A_lp);
    /*M B*/
    row = sizeof(_mat_B_lp) / sizeof(_mat_B_lp[0]);
    column = sizeof(_mat_B_lp[0]) / sizeof(_mat_B_lp[0][0]);
    Matrix*  mat_B_lp = Matrix_gen(row,column,_mat_B_lp);
    /*M C*/
    row = sizeof(_mat_C_lp) / sizeof(_mat_C_lp[0]);
    column = sizeof(_mat_C_lp[0]) / sizeof(_mat_C_lp[0][0]);
    Matrix*  mat_C_lp = Matrix_gen(row,column,_mat_C_lp);

/* [Matrix Operation]*/
    // 乘法
    printf("->> Function: M_mul\n");
    Matrix *mat_3 = M_mul(mat_2, mat_1);
    M_print(mat_3);
    // 加减法
    printf("->> Function: M_add_sub\n");
    Matrix *mat_diff = M_add_sub(1, mat_21, 1, mat_21b);
    M_print(mat_diff);
    // 初等变换
    printf("->> Function: M_E_trans\n");
    Etrans_struct _Etrans_;
    _Etrans_.minuend_line = 2;
    _Etrans_.subtractor_line = 1;
    _Etrans_.scale = 2;
    _Etrans_.next_E_trans = NULL;
    _Etrans_.forward_E_trans = NULL;
    M_E_trans(mat_2, &_Etrans_, _ROW_);
    M_print(mat_2);
    // 单位矩阵
    printf("->> Function: M_I\n");
    M_print(M_I(5));
    // 初等变换to矩阵
    printf("->> Function: Etrans_2_Matrix\n");
    Matrix *mat_4 = Etrans_2_Matrix(&_Etrans_, 5, _ROW_);
    M_print(mat_4);
    // 上三角变换
    printf("->> Function: M_Uptri_\n");
    Uptri_struct *_Uptri_ = M_Uptri_(mat_21);
    M_print(_Uptri_->trans_matrix);
    M_print(_Uptri_->Uptri_matrix);
    // 下三角变换
    printf("->> Function: M_Lowtri_\n");
    Lowtri_struct *_Lowtri_ = M_Lowtri_(mat_21);
    M_print(_Lowtri_->Lowtri_matrix);
    M_print(_Lowtri_->trans_matrix);
    // 对角化
    printf("->> Function: M_Diatri_\n");
    Dia_struct *_Dia_ = M_Diatri_(mat_21);
    M_print(_Dia_->trans_leftmatrix);
    M_print(_Dia_->Diatri_matrix);
    M_print(_Dia_->trans_rightmatrix);
    // 矩阵求逆
    printf("->> Function: M_Inverse\n");
    Matrix *_mat_inv = M_Inverse(mat_21);
    M_print(_mat_inv);
    // 行列交换
    M_Swap(_mat_inv, 1, 2, _ROW_);
    M_print(_mat_inv);
    // 切割部分
    printf("->> Function: M_Cut\n");
    Matrix *_mat_cut = M_Cut(_mat_inv, _END_, _END_, 2, 3);
    M_print(_mat_cut);
    // 转置
    printf("->> Function: M_T\n");
    Matrix *_mat_T = M_T(_mat_inv);
    M_print(_mat_T);
    // 迹
    MATRIX_TYPE _tr_mat = M_tr(_mat_inv);
    printf("Trace(Matrix_%x) = %.4lf\n", _mat_inv, _tr_mat);
    // 行列式
    MATRIX_TYPE _det_mat = M_det(_mat_inv);
    printf("Det(Matrix_%x) = %.4lf\n", mat_21, _det_mat);
    // 填充
    printf("->> Function: M_full\n");
    Matrix *mat_full = M_full(mat_2, 1, 1, 1, 1, 0);
    M_print(mat_full);
    M_print(mat_2);
    // 范数
    printf("NORM_L1(mat_%x) = %lf\n",mat_b, M_norm(mat_b, 1));
    printf("NORM_L2(mat_%x) = %lf\n",mat_b, M_norm(mat_b, 2));
    // 秩
    printf("Rank(mat_%x) = %d\n", mat_A10, M_rank(mat_A10));
    printf("Rank(mat_%x) = %d\n", mat_full, M_rank(mat_full));
    // Hilbert 希尔伯特矩阵
    printf("->> Gen Hilbert-Matrix\n");
    M_print(Hilbert(5));
    // 条件数计算
    printf("->> Condition_Value = %lf\n", M_cond(Hilbert(5),1));
    // 矩阵householder变换
    printf("->> Function: M_householder.\n");
    Matrix * M_H = M_householder(Hilbert(5));
    M_print(M_H);
    // 矩阵特征值 + 特征向量
    printf("->> Function: M_eigen.\n");
    Matrix *target = mat_eigen_test;
    M_print(target);
    Matrix ** M_eigen_val_vec = M_eigen(target);
    enum{val=0, vec=1};
    M_print(M_eigen_val_vec[val]);
    M_print(M_eigen_val_vec[vec]);
    //  矩阵QR分解
    printf("->> Function: M_QR.\n");
    Matrix ** M_Q_R = M_QR(Hilbert(5));
    enum{q=0, r=1};
    M_print(M_Q_R[q]);
    M_print(M_Q_R[r]);
    // 矩阵 SVD 分解.
    printf("->> Function: M_SVD.\n");
    Matrix ** mat_list_SVD = M_SVD(mat_1);
    M_print(mat_1);
    enum{U=0, Dia=1, V=2};
    M_print(mat_list_SVD[U]);
    M_print(mat_list_SVD[Dia]);
    M_print(mat_list_SVD[V]);
    // 矩阵求伪逆
    printf("->> Function: pseudo-inverse (pinv).\n");
    M_print(mat_1);
    Matrix * mat_pinv = M_pinv(mat_1, _SVD_);
    M_print(mat_pinv);

/* [Application]*/
    /* [CASE 1: LP]
    |    min CX
    |s.t. AX=b,X>=0
    LP: linear programming, 求解线性规划.
    [Note.] 需要在main文件引入 "plugin_LP_Sover.h"
    # include "./solver_plugin/plugin_LP_Sover.h"
    */
    M_LP_struct* LP_result = NULL;
    // [LP-Case 1]
    enum LP_method{_Simplex=1,};
    printf("*** LP-SOLVER START ***\n");
    LP_result = LP_Solver(mat_A_lp, mat_B_lp,mat_C_lp, _Simplex); // 使用单纯形法解线性规划.
    printf("*** LP-SOLVER END ***\n");
    if (LP_result != NULL){
        printf("[COST]\n"); // mat_C_lp, C矩阵, 成本矩阵.
        M_print(LP_result->_matrix_c);
        printf("[BASE]\n"); // 最优解的基构成
        M_print(LP_result->_matrix_base);
        printf("[VALUES]\n"); // 最优值
        M_print(M_T(LP_result->_matrix_b));
        printf("[MAT_A]\n"); // 最后的变换系数矩阵.
        M_print(LP_result->_matrix_A);
        printf("[DELTA]\n"); // 各基的delta.
        M_print(LP_result->_matrix_delta);
        printf(">> OPT-VALUES: %lf\n", LP_result->values_opt); // 求解状态.
        printf(">> iter-num: %d\n", LP_result->iter_num);
        printf(">> [Note.] Please Check is Feasible or Not.\n"); // 求解迭代次数.
        LP_free(LP_result);
    }else{
        system("pause");
        printf("[NO FEASIBLE.] SEARCH ALL BRANCHES.\n");
    }

    // [ CASE 2: linear equations solver]linear equations,
    // 解线性方程. e.g. mat_A*x = mat_b
    printf("# Solver:mat_A*x = mat_b\n");
    Matrix *_mat_result = M_mul(M_Inverse(mat_A10), mat_b10);
    M_print(_mat_result);

    // [ CASE OTHERS]
    // 解线性方程. e.g. mat_A*x = mat_b
    // 测试 Hilbert 希尔伯特矩阵(病态矩阵)
    // Hilbert(n)*x = One(n,1)
    int order = 10;
    printf("# Solver:Hilbert(%d)*x = One(%d,1)\n", order, order);
    M_print(M_mul(M_Inverse(Hilbert(order)), M_Ones(order,1)));
    // 测试希尔伯特矩阵的秩
    printf("->>Rank(Hilbert(%d)) = %d\n",order, M_rank(Hilbert(order)));
    // 条件数计算
    printf("->>Condition Value = %lf\n", M_cond(mat_inv_test,1));
    M_print(M_Inverse(mat_inv_test));
    // 验证特征值, 特征向量
    int i;
    Matrix * lamda_I = M_I(target->row);
    for(i =0;i<lamda_I->row;i++){
        lamda_I->data[i*(lamda_I->column)+i]=M_eigen_val_vec[val]->data[i];
    }
    printf("->> (A*x - lamda*x) ?= 0(M_Zeros) \n");
    // [Note.] lamda*x 为对特征向量(列存储), 进行列变换, 所以表示为: M_mul(M_eigen_val_vec[vec], lamda_I).
    M_print(M_add_sub(1, M_mul(target,M_eigen_val_vec[vec]),
                      1, M_mul(M_eigen_val_vec[vec], lamda_I)));
    M_free(M_eigen_val_vec[val]);
    M_free(M_eigen_val_vec[vec]);
    M_free(lamda_I);
    free(M_eigen_val_vec);
    // 验证SVD分解, U*Dia*(V.T) ?= A.
    printf("->> U*Dia*(V.T) ?= A \n");
    Matrix * mat_SVD_dia = M_Zeros(mat_1->row, mat_1->column);
    for(i=0;i<mat_1->row;i++){
        mat_SVD_dia->data[i*(mat_SVD_dia->column)+i] = mat_list_SVD[Dia]->data[i];
    }
    M_print(mat_1); // A
    M_print(M_mul(
            M_mul(mat_list_SVD[U],
                  mat_SVD_dia),
            M_T(mat_list_SVD[V])
    )); // U*Dia*(V.T)
    M_free(mat_SVD_dia);
    M_free(mat_list_SVD[U]);
    M_free(mat_list_SVD[Dia]);
    M_free(mat_list_SVD[V]);
    free(mat_list_SVD);
    // 验证QR分解, Q*R?= A.
    printf("->> (Q*R) ?= A \n");
    M_print(Hilbert(5));
    M_print(M_mul(M_Q_R[q], M_Q_R[r]));
    M_free(M_Q_R[q]);
    M_free(M_Q_R[r]);
    free(M_Q_R);
    // 验证伪逆 pinv, A*A_pinv ?= I.
    printf("->> A*A_pinv ?= I \n");
    M_print(M_mul(mat_1,mat_pinv));

/* [Others]*/
    // Free Memory of Matrix, 释放矩阵内存.
    M_free(_mat_T);

/* [Help]*/
//    help 函数
    // help("help");
    // help("M_free");
    // help("Update");
    help("MatrixHub");

    return 0;
}
