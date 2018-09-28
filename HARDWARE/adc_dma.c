/**
 * @file         adc_dma.c
 * @brief        一个用来进行ADC采集的文件
 * @details      采用了DMA进行数据的保存
 * @author       文劲松
 * @date     date
 * @version  A001
 * @par Copyright (c):
 *       XXX公司
 * @par History:
 *   version: 2018.9.28
 */
#include "adc_dma.h"
#include "usart1.h"

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

__IO uint16_t ADC_ConvertedValue;

static void ADC1_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable DMA clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* Enable ADC1 and GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);

    /* Configure PC.01  as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);				// PC1,输入时不用设置速率
}


static void ADC1_Mode_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    /* DMA channel1 configuration */
    DMA_DeInit(DMA1_Channel1);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 			//ADC地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;	//内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址固定
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;  				//内存地址固定
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;										//循环传输
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    /* Enable DMA channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* ADC1 configuration */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			//独立ADC模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 	 				//禁止扫描模式，扫描模式用于多通道采集
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			//开启连续转换模式，即不停地进行ADC转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发转换
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//采集数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;	 								//要转换的通道数目1
    ADC_Init(ADC1, &ADC_InitStructure);

    /*配置ADC时钟，为PCLK2的8分频，即9MHz*/
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    /*配置ADC1的通道11为55.	5个采样周期，序列为1 */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_55Cycles5);

    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /*复位校准寄存器 */
    ADC_ResetCalibration(ADC1);
    /*等待校准寄存器复位完成 */
    while(ADC_GetResetCalibrationStatus(ADC1));

    /* ADC校准 */
    ADC_StartCalibration(ADC1);
    /* 等待校准完成*/
    while(ADC_GetCalibrationStatus(ADC1));

    /* 由于没有采用外部触发，所以使用软件触发ADC转换 */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
 * 带DMA的ADC采集
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
void adc1_init(void)
{
    ADC1_GPIO_Config();
    ADC1_Mode_Config();
}

/**
 * 返回ADC采集的数据
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
u16 get_adc1_value(void)
{
    /* ADC_SoftwareStartConvCmd(ADC1, ENABLE); */

    return ADC_ConvertedValue;
}

/* 测量ADC采集数据的测试程序 */
void adc_test(void)
{
    u8 send_adc_buf[3] = {0};
    u16 adc_data = 0;

    adc_data = get_adc1_value();
    send_adc_buf[0] = (u8)(adc_data >> 8);
    send_adc_buf[1] = (u8)(adc_data);
    usart1_send_str_len(send_adc_buf, 2);
}
