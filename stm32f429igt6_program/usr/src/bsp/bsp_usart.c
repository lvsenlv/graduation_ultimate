/*************************************************************************
	> File Name: bsp_usart.c
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年04月03日 星期一 15时05分36秒
 ************************************************************************/

#include "bsp_usart.h"
#include <stdio.h>

void usart_init(void)
{
    RCC->AHB1ENR |= 0x1;
    RCC->APB2ENR |= (0x1 << 4);

    gpio_af_init(GPIOA, USART1_TX_PIN, AF7);
    gpio_af_init(GPIOA, USART1_RX_PIN, AF7);
    gpio_init(GPIOA, USART1_TX_PIN, 0x2, 0, 0x2, 0x1);
    gpio_init(GPIOA, USART1_RX_PIN, 0x2, 0, 0x2, 0x1);

    USART1->CR1 &= ~ USART1_CR1_CLEAR_MASK;
    USART1->CR2 &= ~ USART1_CR2_CLEAR_MASK;
    USART1->CR3 &= ~ USART1_CR3_CLEAR_MASK;
    USART1->CR1 |= (USART1_MODE << 2) | (USART1_PARITY << 9) | 
                   (USART1_WORD_LEN << 12);
    USART1->CR2 |= (USART1_STOP_BIT << 12);
    USART1->CR3 |= (USART1_HARD_FLOW << 8);

    float usart_div = (USART1_CLK_FREQUENCE) / (USART1_BAUDRATE * 16.0f);
    uint16_t interger = (uint16_t)usart_div;
    uint16_t fraction = (uint16_t)((usart_div - interger)*16 + 0.5f);

    USART1->BRR = (interger << 4) | fraction;
    
    USART1->CR1 |= (0x1 << 13);
}

int fputc(int ch, FILE *fp)
{
    USART1->DR = (uint16_t)(((uint8_t)ch) & 0x1FF);
    while ((USART1->SR & USART_SR_BIT_TXE) == 0);		

    return (ch);
}

int fgetc(FILE *fp)
{
    while ((USART1->SR & USART_SR_BIT_RXNE) == 0);

    return (int)(USART1->DR & 0x1FF);
}

