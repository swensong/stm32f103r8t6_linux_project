/**
 * @file         usart1.c
 * @brief        一个初始化串口1的文件，包含串口的部分接收文件
 * @details
 * @author       文劲松
 * @date     date
 * @version  A001
 * @par Copyright (c):
 *       XXX公司
 * @par History:
 *   version: 2018.9.28
 */
#include "usart1.h"
#include "led.h"
#include "timer.h"
#include "systick.h"


/* 初始化时钟 */
void USART1_RCC_Configuration(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

/* 初始化串口一的对应IO口 */
void USART1_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/* 配置串口中断优先级 */
void USART1_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * 串口1的初始化函数
 * @param[in]   baud -- 串口文件的波特率
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
void usart1_init(u32 baud)
{
    USART_InitTypeDef USART_InitStructure;

    USART1_NVIC_Configuration();
    USART1_RCC_Configuration();
    USART1_GPIO_Configuration();

    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); /* 接收中断初始化函数 */
    USART_Cmd(USART1, ENABLE);
    USART_ClearFlag(USART1, USART_FLAG_TC); /* 清楚接收中断标志位 */
}

/**
 * 串口1发送字符串的函数
 * 当字符串遇到'\0'时候就会自动退出发送
 * 这是基于c语言的数组文件的特点确定的。
 * @param[in]   *data 发送的数组
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
void usart1_send_str(u8 *data)
{
    while(*data)
    {
        /* Wait until end of transmit */
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

        /* Write one byte in the USART1 Transmit Data Register */
        USART_SendData(USART1, *data);
        data++;
    }
}

/**
 * 串口1发送字符串的函数
 * 可发送含有'\0'文件的数组
 * @param[in]   *data 发送的数组, len 数组的长度
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
void usart1_send_str_len(u8 *data, u8 len)
{
    while(len--)
    {
        /* Wait until end of transmit */
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

        /* Write one byte in the USART1 Transmit Data Register */
        USART_SendData(USART1, *data);
        data++;
    }
}

/********************** 数据接收处理的部分 **************************/
#define USART1_FIFO_BUF_SIZE   128 /* 队列最多存储128个数据缓存 */
#define USART1_FIFO_BUF_SIZE_MASK (USART1_FIFO_BUF_SIZE-1)

/* 接收完成标志位 */
u8 flag_frame = 0;
/* 缓存区的数组 */
unsigned char usart1_rx_fifo_buf[USART1_FIFO_BUF_SIZE];
/* 缓冲写入缓存的位置 */
unsigned int usart1_rx_fifo_buf_in = 0;
/* 缓冲读取到的位置 */
unsigned int usart1_rx_fifo_buf_out = 0;

/* 返回剩余的未读取的长度 */
u16 usart1_rx_fifo_len(void)
{
    return (usart1_rx_fifo_buf_in - usart1_rx_fifo_buf_out);
}

/* 判断是否数组已经读取完成了，读完了返回1， 未读完返回0 */
u8 usart1_rx_fifo_is_empty(void)
{
    return (usart1_rx_fifo_buf_in == usart1_rx_fifo_buf_out);
}

/* 判断数组写入的数据是否超出数组，如果超出返回0， 如果没有超出返回1 */
u8 usart1_rx_fifo_is_full(void)
{
    if (usart1_rx_fifo_len() > USART1_FIFO_BUF_SIZE_MASK)
        return 1;
    else
        return 0;
}

void usart1_rx_fifo_clean(void)
{
    if (0 == usart1_rx_fifo_is_empty())
        usart1_rx_fifo_buf_out = usart1_rx_fifo_buf_in;
}

/* 这个函数是单个写入函数，每运行一次将一个字节的数据存入队列 */
u16 usart1_rx_fifo_put(u8 data)
{
	if(usart1_rx_fifo_is_full())
    {
		return -1;
	}
    else
    {
		usart1_rx_fifo_buf[usart1_rx_fifo_buf_in & USART1_FIFO_BUF_SIZE_MASK] = data;
		usart1_rx_fifo_buf_in++;

		return usart1_rx_fifo_buf_in;
	}
}

/* 这个函数每次都将数据通过给的地址发送出去 */
u16 usart1_rx_fifo_get(u8 * data)
{
	if(usart1_rx_fifo_is_empty())
    {
		return -1;
	}
    else
    {
		*data = usart1_rx_fifo_buf[usart1_rx_fifo_buf_out & USART1_FIFO_BUF_SIZE_MASK];
		usart1_rx_fifo_buf_out++;

		return usart1_rx_fifo_buf_out;
	}
}

/**
 * 将从串口1读取的数据发送到写入的数组
 * @param[in]   *data 一个用于存储读取到的数据的数组
 * @param[out]  栈在前面的位置
 * @retval  OK  成功
 * @retval  ERROR   错误
 * @par 标识符
 *      保留
 * @par 其它
 *      无
 * @par 修改日志
 *      文劲松于2018-09-28创建
 */
int usart1_get_str(unsigned char* data)
{
    while (usart1_rx_fifo_buf_out != usart1_rx_fifo_buf_in)
    {
        *data = usart1_rx_fifo_buf[usart1_rx_fifo_buf_out & USART1_FIFO_BUF_SIZE_MASK];
        usart1_rx_fifo_buf_out++;

        data++;
    }

    return usart1_rx_fifo_buf_out;
}

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        usart1_rx_fifo_put(USART_ReceiveData(USART1));
    }
}

