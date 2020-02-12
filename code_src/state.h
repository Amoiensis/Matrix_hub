#define MATRIX_TYPE double
#define TRANS_TYPE  double 
#define PRECISION "%.6lf\t"

#define _ROW_    1
#define _COLUMN_ 0 

#define _END_	-1
#define _HEAD_	1

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
