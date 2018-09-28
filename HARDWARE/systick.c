#include "systick.h"
#include "led.h"

void SYSTICK_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
  个人感觉可以使用函数代替
  SysTick_Config( 72000000/1000 );
*/
/* void SYSTICK_Configuration (void) */
/* { */
/* //	jiffies = 0; */
/* 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK); */
/* 	if (720000 > SysTick_LOAD_RELOAD_Msk)  while(1);/\* Reload value impossible *\/ */
/* 	SysTick->LOAD  = (720000 & SysTick_LOAD_RELOAD_Msk) - 1;/\* set reload register *\/ */
/* 	//NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);/\* set Priority for Cortex-M0 System Interrupts *\/ */
/* 	SysTick->VAL   = 0;/\* Load the SysTick Counter Value *\/ */
/* 	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | */
/*         SysTick_CTRL_TICKINT_Msk | */
/*         SysTick_CTRL_ENABLE_Msk;/\* Enable SysTick IRQ and SysTick Timer *\/ */
/* // 	if(SysTick_Config(720000) == 1){//10ms */
/* // 		printf("\r\nsystick fail"); */
/* // 		while(1); */
/* // 	} */
/* } */

void systick_init(void)
{
    //systick NVIC init
    SYSTICK_NVIC_Configuration();

    //systick init
    /*  */
    SysTick_Config( 72000000/SYSTICK_PICK );
    /* 定时1ms */
    /* SysTick_Config( 72000000/1000 ); */
}

void delay_ms(int ms)
{
    int i,j;

    for(i = 0; i < ms*2; i++)
        for(j = 0; j < 5993; j++)
            ;
}

void delay_us(int us)
{
    int i,j;

    for(i = 0; i < us; i++)
        for(j = 0; j < 10; j++)
            __ASM("NOP");
}


/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
    /* led_flash_light(1000, ENABLE); */
}






