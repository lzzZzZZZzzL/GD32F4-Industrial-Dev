/************************************************************
 * ��Ȩ��
 * �ļ���DataProcess.c
 * ����: 
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: 
************************************************************/


/************************* ͷ�ļ� *************************/
#include "DataProcess.h"

/************************* �궨�� *************************/


/************************ �������� ************************/
uint8_t log_sample_flag=0;
uint8_t log_overlimit_flag=0;
uint8_t log_hide_flag=0;
uint8_t log_log_flag=0;

//time_data_struct time_data;       //���ԭʼʱ������
//uint32_t unix_time = 0;
unsigned char final_data[9]; //���ڴ洢���ܺ�����ݣ�4�ֽ�ʱ���+4�ֽڵ�ѹ���ݣ�2�ֽ���������+2�ֽ�С�����֣�
static unsigned char oled_time_buffer[30]; 
static unsigned char oled_data_buffer[30];
rtc_parameter_struct rtc_data;

static float adc_data ;
static char hide_char[30];
static float limit;

/************************ �������� ************************/
uint32_t raw_to_unix(uint8_t hour, uint8_t minute, uint8_t second, uint8_t year, uint8_t month, uint8_t day);

/************************ �������� ************************/
/************************************************************ 
 * Function :       DataProcess
 * Comment  :       ���ݴ���
 * Parameter:       ����, �Ƿ����, �Ƿ����
 * Return   :       null
 * Author   :       
 * Date     :       
************************************************************/
void DataProcess(uint8_t cnt, uint8_t cmd_hide, uint8_t sampling_flag)
{
  static uint8_t s_iCnt = 0;
  unsigned short raw_adcdata = 0;
//  float adc_data = 0;
  limit = get_limit();
  float ratio = get_ratio();
  uint16_t integer_part = 0, fractional_part = 0;
  
  s_iCnt++; //�������ļ���ֵ��1
  if(s_iCnt >= cnt)   //�������ļ���ֵ����cnt
  {
    s_iCnt = 0;
    rtc_current_time_get(&rtc_data);	
	  uint8_t hour   = bcd_to_byte(rtc_data.hour);
    uint8_t minute = bcd_to_byte(rtc_data.minute);
    uint8_t second = bcd_to_byte(rtc_data.second);
    uint8_t year   = bcd_to_byte(rtc_data.year);  
    uint8_t month  = bcd_to_byte(rtc_data.month);
    uint8_t day    = bcd_to_byte(rtc_data.date);
    
    adc_flag_clear(ADC0,ADC_FLAG_EOC);      // ���������־
    while(SET != adc_flag_get(ADC0,ADC_FLAG_EOC)){}  // ��ȡת��������־
    raw_adcdata = ADC_RDATA(ADC0);          // ��ȡADC��ѹ������
    adc_data = raw_adcdata * 3.30 / 4095 * ratio;    // ��������ת�ɶ�Ӧ��ȣ�ratio���Ĺ�����
    //printf("%f\r\n", raw_adcdata);
        
    if(cmd_hide && sampling_flag)   //�����Ҽ���
    {
      log_hide_flag=1;
      //printf("cnt = %d, hide = %d, samplinf_flag = %d ,limit = %.2f\r\n", cnt, cmd_hide, sampling_flag, limit);
      
      uint32_t unix_timestamp = raw_to_unix(hour, minute, second, year, month, day);
      
      /*����ѹֵ��Ϊ�������ֺ�С������*/
      integer_part = (uint16_t)adc_data; // ��������
      fractional_part = (uint16_t)((adc_data - integer_part) * 65536);
      
      //printf("fractional_part = %d\r\n",fractional_part);

      /*��ʱ����͵�ѹ������ϳ��������*/
      final_data[0] = (unix_timestamp >> 24) & 0xFF;
      final_data[1] = (unix_timestamp >> 16) & 0xFF;
      final_data[2] = (unix_timestamp >> 8) & 0xFF;
      final_data[3] = unix_timestamp & 0xFF;

      final_data[4] = (integer_part >> 8) & 0xFF; // �������ָ�λ
      final_data[5] = integer_part & 0xFF;        // �������ֵ�λ
      final_data[6] = (fractional_part >> 8) & 0xFF; // С�����ָ�λ
      final_data[7] = fractional_part & 0xFF;     // С�����ֵ�λ
      //final_data[8] = 0; //�����ֽ�
      
      //printf("%u\n", unix_time);  //ʱ���
      
      
      /*�����������*/
      if(adc_data > limit)
      {
        printf("hide: %02X%02X%02X%02X%02X%02X%02X%02X*\r\n", 
             final_data[0], final_data[1], final_data[2], final_data[3],
             final_data[4], final_data[5], final_data[6], final_data[7]);
        log_overlimit_flag=1;
        snprintf(hide_char, 30, "%02X%02X%02X%02X%02X%02X%02X%02X*",
        final_data[0], final_data[1], final_data[2], final_data[3], final_data[4], final_data[5], final_data[6], final_data[7]);
        printf("*");
        LED2_ON();
      }
      else
      {
        printf("hide: %02X%02X%02X%02X%02X%02X%02X%02X\r\n", 
             final_data[0], final_data[1], final_data[2], final_data[3],
             final_data[4], final_data[5], final_data[6], final_data[7]);
        LED2_OFF();
        snprintf(hide_char, 30, "%02X%02X%02X%02X%02X%02X%02X%02X",
        final_data[0], final_data[1], final_data[2], final_data[3], final_data[4], final_data[5], final_data[6], final_data[7]);
      }
    }
    else if((cmd_hide == 0) && (sampling_flag == 1))   //������������
    {
      log_sample_flag=1;
      printf("20%d-%02d-%02d %02d:%02d:%02d ch0=%.2fV ", year, month, day, hour, minute, second, adc_data);
      if(adc_data > limit)
      {
        log_overlimit_flag=1;
        printf("OverLimit (%.2f)", limit);
        LED2_ON();
      }
      else
      {
        LED2_OFF();
      }
      printf("\r\n");
      OLED_Clear();
      snprintf(oled_time_buffer, sizeof(oled_time_buffer), "%02d:%02d:%02d", hour, minute, second);
      snprintf(oled_data_buffer, sizeof(oled_data_buffer), "ch0=%.2fV", adc_data);
      OLED_ShowString(0, 0,  oled_time_buffer, 16);
      OLED_ShowString(0, 16, oled_data_buffer, 16);
      
    }
    else if(sampling_flag == SAMPLING_STOP) //������
    {
      OLED_Clear();
      OLED_ShowString(0, 0, "system idle",16);
      OLED_Refresh();
    }
  }
}

