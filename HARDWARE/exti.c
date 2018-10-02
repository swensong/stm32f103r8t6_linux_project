
#include "exti.h"

#include "usart1.h"

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    /* 配置中断源 */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  配置 PB4 为线中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void EXTI_PB4_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    /* config the extiline clock and AFIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);

    /* config the NVIC */
    NVIC_Configuration();

/* EXTI line gpio config*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   // 下拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* EXTI line mode config */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //下降沿中断
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void EXTI4_IRQHandler(void)
{
    static u8 IO_station = 0;

    if(EXTI_GetITStatus(EXTI_Line4) != RESET) //确保是否产生了EXTI Line中断
    {
        if (0 == IO_station)
        {
            /* 低变高 */
            IO_station = 1;

            // 串口发送接收到数据
//            usart1_send_str("interrupt!\r\n");
            EXTI_ClearITPendingBit(EXTI_Line4);     //清除中断标志位
        }
        else if (1 == IO_station)
        {
            /* 高变低 */
            IO_station = 0;

            // 串口发送接收到数据
            usart1_send_str("interrupt!\r\n");
            EXTI_ClearITPendingBit(EXTI_Line4);     //清除中断标志位
        }
    }
}


/*********************************************END OF FILE**********************/
