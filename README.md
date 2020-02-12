Matrix_hib
=======================================
矩阵运算库--C语言
---------------------------------------
##The lib of Matrix operation for C language. (矩阵运算库--C语言) 

Author: Amoiensis

Email: Amoiensis@outlook.com

Data:2020.02.08
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