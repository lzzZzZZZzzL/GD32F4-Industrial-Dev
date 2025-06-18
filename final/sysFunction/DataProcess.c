/************************************************************
 * 版权：
 * 文件：DataProcess.c
 * 作者: 
 * 平台: 2025CIMC IHD-V04
 * 版本: 
************************************************************/


/************************* 头文件 *************************/
#include "DataProcess.h"

/************************* 宏定义 *************************/


/************************ 变量定义 ************************/
uint8_t log_sample_flag=0;
uint8_t log_overlimit_flag=0;
uint8_t log_hide_flag=0;
uint8_t log_log_flag=0;

//time_data_struct time_data;       //存放原始时间数据
//uint32_t unix_time = 0;
unsigned char final_data[9]; //用于存储加密后的数据：4字节时间戳+4字节电压数据（2字节整数部分+2字节小数部分）
static unsigned char oled_time_buffer[30]; 
static unsigned char oled_data_buffer[30];
rtc_parameter_struct rtc_data;

static float adc_data ;
static char hide_char[30];
static float limit;

/************************ 函数声明 ************************/
uint32_t raw_to_unix(uint8_t hour, uint8_t minute, uint8_t second, uint8_t year, uint8_t month, uint8_t day);

/************************ 函数定义 ************************/
/************************************************************ 
 * Function :       DataProcess
 * Comment  :       数据处理
 * Parameter:       周期, 是否加密, 是否采样
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
  
  s_iCnt++; //计数器的计数值加1
  if(s_iCnt >= cnt)   //计数器的计数值大于cnt
  {
    s_iCnt = 0;
    rtc_current_time_get(&rtc_data);	
	  uint8_t hour   = bcd_to_byte(rtc_data.hour);
    uint8_t minute = bcd_to_byte(rtc_data.minute);
    uint8_t second = bcd_to_byte(rtc_data.second);
    uint8_t year   = bcd_to_byte(rtc_data.year);  
    uint8_t month  = bcd_to_byte(rtc_data.month);
    uint8_t day    = bcd_to_byte(rtc_data.date);
    
    adc_flag_clear(ADC0,ADC_FLAG_EOC);      // 清除结束标志
    while(SET != adc_flag_get(ADC0,ADC_FLAG_EOC)){}  // 获取转换结束标志
    raw_adcdata = ADC_RDATA(ADC0);          // 读取ADC电压数字量
    adc_data = raw_adcdata * 3.30 / 4095 * ratio;    // 将数字量转成对应变比（ratio）的工程量
    //printf("%f\r\n", raw_adcdata);
        
    if(cmd_hide && sampling_flag)   //采样且加密
    {
      log_hide_flag=1;
      //printf("cnt = %d, hide = %d, samplinf_flag = %d ,limit = %.2f\r\n", cnt, cmd_hide, sampling_flag, limit);
      
      uint32_t unix_timestamp = raw_to_unix(hour, minute, second, year, month, day);
      
      /*将电压值分为整数部分和小数部分*/
      integer_part = (uint16_t)adc_data; // 整数部分
      fractional_part = (uint16_t)((adc_data - integer_part) * 65536);
      
      //printf("fractional_part = %d\r\n",fractional_part);

      /*将时间戳和电压数据组合成最终输出*/
      final_data[0] = (unix_timestamp >> 24) & 0xFF;
      final_data[1] = (unix_timestamp >> 16) & 0xFF;
      final_data[2] = (unix_timestamp >> 8) & 0xFF;
      final_data[3] = unix_timestamp & 0xFF;

      final_data[4] = (integer_part >> 8) & 0xFF; // 整数部分高位
      final_data[5] = integer_part & 0xFF;        // 整数部分低位
      final_data[6] = (fractional_part >> 8) & 0xFF; // 小数部分高位
      final_data[7] = fractional_part & 0xFF;     // 小数部分低位
      //final_data[8] = 0; //保留字节
      
      //printf("%u\n", unix_time);  //时间戳
      
      
      /*输出最终数据*/
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
    else if((cmd_hide == 0) && (sampling_flag == 1))   //采样但不加密
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
    else if(sampling_flag == SAMPLING_STOP) //不采样
    {
      OLED_Clear();
      OLED_ShowString(0, 0, "system idle",16);
      OLED_Refresh();
    }
  }
}

/************************************************************ 
 * Function :       sampling
 * Comment  :       一次采样
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/
//void sampling(void)
//{
////  time_data_struct time_data;       //存放原始时间数据
////  get_time(&time_data);       //获取时间数据
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
 * Comment  :       将时间数据转换为UNIX时间戳
 * Parameter:       
 * Return   :       时间戳timestamp
 * Author   :       
 * Date     :       
************************************************************/
uint32_t raw_to_unix(uint8_t hour, uint8_t minute, uint8_t second, uint8_t year, uint8_t month, uint8_t day)
{
  uint32_t timestamp = 0;
  uint32_t unix_year = (uint32_t)year + 2000; //转换为实际年份
  uint8_t utc_hour = hour;  // 用于UTC时间计算

  //printf("jiami_test %d-%02d-%02d %02d:%02d:%02d\r\n", unix_year, month, day, hour, minute, second);
  
  /*调整时区:减去8小时（东八区->UTC）*/
  if(utc_hour < 8)   //需要借位到前一天
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
      /*获取新月份的天数*/
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

  /*计算从1970年到当前年份的总秒数*/
  for(uint32_t y = 1970; y < unix_year; y++)
  {
    if((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0))
    {
      timestamp += 366 * 24 * 60 * 60; //闰年
    }
    else
    {
      timestamp += 365 * 24 * 60 * 60; //平年
    }
  }

  /*计算从1月到当前月份的总秒数*/
  uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if((unix_year % 4 == 0 && unix_year % 100 != 0) || (unix_year % 400 == 0))
  {
    days_in_month[1] = 29; //闰年2月
  }
  for(uint8_t m = 1; m < month; m++)
  {
    timestamp += days_in_month[m - 1] * 24 * 60 * 60;
  }

  timestamp += (day - 1) * 24 * 60 * 60; //计算从1日到当前日期的总秒数

  /*加上UTC时间的时分秒*/
  timestamp += utc_hour * 3600;
  timestamp += minute * 60;
  timestamp += second;
  
//    printf("adjusted UTC time: %d-%02d-%02d %02d:%02d:%02d\n", unix_year, month, day, utc_hour, minute, second);
//    printf("jimi test timestamp = %u\r\n", timestamp);  //时间戳
  
  return timestamp;
}

/*传递需要的字符串*/
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
