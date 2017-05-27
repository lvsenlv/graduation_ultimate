/*************************************************************************
	> File Name: bsp_sdram.h
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年04月02日 星期二 22时23分24秒
 ************************************************************************/
#ifndef __SDRAM_H
#define	__SDRAM_H

#include "stm32f4xx.h"
#include "common.h"

#define IS42S16400J_SIZE                    0x800000    //8MiB
#define SDRAM_BANK_ADDR                     ((uint32_t)0xD0000000)

#define FMC_A0_PIN                          0
#define FMC_A1_PIN                          1
#define FMC_A2_PIN                          2
#define FMC_A3_PIN                          3
#define FMC_A4_PIN                          4
#define FMC_A5_PIN                          5
#define FMC_A6_PIN                          12
#define FMC_A7_PIN                          13
#define FMC_A8_PIN                          14
#define FMC_A9_PIN                          15
#define FMC_A10_PIN                         0
#define FMC_A11_PIN                         1

#define FMC_D0_PIN                          14
#define FMC_D1_PIN                          15
#define FMC_D2_PIN                          0
#define FMC_D3_PIN                          1
#define FMC_D4_PIN                          7
#define FMC_D5_PIN                          8
#define FMC_D6_PIN                          9
#define FMC_D7_PIN                          10
#define FMC_D8_PIN                          11
#define FMC_D9_PIN                          12
#define FMC_D10_PIN                         13
#define FMC_D11_PIN                         14
#define FMC_D12_PIN                         15
#define FMC_D13_PIN                         8
#define FMC_D14_PIN                         9
#define FMC_D15_PIN                         10

#define FMC_CS_PIN                          6
#define FMC_BA1_PIN                         5
#define FMC_BA0_PIN                         4
#define FMC_WE_PIN                          0
#define FMC_RAS_PIN                         11
#define FMC_CAS_PIN                         15
#define FMC_CLK_PIN                         8
#define FMC_CKE_PIN                         7

#define FMC_UDQM_PIN                        1
#define FMC_LDQM_PIN                        0

//                                  coorsponding bit    real value
#define SDRAM_TMRD                          1           // 2
#define SDRAM_TXSR                          6           // 7
#define SDRAM_TRAS                          3           // 4
#define SDRAM_TRC                           6           // 7
#define SDRAM_TWR                           1           // 2
#define SDRAM_TRP                           1           // 2
#define SDRAM_TRCD                          1           // 2

//                                  coorsponding bit    real value
#define SDRAM_COL_NUM                       0x0         // 8
#define SDRAM_ROW_NUM                       0x1         // 12
#define SDRAM_DATA_WIDTH                    0x1         // 16
#define SDRAM_BANK_NUM                      0x1         // 4
#define SDRAM_CAS_LATENCY                   0x2         // 2
#define SDRAM_WRITE_PROTECTION              0x0         // disable
#define SDRAM_SDCLK_PERIOD                  0x2         // 2
#define SDRAM_READ_BURST                    0x1         // enable
#define SDRAM_READ_PIPE                     0x0         // 0

#define SDRAM_MODE_REG_BURST_LENGTH_1                   ((uint16_t)0x0000)
#define SDRAM_MODE_REG_BURST_LENGTH_2                   ((uint16_t)0x0001)
#define SDRAM_MODE_REG_BURST_LENGTH_4                   ((uint16_t)0x0002)
#define SDRAM_MODE_REG_BURST_LENGTH_8                   ((uint16_t)0x0004)
#define SDRAM_MODE_REG_BURST_TYPE_SEQUENTIAL            ((uint16_t)0x0000)
#define SDRAM_MODE_REG_BURST_TYPE_INTERLEAVED           ((uint16_t)0x0008)
#define SDRAM_MODE_REG_CAS_LATENCY_2                    ((uint16_t)0x0020)
#define SDRAM_MODE_REG_CAS_LATENCY_3                    ((uint16_t)0x0030)
#define SDRAM_MODE_REG_OPERATING_MODE_STANDARD          ((uint16_t)0x0000)
#define SDRAM_MODE_REG_WRITEBURST_MODE_PROGRAMMED       ((uint16_t)0x0000) 
#define SDRAM_MODE_REG_WRITEBURST_MODE_SINGLE           ((uint16_t)0x0200)   

static inline void sdram_delay(uint32_t count)
{
    for(; count > 0; count--);
}

void sdram_init(void);
void sdram_entire_reset(void);
_G_STATUS sdram_entire_test(void);



#endif /* __SDRAM_H */
