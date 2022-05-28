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
    DATE: 2020.02.12-2022.05.28
    VERSION: 1.5.1
    CASE: Matrix Operation (C)
    DETAILS: The code_file for Matrix_Hub.
* [LICENSE]
    Copyright (c) 2020-2022 Xiping.Yu
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
        http://www.apache.org/licenses/LICENSE-2.0
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include "state.h"


typedef struct _Matrix {
    /*Store Matrix
	存储矩阵*/
    int row;
    int column;
    MATRIX_TYPE *data;
} Matrix;

typedef struct _Elementary_Transformation {
    /*Store the Operation of Elementary_Transformation
	存储初等变化的运算过程*/
    int minuend_line;
    int subtractor_line;
    TRANS_TYPE scale;
    struct _Elementary_Transformation *forward_E_trans;
    struct _Elementary_Transformation *next_E_trans;
} Etrans_struct;

typedef struct _Upper_triangular_transformation {
    /*Store the result of Upper_triangular_transformation
	存储上三角化的运算结果*/
    Matrix *trans_matrix;
    Matrix *Uptri_matrix;
} Uptri_struct;

typedef struct _Lower_triangular_transformation {
    /*Store the result of Upper_triangular_transformation
	存储下三角化的运算结果*/
    Matrix *trans_matrix;
    Matrix *Lowtri_matrix;
} Lowtri_struct;

typedef struct _Diagonalization_transformation {
    /*Store the result of Upper_triangular_transformation
	存储对角化化的运算结果*/
    Matrix *trans_leftmatrix;
    Matrix *Diatri_matrix;
    Matrix *trans_rightmatrix;
} Dia_struct;

typedef struct _matrix_inverse_struct {
    /*Store the result of matrix_inverse
	存储求逆运算的中间结果，提高算法效率*/
    Matrix *_matrix;
    struct _Elementary_Transformation *_Etrans_head;
} M_inv_struct;

typedef struct _matrix_eigen_struct_single {
    /*Store the result of matrix_eigen
	存储求最大特征值运算的结果*/
    Matrix *eigen_matrix;
    double eigen_value;
} M_eigen_struct;


Matrix *Matrix_gen(int row, int column, MATRIX_TYPE *data);

Matrix *Matrix_copy(Matrix *_mat_sourse);

Matrix *M_mul(Matrix *_mat_left, Matrix *_mat_right);

Matrix *M_add_sub(MATRIX_TYPE scale_mat_subed, Matrix *_mat_subed, MATRIX_TYPE scale_mat_minus, Matrix *_mat_minus);

int M_print(Matrix *_mat);

Matrix *M_I(int order);

int M_E_trans(Matrix *_mat, Etrans_struct *_Etrans_, int line_setting);

Matrix *Etrans_2_Matrix(Etrans_struct *_Etrans_, int order, int line_setting);

Matrix *Etrans_4_Inverse(Matrix *_mat_result, Etrans_struct *_Etrans_, int line_setting);

Uptri_struct *M_Uptri_(Matrix *_mat_source);

M_inv_struct *M_Uptri_4inv(Matrix *_mat_source);

Lowtri_struct *M_Lowtri_(Matrix *_mat_source);

M_inv_struct *M_Lowtri_4inv(Matrix *_mat_source);

Matrix *M_Dia_Inv(Matrix *_mat_source);

Dia_struct *M_Diatri_(Matrix *_mat_source);

Matrix *M_Inverse(Matrix *_mat);

int M_Swap(Matrix *_mat, int _line_1, int _line_2, int line_setting);

Matrix *M_Cut(Matrix *_mat, int row_head, int row_tail, int column_head, int column_tail);

Matrix *M_T(Matrix *_mat_source);

int M_free(Matrix *_mat);

MATRIX_TYPE M_tr(Matrix *_mat);

MATRIX_TYPE M_det(Matrix *_mat);

Matrix *M_full(Matrix *_mat, int row_up, int row_down, int column_left, int column_right, MATRIX_TYPE full_data);

MATRIX_TYPE M_norm(Matrix *_mat, int Setting);

Matrix *M_abs(Matrix *_mat_origin);

Matrix *M_numul(Matrix *_mat, MATRIX_TYPE _num);

Matrix *M_matFull(Matrix *_mat, int row_up, int column_left, Matrix *_mat_full);

Matrix *M_Zeros(int row, int column);

Matrix *M_Ones(int row, int column);

Matrix *M_find(Matrix *_mat, MATRIX_TYPE value);

Matrix *M_sum(Matrix *_mat);

int Min_position(MATRIX_TYPE *data, int size);

Matrix *M_min(Matrix *_mat);

Matrix *M_max(Matrix *_mat);

Matrix *M_minax_val(Matrix *_mat, Matrix *_mat_position);

Matrix *M_logic_equal(Matrix *_mat, MATRIX_TYPE value);

Matrix *M_logic(Matrix *_mat_left, Matrix *_mat_right, int Operation);

Matrix *M_pmuldiv(Matrix *_mat_left, Matrix *_mat_right, int operation);

Matrix *M_setval(Matrix *_mat_ini, Matrix *_mat_val, Matrix *_mat_order, int model);

Matrix *M_numul_m(Matrix *_mat, Matrix *_mat_multi);

M_eigen_struct *M_eigen_max(Matrix *_mat);

int help(char *file_name);

int M_rank(Matrix *_mat);

int Etrans_free(Etrans_struct *_Etrans_);

Matrix *Hilbert(int order);

double M_cond(Matrix *_mat, int Setting);

Matrix ** M_eigen (Matrix *_mat);

Matrix * householder(Matrix * _x);

Matrix * M_householder(Matrix * _mat);

Matrix ** M_QR(Matrix * _mat);

Matrix * M_eigen_val(Matrix * _mat);

void progress_bar(int count, int total);

double M_cond(Matrix *_mat, int Setting){ /* condition number
    矩阵条件数 */
    int i,j;
    double matrix_cond = 0;
    if (_mat->column == _mat->row){
        if((Setting == 1) || (Setting == 2) || (Setting == INF) || (Setting == FRO)){
            Matrix * _mat_inv = M_Inverse(_mat);
            matrix_cond = M_norm(_mat,Setting)*M_norm(_mat_inv,Setting);
            M_free(_mat_inv);
        }else{
            printf(M_cond_025);
            system("pause");
        }
    }else{
        printf(M_cond_024);
        system("pause");
    }
    return matrix_cond;
}

Matrix *Hilbert(int order){/* Generate Hilbert Matrix
    生成希尔伯特矩阵*/
    Matrix *Hilbert_mat = (Matrix *) malloc(sizeof(Matrix));
    Hilbert_mat->column = order;
    Hilbert_mat->row = order;
    int i, j;
    MATRIX_TYPE *data = (MATRIX_TYPE *) malloc((order*order) * sizeof(MATRIX_TYPE));
    for (i = 0; i < order; i++) {
        for (j = 0; j < order; j++){
            if (i>j){ // [注意] 对称赋值: 减少运算; 保持矩阵对角对称;
                data[i*order+j] = data[j*order+i];
            }else{
                data[i*order+j] = 1.0/(i+j+1);
            }
        }
    }
    Hilbert_mat->data = data;
    return Hilbert_mat;
}

int Etrans_free(Etrans_struct *_Etrans_){/* (func: M_rank) free memory for Elementary_Transformation
    (函数: M_rank) 释放初等变换内存空间*/
    Etrans_struct *temp_Etrans = _Etrans_;
    do {
        temp_Etrans = temp_Etrans->forward_E_trans;
        if (temp_Etrans != NULL) {
            free(temp_Etrans->next_E_trans);
        }
    } while (temp_Etrans != NULL);
    return 0;
}

int M_rank(Matrix *_mat){/* Matrix's Rank
    矩阵的秩 */
    (_DETAILED_>=2)?printf(">>rank(Matrix_%x)=\n... => transform...\n", _mat):0;
    M_inv_struct *_Uptri_ = M_Uptri_4inv(_mat);
    MATRIX_TYPE *_mat_uptri = _Uptri_ ->_matrix->data;
    M_print(_Uptri_->_matrix);
    int rank = 0;
    int i, j, row = _mat->row, column = _mat->column;
    for (i=0;i<row;i++){
        for (j=0;j<column;j++){
            if (_mat_uptri[i*column + j] != 0){
                rank ++;
                break;
            }
        }
    }
    free(_Uptri_->_matrix);
    Etrans_free(_Uptri_->_Etrans_head);
    free(_Uptri_);
    (_DETAILED_>=2)?printf("... => transform end.\n"):0;
    return rank;
}

