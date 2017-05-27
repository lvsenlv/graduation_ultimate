/*************************************************************************
	> File Name: matrix.c
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年02月27日 星期一 16时36分15秒
 ************************************************************************/

#include <stdlib.h>
#include "matrix.h"

static matrix_pt matrix_sum_sub(matrix_pt mat_a, matrix_pt mat_b, 
    const uint8_t symbol);
static matrix_pt matrix_multiply(matrix_pt mat_a, matrix_pt mat_b);

matrix_pt matrix_create(MAT_ROW row, MAT_COL col)
{
#ifdef __DEBUG
    if(row <= 0 || col <= 0)
    {
        DISP_ERR("invalid row or col");
        return NULL;
    }
#endif //__DEBUG

    matrix_pt mat = (matrix_pt)malloc(sizeof(matrix_t));
    if(!mat)
    {
        DISP_ERR(ERR_MALLOC);
        return NULL;
    }

    mat->data = NULL;
    mat->row = row;
    mat->col = col;
#ifdef __DEBUG
    mat->data = (MAT_TYPE *)calloc(row*col, sizeof(MAT_TYPE));
    if(!mat->data)
    {
        DISP_ERR(ERR_MALLOC);
        free(mat);
        mat = NULL;
        return NULL;
    }
#else
    mat->data = (MAT_TYPE *)malloc(sizeof(MAT_TYPE)*row*col);
    if(!mat->data)
    {
        DISP_ERR(ERR_MALLOC);
        free(mat);
        mat = NULL;
        return NULL;
    }
#endif //__DEBUG

    return mat;
}

#ifdef __DEBUG
MAT_STATUS matrix_set(matrix_pt mat, MAT_ROW row, 
    MAT_COL col, MAT_TYPE elem)
{
    if(!mat)
    {
        DISP_ERR(ERR_MATRIX);
        return MAT_STATUS_ERR;
    }
    
    if(!mat->data)
    {
        DISP_ERR(ERR_MAT_DATA);
        return MAT_STATUS_ERR;
    }

    if(row < 0 || col < 0 || (row+1) > mat->row || \
        (col+1) > mat->col)
    {
        DISP_ERR("incorrect insert position");
        return MAT_STATUS_ERR;
    }


    VALUE(mat, row, col) = elem;

    return MAT_STATUS_OK;
}
#endif //__DEBUG

MAT_STATUS matrix_disp(matrix_pt mat)
{
#ifdef __DEBUG
    if(!mat)
    {
        DISP_ERR(ERR_MATRIX);
        return MAT_STATUS_ERR;
    }

    if(!mat->data)
        return MAT_STATUS_OK;
#endif //__DEBUG
    MAT_ROW i = 0;
    MAT_COL j = 0;
    for(i = 0; i < mat->row; i++)
    {
        for(j = 0; j < mat->col; j++)
        {
            DISP("%x ", VALUE(mat, i, j));
        }
        DISP("\n");
    }
    //DISP("\n");

    return MAT_STATUS_OK;
}

void matrix_free(matrix_pt mat)
{
    if(mat)
    {
        if(mat->data)
        {
            free(mat->data);
        }
        free(mat);
    }
}

matrix_pt matrix_calculate(matrix_pt mat_a, matrix_pt mat_b, 
    const uint8_t symbol)
{
#ifdef __DEBUG
    if((!mat_a) || (!mat_b))
    {
        DISP_ERR(ERR_MATRIX);
        return NULL;
    }

    if((!mat_a->data) || (!mat_b->data))
    {
        DISP_ERR(ERR_MAT_DATA);
        return NULL;
    }
#endif //__DEBUG

    switch(symbol)
    {
        case MAT_ADD :
#ifdef __DEBUG
            if(mat_a->row != mat_b->row || mat_a->col !=mat_b->col)
            {
                DISP_ERR("dimension of two matrixs must be equal");
                return NULL;
            }
#endif //__DEBUG
            return matrix_sum_sub(mat_a, mat_b, MAT_ADD);
            //break;
        case MAT_SUB :
#ifdef __DEBUG
            if(mat_a->row != mat_b->row || mat_a->col !=mat_b->col)
            {
                DISP_ERR("dimension of two matrixs must be equal");
                return NULL;
            }
#endif //__DEBUG
            return matrix_sum_sub(mat_a, mat_b, MAT_SUB);
            //break;
        case MAT_MUL :
#ifdef __DEBUG
            if(mat_a->col != mat_b->row)
            {
                DISP_ERR("matrixA's col != matrixB's row");
                return NULL;
            }
#endif //__DEBUG
            return matrix_multiply(mat_a, mat_b);
            //break;
        default :
            DISP_ERR("invalid symbol");
            break;
    }

    return NULL;
}

static matrix_pt matrix_sum_sub(matrix_pt mat_a, matrix_pt mat_b, 
    const uint8_t symbol)
{
    uint32_t size = mat_a->row * mat_a->col;
    uint32_t i = 0;
    matrix_pt mat_ret = NULL;

    switch(symbol)
    {
        case MAT_ADD :
            mat_ret = matrix_create(mat_a->row, mat_a->col);
            if(!mat_ret)
            {
                DISP_ERR(ERR_CREATE);
                return NULL;
            }
            
            for(i = 0; i < size; i++)
            {
                mat_ret->data[i] = mat_a->data[i] + \
                    mat_b->data[i];
            }

            return mat_ret;
            //break;
        case MAT_SUB :
            mat_ret = matrix_create(mat_a->row, mat_a->col);
            if(!mat_ret)
            {
                DISP_ERR(ERR_CREATE);
                return NULL;
            }
            
            for(i = 0; i < size; i++)
            {
                mat_ret->data[i] = mat_a->data[i] - \
                    mat_b->data[i];
            }

            return mat_ret;
            //break;
        default :
            return NULL;
            //break;
    }
}

