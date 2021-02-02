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
#include <math.h> 
#include "state.h"


typedef struct _Matrix {
	/*Store Matrix
	存储矩阵*/
	int row;
	int column;
	MATRIX_TYPE * data;
} Matirx;

typedef struct _Elementary_Transformation{
	/*Store the Operation of Elementary_Transformation
	存储初等变化的运算过程*/
	int minuend_line;
	int subtractor_line;
	TRANS_TYPE scale;
	struct _Elementary_Transformation* forward_E_trans;
	struct _Elementary_Transformation* next_E_trans;
}Etrans_struct;

typedef struct _Upper_triangular_transformation{
	/*Store the result of Upper_triangular_transformation
	存储上三角化的运算结果*/
	Matirx* trans_matrix;
	Matirx* Uptri_matrix;
}Uptri_struct;

typedef struct _Lower_triangular_transformation{
	/*Store the result of Upper_triangular_transformation
	存储下三角化的运算结果*/
	Matirx* trans_matrix;
	Matirx* Lowtri_matrix;
}Lowtri_struct;

typedef struct _Diagonalization_transformation{
	/*Store the result of Upper_triangular_transformation
	存储对角化化的运算结果*/
	Matirx* trans_leftmatrix;
	Matirx* Diatri_matrix;
	Matirx* trans_rightmatrix;
}Dia_struct;

typedef struct _matrix_inverse_struct{
	/*Store the result of matrix_inverse
	存储求逆运算的中间结果，提高算法效率*/
	Matirx* _matrix;
	struct _Elementary_Transformation* _Etrans_head;
}M_inv_struct;



Matirx* Matrix_gen(int row, int column, MATRIX_TYPE* data);
Matirx* Matrix_copy(Matirx* _mat_sourse); 
Matirx* M_mul(Matirx* _mat_left,Matirx* _mat_right);
Matirx* M_add_sub(MATRIX_TYPE scale_mat_subed,Matirx* _mat_subed,MATRIX_TYPE scale_mat_minus,Matirx* _mat_minus);
int M_print(Matirx* _mat);
Matirx* M_I(int order);
int M_E_trans(Matirx* _mat,Etrans_struct* _Etrans_,int line_setting);
Matirx* Etrans_2_Matrix(Etrans_struct* _Etrans_,int order,int line_setting);
Matirx* Etrans_2_Inverse(Etrans_struct* _Etrans_,int order,int line_setting);
Uptri_struct* M_Uptri_(Matirx* _mat_source);
Uptri_struct* M_Uptri_4inv (Matirx* _mat_source);
Lowtri_struct* M_Lowtri_(Matirx* _mat_source);
Lowtri_struct* M_Lowtri_4inv (Matirx* _mat_source);
Matirx* M_Dia_Inv(Matirx* _mat_source);
Dia_struct* M_Diatri_(Matirx* _mat_source);
Matirx* M_Inverse(Matirx* _mat);
int M_Swap(Matirx* _mat,int _line_1,int _line_2,int line_setting);
Matirx* M_Cut(Matirx* _mat,int row_head,int row_tail,int column_head,int column_tail); 
Matirx* M_T(Matirx* _mat_source);
int M_free(Matirx* _mat);
MATRIX_TYPE M_tr(Matirx* _mat);
MATRIX_TYPE M_det(Matirx* _mat);
Matirx* M_full(Matirx* _mat,int row_up,int row_down,int column_left,int column_right,MATRIX_TYPE full_data);
MATRIX_TYPE M_norm(Matirx* _mat, int Setting);
Matirx* M_numul(Matirx* _mat,MATRIX_TYPE _num);
Matirx* M_matFull(Matirx* _mat,int row_up,int column_left,Matirx* _mat_full);
Matirx* M_Zeros(int row, int column);
Matirx* M_Ones(int row, int column);
Matirx* M_find(Matirx* _mat, MATRIX_TYPE value);
Matirx* M_sum(Matirx* _mat);
int Min_position(MATRIX_TYPE* data,int size);
Matirx* M_min(Matirx* _mat);
Matirx* M_max(Matirx* _mat);
Matirx* M_minax_val(Matirx* _mat, Matirx* _mat_position);
Matirx* M_logic_equal(Matirx* _mat,MATRIX_TYPE value);
Matirx* M_logic(Matirx* _mat_left, Matirx* _mat_right,int Operation);
Matirx* M_pmuldiv(Matirx* _mat_left, Matirx* _mat_right,int operation);
Matirx* M_setval(Matirx* _mat_ini, Matirx* _mat_val, Matirx* _mat_order, int model);
 
Matirx* Matrix_gen(int row,int column,MATRIX_TYPE* data) {/*Generate Matrix Struct
	导入_生成矩阵*/
	Matirx* _mat = (Matirx*)malloc(sizeof(Matirx));
	_mat->row = row;
	_mat->column = column;
	int size = _mat->row*_mat->column;
	_mat->data = (MATRIX_TYPE*)malloc((size) * sizeof(MATRIX_TYPE));
	int i;
	for (i = 0; i<size; i++) {
		_mat->data[i] = data[i];
	}
	return _mat;
}

Matirx* Matrix_copy(Matirx* _mat_sourse){/*Copy Mtrix(gen new one)
	复制矩阵（生成新矩阵）*/ 
	Matirx* _mat_copy = Matrix_gen(_mat_sourse->row,_mat_sourse->column,_mat_sourse->data);
	return _mat_copy;
}

