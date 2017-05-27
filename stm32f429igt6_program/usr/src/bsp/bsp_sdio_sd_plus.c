/*************************************************************************
	> File Name: sdio_sd.c
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年05月09日 星期二 20时22分55秒
 ************************************************************************/

#include "bspsdio_sd_plus.h"

static void sd_nvic_init(void);
static void sd_gpio_init(void);
static SD_STATUS sd_power_on(void);
static SD_STATUS sd_check_cmd0(void);
static SD_STATUS sd_check_response7(void);
static SD_STATUS sd_check_response1(uint8_t cmd);
static SD_STATUS sd_check_response3(void);
static SD_STATUS sd_check_response2(void);
static SD_STATUS sd_check_response6(uint8_t cmd, uint16_t *rca_ptr);
static void sd_dma_init(uint32_t addr, SD_TRANSFER_WAY transfer_way);


#define sd_check_cmd8()                     sd_check_response7()
#define sd_check_cmd55(cmd)                 sd_check_response1(cmd)
#define sd_check_acmd41()                   sd_check_response3()
#define sd_check_cmd2()                     sd_check_response2()
#define sd_check_cmd9()                     sd_check_response2()
#define sd_check_cmd7(cmd)                  sd_check_response1(cmd)
#define sd_check_acmd6(cmd)                 sd_check_response1(cmd)
#define sd_check_cmd16(cmd)                 sd_check_response1(cmd)
#define sd_check_cmd24(cmd)                 sd_check_response1(cmd)
#define sd_check_cmd23(cmd)                 sd_check_response1(cmd)
#define sd_check_cmd25(cmd)                 sd_check_response1(cmd)
#define sd_check_cmd17(cmd)                 sd_check_response1(cmd)
#define sd_check_cmd18(cmd)                 sd_check_response1(cmd)
#define sd_check_cmd12(cmd)                 sd_check_response1(cmd)

extern SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
extern SDIO_DataInitTypeDef SDIO_DataInitStructure;
static SD_CARD_TYPE g_card_type = SD_CARD_TYPE_STD_CAPACITY_V1_1;
#define SD_DATATIMEOUT 0xFFFFFFFF
//static uint32_t g_cid_table[4], g_csd_table[4];
static uint32_t g_rca = 0;
__IO SD_STATUS g_sd_transfer_status = SD_STATUS_OK;
__IO uint8_t g_sd_transfer_end = 0;
__IO uint8_t g_sd_stop_condition = 0;
__IO uint8_t g_dma_transfer_end = 0;

SD_STATUS sd_init(void)
{
    sd_nvic_init();
    sd_gpio_init();

    //reset sdio
    RCC->APB2RSTR |= (0x1 << 11);
    RCC->APB2RSTR &= ~(0x1 << 11);

    SD_STATUS status = sd_power_on();
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    //if power off
    if((SDIO->POWER & 0x3) == 0)
    {
        return SD_STATUS_IS_POWER_OFF;
    }

    SDIO->ARG = 0;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD2 | 
        (0x3 << 6) | (0x0 << 8) | (0x1 << 10);
       
    status = sd_check_cmd2();
    if(SD_STATUS_OK != status)
    {
        return status;
    }
     
#if 0    
    g_cid_table[0] = SDIO->RESP1;
    g_cid_table[1] = SDIO->RESP2;
    g_cid_table[2] = SDIO->RESP3;
    g_cid_table[3] = SDIO->RESP4;
#endif //#if 0

    if(SD_CARD_TYPE_STD_CAPACITY_V1_1 != g_card_type && 
        SD_CARD_TYPE_STD_CAPACITY_V2_0 != g_card_type && 
        SD_CARD_TYPE_HIGH_CAPACITY != g_card_type)
    {
        return SD_STATUS_UNSUPPORTED_CARD;
    }

    //card publishes its RCA
    SDIO->ARG = 0;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD3 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);

    status = sd_check_response6(SD_CMD_CMD3, (uint16_t *)&g_rca);
    if(SD_STATUS_OK != status)
    {
        return status;
    }
    
