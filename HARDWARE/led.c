/**
 * @file         led.c
 * @brief        一个用来配置LED灯初始化和各种操作的文件
 * @details      led灯的亮灭循环由定时器控制
 * @author       文劲松
 * @date     date
 * @version  A001
 * @par Copyright (c):
 *       XXX公司
 * @par History:
 *   version: 2018.9.28
 */
#include "led.h"

#include "systick.h"

/**
 * led灯的初始化函数
 *
 * @param[in]   无
 * @param[out]  无
 * @retval  OK  成功
 * @retval  ERROR   错误
 * @par 标识符
 *      保留
 * @par 其它
 *      无
 * @par 修改日志
 *      文劲松于2018-09-28创建
 */
void led_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    LED = 0;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    LED1 = 0;
}

/**
 * led灯的闪烁功能
 * led的变化时间要求定时器的定时时间恒定为1ms, 放入定时器后函数才能工作
 * @param[in]   time的闪烁的频率, time 为毫秒的单位
                led_enable = 1时候才能工作
 * @param[out]  无
 * @retval  OK  成功
 * @retval  ERROR   错误
 * @par 标识符
 *      保留
 * @par 其它
 *      无
 * @par 修改日志
 *      文劲松于2018-09-28创建
 */
void led_flash_light(u16 time, u8 led_enable)
{
    static u16 time_cnt = 0;

    if (1 == led_enable)
    {
        if ( time_cnt++ >  time)    /* 每次进入定时器都自动加一 */
        {
            time_cnt = 0;           /* 将计数清零 */
            LED = ~LED;             /* led灯的状态发生转换 */
        }
    }
}

void led_hc_sr04(void)
{
    LED1 = 0;
    delay_us(11);
    LED1 = 1;
    LED1 = 0;
}