/************************************************************ 
 * Function :       sampling
 * Comment  :       һ�β���
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/
//void sampling(void)
//{
////  time_data_struct time_data;       //���ԭʼʱ������
////  get_time(&time_data);       //��ȡʱ������
////  
////  printf( "%d-%02d-%02d %02d:%02d:%02d ch0=%.1fV \r\n", time_data.year, time_data.month, time_data.date, time_data.hour, time_data.minute, time_data.second, ch0);
//  
//  printf( "%d-%02d-%02d %02d:%02d:%02d ch0=%.1fV \r\n", year,month, day,hour,min, sec, ch0);
//  
//  OLED_Clear();
//  snprintf(oled_time_buffer, sizeof(oled_time_buffer), "%02d:%02d:%02d", hour, min, sec);
//  snprintf(oled_data_buffer, sizeof(oled_data_buffer), "ch0=%.2fV", ch0);
//  OLED_ShowString(0,0,oled_time_buffer,16);
//  OLED_ShowString(0,16,oled_data_buffer,16);
//  OLED_Refresh();
//}

/************************************************************ 
 * Function :       raw_to_unix
 * Comment  :       ��ʱ������ת��ΪUNIXʱ���
 * Parameter:       
 * Return   :       ʱ���timestamp
 * Author   :       
 * Date     :       
************************************************************/
uint32_t raw_to_unix(uint8_t hour, uint8_t minute, uint8_t second, uint8_t year, uint8_t month, uint8_t day)
{
  uint32_t timestamp = 0;
  uint32_t unix_year = (uint32_t)year + 2000; //ת��Ϊʵ�����
  uint8_t utc_hour = hour;  // ����UTCʱ�����

  //printf("jiami_test %d-%02d-%02d %02d:%02d:%02d\r\n", unix_year, month, day, hour, minute, second);
  
  /*����ʱ��:��ȥ8Сʱ��������->UTC��*/
  if(utc_hour < 8)   //��Ҫ��λ��ǰһ��
  {
    utc_hour += 24 - 8;
    day--;
    if (day < 1)
    {
      month--;
      if (month < 1)
      {
        month = 12;
        unix_year--;
      }
      /*��ȡ���·ݵ�����*/
      uint8_t days_in_prev_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
      if((unix_year % 4 == 0 && unix_year % 100 != 0) || (unix_year % 400 == 0))
      {
        days_in_prev_month[1] = 29;
      }
      day = days_in_prev_month[month-1];
    }
  }
  else
  {
    utc_hour -= 8;
  }

  /*�����1970�굽��ǰ��ݵ�������*/
  for(uint32_t y = 1970; y < unix_year; y++)
  {
    if((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0))
    {
      timestamp += 366 * 24 * 60 * 60; //����
    }
    else
    {
      timestamp += 365 * 24 * 60 * 60; //ƽ��
    }
  }

  /*�����1�µ���ǰ�·ݵ�������*/
  uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if((unix_year % 4 == 0 && unix_year % 100 != 0) || (unix_year % 400 == 0))
  {
    days_in_month[1] = 29; //����2��
  }
  for(uint8_t m = 1; m < month; m++)
  {
    timestamp += days_in_month[m - 1] * 24 * 60 * 60;
  }

  timestamp += (day - 1) * 24 * 60 * 60; //�����1�յ���ǰ���ڵ�������

  /*����UTCʱ���ʱ����*/
  timestamp += utc_hour * 3600;
  timestamp += minute * 60;
  timestamp += second;
  
//    printf("adjusted UTC time: %d-%02d-%02d %02d:%02d:%02d\n", unix_year, month, day, utc_hour, minute, second);
//    printf("jimi test timestamp = %u\r\n", timestamp);  //ʱ���
  
  return timestamp;
}

/*������Ҫ���ַ���*/
void sample_need(char *sample_need)
{
    snprintf(sample_need, 10, "%.2fV", adc_data);
}

void hide_need(char *hide_need)
{
  snprintf(hide_need, 30, "%.2fV\r\nhide:%s", adc_data, hide_char);
}

void overlimit_need(char *ol_need)
{
  snprintf(ol_need, 20, "%.2fV limit %.2fV", adc_data, limit);
}

/****************************End*****************************/
