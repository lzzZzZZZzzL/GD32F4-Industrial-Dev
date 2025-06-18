/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���RTC.h
 * ����: Qiao Qin @ GigaDevice
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Qiao Qin     2025/4/20     V0.01    original
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


void RTC_Init(void);	// RTC��ʼ��
void rtc_setup(void);	// RTCʱ������
void rtc_show_time(void);	// RTCʱ��
void rtc_show_alarm(void);	// RTC����
uint8_t usart_input_threshold(uint32_t value);  // ��������ֵ��ЧУ��
void rtc_pre_config(void);
uint32_t parse_time_component(const char **str);
void parse_time_string(char *str, rtc_time *time);

uint8_t byte_to_bcd(uint8_t Value);
uint8_t bcd_to_byte(uint8_t Value);   //ʱ�����������

#endif