Matirx* M_add_sub(MATRIX_TYPE scale_mat_subed,Matirx* _mat_subed,MATRIX_TYPE scale_mat_minus,Matirx* _mat_minus){/*Add & Sub
	矩阵加减法*/ 
	Matirx* _mat_result = NULL;
	if((_mat_subed->column == _mat_minus->column)&&(_mat_subed->row == _mat_minus->row)){
		_mat_result = Matrix_copy(_mat_subed);
		int size = (_mat_subed->row)*(_mat_subed->column),i;
		for (i=0;i<size;i++){
			_mat_result->data[i] = (_mat_result->data[i])*scale_mat_subed - (_mat_minus->data[i])*scale_mat_minus;
		}
	}else{
		printf(M_add_sub_003);
	}
	return _mat_result;
}

Matirx* M_mul(Matirx* _mat_left,Matirx* _mat_right){/*Matrix Multiply
	矩阵乘法 
	_mat_result = _mat_left*_mat_right */
	printf(">>Matrix_%x * Matrix_%x =\n",_mat_left,_mat_right);
	/*Determine_Matrix_Dimensions*/
	Matirx* _mat_result = NULL;
	if (_mat_left->column != _mat_right->row){
		printf(M_mul_001);
	}else{
		_mat_result = (Matirx*)malloc(sizeof(Matirx));
		int row = _mat_left->row;
		int mid = _mat_left->column;
		int column = _mat_right->column;
		int i,j,k;
		MATRIX_TYPE* _data = (MATRIX_TYPE*)malloc((row*column)*sizeof(MATRIX_TYPE));
		MATRIX_TYPE temp = 0;
		/*Ergodic*/
		for(i=0;i<row;i++){
			for(j=0;j<column;j++){
				/*Caculate Element*/
				temp = 0;
				for (k=0;k<mid;k++){
					temp += (_mat_left->data[i*mid+k])*(_mat_right->data[k*column+j]);
				}
				_data[i*column+j] = temp;
			}
		}
		_mat_result->row = row;
		_mat_result->column = column;
		_mat_result->data = _data;
	}
	printf("\tMatrix_%x\n",_mat_result);
	return _mat_result;	
}

int M_print(Matirx* _mat){/*Print Matrix
	打印矩阵*/
	printf(">>Matrix_%x:\n",_mat);
	int i,j;
	for (i=0;i<_mat->row;i++){
		for(j=0;j<_mat->column;j++){
			printf(PRECISION,_mat->data[i*(_mat->column)+j]);
		}
		printf("\n");
	}
	return 0;
}

Matirx* M_I(int order){/*Generate I _matrix
	生成单位矩阵*/ 
	Matirx* I_mat = (Matirx*)malloc(sizeof(Matirx));
	I_mat->column = order;
	I_mat->row = order;
	int size = order*order,i;
	MATRIX_TYPE* data = (MATRIX_TYPE*)malloc((size) * sizeof(MATRIX_TYPE));
	for(i=0;i<size;i++){
		data[i] = 0;
	}
	for(i=0;i<order;i++){
		data[i*(order+1)] = 1;
	}
	I_mat->data = data;
	return I_mat;
}

int M_E_trans(Matirx* _mat,Etrans_struct* _Etrans_,int line_setting){/*Element teransfor Matrix
	对矩阵初等变换*/ 
/*lin3_sstting 设置是行初等变换还是列初等变换*/
	int line_num,i;
	if (line_setting == _ROW_){
		/*行初等变换*/
		line_num = _mat->column;
		if (_Etrans_->scale){
			for(i=0;i<line_num;i++){
				_mat->data[(_Etrans_->minuend_line-1)*(_mat->column)+i] -= (_Etrans_->scale)*(_mat->data[(_Etrans_->subtractor_line-1)*(_mat->column)+i]);

			}
		}else{
			if((_Etrans_->minuend_line <0)&&(_Etrans_->subtractor_line<0)){/*交换*/
				M_Swap(_mat,-(_Etrans_->minuend_line),-(_Etrans_->subtractor_line),line_setting);
			}
		}
	}else{
		/*列初等变换*/ 
		line_num = _mat->row;
		if (_Etrans_->scale){
			for(i=0;i<line_num;i++){			
			_mat->data[(_Etrans_->minuend_line-1)+(_mat->column)*i] -= (_Etrans_->scale)*(_mat->data[(_Etrans_->subtractor_line-1)+(_mat->column)*i]);
			}
		}else{
			if((_Etrans_->minuend_line <0)&&(_Etrans_->subtractor_line<0)){/*交换*/ 
				M_Swap(_mat,-(_Etrans_->minuend_line),-(_Etrans_->subtractor_line),line_setting);
			}
		}
	}
	return 0; 
}

Matirx* Etrans_2_Inverse(Etrans_struct* _Etrans_,int order,int line_setting){/*Inverse_Element_trans_to_Matrix
	基本变换转矩阵的转置*/ 	
//	if (_mat_sourse == NULL){
		Matirx* _mat_result = M_I(order);
		Etrans_struct* temp_Etrans = _Etrans_;
		do{
			M_E_trans(_mat_result,temp_Etrans,line_setting);
			temp_Etrans = temp_Etrans->next_E_trans;
		}while(temp_Etrans!=NULL);
		return _mat_result;
//	}else{
//		Matirx* _mat_result = _mat_sourse;//M_I(order);
//		Etrans_struct* temp_Etrans = _Etrans_;
//		do{
//			M_E_trans(_mat_result,temp_Etrans,line_setting);
//			temp_Etrans = temp_Etrans->next_E_trans;
//		}while(temp_Etrans!=NULL);
//		return _mat_result;
//	}
}

