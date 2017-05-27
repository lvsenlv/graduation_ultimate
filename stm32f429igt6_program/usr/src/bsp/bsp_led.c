/*************************************************************************
	> File Name: bsp_led.c
	> Author: lvsenlv
	> Mail: lvsen46000@163.com
	> Created Time: 2017年04月03日 星期一 15时00分20秒
 ************************************************************************/
  
#include "bsp_led.h"
#include "bsp_systick.h"

void led_init(void)
{
    RCC->AHB1ENR |= LED_GPIO_CLK;

    //parameters: port, pin, mode, out_type, speed, pull up or pull down
    gpio_init(GPIOH, LED_R_PIN, 0x1, 0x0, 0x2, 0x1);
    gpio_init(GPIOH, LED_G_PIN, 0x1, 0x0, 0x2, 0x1);
    gpio_init(GPIOH, LED_B_PIN, 0x1, 0x0, 0x2, 0x1);

    LED_R_OFF;
    LED_G_OFF;
    LED_B_OFF;
}

void led_test(void)
{
    uint32_t i = 3;
    while(i--)
    {
        LED_R_ON;
        sleep_ms(500);
        LED_R_OFF;
        sleep_ms(500);
        LED_G_ON;
        sleep_ms(500);
        LED_G_OFF;
        sleep_ms(500);
        LED_B_ON;
        sleep_ms(500);
        LED_B_OFF;
        sleep_ms(500);
    }
}

