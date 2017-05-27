/*************************************************************************
	> File Name: bsp_usart.h
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年04月03日 星期一 15时05分40秒
 ************************************************************************/

#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f4xx.h"

#define USART1_TX_PIN                       9
#define USART1_RX_PIN                       10
#define USART1_CLK_FREQUENCE                ((uint32_t)SYSTEM_CORE_CLOCK / 2)

#define USART1_CR1_CLEAR_MASK               ((uint16_t)0xE9F3)
#define USART1_CR2_CLEAR_MASK               ((uint16_t)0X3 << 12)
#define USART1_CR3_CLEAR_MASK               ((uint16_t)0X3 << 8)

#define USART1_MODE                         0x3
#define USART1_PARITY                       0x0
#define USART1_WORD_LEN                     0x0
#define USART1_STOP_BIT                     0x0
#define USART1_HARD_FLOW                    0x0
#define USART1_BAUDRATE                     ((uint32_t)115200)

#define USART_SR_BIT_TXE                    (0x1 << 7)
#define USART_SR_BIT_RXNE                   (0x1 << 5)

void usart_init(void);

#endif