Matirx* Etrans_2_Matrix(Etrans_struct* _Etrans_,int order,int line_setting){/*Element_trans_to_Matrix
	基本变换转矩阵*/ 
	Etrans_struct* temp_Etrans = _Etrans_;
	Matirx* _mat_result = M_I(order);
	if (_Etrans_ != NULL){
		while ((temp_Etrans->next_E_trans)!=NULL){
			temp_Etrans = temp_Etrans->next_E_trans;
		}
		//M_print(_mat_result);
		do{
			temp_Etrans->scale = (-1)*temp_Etrans->scale;
			M_E_trans(_mat_result,temp_Etrans,line_setting);
			temp_Etrans->scale = (-1)*temp_Etrans->scale;
			temp_Etrans = temp_Etrans->forward_E_trans;
			//M_print(_mat_result);
		}while(temp_Etrans!=NULL);
	}
	return _mat_result;
}

Uptri_struct* M_Uptri_(Matirx* _mat_source){/*Upper_triangular_transformation 
	上三角化*/ 
	Matirx* _mat = Matrix_copy(_mat_source);
	int i,j,k,flag;
	Etrans_struct* _Etrans_temp_last = NULL;
	Etrans_struct*_Etrans_temp_head = NULL;
	
	/*初等变换*/
	for(i=0;i<_mat->column;i++){
		for(j=i+1;j<_mat->row;j++){
			flag = 0;
			Etrans_struct* _Etrans_temp = (Etrans_struct*)malloc(sizeof(Etrans_struct));
			_Etrans_temp->minuend_line = j+1;
			_Etrans_temp->subtractor_line = i+1;
			if ((_mat->data[(_mat->column)*i+i])!= 0){
				_Etrans_temp->scale = (_mat->data[(_mat->column)*j+i])/(_mat->data[(_mat->column)*i+i]);			
			}else{
				_Etrans_temp->scale = 0;
				for(k=i+1;k<_mat->row;k++){
					flag = 1;//无可替代行 
					if((_mat->data[(_mat->column)*k+i])!=0){
						_Etrans_temp->minuend_line = -(i+1);
						_Etrans_temp->subtractor_line = -(k+1);
						flag = 2;//表示能够替换行 
						break;
					}
				}
				if(flag == 1){
					break;
				}			
			}
			_Etrans_temp->forward_E_trans = NULL;
			_Etrans_temp->next_E_trans = NULL;
			if(_Etrans_temp_head ==NULL){
				_Etrans_temp_head = _Etrans_temp;
				_Etrans_temp->forward_E_trans = NULL;
			}else{
				_Etrans_temp->forward_E_trans = _Etrans_temp_last;
		
			}
			if ((i+1)==_mat->column){
					_Etrans_temp->next_E_trans = NULL;
			}else{
				if (_Etrans_temp_last != NULL){
					_Etrans_temp_last->next_E_trans = _Etrans_temp;
				}				
			}		
			M_E_trans(_mat,_Etrans_temp,_ROW_);
			//M_print(_mat); //显示具体矩阵 
			_Etrans_temp_last = _Etrans_temp;
			
			if(flag==2){
					i = i-1;
					break;
			} 		
			}
		}
	Matirx* trans_mat = Etrans_2_Matrix(_Etrans_temp_head,_mat->row,_ROW_);
	Uptri_struct* _Uptri = (Uptri_struct*)malloc(sizeof(Uptri_struct));
	_Uptri->trans_matrix = trans_mat;
	_Uptri->Uptri_matrix = _mat;
	printf(">>Uptri(Matrix_%x)=\n",_mat_source);
	printf("\tMatrix_%x * Matrix_%x\n",trans_mat,_mat);
	return _Uptri;
}

Uptri_struct* M_Uptri_4inv (Matirx* _mat_source){/*Upper_triangular_transformation_for_Inverse 
	上三角化_求逆使用*/ 
	Matirx* _mat = Matrix_copy(_mat_source);
	int i,j,k,flag;
	Etrans_struct* _Etrans_temp_last = NULL;
	Etrans_struct*_Etrans_temp_head = NULL;
	
	/*初等变换*/
	for(i=0;i<_mat->column;i++){
		for(j=i+1;j<_mat->row;j++){
			flag = 0;
			Etrans_struct* _Etrans_temp = (Etrans_struct*)malloc(sizeof(Etrans_struct));
			_Etrans_temp->minuend_line = j+1;
			_Etrans_temp->subtractor_line = i+1;
			if ((_mat->data[(_mat->column)*i+i])!= 0){
				_Etrans_temp->scale = (_mat->data[(_mat->column)*j+i])/(_mat->data[(_mat->column)*i+i]);			
			}else{
				_Etrans_temp->scale = 0;
				for(k=i+1;k<_mat->row;k++){
					flag = 1;//无可替代行 
					if((_mat->data[(_mat->column)*k+i])!=0){
						_Etrans_temp->minuend_line = -(i+1);
						_Etrans_temp->subtractor_line = -(k+1);
						flag = 2;//表示能够替换行 
						break;
					}
				}
				if(flag == 1){
					break;
				}			
			}
			_Etrans_temp->forward_E_trans = NULL;
			_Etrans_temp->next_E_trans = NULL;
			//if (j==1){
			if(_Etrans_temp_head ==NULL){
				_Etrans_temp_head = _Etrans_temp;
				_Etrans_temp->forward_E_trans = NULL;
			}else{
				_Etrans_temp->forward_E_trans = _Etrans_temp_last;
		
			}
			if ((i+1)==_mat->column){
					_Etrans_temp->next_E_trans = NULL;
			}else{
				if (_Etrans_temp_last != NULL){
					_Etrans_temp_last->next_E_trans = _Etrans_temp;
				}				
			}		
			M_E_trans(_mat,_Etrans_temp,_ROW_);
			//M_print(_mat); //显示具体矩阵 
			_Etrans_temp_last = _Etrans_temp;
			
			if(flag==2){
					i = i-1;
					break;
			} 		
		}
	}
	Matirx* trans_mat = Etrans_2_Inverse(_Etrans_temp_head,_mat->row,_ROW_);
	Uptri_struct* _Uptri = (Uptri_struct*)malloc(sizeof(Uptri_struct));
	_Uptri->trans_matrix = trans_mat;
	_Uptri->Uptri_matrix = _mat;
//	M_inv_struct* _Uptri =  (M_inv_struct*)malloc(sizeof(M_inv_struct));
//	 _Uptri->_matrix = _mat;
//	 _Uptri->_Etrans_head = _Etrans_temp_head;
//	printf(">>Uptri(Matrix_%x)=\n",_mat_source);
//	printf("\tMatrix_%x * Matrix_%x\n",_mat,trans_mat);
	return _Uptri;
}

