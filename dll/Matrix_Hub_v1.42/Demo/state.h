#define MATRIX_TYPE double
#define TRANS_TYPE  double 

typedef struct _Matrix {
	/*Store Matrix
	存储矩阵*/
	int row;
	int column;
	MATRIX_TYPE * data;
} Matrix;

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
	Matrix* trans_matrix;
	Matrix* Uptri_matrix;
}Uptri_struct;

typedef struct _Lower_triangular_transformation{
	/*Store the result of Upper_triangular_transformation
	存储下三角化的运算结果*/
	Matrix* trans_matrix;
	Matrix* Lowtri_matrix;
}Lowtri_struct;

typedef struct _Diagonalization_transformation{
	/*Store the result of Upper_triangular_transformation
	存储对角化化的运算结果*/
	Matrix* trans_leftmatrix;
	Matrix* Diatri_matrix;
	Matrix* trans_rightmatrix;
}Dia_struct;

typedef struct _matrix_inverse_struct{
	/*Store the result of matrix_inverse
	存储求逆运算的中间结果，提高算法效率*/
	Matrix* _matrix;
	struct _Elementary_Transformation* _Etrans_head;
}M_inv_struct;

typedef struct _matrix_eigen_struct_single{
	/*Store the result of matrix_eigen
	存储求特征值运算的结果*/
	Matrix* eigen_matrix;
	MATRIX_TYPE eigen_value;
}M_eigen_struct_temp;

/*dll含有的函数*/
//Matrix* Matrix_gen(int row, int column, MATRIX_TYPE* data);
//Matrix* Matrix_copy(Matrix* _mat_sourse); 
//Matrix* M_mul(Matrix* _mat_left,Matrix* _mat_right);
//Matrix* M_add_sub(MATRIX_TYPE scale_mat_subed,Matrix* _mat_subed,MATRIX_TYPE scale_mat_minus,Matrix* _mat_minus);
//int M_print(Matrix* _mat);
//Matrix* M_I(int order);
//int M_E_trans(Matrix* _mat,Etrans_struct* _Etrans_,int line_setting);
//Matrix* Etrans_2_Matrix(Etrans_struct* _Etrans_,int order,int line_setting);
//Matrix* Etrans_2_Inverse(Etrans_struct* _Etrans_,int order,int line_setting);
//Uptri_struct* M_Uptri_(Matrix* _mat_source);
//Uptri_struct* M_Uptri_4inv (Matrix* _mat_source);
//Lowtri_struct* M_Lowtri_(Matrix* _mat_source);
//Lowtri_struct* M_Lowtri_4inv (Matrix* _mat_source);
//Matrix* M_Dia_Inv(Matrix* _mat_source);
//Dia_struct* M_Diatri_(Matrix* _mat_source);
//Matrix* M_Inverse(Matrix* _mat);
//int M_Swap(Matrix* _mat,int _line_1,int _line_2,int line_setting);
//Matrix* M_Cut(Matrix* _mat,int row_head,int row_tail,int column_head,int column_tail); 
//Matrix* M_T(Matrix* _mat_source);
//int M_free(Matrix* _mat);
//MATRIX_TYPE M_tr(Matrix* _mat);
//MATRIX_TYPE M_det(Matrix* _mat);
//Matrix* M_full(Matrix* _mat,int row_up,int row_down,int column_left,int column_right,MATRIX_TYPE full_data);
//MATRIX_TYPE M_norm(Matrix* _mat, int Setting);
//Matrix* M_abs(Matrix* _mat_origin); 
//Matrix* M_numul(Matrix* _mat,MATRIX_TYPE _num);
//Matrix* M_matFull(Matrix* _mat,int row_up,int column_left,Matrix* _mat_full);
//Matrix* M_Zeros(int row, int column);
//Matrix* M_Ones(int row, int column);
//Matrix* M_find(Matrix* _mat, MATRIX_TYPE value);
//Matrix* M_sum(Matrix* _mat);
//int Min_position(MATRIX_TYPE* data,int size);
//Matrix* M_min(Matrix* _mat);
//Matrix* M_max(Matrix* _mat);
//Matrix* M_minax_val(Matrix* _mat, Matrix* _mat_position);
//Matrix* M_logic_equal(Matrix* _mat,MATRIX_TYPE value);
//Matrix* M_logic(Matrix* _mat_left, Matrix* _mat_right,int Operation);
//Matrix* M_pmuldiv(Matrix* _mat_left, Matrix* _mat_right,int operation);
//Matrix* M_setval(Matrix* _mat_ini, Matrix* _mat_val, Matrix* _mat_order, int model);
//Matrix* M_numul_m (Matrix* _mat,Matrix* _mat_multi);
//M_eigen_struct_temp* M_eigen_max(Matrix* _mat);
//int help(char* file_name);