#if 0
    //send CMD9 if it does not secure digital card
    sd_write_cmd(g_rca << 16, SD_CMD_CMD9 | (0x3 << 6) | (0x0 << 8) | (0x1 << 10));
    status = sd_check_cmd9();
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    g_csd_table[0] = SDIO->RESP1;
    g_csd_table[1] = SDIO->RESP2;
    g_csd_table[2] = SDIO->RESP3;
    g_csd_table[3] = SDIO->RESP4;
#endif //#if 0

    //set as 24MHZ, edge_rising, bypass_disable, power_save_diable, bus_width_1, flow_control_diable
    sdio_init(0x0 | (0x0 << 9) | (0x0 << 10) | 
        (0x0 << 11) | (0x0 << 13) | (0x0 << 14));

    //select the card that want to operate
    SDIO->ARG = g_rca << 16;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD7 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);

    status = sd_check_cmd7(SD_CMD_CMD7);
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    SDIO->ARG = g_rca << 16;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD55 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);

    status = sd_check_cmd55(SD_CMD_CMD55);
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    //send ACMD6 with argument as 2 for wide bus mode
    SDIO->ARG = 0x2;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_ACMD6 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);

    status = sd_check_acmd6(SD_CMD_ACMD6);
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    //set as 24MHZ, edge_rising, bypass_disable, power_save_diable, bus_width_4, flow_control_diable
    sdio_init(0x0 | (0x0 << 9) | (0x0 << 10) | 
        (0x1 << 11) | (0x0 << 13) | (0x0 << 14));

    return SD_STATUS_OK;
}

static void sd_nvic_init(void)
{
    NVIC_InitTypeDef nvic_t;
    
    SCB->AIRCR = 0x05FA0000 | NVIC_PriorityGroup_1;
    
    nvic_t.NVIC_IRQChannel = SDIO_IRQn;
    nvic_t.NVIC_IRQChannelCmd = ENABLE;
    nvic_t.NVIC_IRQChannelPreemptionPriority = 1;
    nvic_t.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic_t);

    nvic_t.NVIC_IRQChannel = DMA2_Stream3_IRQn;
    nvic_t.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&nvic_t);
}

static void sd_gpio_init(void)
{
    //enable GPIOC GPIOD DMA clock
    RCC->AHB1ENR |= (0x1 << 2) | (0x1 << 3) | (0x1 << 22);
    //enable SDIO APB2 clock
    RCC->APB2ENR |= (0x1 << 11);
    
    gpio_af_init(GPIOC, SD_PIN_D0, AF12);
    gpio_af_init(GPIOC, SD_PIN_D1, AF12);
    gpio_af_init(GPIOC, SD_PIN_D2, AF12);
    gpio_af_init(GPIOC, SD_PIN_D3, AF12);
    gpio_af_init(GPIOC, SD_PIN_CLK, AF12);
    gpio_af_init(GPIOD, SD_PIN_CMD, AF12);
    
    gpio_init(GPIOC, SD_PIN_D0, 0x2, 0x0, 0x2, 0x1);
    gpio_init(GPIOC, SD_PIN_D1, 0x2, 0x0, 0x2, 0x1);
    gpio_init(GPIOC, SD_PIN_D2, 0x2, 0x0, 0x2, 0x1);
    gpio_init(GPIOC, SD_PIN_D3, 0x2, 0x0, 0x2, 0x1);
    gpio_init(GPIOC, SD_PIN_CLK, 0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOD, SD_PIN_CMD, 0x2, 0x0, 0x2, 0x1);
}