Lowtri_struct* M_Lowtri_(Matirx* _mat_source){/*Lower_triangular_transformation
	下三角化*/ 
	Matirx* _mat = Matrix_copy(_mat_source);
	int i,j,k,flag;
	Etrans_struct* _Etrans_temp_last = NULL;
	Etrans_struct*_Etrans_temp_head = NULL;
	for(i=0;i<_mat->row;i++){
		for(j=i+1;j<_mat->column;j++){
			flag = 0;
			Etrans_struct* _Etrans_temp = (Etrans_struct*)malloc(sizeof(Etrans_struct));
			_Etrans_temp->minuend_line = j+1;
			_Etrans_temp->subtractor_line = i+1;
			
			
			if ((_mat->data[(_mat->column)*i+i])!= 0){
				_Etrans_temp->scale = (_mat->data[(_mat->column)*i+j])/(_mat->data[(_mat->column)*i+i]);;			
			}else{
				_Etrans_temp->scale = 0;
				for(k=i+1;k<_mat->row;k++){
					flag = 1;//无可替代行 
					if((_mat->data[(_mat->column)*k+i])!=0){
						_Etrans_temp->minuend_line = -(i+1);
						_Etrans_temp->subtractor_line = -(k+1);
						flag = 2;//表示能够替换行 
						break;
					}
				}
				if(flag == 1){
					break;
				}			
			}
			
			_Etrans_temp->forward_E_trans = NULL;
			_Etrans_temp->next_E_trans = NULL;
			if (_Etrans_temp_head ==NULL){
				_Etrans_temp_head = _Etrans_temp;
				_Etrans_temp->forward_E_trans = NULL;
			}else{
				_Etrans_temp->forward_E_trans = _Etrans_temp_last;
			}
			if ((i+1)==_mat->column){
					_Etrans_temp->next_E_trans = NULL;
			}else{
				if (_Etrans_temp_last != NULL){
					_Etrans_temp_last->next_E_trans = _Etrans_temp;
				}				
			}			
			M_E_trans(_mat,_Etrans_temp,_COLUMN_);
			M_print(_mat); //显示具体矩阵 
			_Etrans_temp_last = _Etrans_temp;
			if(flag==2){
					i = i-1;
					break;
			} 		
		}
	}
	Matirx* trans_mat = Etrans_2_Matrix(_Etrans_temp_head,_mat->row,_COLUMN_);
	Lowtri_struct* _Lowtri = (Lowtri_struct*)malloc(sizeof(Lowtri_struct));
	_Lowtri->trans_matrix = trans_mat;
	_Lowtri->Lowtri_matrix = _mat;
	printf(">>Lowtri(Matrix_%x)=\n",_mat_source);
	printf("\tMatrix_%x * Matrix_%x\n",_mat,trans_mat);
	return _Lowtri;
}

Lowtri_struct*  M_Lowtri_4inv (Matirx* _mat_source){/*_Lower_triangular_transformation_for_Inverse 
	下三角化_求逆使用*/ 
	Matirx* _mat = Matrix_copy(_mat_source);
	int i,j,k,flag;
	Etrans_struct* _Etrans_temp_last = NULL;
	Etrans_struct*_Etrans_temp_head = NULL;
	for(i=0;i<_mat->row;i++){
		for(j=i+1;j<_mat->column;j++){
			flag = 0;
			Etrans_struct* _Etrans_temp = (Etrans_struct*)malloc(sizeof(Etrans_struct));
			_Etrans_temp->minuend_line = j+1;
			_Etrans_temp->subtractor_line = i+1;
			
			
			if ((_mat->data[(_mat->column)*i+i])!= 0){
				_Etrans_temp->scale = (_mat->data[(_mat->column)*i+j])/(_mat->data[(_mat->column)*i+i]);;			
			}else{
				_Etrans_temp->scale = 0;
				for(k=i+1;k<_mat->row;k++){
					flag = 1;//无可替代行 
					if((_mat->data[(_mat->column)*k+i])!=0){
						_Etrans_temp->minuend_line = -(i+1);
						_Etrans_temp->subtractor_line = -(k+1);
						flag = 2;//表示能够替换行 
						break;
					}
				}
				if(flag == 1){
					break;
				}			
			}
			
			_Etrans_temp->forward_E_trans = NULL;
			_Etrans_temp->next_E_trans = NULL;
			if (_Etrans_temp_head ==NULL){
				_Etrans_temp_head = _Etrans_temp;
				_Etrans_temp->forward_E_trans = NULL;
			}else{
				_Etrans_temp->forward_E_trans = _Etrans_temp_last;
			}
			if ((i+1)==_mat->column){
					_Etrans_temp->next_E_trans = NULL;
			}else{
				if (_Etrans_temp_last != NULL){
					_Etrans_temp_last->next_E_trans = _Etrans_temp;
				}				
			}			
			M_E_trans(_mat,_Etrans_temp,_COLUMN_);
			//M_print(_mat); //显示具体矩阵 
			_Etrans_temp_last = _Etrans_temp;
			if(flag==2){
					i = i-1;
					break;
			} 		
		}
	}
	Matirx* trans_mat = Etrans_2_Inverse(_Etrans_temp_head,_mat->row,_COLUMN_);
	Lowtri_struct* _Lowtri = (Lowtri_struct*)malloc(sizeof(Lowtri_struct));
	_Lowtri->trans_matrix = trans_mat;
	_Lowtri->Lowtri_matrix = _mat;
		
//	printf(">>Lowtri(Matrix_%x)=\n",_mat_source);
//	printf("\tMatrix_%x * Matrix_%x\n",_mat,trans_mat);
	return _Lowtri;
}

