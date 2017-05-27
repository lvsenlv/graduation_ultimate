/*************************************************************************
	> File Name: bsp_systick.h
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年04月04日 星期二 20时25分25秒
 ************************************************************************/

#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"
#include "common.h"

#define INTERRUPT_BASE_FREQUENCE            MS_1

#define US_1                                (SYSTEM_CORE_CLOCK  / 1000000)
#define US_10                               (SYSTEM_CORE_CLOCK  / 100000)
#define US_100                              (SYSTEM_CORE_CLOCK  / 10000)
#define MS_1                                (SYSTEM_CORE_CLOCK  / 1000)
#define MS_10                               (SYSTEM_CORE_CLOCK  / 100)

#if (INTERRUPT_BASE_FREQUENCE == US_1)
    #define sleep_us(count)                 delay_base(count);
    #define sleep_ms(count)                 delay_base(count*1000);
    #define sleep(count)                    delay_base(count*1000000);
#elif (INTERRUPT_BASE_FREQUENCE == US_10)
    #define sleep_us(count)                 delay_base(count/10);
    #define sleep_ms(count)                 delay_base(count*100);
    #define sleep(count)                    delay_base(count*100000);
#elif (INTERRUPT_BASE_FREQUENCE == US_100)
    #define sleep_us(count)                 delay_base(count/100);
    #define sleep_ms(count)                 delay_base(count*10);
    #define sleep(count)                    delay_base(count*10000);
#elif (INTERRUPT_BASE_FREQUENCE == MS_1)
    #define sleep_us(count)                 delay_base(count/1000);
    #define sleep_ms(count)                 delay_base(count);
    #define sleep(count)                    delay_base(count*1000);
#elif (INTERRUPT_BASE_FREQUENCE == MS_10)
    #define sleep_us(count)                 delay_base(count/10000);
    #define sleep_ms(count)                 delay_base(count/10);
    #define sleep(count)                    delay_base(count*100);
#endif


extern __IO uint64_t g_systick_count;

void systick_init(void);
void delay_base(__IO uint32_t count);
void SysTick_Handler(void);


#endif //__SYSTICK_H