int help(char *file_name) {/*Help File
	可以调用如，help("help")，查看help函数的使用方法和内容*/
    printf(">>HELP(");
    printf(file_name);
    printf(")\n");
    char temp_route[_MAX_HELP_LENGTH_] = "./help/";
    char temp_txt[5] = ".txt";
    strcat(temp_route, file_name);
    strcat(temp_route, temp_txt);
    FILE *fp;
    char ch;
    fp = fopen(temp_route, "r");
    if (fp == NULL) {
        printf(temp_route);
        printf(" can not open!\n");
    } else {
        fscanf(fp, "%c", &ch);
        while (!feof(fp)) {
            putchar(ch);
            fscanf(fp, "%c", &ch);
        }
        fclose(fp);
    }
    printf("\n");
}

Matrix *Matrix_gen(int row, int column, MATRIX_TYPE *data) {/*Generate Matrix Struct
	导入_生成矩阵*/
    Matrix *_mat = (Matrix *) malloc(sizeof(Matrix));
    _mat->row = row;
    _mat->column = column;
    int size = _mat->row * _mat->column;
    _mat->data = (MATRIX_TYPE *) malloc((size) * sizeof(MATRIX_TYPE));
    int i;
    for (i = 0; i < size; i++) {
        _mat->data[i] = data[i];
    }
    return _mat;
}

Matrix *Matrix_copy(Matrix *_mat_sourse) {/*Copy Mtrix(gen new one)
	复制矩阵（生成新矩阵）*/
    Matrix *_mat_copy = Matrix_gen(_mat_sourse->row, _mat_sourse->column, _mat_sourse->data);
    return _mat_copy;
}

Matrix *M_add_sub(MATRIX_TYPE scale_mat_subed, Matrix *_mat_subed, MATRIX_TYPE scale_mat_minus, Matrix *_mat_minus) {/*Add & Sub
	矩阵加减法*/
    Matrix *_mat_result = NULL;
    if ((_mat_subed->column == _mat_minus->column) && (_mat_subed->row == _mat_minus->row)) {
        _mat_result = Matrix_copy(_mat_subed);
        int size = (_mat_subed->row) * (_mat_subed->column), i;
        for (i = 0; i < size; i++) {
            _mat_result->data[i] = (_mat_result->data[i]) * scale_mat_subed - (_mat_minus->data[i]) * scale_mat_minus;
        }
    } else {
        printf(M_add_sub_003);
    }
    return _mat_result;
}

Matrix *M_mul(Matrix *_mat_left, Matrix *_mat_right) {/*Matrix Multiply
	矩阵乘法
	_mat_result = _mat_left*_mat_right */
    (_DETAILED_>=3)?printf(">>Matrix_%x * Matrix_%x =\n", _mat_left, _mat_right):0;
    /*Determine_Matrix_Dimensions*/
    Matrix *_mat_result = NULL;
    if (_mat_left->column != _mat_right->row) {
        printf(M_mul_001);
    } else {
        _mat_result = (Matrix *) malloc(sizeof(Matrix));
        int row = _mat_left->row;
        int mid = _mat_left->column;
        int column = _mat_right->column;
        int i, j, k;
        MATRIX_TYPE *_data = (MATRIX_TYPE *) malloc((row * column) * sizeof(MATRIX_TYPE));
        MATRIX_TYPE temp = 0;
        /*Ergodic*/
        for (i = 0; i < row; i++) {
            for (j = 0; j < column; j++) {
                /*Caculate Element*/
                temp = 0;
                for (k = 0; k < mid; k++) {
                    temp += (_mat_left->data[i * mid + k]) * (_mat_right->data[k * column + j]);
                }
                _data[i * column + j] = temp;
            }
        }
        _mat_result->row = row;
        _mat_result->column = column;
        _mat_result->data = _data;
    }
    (_DETAILED_>=3)?printf("\tMatrix_%x\n", _mat_result):0;
    return _mat_result;
}

int M_print(Matrix *_mat) {/*Print Matrix
	打印矩阵*/
    printf(">>Matrix_%x:\n", _mat);
    int i, j;
    for (i = 0; i < _mat->row; i++) {
        for (j = 0; j < _mat->column; j++) {
            printf(PRECISION, _mat->data[i * (_mat->column) + j]);
        }
        printf("\n");
    }
    return 0;
}

Matrix *M_I(int order) {/*Generate I _matrix
	生成单位矩阵*/
    Matrix *I_mat = (Matrix *) malloc(sizeof(Matrix));
    I_mat->column = order;
    I_mat->row = order;
    int size = order * order, i;
    MATRIX_TYPE *data = (MATRIX_TYPE *) malloc((size) * sizeof(MATRIX_TYPE));
    for (i = 0; i < size; i++) {
        data[i] = 0;
    }
    for (i = 0; i < order; i++) {
        data[i * (order + 1)] = 1;
    }
    I_mat->data = data;
    return I_mat;
}

int M_E_trans(Matrix *_mat, Etrans_struct *_Etrans_, int line_setting) {/*Element teransfor Matrix
	对矩阵初等变换*/
/*lin3_sstting 设置是行初等变换还是列初等变换*/
    int line_num, i;
    if (line_setting == _ROW_) {
        /*行初等变换*/
        line_num = _mat->column;
        if (_Etrans_->scale) {
            for (i = 0; i < line_num; i++) {
                _mat->data[(_Etrans_->minuend_line - 1) * (_mat->column) + i] -=
                        (_Etrans_->scale) * (_mat->data[(_Etrans_->subtractor_line - 1) * (_mat->column) + i]);

            }
        } else {
            if ((_Etrans_->minuend_line < 0) && (_Etrans_->subtractor_line < 0)) {/*交换*/
                M_Swap(_mat, -(_Etrans_->minuend_line), -(_Etrans_->subtractor_line), line_setting);
            }
        }
    } else {
        /*列初等变换*/
        line_num = _mat->row;
        if (_Etrans_->scale) {
            for (i = 0; i < line_num; i++) {
                _mat->data[(_Etrans_->minuend_line - 1) + (_mat->column) * i] -=
                        (_Etrans_->scale) * (_mat->data[(_Etrans_->subtractor_line - 1) + (_mat->column) * i]);
            }
        } else {
            if ((_Etrans_->minuend_line < 0) && (_Etrans_->subtractor_line < 0)) {/*交换*/
                M_Swap(_mat, -(_Etrans_->minuend_line), -(_Etrans_->subtractor_line), line_setting);
            }
        }
    }
    return 0;
}

Matrix *Etrans_4_Inverse(Matrix *_mat_result, Etrans_struct *_Etrans_, int line_setting) {/*Inverse_Element_trans_to_Matrix
	基本变换转矩阵的转置*/
    Etrans_struct *temp_Etrans = _Etrans_, *temp_Etrans_pre = _Etrans_;
    int temp_num = 0;
    // 此处方案感谢 @1u2e, github.com/Amoiensis/Matrix_hub/issues/4
    while (temp_Etrans != NULL) {
        temp_num = temp_Etrans->minuend_line;
        temp_Etrans->minuend_line = temp_Etrans->subtractor_line;
        temp_Etrans->subtractor_line = temp_num;
        M_E_trans(_mat_result, temp_Etrans, line_setting);
        // 此处修改方案感谢 @1u2e, github.com/Amoiensis/Matrix_hub/issues/4
        temp_Etrans = temp_Etrans->forward_E_trans;
        free(temp_Etrans_pre);
        temp_Etrans_pre = temp_Etrans;
    }
    return _mat_result;
}

