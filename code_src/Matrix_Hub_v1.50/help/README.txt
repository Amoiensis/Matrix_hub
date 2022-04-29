Matrix_hub
=======================================
���������--C����
---------------------------------------
##The lib of Matrix operation for C language. (���������--C����) 

Author: Amoiensis

Email: Amoiensis@outlook.com

Data:2020.02.12
***************************************************************
�������Ϻ���Ϣ��

https://github.com/Amoiensis/Matrix_hub
***************************************************************

����Ӧ�����ӣ�

Optimization-Algorithm(���Ż��㷨)��https://github.com/Amoiensis/Optimization-Algorithm
***************************************************************

���壺
	
	Folder_--_lib_.lib�ļ�_+_.h�ļ�
	
	Folder_--_code_.c�ļ�_+_.h�ļ�

***************************************************************

[CONTENT](https://github.com/Amoiensis/Matrix_hub)
---------------------------------------
####������-������
|                   ����                   	|                                                 OPERATION                                                	|     func_NAME    	|
|:----------------------------------------:	|:--------------------------------------------------------------------------------------------------------:	|:----------------:	|
|                 ���ɾ���                 	|                                                generation                                                	|    Matrix_gen    	|
|                 ���ƾ���                 	|                                                   copy                                                   	|    Matrix_copy   	|
|                   �˷�                   	|                                                 multiply                                                 	|       M_mul      	|
|                  �Ӽ���                  	|                                                  add/sub                                                 	|     M_add_sub    	|
|                 ������ʾ                 	|                                                   print                                                  	|      M_print     	|
|              ��λ����(����)              	|                                           identity_matrix(gen)                                           	|        M_I       	|
|               ��������任               	|                                          matrix_element_teransf                                          	|     M_E_trans    	|
|               �����任����               	|                                           Element_trans2Matrix                                           	|  Etrans_2_Matrix 	|
|           �����任����������           	|                                         inv_Element_trans2Matrix                                         	| Etrans_4_Inverse 	|
|                 �����ǻ�                 	|                                      Upper_triangular_transformation                                     	|     M_Uptri_     	|
|             �����ǻ�(������)             	|                                Upper_triangular_transformation_for_Inverse                               	|   M_Uptri_4inv   	|
|                 �����ǻ�                 	|                                      Lower_triangular_transformation                                     	|     M_Lowtri_    	|
|             �����ǻ�(������)             	|                                Lower_triangular_transformation_for_Inverse                               	|   M_Lowtri_4inv  	|
|               �ԽǾ�������               	|                                         Matrix_Inv_for_Dia_matrix                                        	|     M_Dia_Inv    	|
|                  �Խǻ�                  	|                                              Diagonalization                                             	|     M_Diatri_    	|
|                   ����                   	|                                              Matrix_Inverse                                              	|     M_Inverse    	|
|              ������(��)����              	|                                                 Swap_Line                                                	|      M_Swap      	|
|                   ת��                   	|                                                 Transpose                                                	|        M_T       	|
|               ��ȡ���־���               	|                                          Cut_out_part_of_Matrix                                          	|       M_Cut      	|
|                 �ͷ��ڴ�                 	|                                           free_mempry_of_Matrix                                          	|      M_free      	|
|                    ��                    	|                                                   trace                                                  	|       M_tr       	|
|                  ����ʽ                  	|                                                Determinant                                               	|       M_det      	|
|                   ���                   	|                                                   Full                                                   	|      M_full      	|
|                   ����                   	|                                                   Norm                                                   	|      M_norm      	|
|                   �������ֵ                   	|                                                   Absolute Value                                                   	|      M_abs	|
|                 ��������                 	|                                           Number Multiplication                                          	|      M_numul     	|
|            (ʹ�þ���)������            	|                                             Full with matrix                                             	|     M_matFull    	|
|              ����(ȫ)�����              	|                                          Generation Zeros Matrix                                         	|      M_Zeros     	|
|              ����(ȫ)һ����              	|                                          Generation Ones Matrix                                          	|      M_Ones      	|
|        Ѱ�Ҿ����Ӧֵλ��(������)        	|                                  Find position with the value in Matrix                                  	|      M_find      	|
|          ���������/����Ԫ�غ�         	|                                  Matrix column sum / Vector element sum                                  	|       M_sum      	|
|    ��������С��λ��/������СԪ��λ��   	|                      Matrix minimum row position /   Vector minimum element position                     	|       M_min      	|
|    �����������λ��/�������Ԫ��λ��   	|                      Matrix maximum row position /   Vector maximum element position                     	|       M_max      	|
|          �������ָ����λ�õ�ֵ          	|              The value of the specified row   position <br>     of each column of the matrix             	|    M_minax_val   	|
| �����λ�������ֵ�Ƚ�(���ؾ���,ȡֵ0/1) 	| Compare each position of the   matrix with the given value<br>     (return the matrix, the value is 0/1) 	|   M_logic_equal  	|
|          �������Ӧλ���߼�����          	|                      Logical operation of   corresponding positions of two matrices                      	|      M_logic     	|
|             �����ӦԪ�س�/��            	|                            Multiply / divide corresponding elements of matrix                            	|     M_pmuldiv    	|
|        ����������ֵ(ʹ�þ��󴫵�)        	|                             Matrix batch assignment (using   matrix transfer)                            	|     M_setval     	|
|        ����Ծ��󣬶Ը��н�������        	|                             Matrix Number Multiplication (using matrix transfer)                            	|     M_numul_m	|
|        		����	        	|                             			Help File			                            	|     help     	|

## [����˵��] 

#### [Matrix Hub v1.43] 2021.10.26

1. ���¾��������㷨�����л�������������ٶ�����������"M_Inverse"������

2. �޸����ȱ任���ڴ����⣬���������ڴ�ռ�ü��٣�

3. ɾ����Etrans_2_Inverse��������������Etrans_4_Inverse�����������ڼ��پ������棻

4. ���¡�M_Uptri_4inv������M_Lowtri_4inv�����ڼ��پ������棻

  **ע�⣺**�Ƽ����µ����µ�Matrix Hub v1.43����v1.42��֮ǰ�汾�����θ����ڴ�ͼ����ٶȶ��õ���ߣ�

#### [Matrix Hub v1.42] 2021.08.06

1. �����������������ֵ����:M_eigen_max()����ʹ��help("M_eigen_max")�鿴����ʹ�ã�
2. ��������ȡ����ֵ���� M_abs()����ʹ��help("M_abs")�鿴����ʹ�ã�
3. ���������;���ĸ��ַ�������M_norm()������1����(1)��2����(2)�������(INF)��F������FRO���ȷ����������˾��������������������,��ʹ��help("M_norm")�鿴����ʹ�ã�

#### [Matrix Hub v1.4] 2021.02.02
1.	���� help() ����������������������ƣ��鿴����ʹ�÷������磬help("help")��help("Matrix_gen")��help("README")��help("Update")����;


2. 	����������M_numul_m ()�������ھ������� ��������ھ�����в��������ж�Ӧ���� ��


3.	��ԭ M_matFull() �����У�����࣬�����ϲ࣬row_up��column_leftȡֵ�ӡ�0������Ϊ��1��_HEAD_����;


4.	����ԭ������"Matirx"����д������Ϊ"Matrix";



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
% Data: 2020.02.24 
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
	
	...
	
//	Operation
	// �˷� 
	Matirx*  mat_3 = M_mul(mat_2,mat_1);
	M_print(mat_3);	
	// �Ӽ���
	Matirx*  mat_diff = M_add_sub(1,mat_21,1,mat_21b);
	M_print(mat_diff);	
	// ���ȱ任 
	Etrans_struct _Etrans_;
	_Etrans_.minuend_line = 2;
	_Etrans_.subtractor_line = 1;
	_Etrans_.scale = 2;
	_Etrans_.next_E_trans = NULL;
	_Etrans_.forward_E_trans = NULL;
	M_E_trans(mat_2,&_Etrans_,_ROW_);
	M_print(mat_2);
	// ��λ���� 
	M_print(M_I(5));
	// ���ȱ任to���� 
	Matirx* mat_4 = Etrans_2_Matrix(&_Etrans_,5,_ROW_);
	M_print(mat_4);
	// �����Ǳ任
	Uptri_struct* _Uptri_ =  M_Uptri_(mat_21);
	M_print(_Uptri_->trans_matrix );
	M_print(_Uptri_->Uptri_matrix);
	// �����Ǳ任
	Lowtri_struct* _Lowtri_ =  M_Lowtri_(mat_21);
	M_print(_Lowtri_->Lowtri_matrix);
	M_print(_Lowtri_->trans_matrix);
	// �Խǻ�
	Dia_struct* _Dia_ = M_Diatri_(mat_21);
	M_print(_Dia_->trans_leftmatrix);
	M_print(_Dia_->Diatri_matrix);
	M_print(_Dia_->trans_rightmatrix);
	// �������� 
	Matirx* _mat_inv = M_Inverse(mat_21);
	M_print(_mat_inv );
	// ���н���
	M_Swap(_mat_inv,1,2,_ROW_);
	M_print(_mat_inv); 
	// �и��
	Matirx* _mat_cut = M_Cut(_mat_inv,_END_,_END_,2,3);
	M_print(_mat_cut);
	// ת��
	Matirx* _mat_T = M_T(_mat_inv);
	M_print(_mat_T);
	// ��
	MATRIX_TYPE _tr_mat = M_tr(_mat_inv);
	printf("Tr(Matrix_%x) = %.4lf\n",_mat_inv,_tr_mat);
	// ����ʽ
	MATRIX_TYPE _det_mat = M_det(_mat_inv);
	printf("Det(Matrix_%x) = %.4lf\n",mat_21,_det_mat);
	// ���
	Matirx* mat_full = M_full(mat_2,1,1,1,1,0);
	M_print(mat_full);
// Application
	// �����Է���
		// mat_A*x = mat_b
	printf("#Solver:mat_A*x = mat_b\n");
	Matirx* _mat_result = M_mul(M_Inverse(mat_A10),mat_b10);
	M_print(_mat_result);
	
//  Others
	M_free(_mat_T);

// Help ����
	help("help"); 
	help("Update"); 

	system("pause"); 
	return 0;
}
```

ATTENTION
---------------------------------------
Please feel free to contact with me for any questions, thank you!

Don't spread the files without permission!

�����ļ�������ѧϰ������

δ���������𴫲���

***************************************