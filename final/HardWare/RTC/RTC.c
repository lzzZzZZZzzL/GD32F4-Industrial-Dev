/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：RTC.c
 * 作者: Qiao Qin @ GigaDevice
 * 平台: 2025CIMC IHD-V04
 * 版本: Qiao Qin     2025/4/20     V0.01    original
************************************************************/


#include "RTC.h"

#define RTC_CLOCK_SOURCE_LXTAL		// 配置RTC时钟源
#define BKP_VALUE    0x32F0

rtc_parameter_struct   rtc_initpara;
rtc_alarm_struct  rtc_alarm;
rtc_time time;
__IO uint32_t prescaler_a = 0, prescaler_s = 0;
uint32_t RTCSRC_FLAG = 0;




/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void RTC_Init(void)
{
    //printf("\n\r  ****************** RTC calendar demo ******************\n\r");

    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enable the access of the RTC registers */
    pmu_backup_write_enable();
	
    rtc_pre_config();
    /* get RTC clock entry selection */
    RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);

    /* check if RTC has aready been configured */
    if((BKP_VALUE != RTC_BKP0) || (0x00 == RTCSRC_FLAG))
    {
        /* backup data register value is not correct or not yet programmed
        or RTC clock source is not configured (when the first time the program 
        is executed or data in RCU_BDCTL is lost due to Vbat feeding) */
			rtc_setup();
    }
    else
    {
        /* detect the reset source */
        if (RESET != rcu_flag_get(RCU_FLAG_PORRST))
        {
            //printf("power on reset occurred....\n\r");
        }
        else if(RESET != rcu_flag_get(RCU_FLAG_EPRST))
        {
            //printf("external reset occurred....\n\r");
        }
        //printf("no need to configure RTC....\n\r");

        //rtc_show_time();
    }
    rcu_all_reset_flag_clear();
}

/*!
    \brief      RTC configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_pre_config(void)
{
    #if defined (RTC_CLOCK_SOURCE_IRC32K)
          rcu_osci_on(RCU_IRC32K);
          rcu_osci_stab_wait(RCU_IRC32K);
          rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);

          prescaler_s = 0x13F;
          prescaler_a = 0x63;
    #elif defined (RTC_CLOCK_SOURCE_LXTAL)
          rcu_osci_on(RCU_LXTAL);
          rcu_osci_stab_wait(RCU_LXTAL);
          rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

          prescaler_s = 0xFF;
          prescaler_a = 0x7F;
    #else
    #error RTC clock source should be defined.
    #endif /* RTC_CLOCK_SOURCE_IRC32K */

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
}