Matrix *Etrans_2_Matrix(Etrans_struct *_Etrans_, int order, int line_setting) {/*Element_trans_to_Matrix
	基本变换转矩阵*/
    Etrans_struct *temp_Etrans = _Etrans_;
    Matrix *_mat_result = M_I(order);
    if (_Etrans_ != NULL) {
        while ((temp_Etrans->next_E_trans) != NULL) {
            temp_Etrans = temp_Etrans->next_E_trans;
        }
        //M_print(_mat_result);
        do {
            temp_Etrans->scale = (-1) * temp_Etrans->scale;
            M_E_trans(_mat_result, temp_Etrans, line_setting);
            temp_Etrans->scale = (-1) * temp_Etrans->scale;
            temp_Etrans = temp_Etrans->forward_E_trans;
            if (temp_Etrans != NULL) {
                free(temp_Etrans->next_E_trans);
            }
        } while (temp_Etrans != NULL);
    }
    return _mat_result;
}

Uptri_struct *M_Uptri_(Matrix *_mat_source) {/*Upper_triangular_transformation
	上三角化*/
    Matrix *_mat = Matrix_copy(_mat_source);
    int i, j, k, flag;
    Etrans_struct *_Etrans_temp_last = NULL;
    Etrans_struct *_Etrans_temp_head = NULL;

    /*初等变换*/
    for (i = 0; i < _mat->column; i++) {
        for (j = i + 1; j < _mat->row; j++) {
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
            } else {
                _Etrans_temp->forward_E_trans = _Etrans_temp_last;

            }
            if ((i + 1) == _mat->column) {
                _Etrans_temp->next_E_trans = NULL;
            } else {
                if (_Etrans_temp_last != NULL) {
                    _Etrans_temp_last->next_E_trans = _Etrans_temp;
                }
            }
            M_E_trans(_mat, _Etrans_temp, _ROW_);
            //M_print(_mat); //显示具体矩阵
            _Etrans_temp_last = _Etrans_temp;

            if (flag == 2) {
                i = i - 1;
                break;
            }
        }
    }
    Matrix *trans_mat = Etrans_2_Matrix(_Etrans_temp_head, _mat->row, _ROW_);
    Uptri_struct *_Uptri = (Uptri_struct *) malloc(sizeof(Uptri_struct));
    _Uptri->trans_matrix = trans_mat;
    _Uptri->Uptri_matrix = _mat;
    (_DETAILED_>=1)?printf(">>Uptri(Matrix_%x)=\n", _mat_source):0;
    (_DETAILED_>=1)?printf("\tMatrix_%x * Matrix_%x\n", trans_mat, _mat):0;
    return _Uptri;
}

M_inv_struct *M_Uptri_4inv(Matrix *_mat_source) {/*Upper_triangular_transformation_for_Inverse
	上三角化_求逆使用*/
    Matrix *_mat = Matrix_copy(_mat_source);
    int i, j, k, flag;
    Etrans_struct *_Etrans_temp_last = NULL;
    Etrans_struct *_Etrans_temp_head = NULL;

    /*初等变换*/
    for (i = 0; i < _mat->column; i++) {
        for (j = i + 1; j < _mat->row; j++) {
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
            //if (j==1){
            if (_Etrans_temp_head == NULL) {
                _Etrans_temp_head = _Etrans_temp;
                _Etrans_temp->forward_E_trans = NULL;
            } else {
                _Etrans_temp->forward_E_trans = _Etrans_temp_last;

            }
            if ((i + 1) == _mat->column) {
                _Etrans_temp->next_E_trans = NULL;
            } else {
                if (_Etrans_temp_last != NULL) {
                    _Etrans_temp_last->next_E_trans = _Etrans_temp;
                }
            }
            M_E_trans(_mat, _Etrans_temp, _ROW_);
            //M_print(_mat); //显示具体矩阵
            _Etrans_temp_last = _Etrans_temp;

            if (flag == 2) {
                i = i - 1;
                break;
            }
        }
    }
    M_inv_struct *_Uptri = (M_inv_struct *) malloc(sizeof(M_inv_struct));
    _Uptri->_matrix = _mat;
    _Uptri->_Etrans_head = _Etrans_temp_last;
    return _Uptri;
}

Lowtri_struct *M_Lowtri_(Matrix *_mat_source) {/*Lower_triangular_transformation
	下三角化*/
    Matrix *_mat = Matrix_copy(_mat_source);
    int i, j, k, flag;
    Etrans_struct *_Etrans_temp_last = NULL;
    Etrans_struct *_Etrans_temp_head = NULL;
    for (i = 0; i < _mat->row; i++) {
        for (j = i + 1; j < _mat->column; j++) {
            flag = 0;
            Etrans_struct *_Etrans_temp = (Etrans_struct *) malloc(sizeof(Etrans_struct));
            _Etrans_temp->minuend_line = j + 1;
            _Etrans_temp->subtractor_line = i + 1;


            if ((_mat->data[(_mat->column) * i + i]) != 0) {
                _Etrans_temp->scale = (_mat->data[(_mat->column) * i + j]) / (_mat->data[(_mat->column) * i + i]);;
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
            } else {
                _Etrans_temp->forward_E_trans = _Etrans_temp_last;
            }
            if ((i + 1) == _mat->column) {
                _Etrans_temp->next_E_trans = NULL;
            } else {
                if (_Etrans_temp_last != NULL) {
                    _Etrans_temp_last->next_E_trans = _Etrans_temp;
                }
            }
            M_E_trans(_mat, _Etrans_temp, _COLUMN_);
            M_print(_mat); //显示具体矩阵
            _Etrans_temp_last = _Etrans_temp;
            if (flag == 2) {
                i = i - 1;
                break;
            }
        }
    }
    Matrix *trans_mat = Etrans_2_Matrix(_Etrans_temp_head, _mat->row, _COLUMN_);
    Lowtri_struct *_Lowtri = (Lowtri_struct *) malloc(sizeof(Lowtri_struct));
    _Lowtri->trans_matrix = trans_mat;
    _Lowtri->Lowtri_matrix = _mat;
    (_DETAILED_>=1)?printf(">>Lowtri(Matrix_%x)=\n", _mat_source):0;
    (_DETAILED_>=1)?printf("\tMatrix_%x * Matrix_%x\n", _mat, trans_mat):0;
    return _Lowtri;
}

M_inv_struct *M_Lowtri_4inv(Matrix *_mat_source) {/*_Lower_triangular_transformation_for_Inverse
	下三角化_求逆使用*/
    Matrix *_mat = Matrix_copy(_mat_source);
    int i, j, k, flag;
    Etrans_struct *_Etrans_temp_last = NULL;
    Etrans_struct *_Etrans_temp_head = NULL;
    for (i = 0; i < _mat->row; i++) {
        for (j = i + 1; j < _mat->column; j++) {
            flag = 0;
            Etrans_struct *_Etrans_temp = (Etrans_struct *) malloc(sizeof(Etrans_struct));
            _Etrans_temp->minuend_line = j + 1;
            _Etrans_temp->subtractor_line = i + 1;


            if ((_mat->data[(_mat->column) * i + i]) != 0) {
                _Etrans_temp->scale = (_mat->data[(_mat->column) * i + j]) / (_mat->data[(_mat->column) * i + i]);;
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
            } else {
                _Etrans_temp->forward_E_trans = _Etrans_temp_last;
            }
            if ((i + 1) == _mat->column) {
                _Etrans_temp->next_E_trans = NULL;
            } else {
                if (_Etrans_temp_last != NULL) {
                    _Etrans_temp_last->next_E_trans = _Etrans_temp;
                }
            }
            M_E_trans(_mat, _Etrans_temp, _COLUMN_);
            //M_print(_mat); //显示具体矩阵
            _Etrans_temp_last = _Etrans_temp;
            if (flag == 2) {
                i = i - 1;
                break;
            }
        }
    }
    M_inv_struct *_Lowtri = (M_inv_struct *) malloc(sizeof(M_inv_struct));
    _Lowtri->_matrix = _mat;
    _Lowtri->_Etrans_head = _Etrans_temp_last;
    return _Lowtri;
}

