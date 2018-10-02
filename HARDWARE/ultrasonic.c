
#include "ultrasonic.h"

// 采集到的数据，单位为0.1mm
__IO u16 display_distance = 0;


static void TIM4_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void TIM4_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    //允许TIM2的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

    //重新将Timer设置为缺省值
    TIM_DeInit(TIM4);

    //采用内部时钟给TIM2提供时钟源
    TIM_InternalClockConfig(TIM4);

    //设置预先分频系数为7200-1, 计数器时钟为72000000/7200 = 10KHz
    TIM_TimeBaseStructure.TIM_Prescaler = TIME4_PRESCALER - 1;

    //设置时钟分割
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    //设置计数器模式为向上计数模式
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    //设置计数溢出大小，每计1000个数就产生一个更新事件
    TIM_TimeBaseStructure.TIM_Period = TIME4_PERIOD;
    //将配置应用到TIM2中
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);

    //清除溢出中断标志
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);

    //禁止ARR预装载缓冲器
    TIM_ARRPreloadConfig(TIM4, DISABLE);

    //开启TIM2的中断
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
}

/**
 * timer2的初始化函数
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
void tim4_init(void)
{
    // interrupt init
    TIM4_NVIC_Configuration();

    // tim2 timer init
    TIM4_Configuration();

    // 开启定时器2
    TIM_Cmd(TIM4, ENABLE);
}

// enter ISR per TIME2_PERIOD_US
void TIM4_IRQHandler(void)
{
    static u16 cnt = 0;

    // 检测是否发生溢出更新事件
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {

        //led_flash_light(20, ENABLE);

        //清除TIM2的中断待处理位
        TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update);
    }
}

u16 get_tim4_time(void)
{
    u16 temp = 0;

    temp = TIM4->CNT;

    return temp;
}


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
static void EXTI_PB4_Config(void)
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
    static u16 data1 = 0;
    static u16 data2 = 0;
    static u16 temp = 0;

    if(EXTI_GetITStatus(EXTI_Line4) != RESET) //确保是否产生了EXTI Line中断
    {
        if (0 == IO_station)
        {
            /* 低变高 */
            IO_station = 1;

            // 串口发送接收到数据
            data1 = get_tim4_time();
        }
        else if (1 == IO_station)
        {
            /* 高变低 */
            IO_station = 0;

            data2 = get_tim4_time();

            if (data2 > data1)
            {
                temp = data2 - data1;
            }
            else
            {
                temp = 5000 - (data1 - data2);
            }
            // 串口发送接收到数据
            usart1_send_str("\r\nthe data exit : ");
            temp = ((temp * 10) * 0.34) / 2;
            display_distance = temp;
            //usart1_send_str_u16(temp);
        }

        EXTI_ClearITPendingBit(EXTI_Line4);     //清除中断标志位
    }
}


void ultrasonic_init(void)
{
    /* 打开外部中断测量时间 */
    EXTI_PB4_Config();

    tim4_init();
}

float get_ultrasonic_distanse(void)
{
    // u16 display_distance = 0;
    float distance = 0;
    float temp = 0;

    temp = (float)display_distance;
    distance = temp / 100;

    return distance;
}
