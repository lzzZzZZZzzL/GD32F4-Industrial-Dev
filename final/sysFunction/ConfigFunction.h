/************************************************************
 * ��Ȩ�� 
 * �ļ���ConfigFunction.h
 * ����: 
 * ƽ̨: 
 * �汾: 
************************************************************/

#ifndef __CONFIGFUNCTION_H
#define __CONFIGFUNCTION_H

/************************* ͷ�ļ� *************************/

#include "HeaderFiles.h"

/************************* �궨�� *************************/


/************************ �������� ************************/


/************************ �������� ************************/
void ratio_function(void);//ִ��"ratio"ָ���Ӧ����
void limit_function(void);//ִ��"linmi"ָ���Ӧ����

void config_save_function(void);//ִ��"config save"ָ���Ӧ����
void config_read_function(void);//ִ��"config read"ָ���Ӧ����
void conf_function(void);//ִ��"conf"ָ���Ӧ����

uint8_t get_wf_ratio_i(void);//��ȡwaiting_for_ratio_inputֵ
uint8_t get_wf_limit_i(void);//��ȡwaiting_for_limit_inputֵ

void set_wf_ratio_i(uint8_t new_one);//������waiting_for_ratio_inputֵ
void set_wf_limit_i(uint8_t new_one);//������waiting_for_ratio_inputֵ

float get_ratio(void);//��ȡ���ratio
float get_limit(void);//��ȡ��ֵlimit


void set_ratio(float new_ratio);//������ratioֵ
void set_limit(float new_limit);//������limitֵ

#endif

/****************************End*****************************/
