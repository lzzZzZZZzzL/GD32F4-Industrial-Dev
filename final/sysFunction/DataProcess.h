/************************************************************
 * 版权：
 * 文件：DataProcess.h
 * 作者: 
 * 平台: 
 * 版本: 
************************************************************/

#ifndef __DATAPROCESS_H
#define __DATAPROCESS_H

/************************* 头文件 *************************/

#include "HeaderFiles.h"

/************************* 宏定义 *************************/


/************************ 变量定义 ************************/


/************************ 函数定义 ************************/

void DataProcess(uint8_t cnt, uint8_t cmd_hide, uint8_t sampling_flag);

void sample_need(char *sample_need);//获取需要写入sample的信息
void hide_need(char *hide_need);//获取需要写入hide的信息
void overlimit_need(char *ol_need);//获取需要写入overlimit的信息

#endif

/****************************End*****************************/
