/*************************************************************************
	> File Name: matrix.h
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年02月27日 星期一 16时37分30秒
 ************************************************************************/

#ifndef __MATRIX_H
#define __MATRIX_H

#include "common.h"

#define     MAT_TYPE                        uint8_t
#define     MAT_ROW                         uint32_t
#define     MAT_COL                         MAT_ROW
#define     MAT_ADD                         ((uint8_t)'+')
#define     MAT_SUB                         ((uint8_t)'-')
#define     MAT_MUL                         ((uint8_t)'*')

#define     VALUE(mat, nrow, ncol)          (mat->data[mat->col*nrow+ncol])

#define     CHECK_ROW_COL(row, col)         ((row <= 0 || col <= 0) ? MAT_STATUS_ERR : MAT_STATUS_OK)
#define     CHECK_MATRIX(mat)               ((NULL == mat) ? MAT_STATUS_ERR : MAT_STATUS_OK)
#define     CHECK_MAT_DATA(mat)             ((NULL == mat->data) ? MAT_STATUS_ERR : MAT_STATUS_OK)

typedef struct matrix_truct_info {
    MAT_TYPE *data; //point to a two-dimensional matrix
    MAT_ROW row;
    MAT_COL col;
}matrix_t, *matrix_pt;

typedef enum {
    MAT_STATUS_OK = 0,
    MAT_STATUS_ERR,
}MAT_STATUS;

#ifdef __DEBUG
MAT_STATUS matrix_set(matrix_pt mat, MAT_ROW row, 
    MAT_COL col, MAT_TYPE elem);
#else
#define     matrix_set(mat, row, col, elem)      (VALUE(mat, row, col) = elem)
#endif //__DEBUG

matrix_pt matrix_create(MAT_ROW row, MAT_COL col);
MAT_STATUS matrix_disp(matrix_pt mat);
void matrix_free(matrix_pt mat);
matrix_pt matrix_calculate(matrix_pt mat_a, matrix_pt mat_b, 
    const uint8_t symbol);



/*************************************************************************
                          extended functions
 ************************************************************************/

#include <stdarg.h>

#ifdef __DEBUG
#define     matrix_zeros(row, col)          matrix_create(row, col)
matrix_pt matrix_ones(MAT_ROW row, MAT_COL col, ...);

#else
matrix_pt matrix_zeros(MAT_ROW row, MAT_COL col);
matrix_pt matrix_ones(MAT_ROW row, MAT_COL col);
#endif //__DEBUG

typedef enum {
    MAT_SUM_ROW = 1,
    MAT_SUM_COL,
    MAT_SUM_ALL,
}MAT_SUM_OPTS;

void *matrix_sum(matrix_pt mat, ...);
MAT_TYPE matrix_max(matrix_pt mat);

#endif

