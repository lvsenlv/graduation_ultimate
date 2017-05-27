/*************************************************************************
	> File Name: bsp_sdram.c
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年04月02日 星期二 22时23分20秒
 ************************************************************************/
 
#include "bsp_sdram.h"

static void sdram_gpio_init(void);

#if 0
static void SDRAM_delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = (100000 * nCount); index != 0; index--);
}
#endif

void sdram_init(void)
{
    sdram_gpio_init();

    RCC->AHB3ENR |= 0x1;

    FMC_Bank5_6->SDCR[0] &= ~ (uint32_t)0x7FFF;
    FMC_Bank5_6->SDCR[0] |=   (SDRAM_SDCLK_PERIOD << 10) | 
                              (SDRAM_READ_BURST   << 12) | 
                              (SDRAM_READ_PIPE    << 13);
    FMC_Bank5_6->SDCR[1] &= ~ (uint32_t)0x7FFF;
    FMC_Bank5_6->SDCR[1] |=   (SDRAM_COL_NUM          << 0)  | 
                              (SDRAM_ROW_NUM          << 2)  | 
                              (SDRAM_DATA_WIDTH       << 4)  | 
                              (SDRAM_BANK_NUM         << 6)  | 
                              (SDRAM_CAS_LATENCY      << 7)  | 
                              (SDRAM_WRITE_PROTECTION << 9)  |
                              (SDRAM_SDCLK_PERIOD     << 10) | 
                              (SDRAM_READ_BURST       << 12) | 
                              (SDRAM_READ_PIPE        << 13);

    FMC_Bank5_6->SDTR[0] &= ~ (uint32_t)0xFFFFFFF;
    FMC_Bank5_6->SDTR[0] |=   (SDRAM_TRC << 12) |
                              (SDRAM_TRP << 20);
    
    FMC_Bank5_6->SDTR[1] &= ~ (uint32_t)0xFFFFFFF;
    FMC_Bank5_6->SDTR[1] |=   (SDRAM_TMRD << 0) | 
                              (SDRAM_TXSR << 4) | 
                              (SDRAM_TRAS << 8) | 
                              (SDRAM_TWR  << 16); 

    //enable clock
    while((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) == FMC_SDSR_BUSY);
    FMC_Bank5_6->SDCMR = 0x1 | (0x1 << 3) | (0x0 << 5) | (0x0 << 9);
    
    //sdram_delay(0xFFF);

    //precharge
    while((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) == FMC_SDSR_BUSY);
    FMC_Bank5_6->SDCMR = 0x2 | (0x1 << 3) | (0x0 << 5) | (0x0 << 9);

    //auto refresh
    while((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) == FMC_SDSR_BUSY);
    FMC_Bank5_6->SDCMR = 0x3 | (0x1 << 3) | (0x1 << 5) | (0x0 << 9);

    //load mode register
    while((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) == FMC_SDSR_BUSY);
    uint32_t tmp = SDRAM_MODE_REG_BURST_LENGTH_4          | 
                   SDRAM_MODE_REG_CAS_LATENCY_2           | 
                   SDRAM_MODE_REG_OPERATING_MODE_STANDARD | 
                   SDRAM_MODE_REG_WRITEBURST_MODE_SINGLE;
    FMC_Bank5_6->SDCMR = 0x4 | (0x1 << 3) | (0x0 << 5) | (tmp << 9);

    FMC_Bank5_6->SDRTR |= (1386 << 1);
    while((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) == FMC_SDSR_BUSY);
}

