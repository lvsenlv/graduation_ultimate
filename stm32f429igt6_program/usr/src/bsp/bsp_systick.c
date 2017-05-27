/*************************************************************************
	> File Name: bsp_systick.c
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年04月04日 星期二 20时25分21秒
 ************************************************************************/
  
#include "bsp_systick.h"

static __IO uint32_t delay_factor;
__IO uint64_t g_systick_count = 0;

void systick_init(void)
{
    if (INTERRUPT_BASE_FREQUENCE > 0xFFFFFF)
    {
        while(1);
    }
    
    SCB->SHP[11] = ((0xF) << 4) & 0xFF;
    SysTick->LOAD = (INTERRUPT_BASE_FREQUENCE);
    SysTick->VAL = 0;
    SysTick->CTRL = 0x7;
}

void delay_base(__IO uint32_t count)
{
    delay_factor = count;
    while(0 != delay_factor);
}

void SysTick_Handler(void)
{
    g_systick_count++;
    
    if(0 != delay_factor)
    {
        delay_factor--;
    }
}