static SD_STATUS sd_power_on(void)
{
    __IO SD_STATUS status = SD_STATUS_OK;
    uint32_t tmp = 0;
    uint32_t voltage = 0, count = 0;
    uint32_t response = 0;

    //set as 400KHZ, edge_rising, bypass_disable, power_save_diable, bus_width_1, flow_control_diable
    sdio_init(0x76 | (0x0 << 9) | (0x0 << 10) | (0x0 << 11) | (0x0 << 13) | (0x0 << 14));

    //power on
    SDIO->POWER = 0x3;

    //enable sdio clock
    //*(__IO uint32_t *)(0x422580A0) = (uint32_t)0x1;
    SDIO->CLKCR |= (0x1 << 8);

    SDIO->ARG = 0;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD0 | 
        (0x0 << 6) | (0x0 << 8) | (0x1 << 10);
    
    status = sd_check_cmd0();
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    //it must delay 0xFFF at least here, wait for stability after sd_check_cmd0
    sd_delay(0xFFF);

    /*Argument:
      bit8 ~ bit11: supply voltage (VHS) 0x1 (range: 2.7-3.6 V)
      bit0 ~ bit7: check pattern (recommended 0xAA) 
    */
    SDIO->ARG = 0x1AA;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD8 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);

    status = sd_check_cmd8();
    if(SD_STATUS_OK == status)
    {
        g_card_type = SD_CARD_TYPE_STD_CAPACITY_V2_0;
        tmp = SD_CHECK_SDSC_SDHC;
    }

    SDIO->ARG = 0x0;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD55 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);

    status = sd_check_cmd55(SD_CMD_CMD55);
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    while((!voltage) && (count < SD_MAX_VOLT_TRIAL))
    {
        SDIO->ARG = 0x0;
        SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK |  SD_CMD_CMD55 | 
            (0x1 << 6) | (0x0 << 8) | (0x1 << 10);

        status = sd_check_cmd55(SD_CMD_CMD55);
        if(SD_STATUS_OK != status)
        {
            return status;
        }

        SDIO->ARG = SD_ARG_VOLTAGE_WINDOW | tmp;
        SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_ACMD41 | 
            (0x1 << 6) | (0x0 << 8) | (0x1 << 10);

        
        status = sd_check_acmd41();
        if(SD_STATUS_OK != status)
        {
            return status;
        }
        sd_delay(0xFFF); //for stability
        
        response = SDIO->RESP1;
        //check bit pwr_up, work in normal voltage or not
        voltage = (((response >> 31) == 1) ? 1 : 0);
        count++;
#if 0
        /*if use the optimization to compile, it would cause error in sending
          AMD41, so it offers a stable way to send ACMD41 here.
        */
        SDIO_InitTypeDef sdio_cmd_t;
        sdio_cmd_t.SDIO_Argument = SD_CMD_ACMD41 | tmp;
        sdio_cmd_t.SDIO_CmdIndex = SD_CMD_SD_APP_OP_COND;
        sdio_cmd_t.SDIO_Response = SDIO_Response_Short;
        sdio_cmd_t.SDIO_Wait = SDIO_Wait_No;
        sdio_cmd_t.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&sdio_cmd_t);
#endif //#if 0
    }
    
    if(count >= SD_MAX_VOLT_TRIAL)
    {
        return SD_STATUS_INVALID_VOLTAGE;
    }

    //check bit ccs
    if(response &= SD_CHECK_SDSC_SDHC)
    {
        g_card_type = SD_CARD_TYPE_HIGH_CAPACITY;
    }

    return SD_STATUS_OK;	
}

static SD_STATUS sd_check_cmd0(void)
{
    uint32_t timeout = SD_CMD_TIMEOUT_TOTAL;

    while((timeout > 0) && (SDIO->STA & SDIO_STA_CMDSENT == 0))
    {
        timeout--;
    }
    
    if(!timeout)
    {
        return SD_STATUS_CMD_RSP_TIMEOUT;
    }

    //set bit0 ~ bit8, bit10
    SDIO->ICR = SDIO_ICR_STATIC_CLEAR;

    return SD_STATUS_OK;
}

static SD_STATUS sd_check_response7(void)
{
    uint32_t timeout = SD_CMD_TIMEOUT_TOTAL;

    while((timeout > 0) && 
        !(SDIO->STA & (SDIO_STA_CCRCFAIL | SDIO_STA_CMDREND | SDIO_STA_CTIMEOUT)))
    {
        timeout--;
    }

    //card is not V2.0 or card does not support the set voltage range
    if((!timeout) || (SDIO->STA & SDIO_STA_CTIMEOUT))
    {
        SDIO->ICR = SDIO_ICR_CTIMEOUTC;
        return SD_STATUS_CMD_RSP_TIMEOUT;
    }

    //card is SD V2.0
    if(SDIO->STA & SDIO_STA_CMDREND)
    {
        SDIO->ICR = SDIO_ICR_CMDRENDC;
        return SD_STATUS_OK;
    }

    return SD_STATUS_OK;
}

