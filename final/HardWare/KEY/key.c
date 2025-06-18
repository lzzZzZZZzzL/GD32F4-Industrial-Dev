/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：key.c
 * 作者: Lingyu Meng
 * 平台: 2025CIMC IHD-V04
 * 版本: Lingyu Meng     2025/3/10     V0.01    original
************************************************************/

/************************* 头文件 *************************/

#include "key.h"

/************************ 全局变量定义 ************************/
static uint8_t cycle_time = CYCLE_5S;
extern SamplingState sampling_flag;

/************************************************************ 
 * Function :       KEY_Init
 * Comment  :       用于初始化KEY端口
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-03-30 V0.1 original
************************************************************/

void KEY_Init(void)
{
	
	rcu_periph_clock_enable(RCU_GPIOE);    									// 初始化GPIO_E总线时钟  
	
	gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_3);	// 配置PE2为上拉输入
  gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_4);
  gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_6);
	gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7);
	
}


/************************************************************ 
 * Function :       KEY_Stat
 * Comment  :       用于读取按键状态
 * Parameter:       按键端口和引脚
 * Return   :       按键状态：1为按键按下，0为按键未按下
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/
uint8_t KEY_Stat(uint32_t port, uint16_t pin)
{
    if(gpio_input_bit_get(port, pin) == RESET)					    //读取GPIO状态，如果按键被按下了
    {
        delay_1ms(20);											//延时消抖
        if(gpio_input_bit_get(port, pin) == RESET)				  //再读一次GPIO，按键真的被按下了吗？
        {
            while(gpio_input_bit_get(port, pin) == RESET);  //等待按键释放
            return 1;											//返回按键状态1
        }
    }
    return 0;
}


/************************************************************ 
 * Function :       EXTI_PIN_Init
 * Comment  :       用于初始化中断
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-03-30 V0.1 original
************************************************************/

void EXTI_PIN_Init(void)
{
	rcu_periph_clock_enable(RCU_SYSCFG);								//使能SYSCFG时钟
	
	syscfg_exti_line_config(EXTI_SOURCE_GPIOE, EXTI_SOURCE_PIN3);		//连接PE2端口到中断线
//  syscfg_exti_line_config(EXTI_SOURCE_GPIOE, EXTI_SOURCE_PIN4);
//  syscfg_exti_line_config(EXTI_SOURCE_GPIOE, EXTI_SOURCE_PIN6);
//  syscfg_exti_line_config(EXTI_SOURCE_GPIOE, EXTI_SOURCE_PIN7);
	
	exti_init(EXTI_3, EXTI_INTERRUPT, EXTI_TRIG_FALLING);				//配置中断为下降沿触发
//  exti_init(EXTI_4, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
//  exti_init(EXTI_6, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
//  exti_init(EXTI_7, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	
	nvic_irq_enable(EXTI3_IRQn,2,2);									//使能外部中断线 EXTI2； 设置优先级
//  nvic_irq_enable(EXTI4_IRQn,2,2);
//  nvic_irq_enable(EXTI5_9_IRQn,2,2);
//  nvic_irq_enable(EXTI5_9_IRQn,2,2);
	
	exti_interrupt_flag_clear(EXTI_3);									// 清除中断标志位
//  exti_interrupt_flag_clear(EXTI_4);
//  exti_interrupt_flag_clear(EXTI_6);
//  exti_interrupt_flag_clear(EXTI_7);


}

/************************************************************ 
 * Function :       EXTI3_IRQHandler
 * Comment  :       EXTI3的中断服务函数，触发中断后LED2翻转
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-03-30 V0.1 original
************************************************************/
void EXTI3_IRQHandler(void)
{
	if(RESET != exti_interrupt_flag_get(EXTI_3))
	{
		gpio_bit_toggle(GPIOA,GPIO_PIN_5);   // 端口电平翻转
    if(sampling_flag == SAMPLING_START)
    {
      stop_function("(key press)");
      set_command_type(2);
      set_command_from(0);
    }
    else
    {
      start_function("(key press)");
      set_command_type(2);
      set_command_from(0);
    }
    set_log_log_flag();
      
//    sampling_flag = (SamplingState)((sampling_flag + 1) % SAMPLING_MAX);
		
		exti_interrupt_flag_clear(EXTI_3);	 // 清除中断标志位
	
	}
}