Matirx* M_Dia_Inv(Matirx* _mat_source){/*M_Inv for Dia_matrix
	对角矩阵求逆*/ 
	Matirx* _mat_inv = NULL;
	if(_mat_source->row != _mat_source->column){
		printf(M_Dia_Inv_002);
	}else{
		_mat_inv = Matrix_copy(_mat_source);
		MATRIX_TYPE* data = _mat_inv->data;
		int i,order = _mat_source->column;
		for(i=0;i<order;i++){
			(data)[i*(order+1)] = 1/(data[i*(order+1)]);
		}
	}
	return _mat_inv;
}

Dia_struct* M_Diatri_(Matirx* _mat_source){/*Diagonalization
	对角化*/ 
	Dia_struct* _Dia_instance = (Dia_struct*)malloc(sizeof(Dia_struct));
	Uptri_struct* _Uptri_ =  M_Uptri_(_mat_source);
	Lowtri_struct* _Lowtri_ =  M_Lowtri_(_Uptri_->Uptri_matrix);
	_Dia_instance->trans_leftmatrix = _Uptri_->trans_matrix;
	_Dia_instance->Diatri_matrix = _Lowtri_->Lowtri_matrix;
	_Dia_instance->trans_rightmatrix = _Lowtri_->trans_matrix;
	printf(">>Diag(Matrix_%x)=\n",_mat_source);
	printf("\tMatrixdl%x * Matrix_d%x * Matrixr_%x\n",_Uptri_->trans_matrix,_Lowtri_->Lowtri_matrix,_Lowtri_->trans_matrix);
	return _Dia_instance;
}

Matirx* M_Inverse(Matirx* _mat){/*Matrix_Inverse
	矩阵求逆*/ 
	Uptri_struct* _Uptri_  =  M_Uptri_4inv(_mat);
	Lowtri_struct* _Lowtri_ =  M_Lowtri_4inv(_Uptri_->Uptri_matrix);
	Matirx* _mat_dia_inv = M_Dia_Inv(_Lowtri_->Lowtri_matrix);
	Matirx* _mat_inv = M_mul(M_mul(_Lowtri_->trans_matrix,_mat_dia_inv),_Uptri_->trans_matrix);
	printf(">>Inv(Matrix_%x)=\n",_mat);
	printf("\tMatrix_inv_%x\n",_mat_inv);
	return _mat_inv;
}

int M_Swap(Matirx* _mat,int _line_1,int _line_2,int line_setting){/*Swap Line
	交换指定行和列*/
	_line_1 = _line_1-1;
	_line_2 = _line_2-1;
	int i;
	MATRIX_TYPE temp;
	if (line_setting == _ROW_){
		if((_line_1<_mat->row)&&(_line_2<_mat->row)){
			for (i=0;i<(_mat->column);i++){
				temp = _mat->data[_line_1*(_mat->column)+i];
				_mat->data[_line_1*(_mat->column)+i] = _mat->data[_line_2*(_mat->column)+i];
				_mat->data[_line_2*(_mat->column)+i] = temp;
				}	
			}else{
				printf(M_swap_004);
				system("pause");
			}				
	}else{
		if((_line_1<_mat->column)&&(_line_2<_mat->column)){
			for (i=0;i<(_mat->row);i++){
				temp = _mat->data[_line_1+(_mat->column)*i];
				_mat->data[_line_1+(_mat->column)*i] = _mat->data[_line_2+(_mat->column)*i];
				_mat->data[_line_2+(_mat->column)*i] = temp;
			}				
		}else{
			printf(M_swap_004);
				system("pause");
		}
	}
	return 0;
}