static SD_STATUS sd_check_response1(uint8_t cmd)
{
    uint32_t status = SDIO->STA;
    while(!(status & (SDIO_STA_CCRCFAIL | SDIO_STA_CMDREND | SDIO_STA_CTIMEOUT)))
    {
        status = SDIO->STA;
    }

    if(status & SDIO_STA_CTIMEOUT)
    {
        SDIO->ICR = SDIO_ICR_CTIMEOUTC;
        return SD_STATUS_CMD_RSP_TIMEOUT;
    }

    if(status & SDIO_STA_CCRCFAIL)
    {
        SDIO->ICR = SDIO_ICR_CCRCFAILC;
        return SD_STATUS_CMD_RSP_CCRCFAIL;
    }

    //check response received is of desired command
    if((uint8_t)(SDIO->RESPCMD) != cmd)
    {
        return SD_STATUS_ILLEGAL_CMD;
    }

    SDIO->ICR = SDIO_ICR_STATIC_CLEAR;

    if(SDIO->RESP1 & SD_OCR_ERROR_BIT)
    {
        return SD_STATUS_OCR_ERROR;
    }

    return SD_STATUS_OK;
}

static SD_STATUS sd_check_response3(void)
{
    uint32_t status = SDIO->STA;
    while(!(status & (SDIO_STA_CCRCFAIL | SDIO_STA_CMDREND | SDIO_STA_CTIMEOUT)))
    {
        status = SDIO->STA;
    }

    if(status & SDIO_STA_CTIMEOUT)
    {
        SDIO->ICR = SDIO_ICR_CTIMEOUTC;
        return SD_STATUS_CMD_RSP_TIMEOUT;
    }

    SDIO->ICR = SDIO_ICR_STATIC_CLEAR;

    return SD_STATUS_OK;
}

static SD_STATUS sd_check_response2(void)
{
    uint32_t status = SDIO->STA;
    while(!(status & (SDIO_STA_CCRCFAIL | SDIO_STA_CMDREND | SDIO_STA_CTIMEOUT)))
    {
        status = SDIO->STA;
    }

    if(status & SDIO_STA_CTIMEOUT)
    {
        SDIO->ICR = SDIO_ICR_CTIMEOUTC;
        return SD_STATUS_CMD_RSP_TIMEOUT;
    }

    if(status & SDIO_STA_CCRCFAIL)
    {
        SDIO->ICR = SDIO_ICR_CCRCFAILC;
        return SD_STATUS_CMD_RSP_CCRCFAIL;
    }

    SDIO->ICR = SDIO_ICR_STATIC_CLEAR;

    return SD_STATUS_OK;
}

static SD_STATUS sd_check_response6(uint8_t cmd, uint16_t *rca_ptr)
{
    uint32_t status = SDIO->STA;
    while(!(status & (SDIO_STA_CCRCFAIL | SDIO_STA_CMDREND | SDIO_STA_CTIMEOUT)))
    {
        status = SDIO->STA;
    }

    if(status & SDIO_STA_CTIMEOUT)
    {
        SDIO->ICR = SDIO_ICR_CTIMEOUTC;
        return SD_STATUS_CMD_RSP_TIMEOUT;
    }

    if(status & SDIO_STA_CCRCFAIL)
    {
        SDIO->ICR = SDIO_ICR_CCRCFAILC;
        return SD_STATUS_CMD_RSP_CCRCFAIL;
    }

    if((uint8_t)(SDIO->RESPCMD) != cmd)
    {
        return SD_STATUS_ILLEGAL_CMD;
    }

    SDIO->ICR = SDIO_ICR_STATIC_CLEAR;

    uint32_t response = SDIO->RESP1;
    if(response & (SD_CHECK_CMD3_COM_CRC_FAILED | SD_CHECK_CMD3_ILLEGAL_CMD | 
        SD_CHECK_CMD3_COM_CRC_FAILED))
    {
        return SD_STATUS_CHECK_CMD3_ERR;
    }

    *rca_ptr = (uint16_t)(response >> 16);
    
    return SD_STATUS_OK;
}

