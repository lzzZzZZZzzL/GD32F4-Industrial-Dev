/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：adc.c
 * 作者: Lingyu Meng
 * 平台: 2025CIMC IHD-V04
 * 版本: Lingyu Meng     2025/03/07      V0.01    original
************************************************************/

/************************* 头文件 *************************/

#include "ADC.h"

/************************* 宏定义 *************************/
#define ADC_GPIO_PORT        GPIOC
#define ADC_PIN              GPIO_PIN_0
#define ADC_GPIO_CLK         RCU_GPIOC
#define ADC_CLK              RCU_ADC0

/************************ 变量定义 ************************/

/************************ 函数定义 ************************/

/************************************************************ 
 * Function :       ADC_Init
 * Comment  :       用于初始化ADC（不使用dma）
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void ADC_port_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);   // GPIOC时钟使能
	rcu_periph_clock_enable(RCU_ADC0);    // 使能ADC时钟
	
	gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);   // 配置PC0为模拟输入
	
	adc_clock_config(ADC_ADCCK_PCLK2_DIV8);   // adc时钟配置
	
	ADC_Init();  // ADC配置
	
	adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL); //  规则采样软件触发
}

/************************************************************ 
 * Function :       ADC_Init
 * Comment  :       用于初始化ADC（不使用dma）
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void ADC_Init(void)
{
    adc_deinit();    // 复位ADC
	
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);    	// 使能连续转换模式
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);   			// 数据右对齐 
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 1);  			// 通道配置，规则组

    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_10, ADC_SAMPLETIME_56);   // 对规则组进行配置

    adc_external_trigger_source_config(ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_INSERTED_T0_CH3);   // ADC 触发配置，利用定时器触发
    adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, ENABLE);   							  // 启用触发
	
    adc_enable(ADC0);   		// 使能ADC接口
	
    delay_1ms(1);  				// 等待1ms

    adc_calibration_enable(ADC0);    // ADC校准和复位ADC校准
}


//void Output_ADCdata(unsigned short cnt)
//{
//    static unsigned short s_iCnt;  //定义静态变量s_iCnt作为计数器
//    unsigned short adc_value;    // ADC采样数字量
//    unsigned char  data[]=" ";
//    float Vol_Value;  // ADC采样值转换成电压值
//  
//    s_iCnt++; //计数器的计数值加1
//    
//    if(s_iCnt >= cnt)   //计数器的计数值大于cnt
//    {
//        s_iCnt = 0;     //重置计数器的计数值为0

//        adc_flag_clear(ADC0, ADC_FLAG_EOC);  				//  清除结束标志
//        while(SET != adc_flag_get(ADC0,ADC_FLAG_EOC)){}  	//  获取转换结束标志
//            
//            adc_value = ADC_RDATA(ADC0);    					// 读取ADC数据
//        Vol_Value = adc_value*3.3/4095;  					// 把数字量转化为工程量
//            
//        printf("数字量为=%d	工程量为%.2f V\r\n", adc_value, Vol_Value);   // 结果打印
//        sprintf(data, "Vol=%.2f V", Vol_Value);
//        OLED_ShowString(0, 16, data, 16);
//    }
//}
/****************************End*****************************/