static void sdram_gpio_init(void)
{
    //bit2 ~ bit6
    RCC->AHB1ENR |= 0xFC;

    gpio_af_init(GPIOF, FMC_A0_PIN,   AF12);
    gpio_af_init(GPIOF, FMC_A1_PIN,   AF12);
    gpio_af_init(GPIOF, FMC_A2_PIN,   AF12);
    gpio_af_init(GPIOF, FMC_A3_PIN,   AF12);
    gpio_af_init(GPIOF, FMC_A4_PIN,   AF12);
    gpio_af_init(GPIOF, FMC_A5_PIN,   AF12);
    gpio_af_init(GPIOF, FMC_A6_PIN,   AF12);
    gpio_af_init(GPIOF, FMC_A7_PIN,   AF12);
    gpio_af_init(GPIOF, FMC_A8_PIN,   AF12);
    gpio_af_init(GPIOF, FMC_A9_PIN,   AF12);
    gpio_af_init(GPIOG, FMC_A10_PIN,  AF12);
    gpio_af_init(GPIOG, FMC_A11_PIN,  AF12);

    gpio_af_init(GPIOD, FMC_D0_PIN,   AF12);
    gpio_af_init(GPIOD, FMC_D1_PIN,   AF12);
    gpio_af_init(GPIOD, FMC_D2_PIN,   AF12);
    gpio_af_init(GPIOD, FMC_D3_PIN,   AF12);
    gpio_af_init(GPIOE, FMC_D4_PIN,   AF12);
    gpio_af_init(GPIOE, FMC_D5_PIN,   AF12);
    gpio_af_init(GPIOE, FMC_D6_PIN,   AF12);
    gpio_af_init(GPIOE, FMC_D7_PIN,   AF12);
    gpio_af_init(GPIOE, FMC_D8_PIN,   AF12);
    gpio_af_init(GPIOE, FMC_D9_PIN,   AF12);
    gpio_af_init(GPIOE, FMC_D10_PIN,  AF12);
    gpio_af_init(GPIOE, FMC_D11_PIN,  AF12);
    gpio_af_init(GPIOE, FMC_D12_PIN,  AF12);
    gpio_af_init(GPIOD, FMC_D13_PIN,  AF12);
    gpio_af_init(GPIOD, FMC_D14_PIN,  AF12);
    gpio_af_init(GPIOD, FMC_D15_PIN,  AF12);

    gpio_af_init(GPIOH, FMC_CS_PIN,   AF12);
    gpio_af_init(GPIOG, FMC_BA1_PIN,  AF12);
    gpio_af_init(GPIOG, FMC_BA0_PIN,  AF12);
    gpio_af_init(GPIOC, FMC_WE_PIN,   AF12);
    gpio_af_init(GPIOF, FMC_RAS_PIN,  AF12);
    gpio_af_init(GPIOG, FMC_CAS_PIN,  AF12);
    gpio_af_init(GPIOG, FMC_CLK_PIN,  AF12);
    gpio_af_init(GPIOH, FMC_CKE_PIN,  AF12);
    gpio_af_init(GPIOE, FMC_UDQM_PIN, AF12);
    gpio_af_init(GPIOE, FMC_LDQM_PIN, AF12);
    
    gpio_init(GPIOF, FMC_A0_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOF, FMC_A1_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOF, FMC_A2_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOF, FMC_A3_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOF, FMC_A4_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOF, FMC_A5_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOF, FMC_A6_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOF, FMC_A7_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOF, FMC_A8_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOF, FMC_A9_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOG, FMC_A10_PIN,  0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOG, FMC_A11_PIN,  0x2, 0x0, 0x2, 0x0);

    gpio_init(GPIOD, FMC_D0_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOD, FMC_D1_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOD, FMC_D2_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOD, FMC_D3_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOE, FMC_D4_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOE, FMC_D5_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOE, FMC_D6_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOE, FMC_D7_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOE, FMC_D8_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOE, FMC_D9_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOE, FMC_D10_PIN,  0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOE, FMC_D11_PIN,  0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOE, FMC_D12_PIN,  0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOD, FMC_D13_PIN,  0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOD, FMC_D14_PIN,  0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOD, FMC_D15_PIN,  0x2, 0x0, 0x2, 0x0);

    gpio_init(GPIOH, FMC_CS_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOG, FMC_BA1_PIN,  0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOG, FMC_BA0_PIN,  0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOC, FMC_WE_PIN,   0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOF, FMC_RAS_PIN,  0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOG, FMC_CAS_PIN,  0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOG, FMC_CLK_PIN,  0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOH, FMC_CKE_PIN,  0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOE, FMC_UDQM_PIN, 0x2, 0x0, 0x2, 0x0);
    gpio_init(GPIOE, FMC_LDQM_PIN, 0x2, 0x0, 0x2, 0x0);
}