SD_STATUS sd_write_single_block(uint8_t *src_buf, 
    uint32_t target_addr, uint32_t block_size)
{
    SD_STATUS status = SD_STATUS_OK;
    
    g_sd_transfer_status = SD_STATUS_OK;
    g_sd_transfer_end = 0;
    g_sd_stop_condition = 0;
    SDIO->DCTRL = 0x0;
    SDIO->MASK |= SDIO_MASK_DCRCFAILIE | SDIO_MASK_DTIMEOUTIE | 
                  SDIO_MASK_DATAENDIE  | SDIO_MASK_RXOVERRIE  | 
                  SDIO_MASK_STBITERRIE;
    sd_dma_init((uint32_t)src_buf, SD_TRANSFER_WRITE);
    //SDIO_DMACmd(ENABLE);
    SDIO->DCTRL |= (0x1 << 3);
        
    if(SD_CARD_TYPE_HIGH_CAPACITY == g_card_type)
    {
        block_size = 512;
        target_addr >>= 9;     // <==> target_addr /= 512;
    }

    SDIO->ARG = block_size;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD16 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);
    
    status = sd_check_cmd16(SD_CMD_CMD16);
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    SDIO->ARG = target_addr;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD24 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);
    
    status = sd_check_cmd24(SD_CMD_CMD24);
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    SDIO->DTIMER = SD_TIMEOUT_TOTAL;
    SDIO->DLEN = block_size;
    SDIO->DCTRL = SDIO_DCTRL_CLEAR_MASK | 0x1 | (0x0 << 1) | (0x0 << 2) | (9 << 4);
   
    return SD_STATUS_OK;
}

SD_STATUS sd_write_multi_blocks(uint8_t *src_buf, 
    uint32_t target_addr, uint32_t block_size, uint32_t block_num)
{
    SD_STATUS status = SD_STATUS_OK;

    g_sd_transfer_status = SD_STATUS_OK;
    g_sd_transfer_end = 0;
    g_sd_stop_condition = 1;
    SDIO->DCTRL = 0x0;
    SDIO->MASK |= SDIO_MASK_DCRCFAILIE | SDIO_MASK_DTIMEOUTIE | 
                  SDIO_MASK_DATAENDIE  | SDIO_MASK_RXOVERRIE  | 
                  SDIO_MASK_STBITERRIE;
    sd_dma_init((uint32_t)src_buf, SD_TRANSFER_WRITE);
    //SDIO_DMACmd(ENABLE);
    SDIO->DCTRL |= (0x1 << 3);
        
    if(SD_CARD_TYPE_HIGH_CAPACITY == g_card_type)
    {
        block_size = 512;
        target_addr >>= 9;     // <==> target_addr /= 512;
    }

    SDIO->ARG = block_size;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD16 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);
    
    status = sd_check_cmd16(SD_CMD_CMD16);
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    SDIO->ARG = g_rca << 16;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD55 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);
    
    status = sd_check_cmd55(SD_CMD_CMD55);
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    SDIO->ARG = block_num;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD23 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);
    
    status = sd_check_cmd23(SD_CMD_CMD23);
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    SDIO->ARG = target_addr;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD25 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);
    
    status = sd_check_cmd25(SD_CMD_CMD25);
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    SDIO->DTIMER = SD_TIMEOUT_TOTAL;
    SDIO->DLEN = block_size*block_num;
    SDIO->DCTRL = SDIO_DCTRL_CLEAR_MASK | 0x1 | (0x0 << 1) | (0x0 << 2) | (9 << 4);

    return SD_STATUS_OK;
}

