Matrix_hib
=======================================
矩阵运算库--C语言
---------------------------------------
##The lib of Matrix operation for C language. (矩阵运算库--C语言) 

Author: Amoiensis

Email: Amoiensis@outlook.com

Data:2020.02.12
***************************************************************
更多资料和信息：

https://github.com/Amoiensis/Matrix_hub
***************************************************************

具体：
	
	Folder_--_lib_.lib文件_+_.h文件

	Folder_--_code_.c文件_+_.h文件

***************************************************************

[CONTENT](https://github.com/Amoiensis/Matrix_hub)
---------------------------------------
####（操作-函数）
|         操作         |                  OPERATION                  |     func_NAME    |
|:--------------------:|:-------------------------------------------:|:----------------:|
|       生成矩阵       |                  generation                 |    Matrix_gen    |
|       复制矩阵       |                     copy                    |    Matrix_copy   |
|         乘法         |                   multiply                  |       M_mul      |
|        加减法        |                   add/sub                   |     M_add_sub    |
|       矩阵显示       |                    print                    |      M_print     |
|    单位矩阵(生成)    |             identity_matrix(gen)            |        M_I       |
|     矩阵基本变换     |            matrix_element_teransf           |     M_E_trans    |
|     基本变换矩阵     |             Element_trans2Matrix            |  Etrans_2_Matrix |
| 基本变换矩阵的逆矩阵 |           inv_Element_trans2Matrix          | Etrans_2_Inverse |
|       上三角化       |       Upper_triangular_transformation       |     M_Uptri_     |
|   上三角化(求逆用)   | Upper_triangular_transformation_for_Inverse |   M_Uptri_4inv   |
|       下三角化       |       Lower_triangular_transformation       |     M_Lowtri_    |
|   下三角化(求逆用)   | Lower_triangular_transformation_for_Inverse |   M_Lowtri_4inv  |
|     对角矩阵求逆     |          Matrix_Inv_for_Dia_matrix          |     M_Dia_Inv    |
|        对角化        |               Diagonalization               |     M_Diatri_    |
|         求逆         |                Matrix_Inverse               |     M_Inverse    |
|    矩阵行(列)调换    |                  Swap_Line                  |      M_Swap      |
|         转置         |                  Transpose                  |        M_T       |
|     切取部分矩阵     |            Cut_out_part_of_Matrix           |       M_Cut      |
|       释放内存       |            free_mempry_of_Matrix            |      M_free      |
|          迹          |                    trace                    |       M_tr       |
|        行列式        |                 Determinant                 |       M_det      |

Demo (Matrix_hub)
---------------------------------------
code:
```
/*
%% IMFORMATION
%% MATRIX_HUB
% Author: Xiping Yu
% Email:Amoiensis@outlook.com
% Github: https://github.com/Amoiensis/Matrix_hub
% Data: 2020.02.12 
% Case: Matrix Operation 
% Dtailed: the code_file of Matrix_hub
*/ 

#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"


int main(int argc, char *argv[]) {
	system("color 0F");
// Matrix
	//	Mat_1
	MATRIX_TYPE _mat_1[3][5] = { 2,2,1,1,1,4,1,1,1,1,1,5,1,1,1 };
	int row = sizeof(_mat_1) / sizeof(_mat_1[0]);
	int column = sizeof(_mat_1[0]) / sizeof(_mat_1[0][0]);
	Matirx*  mat_1 = Matrix_gen(row,column,_mat_1);
	M_print(mat_1);
	//	Mat_2	
	MATRIX_TYPE _mat_2[5][3] = { 1,2,3,4,5,6,7,8,7,10,11,12,11,14,15 };	
	//	MATRIX_TYPE _mat_2[5][2] = { 1,2,3,4,5,6,7,8,9,10};	
	row = sizeof(_mat_2) / sizeof(_mat_2[0]);
	column = sizeof(_mat_2[0]) / sizeof(_mat_2[0][0]);
	Matirx*  mat_2 = Matrix_gen(row,column,_mat_2);
	M_print(mat_2);
	// Mat_A
	MATRIX_TYPE _mat_A[5][5] = { 1,0,7,0,9,0,7,0,9,0,0,0,-5,0,4,2,0,0,2,0,0,3,8,0,1};
	row = sizeof(_mat_A) / sizeof(_mat_A[0]);
	column = sizeof(_mat_A[0]) / sizeof(_mat_A[0][0]);
	Matirx*  mat_A = Matrix_gen(row,column,_mat_A);
	M_print(mat_A);
	//	Mat_21
	MATRIX_TYPE _mat_21[3][3] = { 1,2,3,2,2,2,4,1,3};
	row = sizeof(_mat_21) / sizeof(_mat_21[0]);
	column = sizeof(_mat_21[0]) / sizeof(_mat_21[0][0]);
	Matirx*  mat_21 = Matrix_gen(row,column,_mat_21);
	M_print(mat_21);
	//	Mat_21b
	MATRIX_TYPE _mat_21b[3][3] = { 1,0,0,0,1,0,0,0,1};
	row = sizeof(_mat_21b) / sizeof(_mat_21b[0]);
	column = sizeof(_mat_21b[0]) / sizeof(_mat_21b[0][0]);
	Matirx*  mat_21b = Matrix_gen(row,column,_mat_21b);
	M_print(mat_21b);
	// Mat_b
	MATRIX_TYPE _mat_b[5][1] = { 1,2,7,2,4};
	row = sizeof(_mat_b) / sizeof(_mat_b[0]);
	column = sizeof(_mat_b[0]) / sizeof(_mat_b[0][0]);
	Matirx*  mat_b = Matrix_gen(row,column,_mat_b);
	M_print(mat_b);
	
//	Operation
	// 乘法 
	Matirx*  mat_3 = M_mul(mat_2,mat_1);
	M_print(mat_3);	
	// 加减法
	Matirx*  mat_diff = M_add_sub(1,mat_21,1,mat_21b);
	M_print(mat_diff);	
	// 初等变换 
	Etrans_struct _Etrans_;
	_Etrans_.minuend_line = 2;
	_Etrans_.subtractor_line = 1;
	_Etrans_.scale = 2;
	_Etrans_.next_E_trans = NULL;
	_Etrans_.forward_E_trans = NULL;
	M_E_trans(mat_2,&_Etrans_,_ROW_);
	M_print(mat_2);
	// 单位矩阵 
	M_print(M_I(5));
	// 初等变换to矩阵 
	Matirx* mat_4 = Etrans_2_Matrix(&_Etrans_,5,_ROW_);
	M_print(mat_4);
	Matirx* mat_5 = Etrans_2_Inverse(&_Etrans_,5,_ROW_);
	M_print(mat_5);
	// 上三角变换
	Uptri_struct* _Uptri_ =  M_Uptri_(mat_21);
	M_print(_Uptri_->trans_matrix);
	M_print(_Uptri_->Uptri_matrix );
	// 下三角变换
	Lowtri_struct* _Lowtri_ =  M_Lowtri_(mat_21);
	M_print(_Lowtri_->Lowtri_matrix);
	M_print(_Lowtri_->trans_matrix);
	// 对角化
	Dia_struct* _Dia_ = M_Diatri_(mat_21);
	M_print(_Dia_->trans_leftmatrix);
	M_print(_Dia_->Diatri_matrix);
	M_print(_Dia_->trans_rightmatrix);
	// 矩阵求逆 
	Matirx* _mat_inv = M_Inverse(mat_21);
	M_print(_mat_inv );
	// 行列交换
	M_Swap(_mat_inv,1,2,_ROW_);
	M_print(_mat_inv); 
	// 切割部分
	Matirx* _mat_cut = M_Cut(_mat_inv,_END_,_END_,2,3);
	M_print(_mat_cut);
	// 转置
	Matirx* _mat_T = M_T(_mat_inv);
	M_print(_mat_T);
	// 迹
	MATRIX_TYPE _tr_mat = M_tr(_mat_inv);
	printf("Tr(Matrix_%x) = %.4lf\n",_mat_inv,_tr_mat);
	// 行列式
	MATRIX_TYPE _det_mat = M_det(_mat_inv);
	printf("Det(Matrix_%x) = %.4lf\n",_mat_inv,_det_mat);
	
	
// Application
	// 解线性方程
		// mat_A*x = mat_b
	Matirx* _mat_result = M_mul(M_Inverse(mat_A),mat_b);
	M_print(_mat_result);
	
// Others
	M_free(_mat_T);
	
//	system("pause"); 
	return 0;
}
```

ATTENTION
---------------------------------------
Please feel free to contact with me for any questions, thank you!

Don't spread the files without permission!

所有文件仅仅供学习交流！

未经允许，请勿传播！
***************************************