Matirx* M_Cut(Matirx* _mat,int row_head,int row_tail,int column_head,int column_tail){/*Cut_out_part_of_Matrix
	切取部分矩阵*/
	Matirx* mat_result = NULL;
	if (row_tail<0){
		if (row_tail == _END_){
			row_tail = _mat->row;
		}else{
			printf(M_Cut_007);
			system("pause");			
		}		
	}
	
	if (row_head<0){
		if (row_head == _END_){
			row_head = _mat->row;
		}else{
			printf(M_Cut_007);
			system("pause");			
		}		
	}
	
	if (column_tail<0){
		if (column_tail == _END_){
			column_tail = _mat->column;
		}else{
			printf(M_Cut_007);
			system("pause");			
		}		
	}
	
	if (column_head<0){
		if (column_head == _END_){
			column_head = _mat->column;
		}else{
			printf(M_Cut_007);
			system("pause");			
		}		
	}
	
	if ((row_tail>_mat->row)||(column_tail>_mat->column)){
		printf(M_Cut_005);
		system("pause");
	}else{
		if ((row_head>row_tail)||(column_head>column_tail)){
			printf(M_Cut_006);
			system("pause");
		}else{
				row_head = row_head -1;
				column_head = column_head-1;
				mat_result = (Matirx*)malloc(sizeof(Matirx));
				mat_result->row = row_tail-row_head;
				mat_result->column = column_tail-column_head;
				mat_result->data = (MATRIX_TYPE*)malloc(sizeof(MATRIX_TYPE)*(mat_result->row)*(mat_result->column));
				int i,j;
				for(i=0;i<(row_tail-row_head);i++){
					for(j=0;j<(column_tail-column_head);j++){
						mat_result->data[i*(mat_result->column)+j] = _mat->data[(i+row_head)*(_mat->column)+(j+column_head)];
					}
				}
		}	
	}
	return mat_result;
}

Matirx* M_T(Matirx* _mat_source){/*Transpose
	转置*/ 
	Matirx* _mat = (Matirx*)malloc(sizeof(Matirx));
	_mat->column = _mat_source->row;
	_mat->row = _mat_source->column ;
	MATRIX_TYPE* data = (MATRIX_TYPE*)malloc(sizeof(MATRIX_TYPE)*(_mat->column)*(_mat->row));
	_mat->data = data;
	int i,j;
	for (i=0;i<(_mat->row);i++){
		for(j=0;j<_mat->column;j++){
			data[i*(_mat->column)+j] = _mat_source->data[j*(_mat_source->column)+i];
		}
	}
	return _mat;
}

int M_free(Matirx* _mat){
	free(_mat->data);
	printf(">>Matrix_%x has been freed.\n",_mat);
	free(_mat);
	return 0;
}

MATRIX_TYPE M_tr(Matirx* _mat){/*Trace
	矩阵的迹*/
	MATRIX_TYPE _tr_mat = 0;
	if (_mat->column == _mat->row){
		int i;
		for (i=0;i<_mat->column;i++){
			_tr_mat += _mat->data[i*(_mat->column+1)];
		}
	}else{
		printf(M_tr_008);
		system("poause");
	}
	return _tr_mat;
}

MATRIX_TYPE M_det(Matirx* _mat_){/*Determinant
	行列式*/
	MATRIX_TYPE _det_mat = 0;
	if (_mat_->column == _mat_->row){
		Uptri_struct* _Uptri_ =  M_Uptri_(_mat_);
		Matirx* _mat = _Uptri_->Uptri_matrix;
		_det_mat = 1;
		int i;
		for (i=0;i<_mat->column;i++){
			_det_mat *= _mat->data[i*(_mat->column+1)];
		}
	}else{
		printf(M_det_009);
		system("poause");
	}
	return 	_det_mat;
} 

Matirx* M_full(Matirx* _mat,int row_up,int row_down,int column_left,int column_right,MATRIX_TYPE full_data){/*Full 
	填充矩阵*/
	Matirx* mat_result = NULL;
	mat_result = (Matirx*)malloc(sizeof(Matirx));
	mat_result->row = (_mat->row+row_up+row_down);
	mat_result->column = (_mat->column+column_left+column_right);
	mat_result->data = (MATRIX_TYPE*)malloc(sizeof(MATRIX_TYPE)*(mat_result->row)*(mat_result->column));
	int i,j;
	for(i=0;i<mat_result->row;i++){
		for(j=0;j<mat_result->column;j++){
			if((i>=row_up)&&(i<(row_up+_mat->row))){/*这里的双判断，可以优化*/
				if((j>=column_left)&&(j<(column_left+_mat->column))){
					mat_result->data[i*(mat_result->column)+j] = _mat->data[(_mat->column)*(i-row_up)+(j-column_left)];
				}else{
					mat_result->data[i*(mat_result->column)+j] = full_data;
				}
			}else{
				mat_result->data[i*(mat_result->column)+j] = full_data;
			}
		}
	}
	printf(">>Full<U%d,D%d,L%d,R%d>(Matrix_%x)=\n",row_up,row_down,column_left,column_right,_mat);
	printf("\tMatrix_%x\n",mat_result);
	return mat_result;
}

MATRIX_TYPE M_norm(Matirx* _mat, int Setting) {/*Caculate Matrix norm-num 
	向量/矩阵范数 Setting=1 - 1范数，Setting=2 - 2范数*/
	MATRIX_TYPE* data = _mat->data;
	int row = _mat->row;
	int column = _mat->column;
	MATRIX_TYPE Val_norm = 0;
	int i,j;
	if (Setting == 2){
		for (i = 0; i<row; i++) {
			for (j = 0; j<column; j++){
				Val_norm += data[i*(column)+j]*data[i*(column)+j];
			}
		}
	}else{
		for (i = 0; i<row; i++) {
			for (j = 0; j<column; j++){
				Val_norm += abs(data[i*(column)+j]);
			}
		}
	}
	return Val_norm;
}

Matirx* M_numul(Matirx* _mat,MATRIX_TYPE _num){/*Matrix Multiply
	矩阵数乘 
	_mat_result = _mat_left*_mat_right */
	MATRIX_TYPE* data = _mat->data;
	int Size_mat = (_mat->row)*(_mat->column),i;
	for (i = 0; i<Size_mat; i++) {
			data[i] = data[i]*_num;
	}
	return _mat;	
}

