/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���usart0.h
 * ����: Lingyu Meng
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Lingyu Meng     2025/3/10     V0.01    original
************************************************************/


#ifndef __USART0_H
#define __USART0_H

/************************* ͷ�ļ� *************************/
#include "HeaderFiles.h"

/************************ �������� ************************/
typedef enum {
    SAMPLING_STOP = 0,
    SAMPLING_START
    //SAMPLING_MAX
}SamplingState;//����״̬

extern SamplingState sampling_flag;

#define RX_BUFFER_SIZE 100

//bool data_flag = false;

/************************ �������� ************************/

void USART0_Config(void);// ���ڹ�������
void USART0_SendData(uint16_t *buf,uint16_t len);// ��������
//void USART0_SendString(const char *str);
void process_data(uint8_t *data);// ���ݴ���
void uart_receive_string(char *buffer);
void usart_flush_rx(uint32_t usart_periph);//���������

void start_function(const char *command_from);
void stop_function(const char *command_from);
SamplingState get_sample_flag(void);//��ȡ������־λ
uint8_t get_is_hide(void);
uint8_t get_LED_state(void);
uint8_t get_sampling_flag(void);
void log_need(char *log_need);//��ȡ��Ҫд��log����Ϣ
void set_command_type(uint8_t new_command);
void set_command_from(uint8_t new_command_from);
#endif

/****************************End*****************************/
