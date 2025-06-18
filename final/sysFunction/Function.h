/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：Function.h
 * 作者: Lingyu Meng
 * 平台: 2025CIMC IHD-V04
 * 版本: Lingyu Meng     2025/2/16     V0.01    original
************************************************************/

#ifndef __FUNCTION_H
#define __FUNCTION_H

/************************* 头文件 *************************/

#include "HeaderFiles.h"


/************************* 宏定义 *************************/
#define SAMPLE_FOLDER "0:/sample"//sample文件夹的路径
#define SAMPLE_FOLDER_PATH "0:/sample/sampleData"//sample文件的前缀名

#define OVERLIMIT_FOLDER "0:/overLimit"
#define OVERLIMIT_FOLDER_PATH "0:/overLimit/overLimitData"

#define LOG_FOLDER "0:/log"
#define LOG_FOLDER_PATH "0:/log/log"

#define HIDEDATA_FOLDER "0:/hideData"
#define HIDEDATA_FOLDER_PATH "0:/hideData/hideData"

#define FORMAT_TXT ".txt" //文件的后缀名

#define CONFIG_FILE_PATH "0:/config.ini"

/************************ 变量定义 ************************/


/************************ 函数定义 ************************/

void System_Init(void);      	// 系统初始化
void UsrFunction(void);         // 用户函数
void Get_SD_Capacity_Info(void);
void usart_read_config(void);
int Get_RTC_DateTimeString(char *datetime);
void set_log_log_flag(void);
#endif


/****************************End*****************************/