Matirx* M_matFull(Matirx* _mat,int row_up,int column_left,Matirx* _mat_full){/*Full 
	使用矩阵填充矩阵*/
	MATRIX_TYPE* data = _mat->data;
	int row = _mat->row;
	int column = _mat->column;
	int i,j;
	if ((row_up+_mat_full->row)>row){
		printf(M_Mfull_010);
		system("pause");
	}
	if ((column_left+_mat_full->column)>column){
		printf(M_Mfull_011);
		system("pause");
	}
	int full_row = _mat_full->row, full_column = _mat_full->column;
	for(i=0;i<full_row;i++){
		for(j=0;j<full_column;j++){
			data[(row_up+i)*column+(column_left+j)] = _mat_full->data[i*full_column+j];
		}	
	}
	return _mat;
}

Matirx* M_Zeros(int row, int column){/*Generate Zeros _matrix
	生成全零矩阵*/ 
	Matirx* Zero_mat = (Matirx*)malloc(sizeof(Matirx));
	Zero_mat->column = column;
	Zero_mat->row = row;
	int size = row*column,i;
	MATRIX_TYPE* data = (MATRIX_TYPE*)malloc((size) * sizeof(MATRIX_TYPE));
	for(i=0;i<size;i++){
		data[i] = 0;
	}
	Zero_mat->data = data;
	return Zero_mat;
}

Matirx* M_Ones(int row, int column){/*Generate Ones _matrix
	生成全一矩阵*/ 
	Matirx* Zero_mat = (Matirx*)malloc(sizeof(Matirx));
	Zero_mat->row = row;
	Zero_mat->column = column;
	int size = row*column,i;
	MATRIX_TYPE* data = (MATRIX_TYPE*)malloc((size) * sizeof(MATRIX_TYPE));
	for(i=0;i<size;i++){
		data[i] = 1;
	}
	Zero_mat->data = data;
	return Zero_mat;
}

Matirx* M_find(Matirx* _mat, MATRIX_TYPE value){
	int size_mat = (_mat->row)*(_mat->column);
	int* position = (int*)malloc(sizeof(int)*size_mat);
	int num = 0, temp_column, temp_row,i;
	for (i=0;i<size_mat;i++){
		if (_mat->data[i] == value){
			position[num] = i;
			num = num+1;
		}
	}
	MATRIX_TYPE* data = (MATRIX_TYPE*)malloc(sizeof(MATRIX_TYPE)*num);
	for (i=0;i<num;i++){
		temp_column = position[i]%(_mat->column);
		temp_row = (position[i]-temp_column)/(_mat->column);
		data[i] = temp_column*(_mat->row) + temp_row;
	}
	Matirx* mat_result = (Matirx*)malloc(sizeof(Matirx));
	mat_result->row = num;
	mat_result->column = 1;
	mat_result->data = data;
	free(position);
	return mat_result;
};

Matirx* M_sum(Matirx* _mat){
	Matirx* mat_result = (Matirx*)malloc(sizeof(Matirx));
	int row = _mat->row, column = _mat->column;
	int i,j, size = row*column;
	if(row==_ONE_||column==_ONE_){
		MATRIX_TYPE* data = (MATRIX_TYPE*)malloc(sizeof(MATRIX_TYPE));
		data[0] = 0;
		for(i=0;i<(size);i++){
			data[0] = data[0] + _mat->data[i];
		}
		mat_result->row = 1;
		mat_result->column = 1;
		mat_result->data = data;
	}else{
		MATRIX_TYPE* data = (MATRIX_TYPE*)malloc(sizeof(MATRIX_TYPE)*column);
		for(i=0;i<(column);i++){
			data[i] = 0;
			for(j=0;j<(row);j++){
				data[i] = data[i] + _mat->data[j*column+i];
			}
		}
		mat_result->row = 1;
		mat_result->column = column;
		mat_result->data = data;
	}
	return mat_result;
}

Matirx* M_min(Matirx* _mat){
	Matirx* mat_result = (Matirx*)malloc(sizeof(Matirx));
	int row = _mat->row, column = _mat->column;
	int i,j, size = row*column;
	if(row==_ONE_||column==_ONE_){
		MATRIX_TYPE* data = (MATRIX_TYPE*)malloc(sizeof(MATRIX_TYPE));
		data[0] = Min_position(_mat->data,size);
		mat_result->row = 1;
		mat_result->column = 1;
		mat_result->data = data;
	}else{
		MATRIX_TYPE* data = (MATRIX_TYPE*)malloc(sizeof(MATRIX_TYPE)*column);
		MATRIX_TYPE* temp_data = (MATRIX_TYPE*)malloc(sizeof(MATRIX_TYPE)*row);
		for(i=0;i<(column);i++){
			for(j=0;j<(row);j++){
				temp_data[j] = _mat->data[j*column+i];
			}
			data[i] = Min_position(temp_data,row);
		}
		mat_result->row = 1;
		mat_result->column = column;
		mat_result->data = data;
	}
	return mat_result;
}

int Min_position(MATRIX_TYPE* data,int size){
	MATRIX_TYPE Val_min = data[size-1];
	int min_position = size-1, i;
	for (i=size-2;i>=0;i--){
		if (data[i]<=Val_min){
			Val_min = data[i];
			min_position = i;
		}
	}
	return min_position;
}

