/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：usart0.h
 * 作者: Lingyu Meng
 * 平台: 2025CIMC IHD-V04
 * 版本: Lingyu Meng     2025/3/10     V0.01    original
************************************************************/


#ifndef __USART0_H
#define __USART0_H

/************************* 头文件 *************************/
#include "HeaderFiles.h"

/************************ 变量定义 ************************/
typedef enum {
    SAMPLING_STOP = 0,
    SAMPLING_START
    //SAMPLING_MAX
}SamplingState;//采样状态

extern SamplingState sampling_flag;

#define RX_BUFFER_SIZE 100

//bool data_flag = false;

/************************ 函数声明 ************************/

void USART0_Config(void);// 串口功能配置
void USART0_SendData(uint16_t *buf,uint16_t len);// 发送数据
//void USART0_SendString(const char *str);
void process_data(uint8_t *data);// 数据处理
void uart_receive_string(char *buffer);
void usart_flush_rx(uint32_t usart_periph);//清除缓冲区

void start_function(const char *command_from);
void stop_function(const char *command_from);
SamplingState get_sample_flag(void);//获取采样标志位
uint8_t get_is_hide(void);
uint8_t get_LED_state(void);
uint8_t get_sampling_flag(void);
void log_need(char *log_need);//获取需要写入log的信息
void set_command_type(uint8_t new_command);
void set_command_from(uint8_t new_command_from);
#endif

/****************************End*****************************/
