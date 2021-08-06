#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "state.h"

int main(int argc, char *argv[]) { 
	/*需将dll文件置于当前文件夹下*/ 
	HINSTANCE Hint = LoadLibrary("Matrix_Hub_v1.42.dll");
	
	typedef int (*FUNT)(char* file_name);
	FUNT Help = (FUNT)GetProcAddress(Hint,"help");
	
	typedef Matrix* (*FUNT1)(int row,int column,MATRIX_TYPE* data);
	FUNT1 Matrix_gen = (FUNT1)GetProcAddress(Hint,"Matrix_gen");
	
	typedef Matrix* (*FUNT2)(Matrix* mat);
	FUNT2 M_print = (FUNT2)GetProcAddress(Hint,"M_print");
	FUNT2 M_Inverse = (FUNT2)GetProcAddress(Hint,"M_Inverse");
	
	Help("README");
	
	MATRIX_TYPE _mat_21[3][3] = { 0,2,3,2,1,2,4,1,3};
	int row = sizeof(_mat_21) / sizeof(_mat_21[0]);
	int column = sizeof(_mat_21[0]) / sizeof(_mat_21[0][0]);
	Matrix*  mat_21 = Matrix_gen(row,column,_mat_21);
	M_print(mat_21);
	
	
	M_print(M_Inverse(mat_21));
	
	return 0;
}