Matrix *M_Dia_Inv(Matrix *_mat_source) {/*M_Inv for Dia_matrix
	对角矩阵求逆*/
    Matrix *_mat_inv = NULL;
    if (_mat_source->row != _mat_source->column) {
        printf(M_Dia_Inv_002);
        system("pause");
    } else {
        _mat_inv = Matrix_copy(_mat_source);
        MATRIX_TYPE *data = _mat_inv->data;
        int i, order = _mat_source->column;
        for (i = 0; i < order; i++) {
            if((data)[i * (order + 1)] == 0){ // 不可逆
                printf(M_Dia_Inv_023);
                system("pause");
                (data)[i * (order + 1)] = 1 / (data[i * (order + 1)]);
            }else{
                (data)[i * (order + 1)] = 1 / (data[i * (order + 1)]);
            }
        }
    }
    return _mat_inv;
}

Dia_struct *M_Diatri_(Matrix *_mat_source) {/*Diagonalization
	对角化*/
    Dia_struct *_Dia_instance = (Dia_struct *) malloc(sizeof(Dia_struct));
    Uptri_struct *_Uptri_ = M_Uptri_(_mat_source);
    Lowtri_struct *_Lowtri_ = M_Lowtri_(_Uptri_->Uptri_matrix);
    _Dia_instance->trans_leftmatrix = _Uptri_->trans_matrix;
    _Dia_instance->Diatri_matrix = _Lowtri_->Lowtri_matrix;
    _Dia_instance->trans_rightmatrix = _Lowtri_->trans_matrix;
    (_DETAILED_>=1)?printf(">>Diag(Matrix_%x)=\n", _mat_source):0;
    (_DETAILED_>=1)?printf("\tMatrixdl%x * Matrix_d%x * Matrixr_%x\n", _Uptri_->trans_matrix,
                        _Lowtri_->Lowtri_matrix, _Lowtri_->trans_matrix):0;
    // 释放内存
    M_free(_Uptri_->Uptri_matrix);
    free(_Uptri_);
    free(_Lowtri_);
    return _Dia_instance;
}

Matrix *M_Inverse(Matrix *_mat) {/*Matrix_Inverse
	矩阵求逆*/
    M_inv_struct *_Uptri_ = M_Uptri_4inv(_mat);
//    M_print(_Uptri_->_matrix);
    M_inv_struct *_Lowtri_ = M_Lowtri_4inv(_Uptri_->_matrix);
//    M_print(_Lowtri_->_matrix);
    Matrix *_mat_dia_inv = M_Dia_Inv(_Lowtri_->_matrix);
//    M_print(_mat_dia_inv);
    Matrix *_mat_inv = Etrans_4_Inverse(_mat_dia_inv, _Lowtri_->_Etrans_head, _ROW_);
    _mat_inv = Etrans_4_Inverse(_mat_inv, _Uptri_->_Etrans_head, _COLUMN_);
    (_DETAILED_>=2)?printf(">>Inv(Matrix_%x)=\n", _mat):0;
    (_DETAILED_>=2)?printf("\tMatrix_inv_%x\n", _mat_inv):0;
    // 释放内存
    M_free(_Uptri_->_matrix);
    M_free(_Lowtri_->_matrix);
    free(_Uptri_);
    free(_Lowtri_);
    return _mat_inv;
}

int M_Swap(Matrix *_mat, int _line_1, int _line_2, int line_setting) {/*Swap Line
	交换指定行和列*/
    _line_1 = _line_1 - 1;
    _line_2 = _line_2 - 1;
    int i;
    MATRIX_TYPE temp;
    if (line_setting == _ROW_) {
        if ((_line_1 < _mat->row) && (_line_2 < _mat->row)) {
            for (i = 0; i < (_mat->column); i++) {
                temp = _mat->data[_line_1 * (_mat->column) + i];
                _mat->data[_line_1 * (_mat->column) + i] = _mat->data[_line_2 * (_mat->column) + i];
                _mat->data[_line_2 * (_mat->column) + i] = temp;
            }
        } else {
            printf(M_swap_004);
            system("pause");
        }
    } else {
        if ((_line_1 < _mat->column) && (_line_2 < _mat->column)) {
            for (i = 0; i < (_mat->row); i++) {
                temp = _mat->data[_line_1 + (_mat->column) * i];
                _mat->data[_line_1 + (_mat->column) * i] = _mat->data[_line_2 + (_mat->column) * i];
                _mat->data[_line_2 + (_mat->column) * i] = temp;
            }
        } else {
            printf(M_swap_004);
            system("pause");
        }
    }
    return 0;
}

Matrix *M_Cut(Matrix *_mat, int row_head, int row_tail, int column_head, int column_tail) {/*Cut_out_part_of_Matrix
	切取部分矩阵*/
    Matrix *mat_result = NULL;
    if (row_tail < 0) {
        if (row_tail == _END_) {
            row_tail = _mat->row;
        } else {
            printf(M_Cut_007);
            system("pause");
        }
    }

    if (row_head < 0) {
        if (row_head == _END_) {
            row_head = _mat->row;
        } else {
            printf(M_Cut_007);
            system("pause");
        }
    }

    if (column_tail < 0) {
        if (column_tail == _END_) {
            column_tail = _mat->column;
        } else {
            printf(M_Cut_007);
            system("pause");
        }
    }

    if (column_head < 0) {
        if (column_head == _END_) {
            column_head = _mat->column;
        } else {
            printf(M_Cut_007);
            system("pause");
        }
    }

    if ((row_tail > _mat->row) || (column_tail > _mat->column)) {
        printf(M_Cut_005);
        system("pause");
    } else {
        if ((row_head > row_tail) || (column_head > column_tail)) {
            printf(M_Cut_006);
            system("pause");
        } else {
            row_head = row_head - 1;
            column_head = column_head - 1;
            mat_result = (Matrix *) malloc(sizeof(Matrix));
            mat_result->row = row_tail - row_head;
            mat_result->column = column_tail - column_head;
            mat_result->data = (MATRIX_TYPE *) malloc(sizeof(MATRIX_TYPE) * (mat_result->row) * (mat_result->column));
            int i, j;
            for (i = 0; i < (row_tail - row_head); i++) {
                for (j = 0; j < (column_tail - column_head); j++) {
                    mat_result->data[i * (mat_result->column) + j] = _mat->data[(i + row_head) * (_mat->column) +
                                                                                (j + column_head)];
                }
            }
        }
    }
    return mat_result;
}

Matrix *M_T(Matrix *_mat_source) {/*Transpose
	转置*/
    Matrix *_mat = (Matrix *) malloc(sizeof(Matrix));
    _mat->column = _mat_source->row;
    _mat->row = _mat_source->column;
    MATRIX_TYPE *data = (MATRIX_TYPE *) malloc(sizeof(MATRIX_TYPE) * (_mat->column) * (_mat->row));
    _mat->data = data;
    int i, j;
    for (i = 0; i < (_mat->row); i++) {
        for (j = 0; j < _mat->column; j++) {
            data[i * (_mat->column) + j] = _mat_source->data[j * (_mat_source->column) + i];
        }
    }
    return _mat;
}

int M_free(Matrix *_mat) {/*Free Memory
	释放矩阵，释放内存*/
    free(_mat->data);
    (_DETAILED_>=3)?printf(">>Matrix_%x has been freed.\n", _mat):0;
    free(_mat);
    return 0;
}

MATRIX_TYPE M_tr(Matrix *_mat) {/*Trace
	矩阵的迹*/
    MATRIX_TYPE _tr_mat = 0;
    if (_mat->column == _mat->row) {
        int i;
        for (i = 0; i < _mat->column; i++) {
            _tr_mat += _mat->data[i * (_mat->column + 1)];
        }
    } else {
        printf(M_tr_008);
        system("poause");
    }
    return _tr_mat;
}

MATRIX_TYPE M_det(Matrix *_mat_) {/*Determinant
	行列式*/
    MATRIX_TYPE _det_mat = 0;
    if (_mat_->column == _mat_->row) {
        Uptri_struct *_Uptri_ = M_Uptri_(_mat_);
        Matrix *_mat = _Uptri_->Uptri_matrix;
        _det_mat = 1;
        int i;
        for (i = 0; i < _mat->column; i++) {
            _det_mat *= _mat->data[i * (_mat->column + 1)];
        }
        // 释放内存
        M_free(_Uptri_->Uptri_matrix);
        M_free(_Uptri_->trans_matrix);
        free(_Uptri_);
    } else {
        printf(M_det_009);
        system("poause");
    }
    return _det_mat;
}

