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
	// A_10
	MATRIX_TYPE _mat_A10[10][10] = {0.221746734017240,0.928854139478045,0.488897743920167,0.0987122786555743,0.715037078400694,0.904722238067363,0.489901388512224,0.521649842464284,0.800330575352402,0.0604711791698936,0.117417650855806,0.730330862855453,0.624060088173690,0.261871183870716,0.903720560556316,0.609866648422558,0.167927145682257,0.0967300257808670,0.453797708726920,0.399257770613576,0.296675873218327,0.488608973803579,0.679135540865748,0.335356839962797,0.890922504330789,0.617666389588455,0.978680649641159,0.818148553859625,0.432391503783462,0.526875830508296,0.318778301925882,0.578525061023439,0.395515215668593,0.679727951377338,0.334163052737496,0.859442305646212,0.712694471678914,0.817547092079286,0.825313795402046,0.416799467930787,0.424166759713807,0.237283579771521,0.367436648544477,0.136553137355370,0.698745832334795,0.805489424529686,0.500471624154843,0.722439592366842,0.0834698148589140,0.656859890973707,0.507858284661118,0.458848828179931,0.987982003161633,0.721227498581740,0.197809826685929,0.576721515614685,0.471088374541939,0.149865442477967,0.133171007607162,0.627973359190104,0.0855157970900440,0.963088539286913,0.0377388662395521,0.106761861607241,0.0305409463046367,0.182922469414914,0.0596188675796392,0.659605252908307,0.173388613119006,0.291984079961715,0.262482234698333,0.546805718738968,0.885168008202475,0.653757348668560,0.744074260367462,0.239932010568717,0.681971904149063,0.518594942510538,0.390937802323736,0.431651170248720,0.801014622769739,0.521135830804002,0.913286827639239,0.494173936639270,0.500022435590201,0.886511933076101,0.0424311375007417,0.972974554763863,0.831379742839070,0.0154871256360190,0.0292202775621463,0.231594386708524,0.796183873585212,0.779051723231275,0.479922141146060,0.0286741524641061,0.0714454646006424,0.648991492712356,0.803364391602440,0.984063724379154};
	row = sizeof(_mat_A10) / sizeof(_mat_A10[0]);
	column = sizeof(_mat_A10[0]) / sizeof(_mat_A10[0][0]);
	Matirx*  mat_A10 = Matrix_gen(row,column,_mat_A10);
	M_print(mat_A10);
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
	// B_10
	MATRIX_TYPE _mat_b10[10][1] = {0.167168409914656,0.106216344928664,0.372409740055537,0.198118402542975,0.489687638016024,0.339493413390758,0.951630464777727,0.920332039836564,0.0526769976807926,0.737858095516997};
	row = sizeof(_mat_b10) / sizeof(_mat_b10[0]);
	column = sizeof(_mat_b10[0]) / sizeof(_mat_b10[0][0]);
	Matirx*  mat_b10 = Matrix_gen(row,column,_mat_b10);
	M_print(mat_b10);
	
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
	Matirx* _mat_result = M_mul(M_Inverse(mat_A10),mat_b10);
	M_print(_mat_result);
	
// Others
	M_free(_mat_T);
	
//	system("pause"); 
	return 0;
}
