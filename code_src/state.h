#define MATRIX_TYPE double
#define TRANS_TYPE  double 
#define PRECISION "%.2lf\t"

#define _FULLY_BIG_	1000
#define _INFINITE_ 	1000

#define _ROW_    1
#define _COLUMN_ 0 

#define _END_	-1
#define _HEAD_	1

#define _ONE_ 1
#define _ZERO_ 0

#define _AND_ 1
#define _OR_ 0
#define _NOT_ -1

#define _MUL_ 1
#define _DIV_ -1

#define _ORD4INI_ 1
#define _ORD4VAL_ -1

/*ERROR TABLE*/ 
#define M_mul_001	 	"@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_mul_001)_mat_left->column != _mat_right->row\n"
#define M_Dia_Inv_002 	"@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_Dia_Inv_002)_mat_left->column != _mat_right->row\n"
#define M_add_sub_003 	"@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_add_sub_003)_mat_subed != _mat_minus\n"
#define M_swap_004 		"@ERROR: Matrix_Swap_Line Over!\n\tDetails:(M_swap_004)_Swap_line over the limited\n"
#define M_Cut_005 		"@ERROR: Matrix_Cut Over!\n\tDetails:(M_Cut_005)_Cut_tail over_the limited\n"
#define M_Cut_006 		"@ERROR: Matrix_Cut Wrong!\n\tDetails:(M_Cut_006)_Head_>_Tail\n"
#define M_Cut_007 		"@ERROR: Matrix_Cut Wrong!\n\tDetails:(M_Cut_007)_Range_can't_be_negative!'\n"
#define M_tr_008 		"@ERROR: Matrix_trace Wrong!\n\tDetails:(M_tr_008)_ROW_!=_COLUMN.'\n"
#define M_det_009 		"@ERROR: Matrix_Determinant_ Wrong!\n\tDetails:(M_det_009)_ROW_!=_COLUMN.'\n"
#define M_Mfull_010 	"@ERROR: M_matFull Wrong!\n\tDetails:(M_Mfull_010)_ROW_OVER!.'\n"
#define M_Mfull_011 	"@ERROR: M_matFull Wrong!\n\tDetails:(M_Mfull_011)_COLUMN_OVER!.'\n"
#define M_logic_012	 	"@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_logic_012)_mat_left->row != _mat_right->row\n"
#define M_logic_013 	"@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_logic_013)_mat_left->column != _mat_right->column\n"
#define M_logic_014 	"@ERROR: Operation Wrong(Dont Exist)!\n\tDetails:(M_logic_014)Operation_Wrong !\n"
#define M_pmuldiv_015	"@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_pmuldiv_015)_mat_left->row != _mat_right->row\n"
#define M_pmuldiv_016 	"@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_pmuldiv_016)_mat_left->column != _mat_right->column\n"
#define M_pmuldiv_017 	"@ERROR: Operation Wrong(Dont Exist)!\n\tDetails:(M_pmuldiv_017)Operation_Wrong !\n"
#define M_setval_018 	"@ERROR: Mat_order lager than Mat_ini !\n\tDetails:(M_setval_018)Mat_order_Size_Wrong !\n"
#define M_setval_019 	"@ERROR: Mat_order lager than Mat_val !\n\tDetails:(M_setval_019)Mat_order_Size_Wrong !\n"
#define M_setval_020	"@ERROR: Mat_ini lager than Mat_order !\n\tDetails:(M_setval_020)Mat_ini_Size_Wrong !\n"
