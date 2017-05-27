/*************************************************************************
	> File Name: cut.c
	> Author: 
	> Mail: 
	> Created Time: 2017年04月11日 星期二 22时55分45秒
 ************************************************************************/

#include<stdio.h>
#include"cut.h"
void matrix_cut(_matrix_pt mat1,_matrix_pt mat2)
{
    _MAT_SIZE row1=mat1->row;
    _MAT_SIZE col1=mat1->col;
    _MAT_SIZE i = 0;
    _MAT_TYPE *p = mat1->pMat;
    _MAT_TYPE *q = mat2->pMat;
    
    for (i=0;i<row1*col1;i++)
    {
        if(p[i]==q[i])
        {
            p[i]=1;
            q[i]=1;
        }
    }
            


}