Matirx* M_max(Matirx* _mat){
	Matirx* _mat_ = Matrix_copy(_mat);
	_mat_ = M_numul(_mat_,-1);
	Matirx* mat_result = M_min(_mat_);
	M_free(_mat_);
	return mat_result;
}

Matirx* M_minax_val(Matirx* _mat, Matirx* _mat_position){
	Matirx* mat_val = (Matirx*)malloc(sizeof(Matirx));
	mat_val->row = _mat_position->row;
	mat_val->column = _mat_position->column;
	int i,temp, size_mat = (_mat_position->row)*(_mat_position->column);
	MATRIX_TYPE* data = (MATRIX_TYPE*)malloc(sizeof(MATRIX_TYPE)*size_mat);
	for (i=0;i<size_mat;i++){
		temp = (_mat_position->data[i]);
		data[i] = _mat->data[temp*(_mat->column)+i];
	}
	mat_val->data = data;
	return mat_val;
}

Matirx* M_logic_equal(Matirx* _mat, MATRIX_TYPE value){
	int size_mat = (_mat->row)*(_mat->column), i;
	Matirx* mat_logic = (Matirx*)malloc(sizeof(Matirx));
	mat_logic->row = (_mat->row);
	mat_logic->column = (_mat->column);
	MATRIX_TYPE* data = (MATRIX_TYPE*)malloc(sizeof(MATRIX_TYPE)*size_mat);
	for (i=0;i<size_mat;i++){
		if(_mat->data[i] == value){
			data[i] = 1;
		}else{
			data[i] = 0;
		}
	}
	mat_logic->data = data;
	return mat_logic;
}

Matirx* M_logic(Matirx* _mat_left, Matirx* _mat_right,int Operation){
	Matirx* mat_logic = (Matirx*)malloc(sizeof(Matirx));
	if (_mat_right){
		if (_mat_left->row != _mat_right->row){
			printf(M_logic_012);
			system("pause");
		}
		if (_mat_left->column != _mat_right->column){
			printf(M_logic_013);
			system("pause");
		}
	}
	
	int size_mat = (_mat_left->row)*(_mat_left->column), i;
	mat_logic->row = (_mat_left->row);
	mat_logic->column = (_mat_left->column);
	MATRIX_TYPE* data = (MATRIX_TYPE*)malloc(sizeof(MATRIX_TYPE)*size_mat);
	switch(Operation){
		case _AND_:
			for (i=0;i<size_mat;i++){
				if ((_mat_left->data[i]==0)||(_mat_right->data[i]==0)){
					data[i] = 0;
				}else{
					data[i] = 1;
				}
			}
			break;
		case _OR_:
			for (i=0;i<size_mat;i++){
				if ((_mat_left->data[i]!=0)||(_mat_right->data[i]!=0)){
					data[i] = 1;
				}else{
					data[i] = 0;
				}
			}
			break;
		case _NOT_:
			for (i=0;i<size_mat;i++){
				if (_mat_left->data[i]==0){
					data[i] = 1;
				}else{
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

Matirx* M_pmuldiv(Matirx* _mat_left, Matirx* _mat_right,int operation){/*Point Mul and Div
	矩阵点乘/点除*/
	Matirx* mat_result = (Matirx*)malloc(sizeof(Matirx));
	if (_mat_left->row != _mat_right->row){
			printf(M_pmuldiv_015);
			system("pause");
		}
	if (_mat_left->column != _mat_right->column){
			printf(M_pmuldiv_016);
			system("pause");
	}
	int i,size_mat = (_mat_left->row)*(_mat_left->column);
	mat_result->row = (_mat_left->row);
	mat_result->column = (_mat_left->column);
	MATRIX_TYPE* data = (MATRIX_TYPE*)malloc(sizeof(MATRIX_TYPE)*size_mat);
	if (operation == _MUL_){
		for (i=0;i<size_mat;i++){
				data[i] = (_mat_left->data[i])*(_mat_right->data[i]);
		}
	}else{
		if (operation == _DIV_){
			for (i=0;i<size_mat;i++){
				if (_mat_right->data[i]!=0){
					data[i] = (_mat_left->data[i])/(_mat_right->data[i]);
				} else{
					data[i] = _INFINITE_;
				} 
				
			}
		}else{
			printf(M_pmuldiv_017);
		}	
	}
	mat_result->data = data;
	return mat_result;
}

Matirx* M_setval(Matirx* _mat_ini, Matirx* _mat_val, Matirx* _mat_order, int model){/*Mat Set Value
使用矩阵传递数据，给矩阵指定位置赋值 
*/
	int i,temp,size_ini = (_mat_ini->column)*(_mat_ini->row);
	int size_val = (_mat_val->column)*(_mat_val->row);
	int size_order = (_mat_order->column)*(_mat_order->row);
	if (model == _ORD4INI_){/*_ORD4INI_*/
		for(i=0;i<size_order;i++){
			if ((_mat_order->data[i])<size_ini){
				if (i<size_val){
					temp = (_mat_order->data[i]);
					_mat_ini->data[temp] = _mat_val->data[i];
				}else{
					printf(M_setval_019);
					system("pause");
				}			
			}else{
				printf(M_setval_018);
				system("pause");
			}
		}
	}else{/*_ORD4VAL_*/
		for(i=0;i<size_ini;i++){
			if ((i)<size_order){
				temp = (_mat_order->data[i]);
				if (temp<size_val){
					_mat_ini->data[i] = _mat_val->data[temp];
				}else{
					printf(M_setval_019);
					system("pause");
				}			
			}else{
				printf(M_setval_020);
				system("pause");
			}
		}
	}
	
	return _mat_ini;
}

