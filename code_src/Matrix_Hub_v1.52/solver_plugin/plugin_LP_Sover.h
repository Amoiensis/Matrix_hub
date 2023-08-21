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
    DATE: 2020.02.12-2023.07.24
    VERSION: 1.5.2
    CASE: Matrix Operation (C)
    DETAILS: The plugin-code-file for Matrix_Hub, LP-Solver (Linear Programing).
    LICENSE: Apache-2.0
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "../matrix.h"

// _SILENT_MODE_ == True(1) -> solver keep silence.
#define _SILENT_MODE_ 1

#define _INVERSE_ 1
#define _LP_SOLVER_ 2

#define _NOT_DROP_ 0
#define _DROP_ 1

#define _INTEGER_GAP_ 1e-8
#define _ZERRO_GAP_ 1e-8

typedef struct _matrix_LP_struct {
    /*Store the result of matrix_inverse
	存储求逆运算的中间结果，提高算法效率*/
    Matrix *_matrix;
    struct _Elementary_Transformation *_Etrans_head;
    struct _Elementary_Transformation *_Etrans_tail;
} M_lp_struct;

typedef struct _matrix_LP_result_struct {
    /*Store the result of matrix_inverse
	存储求逆运算的中间结果，提高算法效率*/
    Matrix *_matrix_A;
    Matrix *_matrix_b;
    Matrix *_matrix_c;
    Matrix *_matrix_base;
    Matrix *_matrix_delta;
    int OPT_STATUS;
    MATRIX_TYPE values_opt;
    int iter_num;
} M_LP_struct;

M_LP_struct * LP_Solver(Matrix* mat_A,Matrix* mat_B,Matrix* mat_C, int LP_method);
M_LP_struct * LP_simplex_method(Matrix* mat_A,Matrix* mat_B,Matrix* mat_C);

Matrix * Etrans_4_LP_Solver (Matrix *_mat_result, Etrans_struct *_Etrans_, int line_setting, int usage, int is_drop);
M_lp_struct *M_Dia_E_Row_4_LP(Matrix *_mat_source);
int LP_free(M_LP_struct* _LP_);
int f_position_list_max(MATRIX_TYPE* list, int len);
int f_position_list_min(MATRIX_TYPE* list, int len);