void sdram_entire_reset(void)
{
    uint32_t i = 0;
    uint8_t *ptr = (uint8_t *)SDRAM_BANK_ADDR;
    for(i = 0; i < IS42S16400J_SIZE; i++)
    {
        *ptr++ = 0;
    }
}

_G_STATUS sdram_entire_test(void)
{
    uint32_t i = 0;
    uint32_t *ptr = (uint32_t *)SDRAM_BANK_ADDR;
    uint32_t size = IS42S16400J_SIZE / sizeof(uint32_t);
    for(i = 0; i < size; i++)
    {
        *ptr++ = i;
    }

    ptr = (uint32_t *)SDRAM_BANK_ADDR;
    for(i = 0; i < size; i++)
    {
        if(*ptr++ != i)
        {
            DISP_ERR_PLUS("[%s][%d]: error in %d \n", __func__, __LINE__, i);
            return STAT_ERR;
        }
    }

    return STAT_OK;
}

#if 0
void sdram_init(void)
{
    sdram_gpio_init();
    RCC->AHB3ENR |= 0x1;

    FMC_Bank5_6->SDCR[0] &= ~ (uint32_t)0x7FFF;
    FMC_Bank5_6->SDCR[0] |=   (SDRAM_SDCLK_PERIOD << 10) | 
                              (SDRAM_READ_BURST   << 12) | 
                              (SDRAM_READ_PIPE    << 13);
    FMC_Bank5_6->SDCR[1] &= ~ (uint32_t)0x7FFF;
    FMC_Bank5_6->SDCR[1] |=   (
        (SDRAM_COL_NUM      << 0)  | (SDRAM_ROW_NUM  << 2)          | 
        (SDRAM_DATA_WIDTH   << 4)  | (SDRAM_BANK_NUM << 6)          | 
        (SDRAM_CAS_LATENCY  << 7)  | (SDRAM_WRITE_PROTECTION << 9)  |
        (SDRAM_SDCLK_PERIOD << 10) | (SDRAM_READ_BURST << 12)       | 
        (SDRAM_READ_PIPE    << 13) );

    FMC_Bank5_6->SDTR[0] &= ~ (uint32_t)0xFFFFFFF;
    FMC_Bank5_6->SDTR[0] |=   (SDRAM_TRC << 12) | (SDRAM_TRP << 20);
    FMC_Bank5_6->SDTR[1] &= ~ (uint32_t)0xFFFFFFF;
    FMC_Bank5_6->SDTR[1] |=   (SDRAM_TMRD << 0) | (SDRAM_TXSR << 4) | 
                              (SDRAM_TRAS << 8) | (SDRAM_TWR  << 16); 
    //enable clock, precharge, auto refresh, load mode register
    while((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) == FMC_SDSR_BUSY);
    FMC_Bank5_6->SDCMR = 0x1 | (0x1 << 3) | (0x0 << 5) | (0x0 << 9);
    while((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) == FMC_SDSR_BUSY);
    FMC_Bank5_6->SDCMR = 0x2 | (0x1 << 3) | (0x0 << 5) | (0x0 << 9);
    while((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) == FMC_SDSR_BUSY);
    FMC_Bank5_6->SDCMR = 0x3 | (0x1 << 3) | (0x1 << 5) | (0x0 << 9);
    while((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) == FMC_SDSR_BUSY);

    FMC_Bank5_6->SDCMR = 0x4 | (0x1 << 3) | (0x0 << 5) | 
                        (SDRAM_MODE_REG_SETTING_VALUE << 9);
    FMC_Bank5_6->SDRTR |= (1386 << 1);
    while((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) == FMC_SDSR_BUSY);
}
#endif
