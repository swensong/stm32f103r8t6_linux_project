/**
 * @file         task.c
 * @brief        一个用于简化main.c的初始化和循环的文件
 * @details      task_init用于配置main.c的初始化 		task_driver用于配置在main.c内的循环函数。
 * @author       文劲松
 * @date     date
 * @version  A001
 * @par Copyright (c):
 *       XXX公司
 * @par History:
 *   version: 2018.9.28
 */
#include "task.h"

#include "usart1.h"
#include "timer.h"
#include "led.h"
#include "adc_dma.h"

/**
 * 用于main函数的初始化
 * 初始化的先后次序要求
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
void task_init(void)
{
    u8 buf[] = "init success!\r\n";
    u8 send_adc_buf[3] = {0};
    u16 adc_data = 0;

    led_init();                 /* LED灯的初始化 */
    adc1_init();                /* ADC采集数据的初始化 */

    tim2_init();                /* 定时器2的初始化 */

    usart1_init(115200);        /* 串口的初始化 */

    adc_data = get_adc1_value();
    send_adc_buf[0] = (u8)(adc_data >> 8);
    send_adc_buf[1] = (u8)(adc_data);

    usart1_send_str(buf); /* 发送初始化成功的串口字符 */
    usart1_send_str_len(send_adc_buf, 2);
}

/**
 * 用于main函数的死循环内
 * 死循环的执行函数
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
void task_driver(void)
{
    usart1_driver();
}