Matrix *M_full(Matrix *_mat, int row_up, int row_down, int column_left, int column_right, MATRIX_TYPE full_data) {/*Full
	填充矩阵*/
    Matrix *mat_result = NULL;
    mat_result = (Matrix *) malloc(sizeof(Matrix));
    mat_result->row = (_mat->row + row_up + row_down);
    mat_result->column = (_mat->column + column_left + column_right);
    mat_result->data = (MATRIX_TYPE *) malloc(sizeof(MATRIX_TYPE) * (mat_result->row) * (mat_result->column));
    int i, j;
    for (i = 0; i < mat_result->row; i++) {
        for (j = 0; j < mat_result->column; j++) {
            if ((i >= row_up) && (i < (row_up + _mat->row))) {/*这里的双判断，可以优化*/
                if ((j >= column_left) && (j < (column_left + _mat->column))) {
                    mat_result->data[i * (mat_result->column) + j] = _mat->data[(_mat->column) * (i - row_up) +
                                                                                (j - column_left)];
                } else {
                    mat_result->data[i * (mat_result->column) + j] = full_data;
                }
            } else {
                mat_result->data[i * (mat_result->column) + j] = full_data;
            }
        }
    }
    (_DETAILED_>=2)?(printf(">>Full<U%d,D%d,L%d,R%d>(Matrix_%x)=\n\tMatrix_%x\n",
                         row_up, row_down, column_left, column_right, _mat, mat_result)):0;
    return mat_result;
}

MATRIX_TYPE M_norm(Matrix *_mat, int Setting) {/*Caculate Matrix norm-num
	向量/矩阵范数 Setting=1 - 1范数，Setting=2 - 2范数*/
    MATRIX_TYPE *data = _mat->data;
    int row = _mat->row;
    int column = _mat->column;
    MATRIX_TYPE Val_norm = 0;
    int i, j;
    if (row == _ONE_ || column == _ONE_) {/*向量的范数*/
        switch (Setting) {
            case 1: {/*向量的1范数*/
                for (i = 0; i < row; i++) {
                    for (j = 0; j < column; j++) {
                        /*使用abs()会提示，error C2668: “abs”: 对重载函数的调用不明确
						转而使用fabs().*/
                        Val_norm += fabs(data[i * (column) + j]);
                    }
                }
                break;
            }
            case 2: {/*向量的2范数*/
                for (i = 0; i < row; i++) {
                    for (j = 0; j < column; j++) {
                        Val_norm += data[i * (column) + j] * data[i * (column) + j];
                    }
                }
                Val_norm = pow(Val_norm, 0.5);
                break;
            }
            case INF: {/*向量的∞(inf)无穷范数*/
                Matrix *M_temp_0, *M_temp_1;
                M_temp_0 = M_abs(_mat);
                M_temp_1 = M_max(M_temp_0);
                int temp_num = M_temp_1->data[0];
                Val_norm = (M_temp_0)->data[temp_num];
                // 释放内存
                M_free(M_temp_0);
                M_free(M_temp_1);
                break;
            }
            default: {/*向量的p范数*/
                for (i = 0; i < row; i++) {
                    for (j = 0; j < column; j++) {
                        Val_norm += pow(data[i * (column) + j], Setting);
                    }
                }
                if (Val_norm < 0) {
                    printf(M_norm_warm_01);
                }
                Val_norm = pow(Val_norm, 1.0 / Setting);
                break;
            }
        }
    } else {
        /*矩阵范数*/
        switch (Setting) {
            case 1: {/*矩阵的1范数*/
                Matrix *M_temp_0, *M_temp_1, *M_temp_2;
                M_temp_0 = M_abs(_mat);
                M_temp_1 = M_sum(M_temp_0);
                M_temp_2 = M_max(M_temp_1);
                int temp_num = M_temp_2->data[0];
                Val_norm = (M_temp_1)->data[temp_num];
                M_free(M_temp_0);
                M_free(M_temp_1);
                M_free(M_temp_2);
                break;
            }
            case 2: {/*矩阵的2范数*/
                Matrix *M_temp_0, *M_temp_1;
                M_temp_0 = M_T(_mat);
                M_temp_1 = M_mul(M_temp_0, _mat);
                M_eigen_struct *M_temp_1_eigen = M_eigen_max(M_temp_1);
                Val_norm = M_temp_1_eigen->eigen_value;
                M_free(M_temp_0);
                M_free(M_temp_1);
                M_free(M_temp_1_eigen->eigen_matrix);
                free(M_temp_1_eigen);
                break;
            }
            case INF: {/*矩阵的∞(inf)无穷范数*/
                Matrix *M_temp_0, *M_temp_1, *M_temp_2, *M_temp_;
                M_temp_ = M_T(_mat);
                M_print(M_temp_);
                M_temp_0 = M_abs(M_temp_);
                M_print(M_temp_0);
                M_temp_1 = M_sum(M_temp_0);
                M_print(M_temp_1);
                M_temp_2 = M_max(M_temp_1);
                M_print(M_temp_2);
                int temp_num = M_temp_2->data[0];
                Val_norm = (M_temp_1)->data[temp_num];
                M_free(M_temp_);
                M_free(M_temp_0);
                M_free(M_temp_1);
                M_free(M_temp_2);
                break;
            }
            case FRO: {/*矩阵的F范数（Frobenius范数）*/
                for (i = 0; i < row; i++) {
                    for (j = 0; j < column; j++) {
                        Val_norm += data[i * (column) + j] * data[i * (column) + j];
                    }
                }
                Val_norm = pow(Val_norm, 0.5);
                break;
            }
            default: {
                printf(M_norm_022);
                system("pause");
                break;
            }
        }
    }
    return Val_norm;
}

Matrix *M_abs(Matrix *_mat_origin) {/*Matrix Taking Absolute Value
	矩阵所有元素取绝对值*/
    Matrix *_mat = (Matrix *) malloc(sizeof(Matrix));
    _mat->row = _mat_origin->row;
    _mat->column = _mat_origin->column;
    int size = _mat->row * _mat->column;
    _mat->data = (MATRIX_TYPE *) malloc((size) * sizeof(MATRIX_TYPE));
    int i;
    for (i = 0; i < size; i++) {
        _mat->data[i] = fabs(_mat_origin->data[i]);
    }

    return _mat;
}

Matrix *M_numul(Matrix *_mat, MATRIX_TYPE _num) {/*Matrix Multiply
	矩阵数乘*/
    MATRIX_TYPE *data = _mat->data;
    int Size_mat = (_mat->row) * (_mat->column), i;
    for (i = 0; i < Size_mat; i++) {
        data[i] = data[i] * _num;
    }
    return _mat;
}

Matrix *M_matFull(Matrix *_mat, int row_up, int column_left, Matrix *_mat_full) {/*Full
	使用矩阵填充矩阵
	[注] 最左侧，和最上侧，row_up和column_left取值为_HEAD_(1)*/
    MATRIX_TYPE *data = _mat->data;
    row_up--;
    column_left--;
    int row = _mat->row;
    int column = _mat->column;
    int i, j;
    if ((row_up + _mat_full->row) > row) {
        printf(M_Mfull_010);
        system("pause");
    }
    if ((column_left + _mat_full->column) > column) {
        printf(M_Mfull_011);
        system("pause");
    }
    int full_row = _mat_full->row, full_column = _mat_full->column;
    for (i = 0; i < full_row; i++) {
        for (j = 0; j < full_column; j++) {
            data[(row_up + i) * column + (column_left + j)] = _mat_full->data[i * full_column + j];
        }
    }
    return _mat;
}

Matrix *M_Zeros(int row, int column) {/*Generate Zeros _matrix
	生成全零矩阵*/
    Matrix *Zero_mat = (Matrix *) malloc(sizeof(Matrix));
    Zero_mat->column = column;
    Zero_mat->row = row;
    int size = row * column, i;
    MATRIX_TYPE *data = (MATRIX_TYPE *) malloc((size) * sizeof(MATRIX_TYPE));
    for (i = 0; i < size; i++) {
        data[i] = 0;
    }
    Zero_mat->data = data;
    return Zero_mat;
}

