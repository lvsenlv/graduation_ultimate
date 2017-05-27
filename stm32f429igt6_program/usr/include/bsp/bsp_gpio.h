/*************************************************************************
	> File Name: bsp_gpio.h
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年05月11日 星期四 16时38分03秒
 ************************************************************************/

#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#include "stm32f4xx.h"

#define AF0                                 0x0
#define AF1                                 0x1
#define AF2                                 0x2
#define AF3                                 0x3
#define AF4                                 0x4
#define AF5                                 0x5
#define AF6                                 0x6
#define AF7                                 0x7
#define AF8                                 0x8
#define AF9                                 0x9
#define AF10                                0xA
#define AF11                                0xB
#define AF12                                0xC
#define AF13                                0xD
#define AF14                                0xE
#define AF15                                0xF

/*************************************************************************
gpio:   GPIOA ~ GPIOI
pin:    0 ~ 15
mode: 
        00 input                            01 output       
        10：alternate function              11：analog
out_type: 
        0: push-pull                        1: open-drain
speed: 
        00: low speed                       01: medium speed
        10: high speed                      11: very high speed
pull_up_down: 
        00: no pull-up and pull-down        01: pull-up
        10: pull-down
 ************************************************************************/
static inline void gpio_init(GPIO_TypeDef *gpio, uint8_t pin, 
    uint8_t mode, uint8_t out_type, uint8_t speed, uint8_t pull_up_down)
{
    gpio->MODER &= ~ (0x3 << (pin*2));
    gpio->MODER |=   (mode << (pin*2));

    gpio->OTYPER &= ~ (0x1 << pin);
    gpio->OTYPER |=   (out_type << pin);

    gpio->OSPEEDR &= ~ (0x3 << (pin*2));
    gpio->OSPEEDR |=   (speed << (pin*2));

    gpio->PUPDR &= ~ (0x3 << (pin*2));
    gpio->PUPDR |=   (pull_up_down << (pin*2));
}

//gpio alternate function
static inline void gpio_af_init(GPIO_TypeDef *gpio, 
    uint8_t pin, uint8_t af_label)
{
    gpio->AFR[pin >> 0x3] &= ~ (0xF << ((pin&0x7) * 4));
    gpio->AFR[pin >> 0x3] |=   (af_label << ((pin&0x7) * 4));
}

#endif
