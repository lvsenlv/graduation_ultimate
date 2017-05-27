/*************************************************************************
	> File Name: bsp_led.h
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年04月03日 星期一 15时00分10秒
 ************************************************************************/
 
#ifndef __LED_H
#define	__LED_H

#include "stm32f4xx.h"

#define LED_R_PIN                           10
#define LED_G_PIN                           11
#define LED_B_PIN                           12
#define LED_GPIO_PORT                       GPIOH
#define LED_GPIO_CLK                        ((uint32_t)0x80) //GPIOH -> bit7
#define LED_GPIO_OUT_SPEED                  0x1 //0x1 -> 2MHZ, 0x2 -> 50MHZ, ...

#define HIG(pin)                            (LED_GPIO_PORT->BSRRL = (uint16_t)(0x1 << pin))
#define LOW(pin)                            (LED_GPIO_PORT->BSRRH = (uint16_t)(0x1 << pin))
#define TOGGLE(pin)                         (LED_GPIO_PORT->ODR ^= (uint16_t)(0x1 << pin))

#define LED_R_ON                            LOW(LED_R_PIN)
#define LED_R_OFF                           HIG(LED_R_PIN)
#define LED_R_TOGGLE                        TOGGLE(LED_R_PIN)

#define LED_G_ON                            LOW(LED_G_PIN)
#define LED_G_OFF                           HIG(LED_G_PIN)
#define LED_G_TOGGLE                        TOGGLE(LED_G_PIN)

#define LED_B_ON                            LOW(LED_B_PIN)
#define LED_B_OFF                           HIG(LED_B_PIN)
#define LED_B_TOGGLE                        TOGGLE(LED_B_PIN)

#define LED_STAT_ERR                        {LED_G_OFF;LED_B_OFF;LED_R_ON;}
#define LED_STAT_OK                         {LED_R_OFF;LED_B_OFF;LED_G_ON;}

void led_init(void);
void led_test(void);

#endif