/**
 * 这个函数用于判断接收数组是否结束
 * 这个函数用于一个 1ms定时的定时器中， 当接收完成后，过30ms可判断接收完成
 * @param[in]   time 判定接收时间的长度
 当time = 1是， 30ms判定
 当time = 2是， 60ms判定

 * @param[out]  栈在前面的位置
 * @retval  OK  成功
 * @retval  ERROR   错误
 * @par 标识符
 *      保留
 * @par 其它
 *      无
 * @par 修改日志
 *      文劲松于2018-09-28创建
 */
void usart1_rx_monitor(u16 time)
{
    static u16 cnt_bkp = 0;     /* 作为记录之前一次长度的存储区 */
    static u16 idle_tmr = 0;    /* 没有接受到数据的时间 */


    if (usart1_rx_fifo_len() > 0) /* 接收到了数据 */
    {
        /* 接收到的数据长度于之前比较不相等 */
        /* 说明接受到串口数据了 */
        if (cnt_bkp != usart1_rx_fifo_len())
        {
            cnt_bkp = usart1_rx_fifo_len();
            /* 清零空闲计数 */
            idle_tmr = 0;
        }
        else
        {
            /* 说明没有接受到串口数据了 */
            if (idle_tmr < 30)
            {
                /* 空闲计数增加 */
                idle_tmr += time;
                /* 当连续30ms没有接受到数据时候，判定接收数据已经结束 */
                if (idle_tmr >= 30)
                {
                    /* 标志位 */
                    flag_frame = 1;
                }
            }
        }
    }
    else
    {
        /* 接受长度清零 */
        cnt_bkp = 0;
    }
}

/**
 * 这个函数用于读取指定长度的缓存区字符
 * 当接收到的字符长于所要读得字符，将会将多余的字符一起读取
 * @param[in]   buf 存储字符的数组  len 想要读取的长度
 * @param[out]  具体读取到的长度
 * @retval  OK  成功
 * @retval  ERROR   错误
 * @par 标识符
 *      保留
 * @par 其它
 *      无
 * @par 修改日志
 *      文劲松于2018-09-28创建
 */
u16 usart1_read_str(u8 *buf, u16 len)
{
    if (len > usart1_rx_fifo_len())
    {
        len = usart1_rx_fifo_len();
    }

    usart1_get_str(buf);

    return len;
}

/**
 * 这个函数作为主函数下的驱动使用
 * 使用这个函数必须要放在task_driver下使用
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
void usart1_driver(void)
{
    u16 len = 0;
    u8 buf[40];

    if (1 == flag_frame)
    {
        flag_frame = 0;
        len = usart1_read_str(buf, sizeof(buf));
        buf[len] = '\0';
        usart1_action(buf, len);
    }
}

/**
 * 这个函数比对两个数组指定长度的字符串
 * @param[in]
 ptr1 接收字符串1
 ptr2 接收字符串2
 len  字符串比较长度

 * @param[out]  接收成功的标志位
 * @retval  1   成功
 * @retval  0   错误
 * @par 标识符
 *      保留
 * @par 其它
 *      无
 * @par 修改日志
 *      文劲松于2018-09-28创建
 */
u8 Cmp_Memory( u8 *ptr1, u8 *ptr2, u16 len )
{
    while ( len-- )
    {
        if ( *ptr1++ != *ptr2++ )
        {
            return 0;
        }
    }

    return 1;
}

/**
 * 这个函数用于将字符串转化成相对应的命令
 * @param[in]   buf 为数组 len 为数组长度
 * @param[out]  栈在前面的位置
 * @retval  OK  成功
 * @retval  ERROR   错误
 * @par 标识符
 *      保留
 * @par 其它
 *      无
 * @par 修改日志
 *      文劲松于2018-09-28创建
 */
void usart1_action(u8 *buf, u8 len)
{
    u16 temp = 0;
    /*
      实验原因1：是否是因为无法触发才无法使用，
      实验过程：多次使用串口触发功能，均无法触发接收。
     */
    /*
      实验原因2：是否因为模块电量不足无法触发
      实验结果：超声波能够正常触发中断，但是触发时间过短
     */
    usart1_send_str(buf);

    if (buf[0] == 'a')
    {
        temp = get_tim4_time();
        usart1_send_str("the value is :");
        usart1_send_str_u16(temp);

    }
    else if (buf[0] == 'b')
    {
    }
    else if (buf[0] == 'c')
    {
    }
    else if (buf[0] == 'd')
    {
    }

}

/**
 * 串口1发送u16型号的数据的函数
 * 发送自带换行
 * @param[in]   *data 发送的数组
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
void usart1_send_str_u16(u16 data)
{
    u8 buf[9];

    buf[0] = data / 10000 % 10 + '0';
    buf[1] = data / 1000 % 10 + '0';
    buf[2] = data / 100 % 10 + '0';
    buf[3] = data / 10 % 10 + '0';
    buf[4] = data % 10 + '0';
    buf[5] = '\r';
    buf[6] = '\n';
    buf[7] = '\0';

    usart1_send_str(buf);
}
