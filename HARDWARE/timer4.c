
#include "timer4.h"
#include "sys.h"
#include "usart1.h"

#define PIN	PBin(8)

void TIM4_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; /* 选择定时器4中断 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; /* 抢占式中断优先级0 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        /* 响应式中断优先级0 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           /* 中断使能 */

    NVIC_Init(&NVIC_InitStructure);
}

void TIM4_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;            /* 返回电平引脚PB8 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; /* 浮空输入 */
    GPIO_Init(GPIOB, &GPIO_InitStructure);          /* 初始化设置好的引脚 */
}

void tim4_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;

    TIM4_GPIO_Configuration();
    TIM4_NVIC_Configuration();

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x00;
    TIM_PWMIConfig(TIM2, &TIM_ICInitStructure);
    /* 选择TIM4输入触发源；TIM经滤波定时器输入2 */
    TIM_SelectInputTrigger(TIM4, TIM_TS_TI2FP2);
    /* 选择从机模式：复位模式 */
    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);
    /* 开启复位模式 */
    TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable);
    /* 开启CC2中断 */
    TIM_ITConfig(TIM4, TIM_IT_CC3, ENABLE);
    /* 开启CC3中断 */
    TIM_Cmd(TIM4, ENABLE);

    /* 好像打错了 */
    /* TIM_TimeBaseStructure.TIM_Period = 60000; */
    /* TIM_TimeBaseStructure.TIM_Prescaler = 0; */
    /* TIM_TimeBaseStructure.TIM_ClockDivision = 0; */
    /* TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; */
    /* TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); */
}

void TIM4_IRQHandler(void)
{
    usart1_send_str("in interrrupt!");

    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {

        TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);
    }
}