Matrix *M_Ones(int row, int column) {/*Generate Ones _matrix
	生成全一矩阵*/
    Matrix *Zero_mat = (Matrix *) malloc(sizeof(Matrix));
    Zero_mat->row = row;
    Zero_mat->column = column;
    int size = row * column, i;
    MATRIX_TYPE *data = (MATRIX_TYPE *) malloc((size) * sizeof(MATRIX_TYPE));
    for (i = 0; i < size; i++) {
        data[i] = 1;
    }
    Zero_mat->data = data;
    return Zero_mat;
}

Matrix *M_find(Matrix *_mat, MATRIX_TYPE value) {/*Find position with the value in Matrix
	矩阵中寻找特定值位置；有多个相同值，则返回多个位置；
	（Matrix格式返回，为一行存储中的序号）；
	eg.
	[code]
	// define mat_1
		MATRIX_TYPE _mat_1[2][2] = { 1,0,0,1 };
		int row = sizeof(_mat_1) / sizeof(_mat_1[0]);
		int column = sizeof(_mat_1[0]) / sizeof(_mat_1[0][0]);
		Matrix*  mat_1 = Matrix_gen(row,column,(double *)_mat_1);
	// find mat_1
		M_print(M_find(mat_1,1));
	[output]
	>>Matrix_b380c0:
	0.00
	3.00
	[explain]
	即对应 mat_1 { 1,0,0,1 }中第0号、3号元素，值为1。
  	*/
    int size_mat = (_mat->row) * (_mat->column);
    int *position = (int *) malloc(sizeof(int) * size_mat);
    int num = 0, temp_column, temp_row, i;
    for (i = 0; i < size_mat; i++) {
        if (_mat->data[i] == value) {
            position[num] = i;
            num = num + 1;
        }
    }
    MATRIX_TYPE *data = (MATRIX_TYPE *) malloc(sizeof(MATRIX_TYPE) * num);
    for (i = 0; i < num; i++) {
        temp_column = position[i] % (_mat->column);
        temp_row = (position[i] - temp_column) / (_mat->column);
        data[i] = temp_column * (_mat->row) + temp_row;
    }
    Matrix *mat_result = (Matrix *) malloc(sizeof(Matrix));
    mat_result->row = num;
    mat_result->column = 1;
    mat_result->data = data;
    free(position);
    return mat_result;
};

Matrix *M_sum(Matrix *_mat) {/*Matrix column sum / Vector element sum
	矩阵按列求和/向量元素和*/
    Matrix *mat_result = (Matrix *) malloc(sizeof(Matrix));
    int row = _mat->row, column = _mat->column;
    int i, j, size = row * column;
    if (row == _ONE_ || column == _ONE_) {
        MATRIX_TYPE *data = (MATRIX_TYPE *) malloc(sizeof(MATRIX_TYPE));
        data[0] = 0;
        for (i = 0; i < (size); i++) {
            data[0] = data[0] + _mat->data[i];
        }
        mat_result->row = 1;
        mat_result->column = 1;
        mat_result->data = data;
    } else {
        MATRIX_TYPE *data = (MATRIX_TYPE *) malloc(sizeof(MATRIX_TYPE) * column);
        for (i = 0; i < (column); i++) {
            data[i] = 0;
            for (j = 0; j < (row); j++) {
                data[i] = data[i] + _mat->data[j * column + i];
            }
        }
        mat_result->row = 1;
        mat_result->column = column;
        mat_result->data = data;
    }
    return mat_result;
}

Matrix *M_min(Matrix *_mat) {/*Matrix minimum row position / Vector minimum element position
	矩阵按列最小行位置/向量最小元素位置*/
    Matrix *mat_result = (Matrix *) malloc(sizeof(Matrix));
    int row = _mat->row, column = _mat->column;
    int i, j, size = row * column;
    if (row == _ONE_ || column == _ONE_) {
        MATRIX_TYPE *data = (MATRIX_TYPE *) malloc(sizeof(MATRIX_TYPE));
        data[0] = Min_position(_mat->data, size);
        mat_result->row = 1;
        mat_result->column = 1;
        mat_result->data = data;
    } else {
        MATRIX_TYPE *data = (MATRIX_TYPE *) malloc(sizeof(MATRIX_TYPE) * column);
        MATRIX_TYPE *temp_data = (MATRIX_TYPE *) malloc(sizeof(MATRIX_TYPE) * row);
        for (i = 0; i < (column); i++) {
            for (j = 0; j < (row); j++) {
                temp_data[j] = _mat->data[j * column + i];
            }
            data[i] = Min_position(temp_data, row);
        }
        mat_result->row = 1;
        mat_result->column = column;
        mat_result->data = data;
    }
    return mat_result;
}

int Min_position(MATRIX_TYPE *data, int size) {/*Find min-position in a MATRIX_TYPE[*]
	在MATRIX_TYPE[*]列表中，找到最小值位置（第一个最小值位置）*/
    MATRIX_TYPE Val_min = data[size - 1];
    int min_position = size - 1, i;
    for (i = size - 2; i >= 0; i--) {
        if (data[i] <= Val_min) {
            Val_min = data[i];
            min_position = i;
        }
    }
    return min_position;
}

Matrix *M_max(Matrix *_mat) {/*Matrix maximum row position / Vector maximum element position
	矩阵按列最大行位置/向量最大元素位置*/
    Matrix *_mat_ = Matrix_copy(_mat);
    _mat_ = M_numul(_mat_, -1);
    Matrix *mat_result = M_min(_mat_);
    M_free(_mat_);
    return mat_result;
}

Matrix *M_minax_val(Matrix *_mat, Matrix *_mat_position) {/*use _mat_position(vector)
	to get the value of the specified row position of each column of the matrix (_mat)
	矩阵各列指定行位置的值*/
    Matrix *mat_val = (Matrix *) malloc(sizeof(Matrix));
    mat_val->row = _mat_position->row;
    mat_val->column = _mat_position->column;
    int i, temp, size_mat = (_mat_position->row) * (_mat_position->column);
    MATRIX_TYPE *data = (MATRIX_TYPE *) malloc(sizeof(MATRIX_TYPE) * size_mat);
    for (i = 0; i < size_mat; i++) {
        temp = (_mat_position->data[i]);
        data[i] = _mat->data[temp * (_mat->column) + i];
    }
    mat_val->data = data;
    return mat_val;
}

Matrix *M_logic_equal(Matrix *_mat, MATRIX_TYPE value) {/*Compare each position of the matrix with the given value
	(return the matrix, the value is 0/1)
	矩阵各位置与给定值比较，(返回矩阵,取值0/1)*/
    int size_mat = (_mat->row) * (_mat->column), i;
    Matrix *mat_logic = (Matrix *) malloc(sizeof(Matrix));
    mat_logic->row = (_mat->row);
    mat_logic->column = (_mat->column);
    MATRIX_TYPE *data = (MATRIX_TYPE *) malloc(sizeof(MATRIX_TYPE) * size_mat);
    for (i = 0; i < size_mat; i++) {
        if (_mat->data[i] == value) {
            data[i] = 1;
        } else {
            data[i] = 0;
        }
    }
    mat_logic->data = data;
    return mat_logic;
}

