/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���Function.h
 * ����: Lingyu Meng
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Lingyu Meng     2025/2/16     V0.01    original
************************************************************/

#ifndef __FUNCTION_H
#define __FUNCTION_H

/************************* ͷ�ļ� *************************/

#include "HeaderFiles.h"


/************************* �궨�� *************************/
#define SAMPLE_FOLDER "0:/sample"//sample�ļ��е�·��
#define SAMPLE_FOLDER_PATH "0:/sample/sampleData"//sample�ļ���ǰ׺��

#define OVERLIMIT_FOLDER "0:/overLimit"
#define OVERLIMIT_FOLDER_PATH "0:/overLimit/overLimitData"

#define LOG_FOLDER "0:/log"
#define LOG_FOLDER_PATH "0:/log/log"

#define HIDEDATA_FOLDER "0:/hideData"
#define HIDEDATA_FOLDER_PATH "0:/hideData/hideData"

#define FORMAT_TXT ".txt" //�ļ��ĺ�׺��

#define CONFIG_FILE_PATH "0:/config.ini"

/************************ �������� ************************/


/************************ �������� ************************/

void System_Init(void);      	// ϵͳ��ʼ��
void UsrFunction(void);         // �û�����
void Get_SD_Capacity_Info(void);
void usart_read_config(void);
int Get_RTC_DateTimeString(char *datetime);
void set_log_log_flag(void);
#endif


/****************************End*****************************/

