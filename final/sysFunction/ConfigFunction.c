/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：ConfigFunction.c
 * 作者: 
 * 平台: 2025CIMC IHD-V04
 * 版本: 
************************************************************/


/************************* 头文件 *************************/

#include "ConfigFunction.h"

/************************* 宏定义 *************************/


/************************ 变量定义 ************************/
static float ratio = 1.0;
static uint8_t waiting_for_ratio_input = 0;

static float limit = 1.0;
static uint8_t waiting_for_limit_input = 0;

/************************ 函数定义 ************************/
/************************************************************ 
 * Function :       ratio_function
 * Comment  :       执行"ratio"指令对应功能
************************************************************/
void ratio_function(void)
{
  printf("Ratio = %.2f\r\n", ratio);
  printf("Input value(0~100):\r\n");
  waiting_for_ratio_input = 1;  // 等待输入新的ratio
}

/************************************************************ 
 * Function :       limit_function
 * Comment  :       执行"linmi"指令对应功能
************************************************************/
void limit_function(void)
{
  printf("limit = %.2f\r\n", limit);
  printf("Input value(0~200):\r\n");
  waiting_for_limit_input = 1;  // 等待输入新的limit
}

/************************************************************ 
 * Function :       config_save_function
 * Comment  :       执行"config save"指令对应功能
************************************************************/
void config_save_function(void)
{
  printf("ratio = %.2f\r\n",ratio);
  printf("limit = %.2f\r\n",limit);
  
  save_flash(ratio, FLASH_RATIO); //保存到flash
  save_flash(limit, FLASH_LIMIT);
  
  printf("save parameters to flash\r\n");
}

/************************************************************ 
 * Function :       config_read_function
 * Comment  :       执行"config read"指令对应功能
************************************************************/
void config_read_function(void)
{
  float flash_ratio = read_flash(FLASH_RATIO);  //从flash读
  float flash_limit = read_flash(FLASH_LIMIT);
  
  printf("read parameters from flash\r\n");
  
  printf("ratio = %.2f\r\n", flash_ratio);
  printf("limit = %.2f\r\n", flash_limit);
}

/************************************************************ 
 * Function :       conf_function
 * Comment  :       执行"conf"指令对应功能
************************************************************/
void conf_function(void)
{ 
  usart_read_config();
}

/************************************************************ 
 * Function :       get_wf_ratio_i
 * Comment  :       获取waiting_for_ratio_input值
************************************************************/
uint8_t get_wf_ratio_i(void)
{
  return waiting_for_ratio_input;
}

/************************************************************ 
 * Function :       set_wf_ratio_i
 * Comment  :       设置新waiting_for_ratio_input值
************************************************************/
void set_wf_ratio_i(uint8_t new_one)
{
  waiting_for_ratio_input = new_one;
}

/************************************************************ 
 * Function :       get_wf_ratio_i
 * Comment  :       获取waiting_for_limit_input值
************************************************************/
uint8_t get_wf_limit_i(void)
{
  return waiting_for_limit_input;
}

/************************************************************ 
 * Function :       get_ratiod
 * Comment  :       获取变比ratio
************************************************************/
float get_ratio(void)
{
  return ratio;
}

/************************************************************ 
 * Function :       get_limit
 * Comment  :       获取阈值limit
************************************************************/
float get_limit(void)
{
  return limit;
}

/************************************************************ 
 * Function :       set_wf_limit_i
 * Comment  :       设置新waiting_for_ratio_input值
************************************************************/
void set_wf_limit_i(uint8_t new_one)
{
  waiting_for_limit_input = new_one;
}

/************************************************************ 
 * Function :       set_ratio
 * Comment  :       设置新ratio值
************************************************************/
void set_ratio(float new_ratio)
{
  ratio = new_ratio;
}

/************************************************************ 
 * Function :       set_limit
 * Comment  :       设置新limit值
************************************************************/
void set_limit(float new_limit)
{
  limit = new_limit;
}

/****************************End*****************************/
