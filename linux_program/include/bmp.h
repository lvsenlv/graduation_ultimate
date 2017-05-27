/*************************************************************************
	> File Name: bmp.h
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年03月15日 星期三 19时14分25秒
 ************************************************************************/

#ifndef __BMP_H
#define __BMP_H

#include "common.h"
#include "matrix.h"

#define     HEAD_INFO_SIZE                  54
#define     COUNT_REAL_WIDTH(bits)          ((((bits)+31)>>5)<<2)
#define     FILTER_MODEL_SIZE               3
#define     FILTER_OFFSET                   (FILTER_MODEL_SIZE >> 1)
#define     FILTER_OFFSET_PLUS              ((FILTER_MODEL_SIZE*FILTER_MODEL_SIZE) >> 1)
#define     COL_ARRAY_END                   0xFFFF
#define     COL_ARRAY_REAL_END              0xFFFFF
#define     DIGIT_ONE_RATIO                 3
            //it is the digit 1 if height/widht > DIGIT_ONE_RATIO

#define     MIDDLE(a, b, c) \
            (   (a)>(b)   \
                 ?   \
                    (    (a)>(c)   ?   ( (b)>(c) ? (b):(c) )    :   (a)    )    \
                 :    \
                    (    (b)>(c)   ?   ( (a)>(c) ? (a):(c) )    :   (b)    )    \
            )

#if 0
typedef struct bmp_head_main_info {
    uint32_t size;                  //the size of image
    uint16_t reserved1;             //reserver word
    uint16_t reserved2;             //reserver word
    uint32_t off_bits;              //the offset bit of real color data
}_head_main_t, *_head_main_pt;

typedef struct bmp_head_plus_info {
    uint32_t struct_size;           //the size of this struct
    uint32_t width;                 //the width of image
    uint32_t height;                //the height of image
    uint16_t planes;                //the number of plane of image, default value is 1
    uint16_t bit_count;             //the bits of color, may be 1,2,4,8,16,24,32.
    uint32_t comperssion;           //the way of compression,may be 0,1,2, 0 means no compression
    uint32_t real_size;             //the size of color date
    uint32_t x_pixel;               //the resolution of x
    uint32_t y_pixel;               //the resolution of y
    uint32_t clr_used;              //the number of color that is used, default value is 0
    uint32_t clr_import;            //the number of important color, 0 means all are important
}head_plus_t, *head_plus_pt;

typedef struct bmp_info {
    head_main_t main_info;
    head_plus_t plus_info;
}bmp_t, *bmp_pt;
#endif

typedef struct bmp_vital_info { //use memset to initialize this struct after creating
    uint32_t width;
    uint32_t height;
    uint16_t bit_count;
    uint16_t reserved1;         //align with 4 byte
    uint32_t real_size;
    uint32_t real_width;
    uint32_t reserved2;         //in the numberal image, it means the start row of image
    uint32_t reserved3;         //in the numberal image, it means the end row of image
    uint32_t *reserved4;        //in the numberal image, it points to the col of image
}bmp_vital_t, *bmp_vital_pt;

typedef struct bmp_info {
    matrix_pt head_info;       //this info contains format, main_info and plus_info
    bmp_vital_pt vital_info;
    matrix_pt data_info;
}bmp_t, *bmp_pt;

bmp_pt bmp_create(void);
G_STATUS bmp_head_parse(bmp_pt bmp, uint8_t *ptr);
void bmp_free(bmp_pt bmp);
void bmp_viatl_info_disp(bmp_pt bmp);
G_STATUS bmp_data_parse(bmp_pt bmp, uint8_t *ptr);
bmp_pt bmp_convert_gray(bmp_pt bmp);
G_STATUS bmp_median_filter_slow(bmp_pt bmp);
G_STATUS bmp_median_filter(bmp_pt bmp);
G_STATUS bmp_median_filter_fast(bmp_pt bmp);
G_STATUS bmp_get_threshold(bmp_pt bmp, uint8_t *threshold);
G_STATUS bmp_convert_binary(bmp_pt bmp, uint8_t threshold);
G_STATUS bmp_dilate(bmp_pt bmp, uint8_t struct_width);
G_STATUS bmp_digit_row_locate(bmp_pt bmp);
G_STATUS bmp_digit_col_locate(bmp_pt bmp);
G_STATUS bmp_digit_recognize(bmp_pt bmp);

/*************************************************************************
                          extended functions
 ************************************************************************/
 
uint8_t *bmp_rgb565_to_rgb888(uint32_t img_width, uint32_t img_height, 
    uint16_t *data_addr);

#endif

