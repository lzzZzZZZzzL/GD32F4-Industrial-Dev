/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���key.h
 * ����: Lingyu Meng
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Lingyu Meng     2025/2/16     V0.01    original
************************************************************/
#ifndef __KEY_H
#define __KEY_H

/************************* ͷ�ļ� *************************/

#include "HeaderFiles.h"

/************************* �궨�� *************************/
#define CYCLE_5S    5
#define CYCLE_10S   10
#define CYCLE_15S   15

#define   KEY_Press()     (RESET == gpio_input_bit_get(GPIOE,GPIO_PIN_2))     // ��������
#define   KEY_Release()   (SET == gpio_input_bit_get(GPIOE,GPIO_PIN_2))       // �����ɿ�

// KEY���Ŷ���
#define KEY1_PIN        GPIO_PIN_3
#define KEY2_PIN        GPIO_PIN_4
#define KEY3_PIN        GPIO_PIN_5
#define KEY4_PIN        GPIO_PIN_6

// KEY�˿ڶ���
#define KEY_PORT        GPIOE
#define KEY_CLK         RCU_GPIOE

/************************ �������� ************************/
extern uint8_t cycle_time;

/************************ �������� ************************/

void KEY_Init(void);//KEY��ʼ��
void EXTI_PIN_Init(void); //�жϳ�ʼ��

uint8_t KEY_Stat(uint32_t port, uint16_t pin);
void scan_key(void);

uint8_t get_cycle_time(void);
void set_cycle_time(uint8_t new_cycle_time);

#endif

/****************************End*****************************/
