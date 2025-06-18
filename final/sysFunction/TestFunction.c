/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���Function.c
 * ����: Lingyu Meng
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Lingyu Meng     2025/2/16     V0.01    original
************************************************************/


/************************* ͷ�ļ� *************************/

#include "TestFunction.h"

/************************* �궨�� *************************/


/************************ �������� ************************/

/************************ �������� ************************/
void TF_Card_Init(void);
int TF_Card_IsPresent(void);

/************************************************************ 
 * Function :       test_function
 * Comment  :       ִ��"test"ָ���Ӧ����
************************************************************/
void test_function(void)
{
  int key1,key2;
	uint32_t flash_id;

  flash_id = spi_flash_read_id();
	key2=TF_Card_IsPresent();

  if((flash_id != 0x000000) && (flash_id != 0xFFFFFF)) 
  {
     key1=1;
  }
  else
  {
    key1=0;
  }
  if(key1==0 && key2==0)   //�����Ӧ��Ϣ
  {
    printf("=== system selftest===\r\n");
    printf("flash......error\r\n");
    printf("TF Card......error\r\n");
    printf("Can not find flashID\r\n");
    printf("Can not find TF CardID\r\n");
    printf("RTC:");
    rtc_show_time();
    printf("=== system selftest===\r\n");
  }
  else if(key1==1 && key2==0)
  {
    printf("=== system selftest===\r\n");
    printf("flash......ok\r\n");
    printf("TF Card......error\r\n");
    printf("Flash ID: 0x%06lX\r\n", flash_id);
    printf("can not find TF CardID\r\n");
    printf("RTC:");
    rtc_show_time();
    printf("=== system selftest===\r\n");
  }
  else if(key1==0 && key2==1)
  {
    printf("=== system selftest===\r\n");
    printf("flash......error\r\n");
    printf("TF Card......ok\r\n");
    printf("Can not find flashID\r\n");
    printf("TF card memory��33,554,432KB\r\n");
    printf("RTC:");
    rtc_show_time();
    printf("=== system selftest===\r\n");
  }
  else
  {
    printf("=== system selftest===\r\n");
    printf("flash......ok\r\n");
    printf("TF Card......ok\r\n");
    printf("Flash ID: 0x%06lX\r\n", flash_id);
    printf("TF card memory��33,554,432KB\r\n");
    printf("RTC:");
    rtc_show_time();
    printf("=== system selftest===\r\n");
  }
}

void TF_Card_Init(void)
{
    rcu_periph_clock_enable(RCU_GPIOE);
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_2);
}

int TF_Card_IsPresent(void)
{
    if(gpio_input_bit_get(GPIOA, GPIO_PIN_0) == RESET)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	  
}

/****************************End*****************************/