Matrix *M_logic(Matrix *_mat_left, Matrix *_mat_right, int Operation) {/*Logical operation
	 of corresponding positions of two matrices
	两矩阵对应位置逻辑运算
	*/
    Matrix *mat_logic = (Matrix *) malloc(sizeof(Matrix));
    if (_mat_right) {
        if (_mat_left->row != _mat_right->row) {
            printf(M_logic_012);
            system("pause");
        }
        if (_mat_left->column != _mat_right->column) {
            printf(M_logic_013);
            system("pause");
        }
    }

    int size_mat = (_mat_left->row) * (_mat_left->column), i;
    mat_logic->row = (_mat_left->row);
    mat_logic->column = (_mat_left->column);
    MATRIX_TYPE *data = (MATRIX_TYPE *) malloc(sizeof(MATRIX_TYPE) * size_mat);
    switch (Operation) {
        case _AND_:
            for (i = 0; i < size_mat; i++) {
                if ((_mat_left->data[i] == 0) || (_mat_right->data[i] == 0)) {
                    data[i] = 0;
                } else {
                    data[i] = 1;
                }
            }
            break;
        case _OR_:
            for (i = 0; i < size_mat; i++) {
                if ((_mat_left->data[i] != 0) || (_mat_right->data[i] != 0)) {
                    data[i] = 1;
                } else {
                    data[i] = 0;
                }
            }
            break;
        case _NOT_:
            for (i = 0; i < size_mat; i++) {
                if (_mat_left->data[i] == 0) {
                    data[i] = 1;
                } else {
                    data[i] = 0;
                }
            }
            break;
        default:
            printf(M_logic_014);
    }
    mat_logic->data = data;
    return mat_logic;
}

Matrix *M_pmuldiv(Matrix *_mat_left, Matrix *_mat_right, int operation) {/*Point Mul and Div
	矩阵点乘/点除*/
    Matrix *mat_result = (Matrix *) malloc(sizeof(Matrix));
    if (_mat_left->row != _mat_right->row) {
        printf(M_pmuldiv_015);
        system("pause");
    }
    if (_mat_left->column != _mat_right->column) {
        printf(M_pmuldiv_016);
        system("pause");
    }
    int i, size_mat = (_mat_left->row) * (_mat_left->column);
    mat_result->row = (_mat_left->row);
    mat_result->column = (_mat_left->column);
    MATRIX_TYPE *data = (MATRIX_TYPE *) malloc(sizeof(MATRIX_TYPE) * size_mat);
    if (operation == _MUL_) {
        for (i = 0; i < size_mat; i++) {
            data[i] = (_mat_left->data[i]) * (_mat_right->data[i]);
        }
    } else {
        if (operation == _DIV_) {
            for (i = 0; i < size_mat; i++) {
                if (_mat_right->data[i] != 0) {
                    data[i] = (_mat_left->data[i]) / (_mat_right->data[i]);
                } else {
                    data[i] = _INFINITE_;
                }

            }
        } else {
            printf(M_pmuldiv_017);
        }
    }
    mat_result->data = data;
    return mat_result;
}

Matrix *M_setval(Matrix *_mat_ini, Matrix *_mat_val, Matrix *_mat_order, int model) {/*Mat Set Value
使用矩阵传递数据，给矩阵指定位置赋值
*/
    int i, temp, size_ini = (_mat_ini->column) * (_mat_ini->row);
    int size_val = (_mat_val->column) * (_mat_val->row);
    int size_order = (_mat_order->column) * (_mat_order->row);
    if (model == _ORD4INI_) {/*_ORD4INI_*/
        for (i = 0; i < size_order; i++) {
            if ((_mat_order->data[i]) < size_ini) {
                if (i < size_val) {
                    temp = (_mat_order->data[i]);
                    _mat_ini->data[temp] = _mat_val->data[i];
                } else {
                    printf(M_setval_019);
                    system("pause");
                }
            } else {
                printf(M_setval_018);
                system("pause");
            }
        }
    } else {/*_ORD4VAL_*/
        for (i = 0; i < size_ini; i++) {
            if ((i) < size_order) {
                temp = (_mat_order->data[i]);
                if (temp < size_val) {
                    _mat_ini->data[i] = _mat_val->data[temp];
                } else {
                    printf(M_setval_019);
                    system("pause");
                }
            } else {
                printf(M_setval_020);
                system("pause");
            }
        }
    }

    return _mat_ini;
}

Matrix *M_numul_m(Matrix *_mat, Matrix *_mat_multi) {/*Matrix Multiply
	矩阵数乘 使用矩阵对于矩阵,对于行进行操作
	_mat_result = _mat_left*_mat_right */
    MATRIX_TYPE *data = _mat->data;
    int Size_mat = (_mat->row) * (_mat->column), i, j, temp;
    int row = _mat->row;
    int column = _mat->column;
    MATRIX_TYPE Multi;
    for (i = 0; i < row; i++) {
        Multi = _mat_multi->data[i];
        for (j = 0; j < column; j++) {
            temp = i * column + j;
            data[temp] = data[temp] * Multi;
        }
    }
    return _mat;
}
// 修改: M_eigen_struct_temp -> M_eigen_struct
M_eigen_struct *M_eigen_max(Matrix *_mat) {    /*Matrix Max Eigenvalue(vec)
	求解矩阵最大特征值（幂法）
	_mat_result = Max_eigenvalue(_mat)
	幂法-算法参考：https://max.book118.com/html/2017/0527/109650252.shtm*/
    M_eigen_struct *M_eigen_max = NULL;
    if (_mat->column == _mat->row) {
        Matrix *mat_z = M_Ones(_mat->column, 1), *mat_temp_1 = NULL, *mat_temp_2 = NULL;
        Matrix *mat_y = NULL, *mat_z_gap = NULL;
        MATRIX_TYPE m_value = 0, mat_z_gap_norm = 1;
        MATRIX_TYPE deta = 1e-7; //精度设置
        int temp_num = 0;

        while (mat_z_gap_norm > deta) {
            mat_y = M_mul(_mat, mat_z);
            mat_temp_1 = M_max(mat_y);//需要释放结果空间
            temp_num = ((mat_temp_1)->data[0]);
            m_value = mat_y->data[temp_num];
            mat_temp_2 = mat_z;//需要释放结果空间
            mat_z = M_numul(mat_y, 1 / m_value);
            mat_z_gap = M_add_sub(1, mat_z, 1, mat_temp_2);//需要释放结果空间
            mat_z_gap_norm = M_norm(mat_z_gap, 2);
            // 释放内存
            M_free(mat_temp_1);
            M_free(mat_temp_2);
            M_free(mat_z_gap);
//            M_free(mat_y);
        }

        M_eigen_max = (M_eigen_struct *) malloc(sizeof(M_eigen_struct));
        M_eigen_max->eigen_value = m_value;
        M_eigen_max->eigen_matrix = mat_z;
    } else {
        printf(M_eigen_max_021);
        system("pause");
    }
    return M_eigen_max;
}

Matrix ** M_eigen (Matrix *_mat) {/*Matrix Eigenvalue(vec)
	求解矩阵特征值+特征向量（幂法）
	_mat_result = Max_eigenvalue(_mat)
	幂法-算法参考：https://max.book118.com/html/2017/0527/109650252.shtm
    求解-特征向量参考: https://blog.csdn.net/w_weixiaotao/article/details/111868086*/
    Matrix ** M_array_eigen_vec = NULL;
    if (_mat->column == _mat->row) {
        M_array_eigen_vec = (Matrix **)malloc(sizeof(Matrix *)*2); // 保存Q/R矩阵地址
        enum{val=0, vec=1};
        Matrix *eigen_value = M_eigen_val(_mat);
        M_array_eigen_vec[val] = eigen_value;
        int eigen_count, dim = _mat->column, i, j, ik, jk;
        Matrix *eigen_vector = NULL, *_mat_ = NULL;
        eigen_vector = M_Zeros(dim,dim);// 生成特征向量
        M_array_eigen_vec[vec] = eigen_vector;
        double eigen_value_temp ;
        MATRIX_TYPE coe; // core of elements, 对角线元素/中心元素
        for(eigen_count=0;eigen_count<dim;eigen_count++){
            _mat_ = Matrix_copy(_mat);
            eigen_value_temp = eigen_value->data[eigen_count];
            // (A-lamda*I)
            for (i = 0; i < dim; i++){
                _mat_->data[i * _mat_->column + i] -= eigen_value_temp; // 注意: 这里计算 (A-lamda*I), 当A为I/diag时，可能存在问题;
            }
            // 矩阵化为阶梯型矩阵(归一性): 对角线值为1
            for (i = 0; i < dim-1; i++){
                coe = _mat_->data[i * dim + i];
                for (j = i; j<dim; j++){
                    _mat_->data[i * dim + j] /= coe; //让对角线元素归一化
                }
                for (ik = i + 1; ik < dim; ik++){
                    coe = _mat_->data[ik * dim + i];
                    for (jk = i; jk < dim; jk++){
                        _mat_->data[ik * dim + jk] -= coe * _mat_->data[i * dim + jk];
                    }
                }
            }
             // 让最后一行为1
            double sum1 = 1;
            eigen_vector->data[(dim - 1) * dim + eigen_count] = 1;
            for (ik = dim - 2; ik >= 0; ik--)
            {
                double sum2 = 0;
                for (jk = ik + 1; jk < dim; jk++)
                {
                    sum2 += _mat_->data[ik * dim + jk] * eigen_vector->data[jk * dim + eigen_count];
                }
                sum2 = -sum2 / _mat_->data[ik * dim + ik];
                sum1 += sum2 * sum2;
                eigen_vector->data[ik * dim + eigen_count] = sum2;
            }
            M_free(_mat_);
            sum1 = sqrt(sum1);//当前列的模
            for (int i = 0; i < dim; i++){
                // 向量单位化
                eigen_vector->data[i * dim + eigen_count] /= sum1;
            }
        }
    }else{
        printf(M_eigen_026);
        system("pause");
    }
    return M_array_eigen_vec;
}

