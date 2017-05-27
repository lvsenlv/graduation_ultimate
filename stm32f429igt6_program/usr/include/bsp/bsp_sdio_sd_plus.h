/*************************************************************************
	> File Name: sdio_sd.h
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年05月09日 星期二 20时22分59秒
 ************************************************************************/

#ifndef __SDIO_SD_H
#define __SDIO_SD_H

#include "stm32f4xx.h"

#define SD_PIN_D0                           8
#define SD_PIN_D1                           9
#define SD_PIN_D2                           10
#define SD_PIN_D3                           11
#define SD_PIN_CLK                          12
#define SD_PIN_CMD                          2

#define SD_CMD_CMD0                         0
#define SD_CMD_CMD8                         8
#define SD_CMD_CMD55                        55
#define SD_CMD_ACMD41                       41
#define SD_CMD_CMD2                         2
#define SD_CMD_CMD3                         3
#define SD_CMD_CMD9                         9
#define SD_CMD_CMD7                         7
#define SD_CMD_ACMD6                        6
#define SD_CMD_CMD16                        16
#define SD_CMD_CMD24                        24
#define SD_CMD_CMD23                        23
#define SD_CMD_CMD25                        25
#define SD_CMD_CMD17                        17
#define SD_CMD_CMD18                        18
#define SD_CMD_CMD12                        12

#define SD_CMD_CLEAR_MASK                   ((uint32_t)0xFFFFF800)

#define SD_ARG_CHECK_PATTERN                ((uint32_t)0x1AA)
#define SD_ARG_VOLTAGE_WINDOW               ((uint32_t)0x80100000)

#define SD_CMD_TIMEOUT_TOTAL                ((uint32_t)0x10000)
#define SD_TIMEOUT_TOTAL                    ((uint32_t)0xFFFFFFFF)

#define SD_CHECK_CMD3_UNKNOWN_ERROR         ((uint32_t)0x2000)
#define SD_CHECK_CMD3_ILLEGAL_CMD           ((uint32_t)0x4000)
#define SD_CHECK_CMD3_COM_CRC_FAILED        ((uint32_t)0x8000)


#define SDIO_ICR_STATIC_CLEAR               ((uint32_t)0x5FF)

#define SDIO_DCTRL_CLEAR_MASK               ((uint32_t)0xFFFFFF08)

#define SDIO_FIFO_ADDRESS                   ((uint32_t)0x40012C80)

#define SD_OCR_ERROR_BIT                    ((uint32_t)0xFDFFE008)

#define SD_CHECK_SDSC_SDHC                  ((uint32_t)0x40000000)

#define SD_MAX_VOLT_TRIAL                   ((uint32_t)0xFFFF)


typedef enum {
    SD_CARD_TYPE_STD_CAPACITY_V1_1,
    SD_CARD_TYPE_STD_CAPACITY_V2_0,
    SD_CARD_TYPE_HIGH_CAPACITY,
}SD_CARD_TYPE;

typedef enum {
    SD_STATUS_OK = 0,
    SD_STATUS_CMD_RSP_TIMEOUT,
    SD_STATUS_CMD_RSP_CCRCFAIL,
    SD_STATUS_ILLEGAL_CMD,
    SD_STATUS_OCR_ERROR,
    SD_STATUS_INVALID_VOLTAGE,
    SD_STATUS_IS_POWER_OFF,
    SD_STATUS_CHECK_CMD3_ERR,
    SD_STATUS_UNSUPPORTED_CARD,
    SD_STATUS_DATA_CRC_FAIL,
    SD_STATUS_DATA_TIMEOUT,
    SD_STATUS_RX_OVERRUN,
    SD_STATUS_TX_UNDERRUN,
    SD_STATUS_START_BIT_ERR,
    SD_STATUS_FATAL_ERR,
}SD_STATUS;

typedef enum {
    SD_TRANSFER_READ,
    SD_TRANSFER_WRITE,
}SD_TRANSFER_WAY;

/*************************************************************************
bit0 ~ bit7:    CLKDIV
                            SDIO_CK = SDIOCLK / [CLKDIV + 2],
                            on STM32F4xx devices, SDIOCLK is fixed to 48MHZ,
                            SDIO_CK for initialization should not exceed 400 KHZ.
bit9:           PWRSAV
bit10:          BYPASS
bit11 ~ bit12:  WIDBUS
bit13:          NEGEDGE
bit14:          HWFC_EN
 ************************************************************************/
static inline void sdio_init(uint32_t args)
{
    //clear bit0 ~ bit7, bit9 ~ bit14
    SDIO->CLKCR &= 0xFFFF8100;
    SDIO->CLKCR |= args;
}

static inline void sd_delay(uint32_t count)
{
    for(; count > 0; count--);
}

/*************************************************************************
parameter 1: SDIO argument register

parameter 2: SDIO_CMD
    bit0 ~ bit5:    CMDINDEX
    bit6 ~ bit7:    WAITRESP
    bit8 ~ bit9:    WAITINT,WAITPEND
    bit10:          CPSMEN
 ************************************************************************/
#if 0
#define sd_write_cmd(arg, cmd) { \
                                    uint32_t tmp_cmd = SDIO->CMD; \
                                    SDIO->ARG = arg; \
                                    tmp_cmd &= 0xFFFFF800; \
                                    tmp_cmd |= cmd; \
                                    SDIO->CMD = tmp_cmd; \
                                }

/*static inline void sd_write_cmd(uint32_t arg, uint32_t cmd)
{
    uint32_t tmp;
    SDIO->ARG = arg;
    tmp = SDIO->CMD;
    tmp &= 0xFFFFF800;
    tmp |= cmd;
    SDIO->CMD = tmp;
}*/

static inline void sd_write_arg(uint32_t arg)
{
    SDIO->ARG = arg;
    SDIO->CMD &= 0xFFFFF800;
}

static inline void sd_write_cmd(uint32_t cmd)
{
    SDIO->CMD |= cmd;
}
#endif //#if 0

/*
static inline void sd_delay(__IO uint32_t count)
{
    for(; count > 0; count--);
}*/

static inline uint32_t sd_get_response(uint32_t response_register)
{
    return (*(__IO uint32_t *)(SDIO_BASE + 0x14 + response_register));
}

extern __IO SD_STATUS g_sd_transfer_status;
extern __IO uint8_t g_sd_transfer_end;
extern __IO uint8_t g_sd_stop_condition;
extern __IO uint8_t g_dma_transfer_end;

SD_STATUS sd_init(void);
SD_STATUS sd_write_single_block(uint8_t *src_buf, 
    uint32_t target_addr, uint32_t block_size);
SD_STATUS sd_write_multi_blocks(uint8_t *src_buf, 
    uint32_t target_addr, uint32_t block_size, uint32_t block_num);
SD_STATUS sd_read_single_block(uint8_t *target_buf, 
    uint32_t src_addr, uint32_t block_size);
SD_STATUS sd_read_multi_blocks(uint8_t *target_buf, 
    uint32_t src_addr, uint32_t block_size, uint32_t block_num);
SD_STATUS sd_wait_operation(SD_TRANSFER_WAY transfer_way);

#endif
 
