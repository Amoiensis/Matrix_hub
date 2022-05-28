Matrix_hub
=======================================
���������--C����
---------------------------------------
A lib for Matrix Operations in C language. (���������--C����)

Author: Amoiensis (Xiping.Yu)

Email: Amoiensis@outlook.com

Data: 2020.02.12~2022.05.28

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

## [[����˵��]](https://github.com/Amoiensis/Matrix_hub)

#### [Matrix Hub v1.51] 2022.05.28

1. ��������, ������±� M_eigen_max/ householder/ M_householder/ M_QR/ M_eigen_val;
|                ����                |   Func Name   |                     Operation (detailed)                     |
| :--------------------------------: | :-----------: | :----------------------------------------------------------: |
|      �����������ֵ����������      |  M_eigen_max  | The maximum eigenvalue/ eigen-vector of the Matrix (create). |
| ����householder�任, ���ر任����H |  householder  | Householder transformation for  the Vector, return Transformating-Matrix: H (create). |
|        ����householder�任         | M_householder | Householder transformation for  the Matrix, return Transformated-Matrix: H_Mat (create). |
|             ����QR�ֽ�             |     M_QR      |                  QR Decomposition (create).                  |
|             ��������ֵ             |  M_eigen_val  |           The eigenvalues of the Matrix (create).            |
| ��������ֵ, �����Ӧ��������(����) |    M_eigen    |  The eigenvalues and eigen-vectors of the Matrix (create).   |
2. ���� ������̵�"��ʾ��ϸ�ȼ�"(The Level of Details of Output).
   ��λ: 0/1/2/3 �ĵȼ�: 0->3 ����ϸ ( Ĭ������Ϊ 2��)
   **level - ��ʾ����ĺ���**
   0 - M_print �����趨������⣬��������ʾ��������ϸ����Ϣ��
   1 - M_Uptri_/ M_Lowtri_/ M_Diatri_ ;
   2 - M_full/ M_Inverse/ M_eigen_val/ M_rank / M_Uptri_/ M_Lowtri_/ M_Diatri_/ M_print ;
   3 - M_free/ M_mul/ M_full/ M_Inverse/ M_rank/ M_mul / M_Uptri_/ M_Lowtri_/ M_Diatri_/ M_print ;
3. ������������д��ڵ�����(��л@1u2e): �ṹ���ͷ����⡢һά��������;
    �����https://github.com/Amoiensis/Matrix_hub/issues/4
4. ���汾������ڴ���ԣ�Ŀǰ���Ժ������ڴ�й©���⣻
5. ����"README", �ο�"���ܱ�"�� OPERATION ��, �緵��ֵΪ�¿��ٿռ������� "create", �����ṩ�ڴ����Ĳο�,
       ������ο���Ӧ OPERATION ˵�� help("README").
5. ��Matrix_Hub v1.51 ��, ����ʹ�� help("MatrixHub") �鿴��ǰ�汾��.



#### [Matrix Hub v1.50] 2022.04.29

1. ��������, ������������ M_cond (matrix.h);
2. ��һ���˷� v1.44 �ڴ�����, �����ڴ����, ��ʹ�� help("Memory_Manager") �鿴;

   **ע��**�����θ����ڴ���������ƣ����޸�v1.4x�ڴ����⡣



#### [Matrix Hub v1.44] 2022.04.28

1. �������� ��������: M_rank (matrix.h);

2. �������� Etrans_free, ʵ�� M_rank ���ȱ任�ڴ��ͷ�; ��� help("Etrans_free"), (matrix.h)

3. ����ϣ�����ؾ���(��̬����)����: Hilbert (matrix.h);

4. �������󲻿��汨��, Error: M_Dia_Inv_023: "@ERROR: Matrix is not invertible!" (state.h);

5. ���޸� v1.43 ���㲻�ȶ�����;

   **ע��**�����θ����ڴ�ͼ����ٶȶ��õ���ߣ����޸�v1.43�ȶ������⡣



#### [Matrix Hub v1.43] 2021.10.26

1. ���¾��������㷨�����л�������������ٶ�����������"M_Inverse"������

2. �޸����ȱ任���ڴ����⣬���������ڴ�ռ�ü��٣�

3. ɾ����Etrans_2_Inverse��������������Etrans_4_Inverse�����������ڼ��پ������棻

4. ���¡�M_Uptri_4inv������M_Lowtri_4inv�����ڼ��پ������棻

   ~~**ע��**���Ƽ�Ŀǰ��ʹ�� [Matrix Hub v1.42]v1.42�汾�����θ����ڴ�ͼ����ٶȶ��õ���ߣ����Ǵ���һЩ�ȶ������⣬�����޸���~~

#### [Matrix Hub v1.42] 2021.08.06

1. �����������������ֵ����:M_eigen_max()����ʹ��help("M_eigen_max")�鿴����ʹ�ã�
2. ��������ȡ����ֵ���� M_abs()����ʹ��help("M_abs")�鿴����ʹ�ã�
3. ���������;���ĸ��ַ�������M_norm()������1����(1)��2����(2)�������(INF)��F������FRO���ȷ����������˾��������������������,��ʹ��help("M_norm")�鿴����ʹ�ã�

#### [Matrix Hub v1.4] 2021.02.02

1.	���� help() ����������������������ƣ��鿴����ʹ�÷������磬help("help")��help("Matrix_gen")��help("README")��help("Update")����;


2. 	����������M_numul_m ()�������ھ������� ��������ھ�����в��������ж�Ӧ���� ��


3.	��ԭ M_matFull() �����У�����࣬�����ϲ࣬row_up��column_leftȡֵ�ӡ�0������Ϊ��1��_HEAD_����;


4.	����ԭ������"Matirx"����д������Ϊ"Matrix";



