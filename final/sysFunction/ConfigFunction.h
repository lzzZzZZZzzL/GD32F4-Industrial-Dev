/************************************************************
 * 版权： 
 * 文件：ConfigFunction.h
 * 作者: 
 * 平台: 
 * 版本: 
************************************************************/

#ifndef __CONFIGFUNCTION_H
#define __CONFIGFUNCTION_H

/************************* 头文件 *************************/

#include "HeaderFiles.h"

/************************* 宏定义 *************************/


/************************ 变量定义 ************************/


/************************ 函数声明 ************************/
void ratio_function(void);//执行"ratio"指令对应功能
void limit_function(void);//执行"linmi"指令对应功能

void config_save_function(void);//执行"config save"指令对应功能
void config_read_function(void);//执行"config read"指令对应功能
void conf_function(void);//执行"conf"指令对应功能

uint8_t get_wf_ratio_i(void);//获取waiting_for_ratio_input值
uint8_t get_wf_limit_i(void);//获取waiting_for_limit_input值

void set_wf_ratio_i(uint8_t new_one);//设置新waiting_for_ratio_input值
void set_wf_limit_i(uint8_t new_one);//设置新waiting_for_ratio_input值

float get_ratio(void);//获取变比ratio
float get_limit(void);//获取阈值limit


void set_ratio(float new_ratio);//设置新ratio值
void set_limit(float new_limit);//设置新limit值

#endif

/****************************End*****************************/