SD_STATUS sd_read_single_block(uint8_t *target_buf, 
    uint32_t src_addr, uint32_t block_size)
{
    SD_STATUS status = SD_STATUS_OK;
    
    g_sd_transfer_status = SD_STATUS_OK;
    g_sd_transfer_end = 0;
    g_sd_stop_condition = 0;
    SDIO->DCTRL = 0x0;
    SDIO->MASK |= SDIO_MASK_DCRCFAILIE | SDIO_MASK_DTIMEOUTIE | 
                  SDIO_MASK_DATAENDIE  | SDIO_MASK_RXOVERRIE  | 
                  SDIO_MASK_STBITERRIE;
    sd_dma_init((uint32_t)target_buf, SD_TRANSFER_READ);
    //SDIO_DMACmd(ENABLE);
    SDIO->DCTRL |= (0x1 << 3);
        
    if(SD_CARD_TYPE_HIGH_CAPACITY == g_card_type)
    {
        block_size = 512;
        src_addr >>= 9;     // <==> target_addr /= 512;
    }

    SDIO->ARG = block_size;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD16 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);
    
    status = sd_check_cmd16(SD_CMD_CMD16);
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    SDIO->DTIMER = SD_TIMEOUT_TOTAL;
    SDIO->DLEN = block_size;
    SDIO->DCTRL = SDIO_DCTRL_CLEAR_MASK | 0x1 | (0x1 << 1) | (0x0 << 2) | (9 << 4);

    SDIO->ARG = src_addr;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD17 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);
    
    status = sd_check_cmd17(SD_CMD_CMD17);
    if(SD_STATUS_OK != status)
    {
        return status;
    }
    
    return SD_STATUS_OK;
}

SD_STATUS sd_read_multi_blocks(uint8_t *target_buf, 
    uint32_t src_addr, uint32_t block_size, uint32_t block_num)
{
    SD_STATUS status = SD_STATUS_OK;
    
    g_sd_transfer_status = SD_STATUS_OK;
    g_sd_transfer_end = 0;
    g_sd_stop_condition = 1;
    SDIO->DCTRL = 0x0;
    SDIO->MASK |= SDIO_MASK_DCRCFAILIE | SDIO_MASK_DTIMEOUTIE | 
                  SDIO_MASK_DATAENDIE  | SDIO_MASK_RXOVERRIE  | 
                  SDIO_MASK_STBITERRIE;
    sd_dma_init((uint32_t)target_buf, SD_TRANSFER_READ);
    //SDIO_DMACmd(ENABLE);
    SDIO->DCTRL |= (0x1 << 3);
        
    if(SD_CARD_TYPE_HIGH_CAPACITY == g_card_type)
    {
        block_size = 512;
        src_addr >>= 9;     // <==> target_addr /= 512;
    }

    SDIO->ARG = block_size;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD16 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);
    
    status = sd_check_cmd16(SD_CMD_CMD16);
    if(SD_STATUS_OK != status)
    {
        return status;
    }

    SDIO->DTIMER = SD_TIMEOUT_TOTAL;
    SDIO->DLEN = block_size*block_num;
    SDIO->DCTRL = SDIO_DCTRL_CLEAR_MASK | 0x1 | (0x1 << 1) | (0x0 << 2) | (9 << 4);

    SDIO->ARG = src_addr;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD18 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);
    
    status = sd_check_cmd18(SD_CMD_CMD18);
    if(SD_STATUS_OK != status)
    {
        return status;
    }
    
    return SD_STATUS_OK;
}

static void sd_dma_init(uint32_t addr, SD_TRANSFER_WAY transfer_way)
{
    DMA_InitTypeDef dma_init_t;

    DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_FEIF3 | DMA_FLAG_DMEIF3 | 
                                DMA_FLAG_TEIF3 | DMA_FLAG_HTIF3  | 
                                DMA_FLAG_TCIF3);
    DMA_Cmd(DMA2_Stream3, DISABLE);
    DMA_DeInit(DMA2_Stream3);

    switch(transfer_way)
    {
        case SD_TRANSFER_WRITE :
            dma_init_t.DMA_DIR = DMA_DIR_MemoryToPeripheral;
            break;
        case SD_TRANSFER_READ :
            dma_init_t.DMA_DIR = DMA_DIR_PeripheralToMemory;
            break;
        default :
            while(1);
            //break;
    }

    dma_init_t.DMA_Channel = DMA_Channel_4;
    dma_init_t.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
    dma_init_t.DMA_Memory0BaseAddr = addr;
    dma_init_t.DMA_BufferSize = 1;
    dma_init_t.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_init_t.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_init_t.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    dma_init_t.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    dma_init_t.DMA_Mode = DMA_Mode_Normal;
    dma_init_t.DMA_Priority = DMA_Priority_VeryHigh;
    dma_init_t.DMA_FIFOMode = DMA_FIFOMode_Enable;
    dma_init_t.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    dma_init_t.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
    dma_init_t.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
    DMA_Init(DMA2_Stream3, &dma_init_t);
    DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);
    DMA_FlowControllerConfig(DMA2_Stream3, DMA_FlowCtrl_Peripheral);

    DMA_Cmd(DMA2_Stream3, ENABLE);
}

