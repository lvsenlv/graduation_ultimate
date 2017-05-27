/*************************************************************************
	> File Name: main.c
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年04月02日 星期日 14时54分54秒
 ************************************************************************/

#include <stdlib.h>
#include "bsp_ov2640.h"
#include "bsp_systick.h"
#include "bsp_sdram.h"
#include "image.h"
#include "cut.h"
#include "hough.h"

static void BL8782_PDN_INIT(void);

extern uint16_t *g_dma_target_ptr;
extern const uint16_t g_img_height;
extern const uint16_t g_img_width;
extern const uint32_t g_img_size;
extern __IO uint32_t current_height;
extern __IO uint32_t count_dma;
extern __IO uint32_t count_frames;

int main(void)
{   
    BL8782_PDN_INIT();
    OV2640_HW_Init();

    OV2640_IDTypeDef ov2640_id;
    OV2640_ReadID(&ov2640_id);
    if(ov2640_id.PIDH != 0x26)
    {
        DISP_ERR("not detected camera, please check the connection again \r\n");
        return -1;
    }
    
    uint16_t *initail_data_ptr = (uint16_t *)malloc(g_img_size);
    if(!initail_data_ptr)
    {
        DISP_ERR(ERR_MALLOC);
        return -1;
    }
    g_dma_target_ptr = initail_data_ptr;

    OV2640_Init();
    OV2640_UXGAConfig();

    DCMI_Cmd(ENABLE);
    DCMI_CaptureCmd(ENABLE);
    sleep(1);

    DCMI_Cmd(DISABLE);
    DCMI_CaptureCmd(DISABLE);
    DMA_Cmd(DMA2_Stream1, DISABLE);
    //DISP("disabled dma and dcmi \r\n");
    //DISP("count_frames = %d \r\n",  count_frames);
    //DISP("count_dma = %d \r\n", count_dma);
    //DISP("current_height = %d \r\n", current_height);

    uint8_t *rgb888_ptr = NULL;
    rgb888_ptr = bmp_rgb565_to_rgb888(g_img_width, g_img_height, g_dma_target_ptr);
    if(!rgb888_ptr)
    {
        DISP_ERR("error in bmp_rgb565_to_rgb888 \r\n");
        free(g_dma_target_ptr);
        return -1;
    }
    
    if(STAT_ERR == sd_mount())
    {
        free(g_dma_target_ptr);
        free(rgb888_ptr);
        return -1;
    }
/*
    if(image_write_rbg888(g_img_width, g_img_height, rgb888_ptr, "0:rgb888.bmp"))
    {
        DISP_ERR("error in image_write_rbg888");
        free(g_dma_target_ptr);
        free(rgb888_ptr);
        return -1;
    }
*/    
    free(g_dma_target_ptr);


/*************************************************************************
                         digit recognization
 ************************************************************************/

    DISP("be in recognizing ...... \r\n");
    //DISP("it may cost some time depending on your MCU \r\n");
    g_systick_count = 0;

    _bmp_pt bmp = bmp_create();
    if(!bmp)
    {
        DISP_ERR("error in bmp_create");
        free(rgb888_ptr);
        sd_umount();
        return -1;
    }

    if(bmp_head_parse(bmp, rgb888_ptr))
    {
        free(rgb888_ptr);
        bmp_free(bmp);
        sd_umount();
        return -1;
    }

    if(bmp_data_parse(bmp, rgb888_ptr+HEAD_INFO_SIZE))
    {
        DISP_ERR("error in bmp_data_parse");
        bmp_free(bmp);
        sd_umount();
        return -1;
    }

    _bmp_pt bmp_ret = NULL;
    bmp_ret = bmp_convert_gray(bmp);
    if(!bmp_ret)
    {
        DISP_ERR("error in bmp_convert_gray");
        bmp_free(bmp);
        return -1;
    }
/*    if(image_write_gray(bmp_ret, "0:gray.bmp"))
    {
        DISP_ERR("error in image_write_gray");
        bmp_free(bmp);
        bmp_free(bmp_ret);
        return -1;
    }
*/
    //abandon initial image data
    bmp_free(bmp);
    
    if(bmp_median_filter_fast(bmp_ret))
    {
        DISP_ERR("error in bmp_median_filter");
        bmp_free(bmp_ret);
        return -1;
    }
/*    if(image_write_gray(bmp_ret, "0:filter.bmp"))
    {
        DISP_ERR("error in image_write_gray");
        bmp_free(bmp_ret);
        return -1;
    }
*/
    uint8_t threshold = 0;
    if(bmp_get_threshold(bmp_ret, &threshold))
    {
        DISP_ERR("error in bmp_get_threshold");
        bmp_free(bmp_ret);
        return -1;
    }
    if(bmp_convert_binary(bmp_ret, threshold))
    {
        DISP_ERR("error in bmp_convert_binary");
        bmp_free(bmp_ret);
        return -1;
    }
/*    if(image_write_binary(bmp_ret, "0:binary.bmp"))
    {
        DISP_ERR("error in image_write_binary");
        bmp_free(bmp_ret);
        return -1;
    }
*/
    if(bmp_dilate(bmp_ret, 3))
    {
        DISP_ERR("error in bmp_dilate");
        bmp_free(bmp_ret);
        return -1;
    }
/*    if(image_write_binary(bmp_ret, "0:dilate.bmp"))
    {
        DISP_ERR("error in image_write_binary");
        bmp_free(bmp_ret);
        return -1;
    }
*/   
    if(bmp_digit_row_locate(bmp_ret))
    {
        DISP_ERR("error in bmp_digit_row_locate");
        bmp_free(bmp_ret);
        return -1;
    }
/*    if(image_write_plus_row(bmp_ret, "0:row.bmp"))
    {
        DISP_ERR("error in image_write_plus_row");
        bmp_free(bmp_ret);
        return -1;
    }
*/    
    if(bmp_digit_col_locate(bmp_ret))
    {
        DISP_ERR("error in bmp_digit_col_locate");
        bmp_free(bmp_ret);
        return -1;
    }
/*    if(image_write_plus_col(bmp_ret, "0:"))
    {
        DISP_ERR("error in image_write_plus_col");
        bmp_free(bmp_ret);
        return -1;
    }
*/
    if(bmp_digit_recognize(bmp_ret))
    {
        DISP_ERR("error in bmp_digit_recognize");
        bmp_free(bmp_ret);
        return -1;
    }    

    uint32_t *col_array_remain = bmp_ret->vital_info->reserved4;
    while(COL_ARRAY_END != *col_array_remain++);
    
    //DISP("result of recognition: \r\n");
    while(COL_ARRAY_REAL_END != *col_array_remain)
    {
        DISP("%d \t", *col_array_remain++);
    }
    DISP("\r\n");

    bmp_free(bmp_ret); 
    sd_umount();

    DISP("it costs about %lldms \r\n", g_systick_count);
    //DISP("end of project \r\n");

    return 0;
}

static void BL8782_PDN_INIT(void)
{
  /*定义一个GPIO_InitTypeDef类型的结构体*/
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOG, ENABLE); 							   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);	
  
  GPIO_ResetBits(GPIOG,GPIO_Pin_9);  //禁用WiFi模块
}

