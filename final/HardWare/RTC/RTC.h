/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：RTC.h
 * 作者: Qiao Qin @ GigaDevice
 * 平台: 2025CIMC IHD-V04
 * 版本: Qiao Qin     2025/4/20     V0.01    original
************************************************************/

#ifndef __RTC_H
#define __RTC_H
#include "HeaderFiles.h"

typedef struct {
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
} rtc_time;


void RTC_Init(void);	// RTC初始化
void rtc_setup(void);	// RTC时钟设置
void rtc_show_time(void);	// RTC时间
void rtc_show_alarm(void);	// RTC闹钟
uint8_t usart_input_threshold(uint32_t value);  // 用作输入值有效校验
void rtc_pre_config(void);
uint32_t parse_time_component(const char **str);
void parse_time_string(char *str, rtc_time *time);

uint8_t byte_to_bcd(uint8_t Value);
uint8_t bcd_to_byte(uint8_t Value);   //时间戳辅助函数

#endif