//    householder变换, 参考:https://blog.csdn.net/qq_40922398/article/details/112788453
Matrix* householder(Matrix * _x) {
    Matrix *H = NULL;
    Matrix *y = M_Zeros(_x->row,_x->column);
    y->data[0] = M_norm(_x, 2);
    Matrix *w = NULL;
    if(_x->data[0] > 0){
        w = M_add_sub(1,_x,-1,y);
        M_numul(w,1/M_norm(w, 2));
    }else{
        w = M_add_sub(1,_x,1,y);
        M_numul(w,1/M_norm(w, 2));
    }
    Matrix *I= M_I(_x->row);
    Matrix *w_T = M_T(w);
    Matrix *M_dot = M_mul(w,w_T);
    H = M_add_sub(1,I,2,M_dot);
    M_free(y);
    M_free(w);
    M_free(I);
    M_free(w_T);
    M_free(M_dot);
    return H;
}

Matrix * M_householder(Matrix * _mat) {
    Matrix *h_Mat = NULL;
    if (_mat->column == _mat->row) {
        int i,j,k, dim = _mat->column;
        Matrix * Ri = Matrix_copy(_mat);
        Matrix * temp =NULL;
        Matrix *Q, *Qi, *Hi;
        for(i=1;i<dim;i++){
            Matrix* x = M_Cut(_mat,i+1,_END_,i,i);
            // householder 具体计算
            Hi = householder(x);
            M_free(x);
            Qi = M_I(dim);
            for(j=0;j<dim-i;j++){ // Qi[i:, i:] = Hi
                for(k=0;k<dim-i;k++){
                    Qi->data[(j+i)*dim+(k+i)] = Hi->data[j*(dim-i)+k];
                }
            }
            M_free(Hi);
            if (i == 1){
                Q = Matrix_copy(Qi);
            }else{
                temp = Q;
                Q = M_mul(Qi,temp);
                M_free(temp);
            }
            temp = Ri;
            Ri = M_mul(Qi,Ri);
            M_free(temp);
            temp = Ri;
            Ri = M_mul(Ri,Qi);
            M_free(temp);
            M_free(Qi);
        }
        h_Mat = Ri;
        M_free(Q); // 变换矩阵 Q
    }else{
        printf(M_householder_027); // 待完善补充
        system("pause");
    }
    return h_Mat;
}

Matrix ** M_QR(Matrix * _mat){
    Matrix ** M_array_Q_R = (Matrix **)malloc(sizeof(Matrix *)*2); // 保存Q/R矩阵地址
    enum{q=0, r=1};
    M_array_Q_R[q] = NULL;
    M_array_Q_R[r] = NULL;
    int i, j, k, dim = _mat->row;
    Matrix *Q=NULL, *D=NULL, *Qi=NULL, *Hi=NULL, *x=NULL, *temp_1=NULL, *temp_2=NULL;
    Matrix * Ri = Matrix_copy(_mat); // 注意
    for(i=0;i<dim;i++){
        x = M_Cut(_mat,i+1,_END_,i+1,i+1);
        Hi = householder(x);
        M_free(x);
        // Ri[i:, i:] = np.dot(Hi, Ri[i:, i:])
        temp_1 = M_Cut(Ri,i+1,_END_,i+1,_END_);
        temp_2 = M_mul(Hi,temp_1);
        M_free(temp_1);
        for(j=0;j<dim-i;j++){
            for(k=0;k<dim-i;k++){
                Ri->data[(j+i)*dim+(k+i)] = temp_2->data[j*(dim-i)+k];
            }
        }
        M_free(temp_2);
        Qi = M_I(dim);
        for(j=0;j<dim-i;j++){ // Qi[i:, i:] = Hi
            for(k=0;k<dim-i;k++){
                Qi->data[(j+i)*dim+(k+i)] = Hi->data[j*(dim-i)+k];
            }
        }
        M_free(Hi);
        if (i == 0){
            Q = Matrix_copy(Qi);
        }else{
            temp_1 = Q;
            Q = M_mul(Qi,temp_1);
            M_free(temp_1);
        }
        M_free(Qi);
    }
    D = M_I(dim);
    for(i=0;i<dim;i++){
        D->data[i] = (Ri->data[i*dim+i] < 0)?-1:1;
    }
    M_array_Q_R[r] = M_mul(D,Ri);
    temp_1 = M_T(Q);
    temp_2 = M_Dia_Inv(D);
    M_array_Q_R[q] = M_mul(temp_1, temp_2);
    M_free(temp_1);
    M_free(temp_2);
    M_free(Ri);
    M_free(D);
    M_free(Q);
    return M_array_Q_R;
}

Matrix * M_eigen_val(Matrix * _mat){
    (_DETAILED_>=2)?printf(">>Eigen_Value(Matrix_%x)=\n", _mat):0;
    (_DETAILED_>=2)?printf("...CACULATING...\n#if need help: use \'help(\"M_eigen_val\")\'#\n"):0;
    double *eigen_val = NULL;
    Matrix ** M_array_Q_R = NULL; // 保存Q/R矩阵地址
    enum{q=0, r=1};
    double eps = 1e-5, delta = 1; // 设置计算误差
    int i, dim=_mat->row, epoch = 0;
    Matrix *Ak0, *Ak, *Qk, *Rk, *M_eigen_val;
    Ak = Matrix_copy(_mat);
    while((delta > eps)&&(epoch < _MAX_LOOP_NUM_)){
        M_array_Q_R = M_QR(Ak);
        Qk = M_array_Q_R[q];
        Rk = M_array_Q_R[r];
        Ak0 = Ak;
        Ak = M_mul(Rk, Qk);
        delta = 0;
        for(i=0;i<dim;i++){
            delta += fabs(Ak->data[i*dim+i]-Ak0->data[i*dim+i]);
        }
        M_free(Ak0);
        M_free(Qk);
        M_free(Rk);
        free(M_array_Q_R);
        (_progress_bar_)?progress_bar(epoch,_MAX_LOOP_NUM_):0;
        epoch++;
    }
    if(epoch >= _MAX_LOOP_NUM_){
        printf("\n>>ATTENTION: QR Decomposition end with delta = %.3e!(epoch=%d, eps=%.2e)\n", delta,_MAX_LOOP_NUM_,eps);
    }
    M_eigen_val = (Matrix*)malloc(sizeof(Matrix));
    M_eigen_val->column = dim;
    M_eigen_val->row = 1;
    eigen_val = (double*)malloc(sizeof(double)*dim);
    for(i=0;i<dim;i++){
        eigen_val[i] = Ak->data[i*dim+i];
    }
    M_eigen_val->data = eigen_val;
    M_free(Ak);
    (_DETAILED_>=2)?printf("...END...\n>>Eigen_Value = (Matrix_%x)\n", M_eigen_val):0;
    return M_eigen_val;
}

void progress_bar(int count, int total) {
    double num = (int)((1.0*count/total)*50);
    printf("%% %.2f[", num*2);
    for(int i = 0; i < 50; i++){
        (i < num)?printf(">"):printf(" ");
    }
    printf("]\r");
}