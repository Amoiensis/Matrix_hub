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
    DETAILS: The statement-code-file for Matrix_Hub.
    LICENSE: Apache-2.0
*/

/* Num Type of Matrix - 矩阵数值类型*/
#define MATRIX_TYPE double
#define TRANS_TYPE  double
/* Displayed Precision - 输出显示精度*/
#define PRECISION "%.3lf\t"
/* The Level of Details of Output - 显示详细等级*/
#define _DETAILED_ 2
/* 档位: 0/1/2/3 四等级: 0->3 逐渐详细 (default-2)
 * level - 显示详情的函数
 * 0 - M_print （除设定的输出外，不额外显示其他计算细节信息）
 * 1 - M_Uptri_/ M_Lowtri_/ M_Diatri_
 * 2 - M_full/ M_Inverse/ M_eigen_val/ M_rank / M_Uptri_/ M_Lowtri_/ M_Diatri_/ M_print
 * 3 - M_free/ M_mul/ M_full/ M_Inverse/ M_rank/ M_mul / M_Uptri_/ M_Lowtri_/ M_Diatri_/ M_print
 * */
/* 进度条显示 */
#define _progress_bar_ 1


/* Control Constant - 设置的常数*/
#define _FULLY_BIG_    1000
#define _INFINITE_    1000

#define _MAX_HELP_LENGTH_ 30

#define _ROW_    1
#define _COLUMN_ 0

#define _END_    -1
#define _HEAD_    1

#define _ONE_ 1
#define _ZERO_ 0

#define _AND_ 1
#define _OR_ 0
#define _NOT_ -1

#define _MUL_ 1
#define _DIV_ -1

#define _ORD4INI_ 1
#define _ORD4VAL_ -1

#define INF INT_MAX
#define FRO INT_MIN

#define _MAX_LOOP_NUM_ (int)1e+5

/*ERROR TABLE - 错误提示*/
#define M_mul_001        "@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_mul_001)_mat_left->column != _mat_right->row\n"
#define M_Dia_Inv_002    "@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_Dia_Inv_002)_mat_left->column != _mat_right->row\n"
#define M_add_sub_003    "@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_add_sub_003)_mat_subed != _mat_minus\n"
#define M_swap_004        "@ERROR: Matrix_Swap_Line Over!\n\tDetails:(M_swap_004)_Swap_line over the limited\n"
#define M_Cut_005        "@ERROR: Matrix_Cut Over!\n\tDetails:(M_Cut_005)_Cut_tail over_the limited\n"
#define M_Cut_006        "@ERROR: Matrix_Cut Wrong!\n\tDetails:(M_Cut_006)_Head_>_Tail\n"
#define M_Cut_007        "@ERROR: Matrix_Cut Wrong!\n\tDetails:(M_Cut_007)_Range_can't_be_negative!'\n"
#define M_tr_008        "@ERROR: Matrix_trace Wrong!\n\tDetails:(M_tr_008)_ROW_!=_COLUMN.'\n"
#define M_det_009        "@ERROR: Matrix_Determinant_ Wrong!\n\tDetails:(M_det_009)_ROW_!=_COLUMN.'\n"
#define M_Mfull_010    "@ERROR: M_matFull Wrong!\n\tDetails:(M_Mfull_010)_ROW_OVER!.'\n"
#define M_Mfull_011    "@ERROR: M_matFull Wrong!\n\tDetails:(M_Mfull_011)_COLUMN_OVER!.'\n"
#define M_logic_012        "@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_logic_012)_mat_left->row != _mat_right->row\n"
#define M_logic_013    "@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_logic_013)_mat_left->column != _mat_right->column\n"
#define M_logic_014    "@ERROR: Operation Wrong(Dont Exist)!\n\tDetails:(M_logic_014)Operation_Wrong !\n"
#define M_pmuldiv_015    "@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_pmuldiv_015)_mat_left->row != _mat_right->row\n"
#define M_pmuldiv_016    "@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_pmuldiv_016)_mat_left->column != _mat_right->column\n"
#define M_pmuldiv_017    "@ERROR: Operation Wrong(Dont Exist)!\n\tDetails:(M_pmuldiv_017)Operation_Wrong !\n"
#define M_setval_018    "@ERROR: Mat_order lager than Mat_ini !\n\tDetails:(M_setval_018)Mat_order_Size_Wrong !\n"
#define M_setval_019    "@ERROR: Mat_order lager than Mat_val !\n\tDetails:(M_setval_019)Mat_order_Size_Wrong !\n"
#define M_setval_020    "@ERROR: Mat_ini lager than Mat_order !\n\tDetails:(M_setval_020)Mat_ini_Size_Wrong !\n"
#define M_eigen_max_021    "@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_eigen_max_021)Mat->column != Mat->row!\n\t\t(For eigen, the Matrix must be a square matrix!)\n"
#define M_norm_022        "@ERROR: M_norm Wrong!\n\t Details:(M_norm_022)The Norm-Setting should be 1/2/INF/p for Vector and 1/2/INF/FRO for Matrix!\n"
#define M_Dia_Inv_023    "@ERROR: Matrix is not invertible!\n\t Details:(M_Dia_Inv_023)Please Check: Inverse element of Dia == 0! \n"
#define M_cond_024    "@ERROR: M_cond Wrong! \n\t Details:(M_cond_024) Matrix should be Square-Matrix! _mat_left->row != _mat_right->row \n"
#define M_cond_025    "@ERROR: M_cond Wrong!\n\t Details:(M_cond_024) M_cond (mat, int Setting <<-- should be 1/2/INF/FRO) \n"
#define M_eigen_026    "@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_eigen_026)Mat->column != Mat->row!\n\t\t(For eigen, the Matrix must be a square matrix!)\n"
#define M_householder_027    "@ERROR: Matrix_Dimensions Wrong!\n\tDetails:(M_householder_027)Mat->column != Mat->row!\n\t\t(For Matrix_householder, the Matrix must be a square matrix!)\n"



/*WARNING TABLE - 警告提示*/
#define M_norm_warm_01        "@WARNING: ||A||_p = sum((a_ij)^p)^(1/p)\n\tFor matrix's p-normvalue, the result can not be a complex number! (e.g. A+Bi)\n"