SD_STATUS sd_wait_operation(SD_TRANSFER_WAY transfer_way)
{  
    uint32_t timeout = SD_TIMEOUT_TOTAL;
    SD_STATUS status = SD_STATUS_OK;

    while((!g_dma_transfer_end) && (!g_sd_transfer_end) && 
          (g_sd_transfer_status == SD_STATUS_OK) && (timeout > 0))
    {
        timeout--;
    }

    g_dma_transfer_end = 0;
    timeout = SD_TIMEOUT_TOTAL;

    switch(transfer_way)
    {
        case SD_TRANSFER_WRITE : 
            while((SDIO->STA & SDIO_STA_TXACT) && (timeout > 0))
            {
                timeout--;
            }
            break;
        case SD_TRANSFER_READ :
            while((SDIO->STA & SDIO_STA_RXACT) && (timeout > 0))
            {
                timeout--;
            }
            break;
        default :
            return SD_STATUS_FATAL_ERR;
            //break;
    }

    if(1 == g_sd_stop_condition)
    {
        SDIO->ARG = 0;
        SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD12 | 
            (0x1 << 6) | (0x0 << 8) | (0x1 << 10);
    
        status = sd_check_cmd12(SD_CMD_CMD12);
        g_sd_stop_condition = 0;
    }

    if((!timeout) && (SD_STATUS_OK == status))
    {
        status = SD_STATUS_DATA_TIMEOUT;
    }

    if(SD_STATUS_OK != g_sd_transfer_status)
    {
        return g_sd_transfer_status;
    }
    else
    {
        return status;
    }
}


SD_STATUS sd_write_single_block(uint8_t *src_buf, 
    uint32_t target_addr, uint32_t block_size)
{
    SD_STATUS status = SD_STATUS_OK;
    g_sd_transfer_status = SD_STATUS_OK;
    g_sd_transfer_end = 0;
    g_sd_stop_condition = 0;
    SDIO->DCTRL = 0x0;
    SDIO->MASK |= SDIO_MASK_DCRCFAILIE | SDIO_MASK_DTIMEOUTIE | 
                  SDIO_MASK_DATAENDIE  | SDIO_MASK_RXOVERRIE  | 
                  SDIO_MASK_STBITERRIE;
    sd_dma_init((uint32_t)src_buf, SD_TRANSFER_WRITE);
    SDIO->DCTRL |= (0x1 << 3);
        
    if(SD_CARD_TYPE_HIGH_CAPACITY == g_card_type)
    {
        block_size = 512;
        target_addr >>= 9;     // <==> target_addr /= 512;
    }

    SDIO->ARG = block_size;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD16 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);
    status = sd_check_cmd16(SD_CMD_CMD16);
    if(SD_STATUS_OK != status)
        return status;

    SDIO->ARG = target_addr;
    SDIO->CMD = SDIO->CMD & SD_CMD_CLEAR_MASK | SD_CMD_CMD24 | 
        (0x1 << 6) | (0x0 << 8) | (0x1 << 10);
    status = sd_check_cmd24(SD_CMD_CMD24);
    if(SD_STATUS_OK != status)
        return status;

    SDIO->DTIMER = SD_TIMEOUT_TOTAL;
    SDIO->DLEN = block_size;
    SDIO->DCTRL = SDIO_DCTRL_CLEAR_MASK | 0x1 | (0x0 << 1) | (0x0 << 2) | (9 << 4);
    return SD_STATUS_OK;
}