static matrix_pt matrix_multiply(matrix_pt mat_a, matrix_pt mat_b)
{
    MAT_ROW rowA = mat_a->row;
    MAT_COL colA = mat_a->col, colB = mat_b->col;
    matrix_pt mat_ret = NULL;

    mat_ret = matrix_create(rowA, colB);
    if(!mat_ret)
    {
        DISP_ERR(ERR_CREATE);
        return NULL;
    }
    
    MAT_ROW i = 0;
    MAT_COL j = 0, k = 0;
    MAT_TYPE sum = 0;

    for(i = 0; i < rowA; i++)
    {
        for(j = 0; j < colB; j++)
        {
            sum = 0;
            for(k = 0; k < colA; k++)
            {
                sum += VALUE(mat_a, i, k) * VALUE(mat_b, k, j);
            }
            VALUE(mat_ret, i, j) = sum;
        }
    }
    
    return mat_ret;
}



/*************************************************************************
                          extended functions
 ************************************************************************/
 
#include <string.h>

void *matrix_sum(matrix_pt mat, ...)
{
#ifdef __DEBUG
    if(!mat)
    {
        DISP_ERR(ERR_MATRIX);
        return NULL;
    }
    if(!mat->data)
    {
        DISP_ERR(ERR_MAT_DATA);
        return NULL;
    }
#endif //__DEBUG

    va_list arg_ptr;
    int opts = 0;
    uint32_t ret = 0;
    MAT_ROW row = mat->row;
    MAT_COL col = mat->col;
    uint32_t i = 0, j = 0;
    matrix_pt mat_ret = NULL;
    uint32_t size = row * col;

    va_start(arg_ptr, mat);
    opts = va_arg(arg_ptr, int);

    switch(opts)
    {
        case MAT_SUM_COL:
            if(1 == row)
                return NULL;

            mat_ret = matrix_create(1, col);
            if(!mat_ret)
            {
                DISP_ERR(ERR_CREATE);
                return NULL;
            }

            for(j = 0; j < col; j++)
            {
                ret = 0;
                for(i = 0; i < row; i++)
                {
                    ret += VALUE(mat, i, j);
                }
                mat_ret->data[j] = ret;
            }
            
            va_end(arg_ptr);
            return mat_ret;
            //break;
        case MAT_SUM_ALL :
            for(i = 0; i < size; i++)
            {
                ret += mat->data[i];
            }

            va_end(arg_ptr);
            return (uint32_t *)ret;
            //break;
        case MAT_SUM_ROW:
        default :    
            if(1 == col)
                return NULL;

            mat_ret = matrix_create(row, 1);
            if(!mat_ret)
            {
                DISP_ERR(ERR_CREATE);
                return NULL;
            }
            
            for(i = 0; i < row; i++)
            {
                ret = 0;
                for(j = 0; j < col; j++)
                {
                    ret += VALUE(mat, i, j);
                }
                mat_ret->data[i] = ret;
            }
            break;
    }

    va_end(arg_ptr);
    return mat_ret;
}

MAT_TYPE matrix_max(matrix_pt mat)
{
#ifdef __DEBUG
    if(!mat)
    {
        DISP_ERR(ERR_MATRIX);
        return -1;
    }
    if(!mat->data)
    {
        DISP_ERR(ERR_MAT_DATA);
        return -1;
    }
#endif //__DEBUG

    uint32_t size = mat->row * mat->col;
    uint32_t i = 0;
    MAT_TYPE ret = mat->data[0];
    
    for(i = 1; i < size; i++)
    {
        ret = (mat->data[i] > ret ? mat->data[i] : ret);
    }

    return ret;
}

#ifndef __DEBUG
matrix_pt matrix_zeros(MAT_ROW row, MAT_COL col)
{
    matrix_pt mat_ret = NULL;
    
    mat_ret = matrix_create(row, col);
    if(!mat_ret)
    {
        DISP_ERR(ERR_CREATE);
        return NULL;
    }

    memset(mat_ret->data, 0, row*col*sizeof(MAT_TYPE));
    
    return mat_ret;
}

matrix_pt matrix_ones(MAT_ROW row, MAT_COL col)
{
    matrix_pt mat_ret = matrix_create(row, col);
    if(!mat_ret)
    {
        DISP_ERR(ERR_CREATE);
        return NULL;
    }
    
    uint32_t size = row * col;
    uint32_t i = 0;
    for(i = 0; i < size ; i++)
    {
        mat_ret->data[i] = 1;
    }
    
    return mat_ret;
}

#else //__DEBUG
matrix_pt matrix_ones(MAT_ROW row, MAT_COL col, ...)
{
    matrix_pt mat_ret = matrix_create(row, col);
    if(!mat_ret)
    {
        DISP_ERR(ERR_CREATE);
        return NULL;
    }

    va_list arg_ptr = NULL;
    int elem = 0;

    va_start(arg_ptr, col);
    elem = va_arg(arg_ptr, int);
    
    elem = ((elem > 65535 || elem < -65535) ? 1 : elem);

    uint32_t size = row * col;
    uint32_t i = 0;
    for(i = 0; i < size ; i++)
    {
        mat_ret->data[i] = elem;
    }

    va_end(arg_ptr);
    return mat_ret;
}
#endif //__DEBUG