int LP_free(M_LP_struct* _LP_){
    M_free(_LP_->_matrix_A);
    M_free(_LP_->_matrix_b);
    M_free(_LP_->_matrix_c);
    M_free(_LP_->_matrix_base);
    M_free(_LP_->_matrix_delta);
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

Matrix *Etrans_4_LP_Solver (Matrix *_mat_result, Etrans_struct *_Etrans_, int line_setting, int usage, int is_drop) {/*Inverse_Element_trans_to_Matrix
	基本变换转矩阵的转置*/
    Etrans_struct *temp_Etrans = _Etrans_, *temp_Etrans_pre = _Etrans_;
    int temp_num = 0;
    // 此处方案感谢 @1u2e, github.com/Amoiensis/Matrix_hub/issues/4
    while (temp_Etrans != NULL) {
        if(usage == _INVERSE_){
            temp_num = temp_Etrans->minuend_line;
            temp_Etrans->minuend_line = temp_Etrans->subtractor_line;
            temp_Etrans->subtractor_line = temp_num;
        }
        M_E_trans(_mat_result, temp_Etrans, line_setting);
        if (usage == _LP_SOLVER_){
            temp_Etrans = temp_Etrans->next_E_trans;
        }
        if(usage == _INVERSE_){
            temp_Etrans = temp_Etrans->forward_E_trans;
        }
        if (is_drop == _DROP_){
            free(temp_Etrans_pre);
        }
        temp_Etrans_pre = temp_Etrans;
    }

    return _mat_result;
}

M_lp_struct *M_Dia_E_Row_4_LP(Matrix *_mat_source){/*Upper_triangular_transformation_for_Inverse
	上三角化_求逆使用*/
    Matrix *_mat = Matrix_copy(_mat_source);
    int i, j, k, flag;
    Etrans_struct *_Etrans_temp_last = NULL;
    Etrans_struct *_Etrans_temp_head = NULL;
    Etrans_struct *_Etrans_temp_head_store = NULL;
    /*初等变换*/
    for (i = 0; i < _mat->row; i++) {
        for (j = i + 1; j < _mat->row; j++) {
            flag = 0;
            if (((_mat->data[(_mat->column) * j + i]) == 0) & ((_mat->data[(_mat->column) * i + i]) != 0)){
                continue;
            }
            Etrans_struct *_Etrans_temp = (Etrans_struct *) malloc(sizeof(Etrans_struct));
            _Etrans_temp->minuend_line = j + 1;
            _Etrans_temp->subtractor_line = i + 1;
            if ((_mat->data[(_mat->column) * i + i]) != 0) {
                _Etrans_temp->scale = (_mat->data[(_mat->column) * j + i]) / (_mat->data[(_mat->column) * i + i]);
            } else {
                _Etrans_temp->scale = 0;
                for (k = i + 1; k < _mat->row; k++) {
                    flag = 1;//无可替代行
                    if ((_mat->data[(_mat->column) * k + i]) != 0) {
                        _Etrans_temp->minuend_line = -(i + 1);
                        _Etrans_temp->subtractor_line = -(k + 1);
                        flag = 2;//表示能够替换行
                        break;
                    }
                }

                if (flag == 1) {
                    break;
                }
            }
            _Etrans_temp->forward_E_trans = NULL;
            _Etrans_temp->next_E_trans = NULL;
            //if (j==1){
            if (_Etrans_temp_head == NULL) {
                _Etrans_temp_head = _Etrans_temp;
                _Etrans_temp->forward_E_trans = NULL;
                // [新增] 存储初逢变换
                _Etrans_temp_head_store =_Etrans_temp_head;
            } else {
                _Etrans_temp->forward_E_trans = _Etrans_temp_last;
            }
            if ((i + 1) == _mat->column) {
                _Etrans_temp->next_E_trans = NULL;
            }
            if (_Etrans_temp_last != NULL) {
                _Etrans_temp_last->next_E_trans = _Etrans_temp;
            }
            M_E_trans(_mat, _Etrans_temp, _ROW_);
            _Etrans_temp_last = _Etrans_temp;
            if (flag == 2) {
                i = i - 1;
                break;
            }
        }
    }

    for (i =(_mat->row-1); i >= 0; i--) {
        for (j = i-1; j>=0; j--) {
            if (((_mat->data[(_mat->column) * j + i]) == 0) & ((_mat->data[(_mat->column) * i + i]) != 0)){
                continue;
            }
            flag = 0;
            Etrans_struct *_Etrans_temp = (Etrans_struct *) malloc(sizeof(Etrans_struct));
            _Etrans_temp->minuend_line = j + 1;
            _Etrans_temp->subtractor_line = i + 1;
            if ((_mat->data[(_mat->column) * i + i]) != 0) {
                _Etrans_temp->scale = (_mat->data[(_mat->column) * j + i]) / (_mat->data[(_mat->column) * i + i]);
            } else {
                _Etrans_temp->scale = 0;
                for (k = i + 1; k < _mat->row; k++) {
                    flag = 1;//无可替代行
                    if ((_mat->data[(_mat->column) * k + i]) != 0) {
                        _Etrans_temp->minuend_line = -(i + 1);
                        _Etrans_temp->subtractor_line = -(k + 1);
                        flag = 2;//表示能够替换行
                        break;
                    }
                }

                if (flag == 1) {
                    break;
                }
            }
            _Etrans_temp->forward_E_trans = NULL;
            _Etrans_temp->next_E_trans = NULL;
            if (_Etrans_temp_head == NULL) {
                _Etrans_temp_head = _Etrans_temp;
                _Etrans_temp->forward_E_trans = NULL;
                // [新增] 存储初逢变换
                _Etrans_temp_head_store =_Etrans_temp_head;
            } else {
                _Etrans_temp->forward_E_trans = _Etrans_temp_last;
            }
            if ((i-1) == 0) {
                _Etrans_temp->next_E_trans = NULL;
            }
            if (_Etrans_temp_last != NULL) {
                _Etrans_temp_last->next_E_trans = _Etrans_temp;
            }
            M_E_trans(_mat, _Etrans_temp, _ROW_);
            _Etrans_temp_last = _Etrans_temp;
            if (flag == 2) {
                i = i - 1;
                break;
            }

        }
    }

    M_lp_struct *_Dia_ = (M_lp_struct *) malloc(sizeof(M_lp_struct));
    _Dia_->_matrix = _mat;
    _Dia_->_Etrans_head = _Etrans_temp_last; // _INVERSE_
    _Dia_->_Etrans_tail = _Etrans_temp_head_store; // _LP_SOLVER_
    return _Dia_;
}

M_LP_struct * LP_Solver(Matrix* mat_A,Matrix* mat_B,Matrix* mat_C, int LP_method){
    enum LP_method{_Simplex=1,};
    M_LP_struct * result = NULL;
    if(LP_method == _Simplex){
        result = LP_simplex_method(mat_A,mat_B,mat_C);
    }
    return result;
}


M_LP_struct * LP_simplex_method(Matrix* mat_A,Matrix* mat_B,Matrix* mat_C){/*
 * Simplex
 */
    M_LP_struct * M_LP_Result = NULL;
// 初始基变量的选择, 可以使用 rank 矩阵的秩来进行, 可以避免重复计算;
    int base_num = mat_A->row, variable_num = mat_A->column,row, col;
    int max_delta_position = -1, min_theta_position = -1, staues_opt = 0;
// 直接对[A, b]矩阵求rank.
// 后续可以直接对 约束进行处理, 删除部分重复的约束.
    Matrix *mat_base = NULL, *vector_base_index=NULL, *mat_C_base , *Delta_opt = NULL, *thelta_opt = NULL
    , * mat_A_copy = NULL, * mat_b_copy = NULL;
    vector_base_index = M_Zeros(1, base_num);
    mat_A_copy = Matrix_copy(mat_A);
    mat_b_copy = Matrix_copy(mat_B);
    // 获取基解位置
    M_lp_struct *_Uptri_ = M_Dia_E_Row_4_LP(mat_A_copy);//M_Uptri_4_LP(mat_A_copy);
    // 人工变量在开头.
    for(row=0;row<base_num;row++){
        for(col=0;col<variable_num;col++){
            // mat_B->column = 1, variable_num = mat_A->column.
            if(_Uptri_->_matrix->data[row*(variable_num) + col] != 0){
                vector_base_index->data[row] = col;
                break;
            }
            if (col == (variable_num-1)){
                printf("[ERROR] No base, please check Mat_A.");
                system("pause");
            }
        }
    }

    int Counter = 0;
    while(Counter < _MAX_LOOP_NUM_) {
        // 调整基变量
        if ((max_delta_position >= 0) & (min_theta_position >= 0)){
            for(row=0;row<base_num;row++){
                if(vector_base_index->data[row] == min_theta_position){
                    vector_base_index->data[row] = max_delta_position;
                    break;
                }
                if (row == (base_num-1)){
                    // 避免循环找基解;
                    vector_base_index->data[0] = max_delta_position;
                }
            }
        }
        if(!_SILENT_MODE_){
            printf("[BASE]\n");
            M_print(vector_base_index);
        }
        // 将上三角矩阵取出
        if (Counter == 0){
            mat_base = M_Sample(_Uptri_->_matrix, vector_base_index, _COLUMN_);
        }else{
            mat_base = M_Sample(mat_A_copy, vector_base_index, _COLUMN_);
            _Uptri_ = M_Dia_E_Row_4_LP(mat_base);
        }
        mat_C_base = M_Sample(mat_C, vector_base_index, _COLUMN_);
        mat_A_copy = Etrans_4_LP_Solver(mat_A_copy, _Uptri_->_Etrans_tail, _ROW_, _LP_SOLVER_, _NOT_DROP_);
        mat_b_copy = Etrans_4_LP_Solver(mat_b_copy, _Uptri_->_Etrans_tail, _ROW_, _LP_SOLVER_, _DROP_);
        // 将倍数进行处理
        MATRIX_TYPE factor = 1;
        for (row = 0; row < base_num; row++) {
            factor = mat_A_copy->data[row * (variable_num) + (int)vector_base_index->data[row]];
            if (factor != 1){
                mat_b_copy->data[row] /= factor;
                for (col = 0; col < (variable_num); col++) {
                    mat_A_copy->data[row * (variable_num) + col] /= factor;
                }
            }
        }
        M_free(_Uptri_->_matrix);
        free(_Uptri_);

        if(!_SILENT_MODE_){
            printf("[TABLE] A/b\n");
            M_print(mat_A_copy);
            M_print(mat_b_copy); // 具体变量取值;
        }
        Matrix *mat_C_copy = M_mul(mat_C_base, mat_A_copy);
        M_free(mat_C_base);
//        M_free(Delta_opt);
        Delta_opt = M_add_sub(1, mat_C, 1, mat_C_copy);

        if(!_SILENT_MODE_){
            printf("[DELTA-OPT]\n");
            M_print(Delta_opt);
        }
        M_free(mat_C_copy);
        M_free(mat_base);
        // 进基解, 出基解M_print(Delta_opt);
        max_delta_position = f_position_list_max(Delta_opt->data, Delta_opt->column);
        if ((Delta_opt->data[max_delta_position]) > 0) {
            (!_SILENT_MODE_)?printf("Next IN Base: %d\n", max_delta_position):0;
            // 计算出基变量
            thelta_opt = M_Zeros(mat_A->row, 1);
            for (row = 0; row < base_num; row++){
                MATRIX_TYPE temp_a, temp_b;
                temp_a = mat_A_copy->data[row * (variable_num) + max_delta_position];
                temp_b = mat_b_copy->data[row];
                if (temp_a > 0) {
                    thelta_opt->data[row] = (temp_b / temp_a);
                }else{
                    thelta_opt->data[row] = _FULLY_BIG_;
                }
            }
            if(!_SILENT_MODE_){
                printf("[thelta_opt]\n");
                M_print(thelta_opt);
            }
            min_theta_position = f_position_list_min(thelta_opt->data, thelta_opt->row);
            if (thelta_opt->data[min_theta_position] > _ZERRO_GAP_ ) {
                (!_SILENT_MODE_)?printf("Next OUT Base: %.0lf\n", vector_base_index->data[min_theta_position]):0;
                M_free(thelta_opt);
            } else {
                (!_SILENT_MODE_)?printf("INFINITE SOLUTION, POSITION: %d.\n", min_theta_position):0;
                M_free(thelta_opt);
                staues_opt = 1;
                goto END_OPT;
            }
        } else {
            (!_SILENT_MODE_)?printf("END SOLVER - OPT %d\n", max_delta_position):0;
            staues_opt = 0;
            goto END_OPT;
        }
        Counter += 1;
    }
    END_OPT:
    (!_SILENT_MODE_)?printf("*** SOLVER END ***\n"):0;
    MATRIX_TYPE values_opt = 0, temp_values = 0;
    for(row=0;row<base_num;row++){
        temp_values = mat_C->data[(int)vector_base_index->data[row]] * mat_b_copy->data[row];
        values_opt += temp_values;
    }
    M_LP_Result = (M_LP_struct*) malloc(sizeof(M_LP_struct));
    M_LP_Result->_matrix_A = mat_A_copy;
    M_LP_Result->_matrix_b = mat_b_copy;
    M_LP_Result->_matrix_c = mat_C;
    M_LP_Result->_matrix_base = vector_base_index;
    M_LP_Result->_matrix_delta = Delta_opt;
    M_LP_Result->values_opt = values_opt;
    M_LP_Result->OPT_STATUS = staues_opt;
    M_LP_Result->iter_num = Counter;
    return M_LP_Result;
}
