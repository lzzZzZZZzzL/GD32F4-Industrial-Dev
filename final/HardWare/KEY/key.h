/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：key.h
 * 作者: Lingyu Meng
 * 平台: 2025CIMC IHD-V04
 * 版本: Lingyu Meng     2025/2/16     V0.01    original
************************************************************/
#ifndef __KEY_H
#define __KEY_H

/************************* 头文件 *************************/

#include "HeaderFiles.h"

/************************* 宏定义 *************************/
#define CYCLE_5S    5
#define CYCLE_10S   10
#define CYCLE_15S   15

#define   KEY_Press()     (RESET == gpio_input_bit_get(GPIOE,GPIO_PIN_2))     // 按键按下
#define   KEY_Release()   (SET == gpio_input_bit_get(GPIOE,GPIO_PIN_2))       // 按键松开

// KEY引脚定义
#define KEY1_PIN        GPIO_PIN_3
#define KEY2_PIN        GPIO_PIN_4
#define KEY3_PIN        GPIO_PIN_5
#define KEY4_PIN        GPIO_PIN_6

// KEY端口定义
#define KEY_PORT        GPIOE
#define KEY_CLK         RCU_GPIOE

/************************ 变量定义 ************************/
extern uint8_t cycle_time;

/************************ 函数定义 ************************/

void KEY_Init(void);//KEY初始化
void EXTI_PIN_Init(void); //中断初始化

uint8_t KEY_Stat(uint32_t port, uint16_t pin);
void scan_key(void);

uint8_t get_cycle_time(void);
void set_cycle_time(uint8_t new_cycle_time);

#endif

/****************************End*****************************/
