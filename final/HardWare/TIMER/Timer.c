/************************************************************
 * 版权： 
 * 文件：Timer.c
 * 作者: 
 * 平台: 2025CIMC IHD-V04
 * 版本: 
************************************************************/

/************************* 头文件 *************************/

#include "Timer.h"

/************************* 宏定义 *************************/


/************************ 变量定义 ************************/
static unsigned char  millisecond_flag  = 0;    //5ms标志位
static unsigned char  second_flag = 0;          //1s标志位

/************************ 函数声明 ************************/
static void ConfigTimer1(unsigned short arr, unsigned short psc); //配置TIMER1
static void ConfigTimer4(unsigned short arr, unsigned short psc); //配置TIMER4

/************************ 函数定义 ************************/
/************************************************************ 
 * Function :       ConfigTimer1
 * Comment  :       配置TIMER1
 * Parameter:       arr-自动重装值，psc-预分频器值
 * Return   :       null
 * Author   :       
 * Date     :
************************************************************/
static  void ConfigTimer1(unsigned short arr, unsigned short psc)
{
  timer_parameter_struct timer_initpara;                //timer_initpara用于存放定时器的参数

  //使能RCU相关时钟
  rcu_periph_clock_enable(RCU_TIMER1);                  //使能TIMER1的时钟

  timer_deinit(TIMER1);                                 //设置TIMER1参数恢复默认值
  timer_struct_para_init(&timer_initpara);              //初始化timer_initpara
 
  //配置TIMER1
  timer_initpara.prescaler         = psc;               //设置预分频器值
  timer_initpara.counterdirection  = TIMER_COUNTER_UP;  //设置向上计数模式
  timer_initpara.period            = arr;               //设置自动重装载值
  timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;  //设置时钟分割
  timer_init(TIMER1, &timer_initpara);                  //根据参数初始化定时器

  timer_interrupt_enable(TIMER1, TIMER_INT_UP);         //使能定时器的更新中断
  nvic_irq_enable(TIMER1_IRQn, 1, 0);                   //配置NVIC 设置优先级

  timer_enable(TIMER1);  //使能定时器
}

/************************************************************ 
 * Function :       ConfigTimer4
 * Comment  :       配置TIMER4
 * Parameter:       arr-自动重装值，psc-预分频器值
 * Return   :       null
 * Author   :       
 * Date     :
************************************************************/
static  void ConfigTimer4(unsigned short arr, unsigned short psc)
{
  timer_parameter_struct timer_initpara;                //timer_initpara用于存放定时器的参数

  //使能RCU相关时钟
  rcu_periph_clock_enable(RCU_TIMER4);                  //使能TIMER4的时钟

  timer_deinit(TIMER4);                                 //设置TIMER4参数恢复默认值
  timer_struct_para_init(&timer_initpara);              //初始化timer_initpara
 
  //配置TIMER4
  timer_initpara.prescaler         = psc;               //设置预分频器值
  timer_initpara.counterdirection  = TIMER_COUNTER_UP;  //设置向上计数模式
  timer_initpara.period            = arr;               //设置自动重装载值
  timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;  //设置时钟分割
  timer_init(TIMER4, &timer_initpara);                  //根据参数初始化定时器

  timer_interrupt_enable(TIMER4, TIMER_INT_UP);         //使能定时器的更新中断
  nvic_irq_enable(TIMER4_IRQn, 1, 0);                   //配置NVIC设置优先级

  timer_enable(TIMER4);  //使能定时器
}

/************************************************************ 
 * Function :       TIMER1_IRQHandler
 * Comment  :       TIMER1中断服务函数
 * Parameter:       null
 * Return   :       null
 * Author   :       
 * Date     :
************************************************************/
void TIMER1_IRQHandler(void)  
{
  static  signed short s_iCnt5  = 0;
  
  if(timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP) == SET)    //判断定时器更新中断是否发生
  {
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);           //清除定时器更新中断标志 
  }
  
  s_iCnt5++;
  if(s_iCnt5 >= 5)  //5ms计数器的计数值大于或等于5
  {
    s_iCnt5 = 0;       //重置5ms计数器的计数值为0
    millisecond_flag = 1;      //将5ms标志位的值设置1
  }
}

/************************************************************ 
 * Function :       TIMER4_IRQHandler
 * Comment  :       TIMER4中断服务函数
 * Parameter:       null
 * Return   :       null
 * Author   :       
 * Date     :
************************************************************/
void TIMER4_IRQHandler(void)  
{
  static  signed short s_iCnt1000  = 0;                              //定义一个静态变量s_iCnt1000作为1s计数器

  if (timer_interrupt_flag_get(TIMER4, TIMER_INT_FLAG_UP) == SET)    //判断定时器更新中断是否发生
  {
    timer_interrupt_flag_clear(TIMER4, TIMER_INT_FLAG_UP);           //清除定时器更新中断标志 
  }
  
  s_iCnt1000++;           //1000ms计数器的计数值加1

  if(s_iCnt1000 >= 1000)  //1000ms计数器的计数值大于或等于1000
  {
    s_iCnt1000 = 0;       //重置1000ms计数器的计数值为0
    second_flag = 1;      //将1s标志位的值设置1
  }
}

/************************************************************ 
 * Function :       InitTimer
 * Comment  :       初始化定时器模块
 * Parameter:       null
 * Return   :       null
 * Author   :       
 * Date     :
************************************************************/
void InitTimer(void)
{
  ConfigTimer1(499, 239);
  ConfigTimer4(499, 239);
}

/************************************************************ 
 * Function :       get_5ms_Flag
 * Comment  :       获取5ms标志位的值
 * Parameter:       null
 * Return   :       millisecond_flag标志位的值
 * Author   :       
 * Date     :
************************************************************/
unsigned char  get_5ms_Flag(void)
{
  return(millisecond_flag);     //返回5ms标志位的值
}

/************************************************************ 
 * Function :       clear_5ms_flag
 * Comment  :       清除5ms标志位
 * Parameter:       null
 * Return   :       null
 * Author   :       
 * Date     :
************************************************************/
void  clear_5ms_flag(void)
{
  millisecond_flag = 0;
}

/************************************************************ 
 * Function :       get_1s_Flag
 * Comment  :       获取1s标志位的值
 * Parameter:       null
 * Return   :       second_flag标志位的值
 * Author   :       
 * Date     :
************************************************************/
unsigned char  get_1s_Flag(void)
{
  return(second_flag);    //返回1s标志位的值
}

/************************************************************ 
 * Function :       clear_1s_flag
 * Comment  :       清除1s标志位
 * Parameter:       null
 * Return   :       null
 * Author   :       
 * Date     :
************************************************************/
void  clear_1s_flag(void)
{
  second_flag = 0;    //将1s标志位的值设置为0
}
