/************************************************************
 * ��Ȩ�� 
 * �ļ���Timer.c
 * ����: 
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: 
************************************************************/

/************************* ͷ�ļ� *************************/

#include "Timer.h"

/************************* �궨�� *************************/


/************************ �������� ************************/
static unsigned char  millisecond_flag  = 0;    //5ms��־λ
static unsigned char  second_flag = 0;          //1s��־λ

/************************ �������� ************************/
static void ConfigTimer1(unsigned short arr, unsigned short psc); //����TIMER1
static void ConfigTimer4(unsigned short arr, unsigned short psc); //����TIMER4

/************************ �������� ************************/
/************************************************************ 
 * Function :       ConfigTimer1
 * Comment  :       ����TIMER1
 * Parameter:       arr-�Զ���װֵ��psc-Ԥ��Ƶ��ֵ
 * Return   :       null
 * Author   :       
 * Date     :
************************************************************/
static  void ConfigTimer1(unsigned short arr, unsigned short psc)
{
  timer_parameter_struct timer_initpara;                //timer_initpara���ڴ�Ŷ�ʱ���Ĳ���

  //ʹ��RCU���ʱ��
  rcu_periph_clock_enable(RCU_TIMER1);                  //ʹ��TIMER1��ʱ��

  timer_deinit(TIMER1);                                 //����TIMER1�����ָ�Ĭ��ֵ
  timer_struct_para_init(&timer_initpara);              //��ʼ��timer_initpara
 
  //����TIMER1
  timer_initpara.prescaler         = psc;               //����Ԥ��Ƶ��ֵ
  timer_initpara.counterdirection  = TIMER_COUNTER_UP;  //�������ϼ���ģʽ
  timer_initpara.period            = arr;               //�����Զ���װ��ֵ
  timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;  //����ʱ�ӷָ�
  timer_init(TIMER1, &timer_initpara);                  //���ݲ�����ʼ����ʱ��

  timer_interrupt_enable(TIMER1, TIMER_INT_UP);         //ʹ�ܶ�ʱ���ĸ����ж�
  nvic_irq_enable(TIMER1_IRQn, 1, 0);                   //����NVIC �������ȼ�

  timer_enable(TIMER1);  //ʹ�ܶ�ʱ��
}

/************************************************************ 
 * Function :       ConfigTimer4
 * Comment  :       ����TIMER4
 * Parameter:       arr-�Զ���װֵ��psc-Ԥ��Ƶ��ֵ
 * Return   :       null
 * Author   :       
 * Date     :
************************************************************/
static  void ConfigTimer4(unsigned short arr, unsigned short psc)
{
  timer_parameter_struct timer_initpara;                //timer_initpara���ڴ�Ŷ�ʱ���Ĳ���

  //ʹ��RCU���ʱ��
  rcu_periph_clock_enable(RCU_TIMER4);                  //ʹ��TIMER4��ʱ��

  timer_deinit(TIMER4);                                 //����TIMER4�����ָ�Ĭ��ֵ
  timer_struct_para_init(&timer_initpara);              //��ʼ��timer_initpara
 
  //����TIMER4
  timer_initpara.prescaler         = psc;               //����Ԥ��Ƶ��ֵ
  timer_initpara.counterdirection  = TIMER_COUNTER_UP;  //�������ϼ���ģʽ
  timer_initpara.period            = arr;               //�����Զ���װ��ֵ
  timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;  //����ʱ�ӷָ�
  timer_init(TIMER4, &timer_initpara);                  //���ݲ�����ʼ����ʱ��

  timer_interrupt_enable(TIMER4, TIMER_INT_UP);         //ʹ�ܶ�ʱ���ĸ����ж�
  nvic_irq_enable(TIMER4_IRQn, 1, 0);                   //����NVIC�������ȼ�

  timer_enable(TIMER4);  //ʹ�ܶ�ʱ��
}

/************************************************************ 
 * Function :       TIMER1_IRQHandler
 * Comment  :       TIMER1�жϷ�����
 * Parameter:       null
 * Return   :       null
 * Author   :       
 * Date     :
************************************************************/
void TIMER1_IRQHandler(void)  
{
  static  signed short s_iCnt5  = 0;
  
  if(timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP) == SET)    //�ж϶�ʱ�������ж��Ƿ���
  {
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);           //�����ʱ�������жϱ�־ 
  }
  
  s_iCnt5++;
  if(s_iCnt5 >= 5)  //5ms�������ļ���ֵ���ڻ����5
  {
    s_iCnt5 = 0;       //����5ms�������ļ���ֵΪ0
    millisecond_flag = 1;      //��5ms��־λ��ֵ����1
  }
}

/************************************************************ 
 * Function :       TIMER4_IRQHandler
 * Comment  :       TIMER4�жϷ�����
 * Parameter:       null
 * Return   :       null
 * Author   :       
 * Date     :
************************************************************/
void TIMER4_IRQHandler(void)  
{
  static  signed short s_iCnt1000  = 0;                              //����һ����̬����s_iCnt1000��Ϊ1s������

  if (timer_interrupt_flag_get(TIMER4, TIMER_INT_FLAG_UP) == SET)    //�ж϶�ʱ�������ж��Ƿ���
  {
    timer_interrupt_flag_clear(TIMER4, TIMER_INT_FLAG_UP);           //�����ʱ�������жϱ�־ 
  }
  
  s_iCnt1000++;           //1000ms�������ļ���ֵ��1

  if(s_iCnt1000 >= 1000)  //1000ms�������ļ���ֵ���ڻ����1000
  {
    s_iCnt1000 = 0;       //����1000ms�������ļ���ֵΪ0
    second_flag = 1;      //��1s��־λ��ֵ����1
  }
}

/************************************************************ 
 * Function :       InitTimer
 * Comment  :       ��ʼ����ʱ��ģ��
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
 * Comment  :       ��ȡ5ms��־λ��ֵ
 * Parameter:       null
 * Return   :       millisecond_flag��־λ��ֵ
 * Author   :       
 * Date     :
************************************************************/
unsigned char  get_5ms_Flag(void)
{
  return(millisecond_flag);     //����5ms��־λ��ֵ
}

/************************************************************ 
 * Function :       clear_5ms_flag
 * Comment  :       ���5ms��־λ
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
 * Comment  :       ��ȡ1s��־λ��ֵ
 * Parameter:       null
 * Return   :       second_flag��־λ��ֵ
 * Author   :       
 * Date     :
************************************************************/
unsigned char  get_1s_Flag(void)
{
  return(second_flag);    //����1s��־λ��ֵ
}

/************************************************************ 
 * Function :       clear_1s_flag
 * Comment  :       ���1s��־λ
 * Parameter:       null
 * Return   :       null
 * Author   :       
 * Date     :
************************************************************/
void  clear_1s_flag(void)
{
  second_flag = 0;    //��1s��־λ��ֵ����Ϊ0
}
