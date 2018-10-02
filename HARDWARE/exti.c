
#include "exti.h"

#include "usart1.h"

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    /* �����ж�Դ */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  ���� PB4 Ϊ���жϿڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
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
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // ��������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   // ��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* EXTI line mode config */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //�½����ж�
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void EXTI4_IRQHandler(void)
{
    static u8 IO_station = 0;

    if(EXTI_GetITStatus(EXTI_Line4) != RESET) //ȷ���Ƿ������EXTI Line�ж�
    {
        if (0 == IO_station)
        {
            /* �ͱ�� */
            IO_station = 1;

            // ���ڷ��ͽ��յ�����
//            usart1_send_str("interrupt!\r\n");
            EXTI_ClearITPendingBit(EXTI_Line4);     //����жϱ�־λ
        }
        else if (1 == IO_station)
        {
            /* �߱�� */
            IO_station = 0;

            // ���ڷ��ͽ��յ�����
            usart1_send_str("interrupt!\r\n");
            EXTI_ClearITPendingBit(EXTI_Line4);     //����жϱ�־λ
        }
    }
}


/*********************************************END OF FILE**********************/
