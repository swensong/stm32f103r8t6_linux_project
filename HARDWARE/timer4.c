
#include "timer4.h"
#include "sys.h"
#include "usart1.h"

#define PIN	PBin(8)

void TIM4_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; /* 选择定时器4中断 */
    /* NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; /\* 抢占式中断优先级0 *\/ */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        /* 响应式中断优先级0 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           /* 中断使能 */

    NVIC_Init(&NVIC_InitStructure);
}

void TIM4_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;            /* 返回电平引脚PB7 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /* GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; /\* 浮空输入 *\/ */
    GPIO_Init(GPIOB, &GPIO_InitStructure);          /* 初始化设置好的引脚 */
}

void timer4_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;

    TIM4_GPIO_Configuration();
    TIM4_NVIC_Configuration();

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x00;

    TIM_PWMIConfig(TIM4, &TIM_ICInitStructure);

    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    /* 选择TIM4输入触发源；TIM经滤波定时器输入2 */
    /* TIM_SelectInputTrigger(TIM4, TIM_TS_ETRF); */
    TIM_SelectInputTrigger(TIM4, TIM_TS_TI2FP2);
    /* 选择从机模式：复位模式 */
    TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Reset);
    /* 开启复位模式 */
    TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable);
    /* 开启CC3中断 */
    TIM_Cmd(TIM4, ENABLE);
    /* 开启CC2中断 */
    TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE);
    /* 开启CC2中断 */
    TIM_Cmd(TIM4, ENABLE);

    /* 好像打错了 */
    /* TIM_TimeBaseStructure.TIM_Period = 60000; */
    /* TIM_TimeBaseStructure.TIM_Prescaler = 0; */
    /* TIM_TimeBaseStructure.TIM_ClockDivision = 0; */
    /* TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; */
    /* TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); */
}

//上面初始化
//以下进行频率和占空比捕获
extern __IO uint32_t TimeDisplay;
extern __IO uint16_t IC2Value;
extern __IO uint16_t DutyCycle;
extern __IO uint32_t Frequency;

void TIM4_IRQHandler(void)
{
    u8 IC2Value_buf[9];
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);


    /* Get the Input Capture value */
    IC2Value = TIM_GetCapture2(TIM4);

    if (IC2Value != 0)
    {
        /* Duty cycle computation */
        DutyCycle = (TIM_GetCapture1(TIM4) * 100) / IC2Value;

        /* Frequency computation */
        Frequency = 1000000 / (IC2Value + 1);

        IC2Value_buf[0] = IC2Value / 10000 % 10 + '0';
        IC2Value_buf[1] = IC2Value / 1000 % 10 + '0';
        IC2Value_buf[2] = IC2Value / 100 % 10 + '0';
        IC2Value_buf[3] = IC2Value / 10 % 10 + '0';
        IC2Value_buf[4] = IC2Value % 10 + '0';
        IC2Value_buf[5] = '\r';
        IC2Value_buf[6] = '\n';
        IC2Value_buf[7] = '\0';

        usart1_send_str("interrupt!\r\n");
        usart1_send_str(IC2Value_buf);
    }
    else
    {
        DutyCycle = 0;
        Frequency = 0;
    }

    //usart1_send_str("中断!\r\n");
}