/*!
    \brief      use hyperterminal to setup RTC time and alarm
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_setup(void)
{
    char result[32];
		char input[20];
    uint32_t year, month, day, hour, minute, second;
	
    rtc_initpara.factor_asyn = prescaler_a;
    rtc_initpara.factor_syn = prescaler_s;
    rtc_initpara.day_of_week = RTC_SATURDAY;
    rtc_initpara.month = RTC_APR;
    rtc_initpara.display_format = RTC_24HOUR;
    rtc_initpara.am_pm = RTC_AM;


    printf("Input Datetime\n\r");
    //printf("Enter time (XXXX-XX-XX XX:XX:XX):\n\r");
    
    // 彻底清除缓冲区并禁用中断
    usart_interrupt_disable(USART0, USART_INT_RBNE);  // 禁用接收中断
    usart_flush_rx(USART0);                           // 清除缓冲区
    
    uart_receive_string(input);  // 轮询接收
    
    usart_interrupt_enable(USART0, USART_INT_RBNE);   // 重新启用中断
    //uart_receive_string(input);

    if(sscanf(input, "%u-%u-%u %u:%u:%u", 
                 &year, &month, &day, &hour, &minute, &second) != 6)
    {
          printf("Error: Invalid format! Use YYYY-MM-DD HH:MM:SS\n\r");
          return;
    }
    
    if (year < 2000 || year > 2099 || month < 1 || month > 12 || 
        day < 1 || day > 31 || hour > 23 || minute > 59 || second > 59)
    {
        printf("Error: Out of range!\n\r");
        return;
    }
	  year=year-2000;
//    printf("Parsed: %06u-%02u-%02u %02u:%02u:%02u\r\n", 
//                   year, month, day, hour, minute, second);
//		
//		printf("Year: %u\r\n", (uint8_t)year);
//    printf("Month: %u\n", (uint8_t)month);
//    printf("Day: %u\n", (uint8_t)day);
//    printf("Hour: %u\n", (uint8_t)hour);
//    printf("Minute: %u\n", (uint8_t)minute);
//    printf("Second: %u\r\n", (uint8_t)second);
    
		
		rtc_initpara.year=byte_to_bcd(year);
		//printf("Year: %d\r\n", rtc_initpara.year);
		rtc_initpara.month=byte_to_bcd(month);
		//printf("Year: %d\n", rtc_initpara.month);
		rtc_initpara.date=byte_to_bcd(day);
		//printf("Year: %d\n", rtc_initpara.date);
		rtc_initpara.hour=byte_to_bcd(hour);
		//printf("Year: %d\n", rtc_initpara.minute);
		rtc_initpara.minute=byte_to_bcd(minute);
		//printf("Year: %d\n", rtc_initpara.minute);
		rtc_initpara.second=byte_to_bcd(second);
		//printf("Year: %d\n", rtc_initpara.second);
    
    snprintf(result, sizeof(result), "success to %s", input);
    
   if(ERROR == rtc_init(&rtc_initpara))
    {
        printf("\n\r** RTC configuration failed! **\n\r");
    }
    else
    {
      printf("\n\rRTC Config success\r\nTime:");
        rtc_show_time();
        RTC_BKP0 = BKP_VALUE;
      set_command_type(10);
    }
 }
		
void parse_time_string(char *str, rtc_time *time) {
    char *token;

    // Extract year
    token = strtok(str, "-");
    time->year = atoi(token);

    // Extract month
    token = strtok(NULL, "-");
    time->month = atoi(token);

    // Extract day
    token = strtok(NULL, " ");
    time->day = atoi(token);

    // Extract hour
    token = strtok(NULL, ":");
    time->hour = atoi(token);

    // Extract minute
    token = strtok(NULL, ":");
    time->minute = atoi(token);

    // Extract second
    token = strtok(NULL, "");
    time->second = atoi(token);
}


/*!
    \brief      display the current time
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_show_time(void)
{
//    uint32_t time_subsecond = 0;
//    uint8_t subsecond_ss = 0,subsecond_ts = 0,subsecond_hs = 0;

    rtc_current_time_get(&rtc_initpara);
	
	  uint8_t hour   = bcd_to_byte(rtc_initpara.hour);
    uint8_t minute = bcd_to_byte(rtc_initpara.minute);
    uint8_t second = bcd_to_byte(rtc_initpara.second);
    uint8_t year   = bcd_to_byte(rtc_initpara.year);  
    uint8_t month  = bcd_to_byte(rtc_initpara.month);
    uint8_t day    = bcd_to_byte(rtc_initpara.date);
	
	  //printf("\nCurrent Time:");
    printf("%04u-%02u-%02u ", year+2000, month, day);
    printf("%02u:%02u:%02u \r\n", hour, minute, second);
	 
//	    printf("\r\nCurrent time:: %04u-%02u-%02u %02u:%02u:%02u\r\n", 
//       2000 + rtc_initpara.year, rtc_initpara.month, rtc_initpara.date,
//       rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second);
	
//	 printf("\r\nCurrent time:: %04x-%02x-%02x %02x:%02x:%02x\r\n", 
//       2000 + rtc_initpara.year, rtc_initpara.month, rtc_initpara.date,
//       rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second);

    /* get the subsecond value of current time, and convert it into fractional format */
//    time_subsecond = rtc_subsecond_get();
//    subsecond_ss=(1000-(time_subsecond*1000+1000)/400)/100;
//    subsecond_ts=(1000-(time_subsecond*1000+1000)/400)%100/10;
//    subsecond_hs=(1000-(time_subsecond*1000+1000)/400)%10;

//    printf("\r\nCurrent time: 20%0.2x-%0.2x-%0.2x", \
//           rtc_initpara.year, rtc_initpara.month, rtc_initpara.date);

//    printf(" : %0.2x:%0.2x:%0.2x \r\n", \
//           rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second);
}

/*!
    \brief      display the alram value
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_show_alarm(void)
{
    rtc_alarm_get(RTC_ALARM0,&rtc_alarm);
    printf("The alarm: %0.2x:%0.2x:%0.2x \n\r", rtc_alarm.alarm_hour, rtc_alarm.alarm_minute,\
           rtc_alarm.alarm_second);
}

/*!
    \brief      get the input character string and check if it is valid
    \param[in]  none
    \param[out] none
    \retval     input value in BCD mode
*/
uint8_t usart_input_threshold(uint32_t value)
{
    uint32_t index = 0;
    uint32_t tmp[2] = {0, 0};

    while (index < 2){
        while (RESET == usart_flag_get(USART0, USART_FLAG_RBNE));
        tmp[index++] = usart_data_receive(USART0);
        if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39)){
            printf("\n\r please input a valid number between 0 and 9 \n\r");
            index--;
        }
    }

    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
    if (index > value){
        printf("\n\r please input a valid number between 0 and %d \n\r", value);
        return 0xFF;
    }

    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) <<4);
    return index;
}


/**
  * @brief  Convert a 2 digit decimal to BCD format.
  * @param  Value: Byte to be converted
  * @retval Converted byte
  */
uint8_t byte_to_bcd(uint8_t Value)
{
  uint32_t bcdhigh = 0;
 
  while(Value >= 10)
  {
    bcdhigh++;
    Value -= 10;
  }
 
  return  ((uint8_t)(bcdhigh << 4) | Value);
}
 

uint8_t bcd_to_byte(uint8_t Value)
{
  uint32_t tmp = 0;
  tmp = ((uint8_t)(Value &(uint8_t)0xF0) >> (uint8_t)0x4) * 10;
  return (tmp + (Value &(uint8_t)0x0F));
}