[CONTENT](https://github.com/Amoiensis/Matrix_hub)
---------------------------------------

####������-������
|                     ����                      |    Func Name     |                     Operation (detailed)                     |
| :-------------------------------------------: | :--------------: | :----------------------------------------------------------: |
|                   ���ɾ���                    |    Matrix_gen    |                   Generate  a new Matrix.                    |
|                   ���ƾ���                    |   Matrix_copy    |                    Copy  to a new Matrix.                    |
|                     �˷�                      |      M_mul       |    Matrix  multiplication (create new one, abbr. create).    |
|                    �Ӽ���                     |    M_add_sub     |               Addition/  subtraction (create).               |
|                   ������ʾ                    |     M_print      |                       Print,  Display.                       |
|                ��λ����(����)                 |       M_I        |            Generate  a identity Matrix (create).             |
|                 ��������任                  |    M_E_trans     |              Matrix  elementary transformation.              |
|                 �����任����                  | Etrans_2_Matrix  | Transforms  the elementary transformation into Matrix (create). |
|             �����任����������              | Etrans_4_Inverse |   Inverse  Matrix of elementary transformations (create).    |
|                   �����ǻ�                    |     M_Uptri_     | Upper-Triangulation  transformation on the Matrix (create).  |
|               �����ǻ�(������)                |   M_Uptri_4inv   | For  inverse, upper-triangulation transformation on the Matrix (create). |
|                   �����ǻ�                    |    M_Lowtri_     | Lower-Triangulation  transformation on the Matrix (create).  |
|               �����ǻ�(������)                |  M_Lowtri_4inv   | For  inverse , lower-triangulation transformation on the Matrix (create). |
|                 �ԽǾ�������                  |    M_Dia_Inv     |        The  inverse of the diagonal Matrix (create).         |
|                    �Խǻ�                     |    M_Diatri_     |                  Diagonalization  (create).                  |
|                     ����                      |    M_Inverse     |                      Inverse  (create).                      |
|                ������(��)����                 |      M_Swap      |              Swap  row or cloumn of the Matrix.              |
|                     ת��                      |       M_T        |                     Transpose  (create).                     |
|                 ��ȡ���־���                  |      M_Cut       |       Cut out a part-matrix from the Matrix (create).        |
|                   �ͷ��ڴ�                    |      M_free      |           Free  the memory of the Matrix (create).           |
|                      ��                       |       M_tr       |                            Trace.                            |
|                    ����ʽ                     |      M_det       |                         Determinant.                         |
|                     ���                      |      M_full      |             Full  the Matrix with data (create).             |
|                     ����                      |      M_norm      |                  Norm  (1/ 2/ p/ INF/ FRO).                  |
|                  �������ֵ                   |      M_abs       |   Absolute  the value of elements in the Matrix  (create).   |
|                   ��������                    |     M_numul      |               Number  Multiplication (create).               |
|              (ʹ�þ���)������               |    M_matFull     |            Full  the Matrix with another Matrix.             |
|                ����(ȫ)�����                 |     M_Zeros      |            Generation  All-Zeros-Matrix (create).            |
|                ����(ȫ)һ����                 |      M_Ones      |            Generation  All-Ones-Matrix (create).             |
|          Ѱ�Ҿ����Ӧֵλ��(������)           |      M_find      | Find  all the positions with a certain value in the Matrix (create). |
|            ���������/����Ԫ�غ�            |      M_sum       | Matrix  Column-Summation (create). / Vector element Sum (create) . |
|   ��������С��λ��     /������СԪ��λ��    |      M_min       | Minimum-value position for each row in  the Matrix (create) .  / Vector minimum element position (create) . |
|   �����������λ��     /�������Ԫ��λ��    |      M_max       | Maximum-value position for each row in  the Matrix (create)./ Vector Maximum element position (create) . |
|            �������ָ����λ�õ�ֵ             |   M_minax_val    | The value of those given (row) positions for each column in the matrix (create). |
| �����λ�������ֵ�Ƚ�     (���ؾ���,ȡֵ0/1) |  M_logic_equal   | Compare every element /pisition of the  Matrix with certain value (create).     [ More : Return a new Matrix, whose every value is 0/1. ] |
|            �������Ӧλ���߼�����             |     M_logic      | Logical  operation of corresponding positions of two matrices |
|         �����ӦԪ�س�/�� (�������)          |    M_pmuldiv     | Hadamard  Product : Multiply / Divide every element in the two Matrix-s (create). |
|          ����������ֵ(ʹ�þ��󴫵�)           |     M_setval     |       Setting  Values of a Matrix with another Matrix.       |
|          ����Ծ��󣬶Ը��н�������           |    M_numul_m     |    Matrix  Number Multiplication (using matrix transfer)     |
|                   ��������                    |      M_rank      |                            Rank.                             |
|      (����: M_rank) �ͷų��ȱ任�ڴ�ռ�      |   Etrans_free    | (In  Func: M_rank) Free memory for Elementary Transformation. |
|                 ������������                  |      M_cond      |                Conditon  Value of the Matrix.                |
|               ����ϣ�����ؾ���                |     Hilbert      |              Generate  Hilbert Matrix (create).              |
|           �����������ֵ����������            |   M_eigen_max    | The maximum eigenvalue/ eigen-vector of the Matrix (create). |
|      ����householder�任, ���ر任����H       |   householder    | Householder transformation for  the Vector, return Transformating-Matrix: H (create). |
|              ����householder�任              |  M_householder   | Householder transformation for  the Matrix, return Transformated-Matrix: H_Mat (create). |
|                  ����QR�ֽ�                   |       M_QR       |                  QR Decomposition (create).                  |
|                  ��������ֵ                   |   M_eigen_val    |           The eigenvalues of the Matrix (create).            |
|      ��������ֵ, �����Ӧ��������(����)       |     M_eigen      |  The eigenvalues and eigen-vectors of the Matrix (create).   |
|                     ����                      |       help       |                            Help.                             |


Demo.c (Matrix_hub)
---------------------------------------

```
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
    DETAILS: The demo-code for Matrix_Hub.
    LICENSE: Apache-2.0
*/

#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"


int main(int argc, char *argv[]) {

//  Matrix
    //	Mat_1
    ...
    [ �������ֵ���� demo.c ]

//	Operation
    // �˷�
    Matrix *mat_3 = M_mul(mat_2, mat_1);
    M_print(mat_3);
    // �Ӽ���
    Matrix *mat_diff = M_add_sub(1, mat_21, 1, mat_21b);
    M_print(mat_diff);
    // ��λ����
    M_print(M_I(5));
    // �����Ǳ任
    Uptri_struct *_Uptri_ = M_Uptri_(mat_21);
    M_print(_Uptri_->trans_matrix);
    M_print(_Uptri_->Uptri_matrix);
    // �����Ǳ任
    Lowtri_struct *_Lowtri_ = M_Lowtri_(mat_21);
    M_print(_Lowtri_->Lowtri_matrix);
    M_print(_Lowtri_->trans_matrix);
    // �Խǻ�
    Dia_struct *_Dia_ = M_Diatri_(mat_21);
    M_print(_Dia_->trans_leftmatrix);
    M_print(_Dia_->Diatri_matrix);
    M_print(_Dia_->trans_rightmatrix);
    // ��������
    Matrix *_mat_inv = M_Inverse(mat_21);
    M_print(_mat_inv);
    // ���н���
    M_Swap(_mat_inv, 1, 2, _ROW_);
    M_print(_mat_inv);
    // �и��
    Matrix *_mat_cut = M_Cut(_mat_inv, _END_, _END_, 2, 3);
    M_print(_mat_cut);
    // ת��
    Matrix *_mat_T = M_T(_mat_inv);
    M_print(_mat_T);
    // ��
    MATRIX_TYPE _tr_mat = M_tr(_mat_inv);
    printf("Tr(Matrix_%x) = %.4lf\n", _mat_inv, _tr_mat);
    // ����ʽ
    MATRIX_TYPE _det_mat = M_det(_mat_inv);
    printf("Det(Matrix_%x) = %.4lf\n", mat_21, _det_mat);
    // ���
    Matrix *mat_full = M_full(mat_2, 1, 1, 1, 1, 0);
    M_print(mat_full);
    // ����
    printf("NORM1 = %lf\n", M_norm(mat_b, 1));
    printf("NORM2 = %lf\n", M_norm(mat_b, 2));
    // ��
    printf("Rank(%x) = %d\n", mat_A10, M_rank(mat_A10));
    printf("Rank(%x) = %d\n", mat_full, M_rank(mat_full));
    // Hilbert ϣ�����ؾ���
    printf(">> Gen Hilbert-Matrix\n");
    M_print(Hilbert(5));
    // ��������
    printf("������Ϊ: %lf\n", M_cond(Hilbert(5),1));
    //  ����QR�ֽ�
    Matrix * M_H = M_householder(Hilbert(5));
    M_print(M_H);
    //  ��������ֵ
    Matrix * M_eigen_I = M_eigen_val(M_H);
    M_print(M_eigen_I);
    // ����ֵ+��������
    Matrix *target = M_H;
    M_print(target);
    Matrix ** M_eigen_val_vec = M_eigen(target);
    M_print(M_eigen_val_vec[0]);
    M_print(M_eigen_val_vec[1]);
    // ��֤����ֵ, ��������
    M_print(M_mul(target,M_eigen_val_vec[1]));

// Application
    // �����Է���
    // mat_A*x = mat_b
    printf("#Solver:mat_A*x = mat_b\n");
    Matrix *_mat_result = M_mul(M_Inverse(mat_A10), mat_b10);
    M_print(_mat_result);
    // ���� Hilbert ϣ�����ؾ���(��̬����)
    // Hilbert(n)*x = One(n,1)
    int order = 10;
    printf("#Solver:Hilbert(%d)*x = One(%d,1)\n", order, order);
    M_print(M_mul(M_Inverse(Hilbert(order)), M_Ones(order,1)));
    // ����ϣ�����ؾ������
    printf("Rank(Hilbert(n)) = %d\n", M_rank(Hilbert(5)));

//  Others
    M_free(_mat_T); // �ͷž����ڴ�
    // [����] ������: һά�����޷�ʵ������
    M_print(M_Inverse(M_Zeros(1,1))); // ����֤ͨ��

// Help ����
    help("help");
    help("M_rank");
    help("Update");
    help("MatrixHub");

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

