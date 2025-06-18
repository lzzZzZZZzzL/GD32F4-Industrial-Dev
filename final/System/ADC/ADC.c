/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���adc.c
 * ����: Lingyu Meng
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Lingyu Meng     2025/03/07      V0.01    original
************************************************************/

/************************* ͷ�ļ� *************************/

#include "ADC.h"

/************************* �궨�� *************************/
#define ADC_GPIO_PORT        GPIOC
#define ADC_PIN              GPIO_PIN_0
#define ADC_GPIO_CLK         RCU_GPIOC
#define ADC_CLK              RCU_ADC0

/************************ �������� ************************/

/************************ �������� ************************/

/************************************************************ 
 * Function :       ADC_Init
 * Comment  :       ���ڳ�ʼ��ADC����ʹ��dma��
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void ADC_port_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);   // GPIOCʱ��ʹ��
	rcu_periph_clock_enable(RCU_ADC0);    // ʹ��ADCʱ��
	
	gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);   // ����PC0Ϊģ������
	
	adc_clock_config(ADC_ADCCK_PCLK2_DIV8);   // adcʱ������
	
	ADC_Init();  // ADC����
	
	adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL); //  ��������������
}

/************************************************************ 
 * Function :       ADC_Init
 * Comment  :       ���ڳ�ʼ��ADC����ʹ��dma��
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void ADC_Init(void)
{
    adc_deinit();    // ��λADC
	
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);    	// ʹ������ת��ģʽ
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);   			// �����Ҷ��� 
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 1);  			// ͨ�����ã�������

    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_10, ADC_SAMPLETIME_56);   // �Թ������������

    adc_external_trigger_source_config(ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_INSERTED_T0_CH3);   // ADC �������ã����ö�ʱ������
    adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, ENABLE);   							  // ���ô���
	
    adc_enable(ADC0);   		// ʹ��ADC�ӿ�
	
    delay_1ms(1);  				// �ȴ�1ms

    adc_calibration_enable(ADC0);    // ADCУ׼�͸�λADCУ׼
}


//void Output_ADCdata(unsigned short cnt)
//{
//    static unsigned short s_iCnt;  //���徲̬����s_iCnt��Ϊ������
//    unsigned short adc_value;    // ADC����������
//    unsigned char  data[]=" ";
//    float Vol_Value;  // ADC����ֵת���ɵ�ѹֵ
//  
//    s_iCnt++; //�������ļ���ֵ��1
//    
//    if(s_iCnt >= cnt)   //�������ļ���ֵ����cnt
//    {
//        s_iCnt = 0;     //���ü������ļ���ֵΪ0

//        adc_flag_clear(ADC0, ADC_FLAG_EOC);  				//  ���������־
//        while(SET != adc_flag_get(ADC0,ADC_FLAG_EOC)){}  	//  ��ȡת��������־
//            
//            adc_value = ADC_RDATA(ADC0);    					// ��ȡADC����
//        Vol_Value = adc_value*3.3/4095;  					// ��������ת��Ϊ������
//            
//        printf("������Ϊ=%d	������Ϊ%.2f V\r\n", adc_value, Vol_Value);   // �����ӡ
//        sprintf(data, "Vol=%.2f V", Vol_Value);
//        OLED_ShowString(0, 16, data, 16);
//    }
//}
/****************************End*****************************/