/************************************************************ 
 * Function :       scan_key
 * Comment  :       检查按键是否被按下
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-03-30 V0.1 original
************************************************************/
void scan_key(void)
{
  if(KEY_Stat(KEY_PORT,KEY2_PIN))
  {
//      gpio_bit_toggle(GPIOA,GPIO_PIN_6);   // 端口电平翻转
    set_log_log_flag();
    cycle_time = CYCLE_5S;
    set_log_log_flag();
    set_command_type(14);
    save_flash(CYCLE_5S, FLASH_CYCLE);        // 保存采样周期到flash
    printf("sample cycle adjust:5s\r\n\r\n");
    set_command_type(14);
  }
  else if(KEY_Stat(KEY_PORT,KEY3_PIN))
  {
    char result[32];
//      gpio_bit_toggle(GPIOA,GPIO_PIN_6);   // 端口电平翻转
    cycle_time = CYCLE_10S;
    set_log_log_flag();
    set_command_type(14);
    save_flash(CYCLE_10S, FLASH_CYCLE);        // 保存采样周期到flash
    printf("sample cycle adjust:10s\r\n\r\n");
  }
  else if(KEY_Stat(KEY_PORT,KEY4_PIN))
  {
//      gpio_bit_toggle(GPIOA,GPIO_PIN_6);   // 端口电平翻转
    cycle_time = CYCLE_15S;
    set_log_log_flag();
    set_command_type(14);
    save_flash(CYCLE_15S, FLASH_CYCLE);        // 保存采样周期到flash
    printf("sample cycle adjust:15s\r\n\r\n");
  }
}
///************************************************************ 
// * Function :       EXTI2_IRQHandler
// * Comment  :       EXTI2的中断服务函数，触发中断后LED2翻转
// * Parameter:       null
// * Return   :       null
// * Author   :       Lingyu Meng
// * Date     :       2025-03-30 V0.1 original
//************************************************************/
//void EXTI4_IRQHandler(void)
//{
//	if(RESET != exti_interrupt_flag_get(EXTI_4))
//	{
//		gpio_bit_toggle(GPIOA,GPIO_PIN_6);   // 端口电平翻转
//    cycle_time=CYCLE_10S;
//		
//		exti_interrupt_flag_clear(EXTI_4);	 // 清除中断标志位
//	
//	}
//}


///************************************************************ 
// * Function :       EXTI2_IRQHandler
// * Comment  :       EXTI2的中断服务函数，触发中断后LED2翻转
// * Parameter:       null
// * Return   :       null
// * Author   :       Lingyu Meng
// * Date     :       2025-03-30 V0.1 original
//************************************************************/
//void EXTI6_IRQHandler(void)
//{
//	if(RESET != exti_interrupt_flag_get(EXTI_6))
//	{
//		gpio_bit_toggle(GPIOA,GPIO_PIN_6);   // 端口电平翻转
////    cycle_time=CYCLE_10S;
////    printf("change 10   %d",((signed short)cycle_time)/1000);
//		
//		exti_interrupt_flag_clear(EXTI_6);	 // 清除中断标志位
//	
//	}
//}


///************************************************************ 
// * Function :       EXTI2_IRQHandler
// * Comment  :       EXTI2的中断服务函数，触发中断后LED2翻转
// * Parameter:       null
// * Return   :       null
// * Author   :       Lingyu Meng
// * Date     :       2025-03-30 V0.1 original
//************************************************************/
//void EXTI7_IRQHandler(void)
//{
//	if(RESET != exti_interrupt_flag_get(EXTI_7))
//	{
//    gpio_bit_toggle(GPIOA,GPIO_PIN_6);
//		cycle_time=CYCLE_15S;
//		
//		exti_interrupt_flag_clear(EXTI_7);	 // 清除中断标志位
//	
//	}
//}

/************************************************************ 
 * Function :       get_cycle_time
 * Comment  :       获取周期时间
 * Parameter:       null
 * Return   :       cycle_time
************************************************************/
uint8_t get_cycle_time(void)
{
  return cycle_time;
}

/************************************************************ 
 * Function :       set_cycle_time
 * Comment  :       设置周期时间
 * Parameter:       null
 * Return   :       null
************************************************************/
void set_cycle_time(uint8_t new_cycle_time)
{
  cycle_time = new_cycle_time;
}

/****************************End*****************************/

