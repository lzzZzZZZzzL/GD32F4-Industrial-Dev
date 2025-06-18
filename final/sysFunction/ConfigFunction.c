/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���ConfigFunction.c
 * ����: 
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: 
************************************************************/


/************************* ͷ�ļ� *************************/

#include "ConfigFunction.h"

/************************* �궨�� *************************/


/************************ �������� ************************/
static float ratio = 1.0;
static uint8_t waiting_for_ratio_input = 0;

static float limit = 1.0;
static uint8_t waiting_for_limit_input = 0;

/************************ �������� ************************/
/************************************************************ 
 * Function :       ratio_function
 * Comment  :       ִ��"ratio"ָ���Ӧ����
************************************************************/
void ratio_function(void)
{
  printf("Ratio = %.2f\r\n", ratio);
  printf("Input value(0~100):\r\n");
  waiting_for_ratio_input = 1;  // �ȴ������µ�ratio
}

/************************************************************ 
 * Function :       limit_function
 * Comment  :       ִ��"linmi"ָ���Ӧ����
************************************************************/
void limit_function(void)
{
  printf("limit = %.2f\r\n", limit);
  printf("Input value(0~200):\r\n");
  waiting_for_limit_input = 1;  // �ȴ������µ�limit
}

/************************************************************ 
 * Function :       config_save_function
 * Comment  :       ִ��"config save"ָ���Ӧ����
************************************************************/
void config_save_function(void)
{
  printf("ratio = %.2f\r\n",ratio);
  printf("limit = %.2f\r\n",limit);
  
  save_flash(ratio, FLASH_RATIO); //���浽flash
  save_flash(limit, FLASH_LIMIT);
  
  printf("save parameters to flash\r\n");
}

/************************************************************ 
 * Function :       config_read_function
 * Comment  :       ִ��"config read"ָ���Ӧ����
************************************************************/
void config_read_function(void)
{
  float flash_ratio = read_flash(FLASH_RATIO);  //��flash��
  float flash_limit = read_flash(FLASH_LIMIT);
  
  printf("read parameters from flash\r\n");
  
  printf("ratio = %.2f\r\n", flash_ratio);
  printf("limit = %.2f\r\n", flash_limit);
}

/************************************************************ 
 * Function :       conf_function
 * Comment  :       ִ��"conf"ָ���Ӧ����
************************************************************/
void conf_function(void)
{ 
  usart_read_config();
}

/************************************************************ 
 * Function :       get_wf_ratio_i
 * Comment  :       ��ȡwaiting_for_ratio_inputֵ
************************************************************/
uint8_t get_wf_ratio_i(void)
{
  return waiting_for_ratio_input;
}

/************************************************************ 
 * Function :       set_wf_ratio_i
 * Comment  :       ������waiting_for_ratio_inputֵ
************************************************************/
void set_wf_ratio_i(uint8_t new_one)
{
  waiting_for_ratio_input = new_one;
}

/************************************************************ 
 * Function :       get_wf_ratio_i
 * Comment  :       ��ȡwaiting_for_limit_inputֵ
************************************************************/
uint8_t get_wf_limit_i(void)
{
  return waiting_for_limit_input;
}

/************************************************************ 
 * Function :       get_ratiod
 * Comment  :       ��ȡ���ratio
************************************************************/
float get_ratio(void)
{
  return ratio;
}

/************************************************************ 
 * Function :       get_limit
 * Comment  :       ��ȡ��ֵlimit
************************************************************/
float get_limit(void)
{
  return limit;
}

/************************************************************ 
 * Function :       set_wf_limit_i
 * Comment  :       ������waiting_for_ratio_inputֵ
************************************************************/
void set_wf_limit_i(uint8_t new_one)
{
  waiting_for_limit_input = new_one;
}

/************************************************************ 
 * Function :       set_ratio
 * Comment  :       ������ratioֵ
************************************************************/
void set_ratio(float new_ratio)
{
  ratio = new_ratio;
}

/************************************************************ 
 * Function :       set_limit
 * Comment  :       ������limitֵ
************************************************************/
void set_limit(float new_limit)
{
  limit = new_limit;
}

/****************************End*****************************/
