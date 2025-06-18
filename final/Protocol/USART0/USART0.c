/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���usart0.c
 * ����: Lingyu Meng
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Lingyu Meng     2025/3/10     V0.01    original
 * �޸�: ChatGPT          2025/6/14     V0.02    �����ַ���ָ���
************************************************************/

/************************* ͷ�ļ� *************************/
#include "USART0.h"

/************************* �궨�� *************************/


/************************ �������� ************************/
uint8_t rx_buffer[RX_BUFFER_SIZE];    // ���ջ�����
uint8_t rx_index = 0;                 // ��ǰ����������

SamplingState sampling_flag = SAMPLING_STOP;

static uint8_t is_hide = 0;     //�Ƿ���ܱ�־λ
static uint8_t LED_state = 0;   //LED״̬λ��0����1��˸

static uint8_t command_from;
static uint8_t command_type;
/************************ �������� ************************/
void test_function(void);
  
/************************ �������� ************************/

/************************************************************
 * �ض��� printf ����� USART0
************************************************************/
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}

/************************************************************ 
 * Function :       USART0_Config
 * Comment  :       ��ʼ�� USART0
************************************************************/
void USART0_Config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);    
    rcu_periph_clock_enable(RCU_USART0);   

    gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9 | GPIO_PIN_10);				
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);  		
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);  
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_10);  	
    gpio_output_options_set(GPIOA, GPIO_MODE_ANALOG, GPIO_OSPEED_50MHZ, GPIO_PIN_10);  

    usart_deinit(USART0);    	
    usart_word_length_set(USART0, USART_WL_8BIT);   
    usart_stop_bit_set(USART0, USART_STB_1BIT);     
    usart_parity_config(USART0, USART_PM_NONE);		
    usart_baudrate_set(USART0, 115200U);     		
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);     
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);   
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_enable(USART0);          	

    usart_interrupt_enable(USART0, USART_INT_RBNE);  // ���������ж�
    nvic_irq_enable(USART0_IRQn, 0, 0);              // ʹ��NVIC�ж�
}

/************************************************************ 
 * Function :       USART0_SendData
 * Comment  :       ͨ�� USART0 ������������
************************************************************/
void USART0_SendData(uint16_t *buf, uint16_t len)
{
    uint16_t t;
    for(t = 0; t < len; t++)      
    {           
        while(usart_flag_get(USART0, USART_FLAG_TC) == RESET);  
        usart_data_transmit(USART0, buf[t]);
    }     
    while(usart_flag_get(USART0, USART_FLAG_TC) == RESET);          
}

///************************************************************ 
// * Function :       USART0_SendString
// * Comment  :       ��USART0����һ����'\0'��β���ַ���
// * Parameter:       str - �ַ���ָ��
// * Return   :       null
//************************************************************/
//void USART0_SendString(const char *str)
//{
//    while (*str)
//    {
//        usart_data_transmit(USART0, (uint8_t)(*str));
//        while (RESET == usart_flag_get(USART0, USART_FLAG_TBE));
//        str++;
//    }
//}

/************************************************************ 
 * Function :       USART0_IRQHandler
 * Comment  :       USART0 �жϷ�������֧���ַ�������
************************************************************/
void USART0_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
    {			
        uint8_t received_char = usart_data_receive(USART0);    

        if (received_char == '\r' || received_char == '\n')  // �յ��س����У���Ϊ�������
        {
            rx_buffer[rx_index] = '\0';           // �ַ���������
            process_data(rx_buffer);              // ������������
            rx_index = 0;                         // ����������׼����һ��
        }
        else
        {
            if (rx_index < RX_BUFFER_SIZE - 1)    // �������������
            {
                rx_buffer[rx_index++] = received_char;
            }
            else
            {
                rx_index = 0;  // ������������������½���
            }
        }
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);  
    }    
}

/************************************************************ 
 * Function :       process_data
 * Comment  :       ���ַ���������յ�������
************************************************************/
void process_data(uint8_t *data)
{
  //printf("Debug: Enter process_data with [%s]\r\n", data);
  if(get_wf_ratio_i())
  {
      float new_ratio;
      char result[18];
    
      int parsed_count = sscanf((char *)data, "%f", &new_ratio);

      if (parsed_count == 1 && new_ratio >= 0.0f && new_ratio <= 100.0f)
      {
          set_ratio(new_ratio);
          printf("ratio modified success\r\n");
          printf("Ratio = %.2f\r\n", new_ratio);
          snprintf(result, sizeof(result), "success to:%.2f", new_ratio);
        set_log_log_flag();
          command_type = 6;
      }
      else
      {
          printf("ratio invalid\r\n");
          printf("Ratio = %.2f\r\n", new_ratio);
      }
      set_wf_ratio_i(0);
      return;
  }
  if(get_wf_limit_i())
  {
      float new_limit;
      int parsed_count = sscanf((char *)data, "%f", &new_limit);
      char result[18];
    
      if (parsed_count == 1 && new_limit >= 0.0f && new_limit <= 200.0f)
      {
          set_limit(new_limit);
          printf("limit modified success\r\n");
          printf("limit = %.2f\r\n", new_limit);
          snprintf(result, sizeof(result), "success to:%.2f", new_limit);
        set_log_log_flag();
          command_type = 8;
      }
      else
      {
          printf("limit invalid\r\n");
          printf("limit = %.2f\r\n", new_limit);
      }
      set_wf_limit_i(0);
      return;
  }
  
  if(strcmp((char *)data, "start") == 0)        //��ʼ����
  {
    LED_state = 1;    //LED����˸��־λ
    set_log_log_flag();
    command_type = 1;
    start_function("(command)");
    command_from = 1;
  }
  else if(strcmp((char *)data, "stop") == 0)    //ֹͣ����
  {
    LED_state = 0;    //LED����
    stop_function("(command)");
    set_log_log_flag();
    command_type = 2;
    command_from = 1;
  }
  else if(strcmp((char*)data, "hide") == 0)     //��������
  {
    is_hide = 1;
    set_log_log_flag();
    command_type = 3;
  }
  else if(strcmp((char*)data, "unhide") == 0)   //ȡ������   
  {
    is_hide = 0;
    set_log_log_flag();
    command_type = 4;
  }
  else if(strcmp((char *)data, "ratio") == 0)   //��ȡ���
  {
    ratio_function();
    set_log_log_flag();
    command_type = 5;
  }
  else if(strcmp((char *)data, "limit") == 0)   //��ȡ��ֵ
  {
    limit_function();
    set_log_log_flag();
    command_type = 7;
  }
  else if(strcmp((char*)data, "config save") == 0)  //�������
  {
    config_save_function();
  }
  else if(strcmp((char*)data, "config read") == 0)  //��������
  {
    config_read_function();
  }
  else if (strcmp((char *)data, "conf") == 0)
  {
    conf_function();
  }
  else if(strcmp((char*)data, "RTC now") == 0)    //���ڴ�ӡһ�ε�ǰʱ��
  {
    printf("\nCurrent Time:");
    rtc_show_time();
  }
  else if(strcmp((char*)data, "test") == 0)       //�ϵ����
  {
    set_log_log_flag();
    command_type = 11;
    test_function();
  }
  else if(strcmp((char*)data, "RTC Config") == 0) //���û�׼ʱ��
  {
    usart_flush_rx(USART0);    //����Ӆ�������
    set_log_log_flag();
    command_type = 9;
    rtc_setup();
  }
  else
  {
    //�Ǳ�׼ָ�ԭ���ش�
    for (uint8_t i = 0; data[i] != '\0'; i++)
    {
        usart_data_transmit(USART0, data[i]);
        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    }
    usart_data_transmit(USART0, '\r');
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    usart_data_transmit(USART0, '\n');
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
  }
}

void uart_receive_string(char *buffer)
{
    uint8_t recv_char;
    uint16_t index = 0;

    while (1)
    {
        while (usart_flag_get(USART0, USART_FLAG_RBNE) == RESET);
        recv_char = usart_data_receive(USART0);

        if (recv_char == '\n' || recv_char == '\r')
        {
            buffer[index] = '\0';  
            break;
        }
        else
        {
            buffer[index++] = recv_char;
        }

        if (index >= 255)
        {
            buffer[255] = '\0';
            break;
        }
    }
}

/************************************************************ 
 * Function :       start_function
 * Comment  :       ִ��"start"ָ���Ӧ����
* Parameter:       command_from:�����򴮿�����
************************************************************/
void start_function(const char *command_from)
{
  printf("Periodic Sampling\r\n");
  printf("cycle_buffer: %d s \r\n", (uint16_t)get_cycle_time());
  LED1_ON();
  LED_state = 1;
  sampling_flag = SAMPLING_START;
}

/************************************************************ 
 * Function :       stop_function
 * Comment  :       ִ��"stop"ָ���Ӧ����
************************************************************/
void stop_function(const char *command_from)
{
  printf("Periodic Sampling STOP\r\n");
  OLED_ShowString(0, 0, "system idle", 16);
  LED1_OFF();
  LED_state = 0;
  sampling_flag = SAMPLING_STOP;
}

void usart_flush_rx(uint32_t usart_periph) 
{
    
    while(usart_flag_get(usart_periph, USART_FLAG_RBNE))
    {
        usart_data_receive(usart_periph); 
    }
    
    if(usart_flag_get(usart_periph, USART_FLAG_IDLE))
    {
        usart_interrupt_flag_clear(usart_periph, USART_INT_FLAG_IDLE);
    }
}

/************************************************************ 
 * Function :       get_is_hide
 * Comment  :       �����ļ���ȡis_hide��־λ
************************************************************/
uint8_t get_is_hide(void)
{
  return is_hide;
}

/************************************************************ 
 * Function :       get_LED_state1
 * Comment  :       �����ļ���ȡLED_state��־λ
************************************************************/
uint8_t get_LED_state(void)
{
  return LED_state;
}

/************************************************************ 
 * Function :       get_sampling_flag
 * Comment  :       �����ļ���ȡsampling_flag��־λ
************************************************************/
uint8_t get_sampling_flag(void)
{
  return (uint8_t)sampling_flag;
}


/************************************************************ 
 * Function :       log_neeg
 * Comment  :       �����ļ���ȡsampling_flag��־λ
************************************************************/
void log_need(char *log_need)
{
  switch (command_type)
  {
    case 1: 
          if(command_from == 1)    //�ж��Ƿ�Ϊ���ڴ���
          {
            snprintf(log_need, 50, "sample start - cycle %ds(command)", get_cycle_time());
          }
          else
          {
            snprintf(log_need, 50, "sample start - cycle %ds(key press)", get_cycle_time());
          }
          command_type = 0; 
          break;
    case 2: 
          if(command_from == 1)    //�ж��Ƿ�Ϊ���ڴ���
          {
            snprintf(log_need, 50, "sample stop(command)");
          }
          else
          {
            snprintf(log_need, 50, "sample stop(key press)");
          }
          command_type = 0;
          break;
    case 3: 
      snprintf(log_need, 16, "hide data");
      command_type = 0; 
      break;
    case 4: 
      snprintf(log_need, 16, "unhide data");
      command_type = 0; 
      break;
    case 5: 
      snprintf(log_need, 18, "ratio config");
      command_type = 0; 
      break;
    case 6: 
      snprintf(log_need, 32, "ratio config success to %.2f", get_ratio());
      command_type = 0; 
      break;
    case 7: 
      snprintf(log_need, 18, "limit config");
      command_type = 0; 
      break;
    case 8: 
      snprintf(log_need, 32, "limit config success to %.2f", get_limit());
      command_type = 0; 
      break;
    case 9: 
      snprintf(log_need, 18, "rtc config");
      command_type = 0; 
      break;
    case 10: 
      char datetime[25]=" ";
      Get_RTC_DateTimeString(datetime);
      snprintf(log_need, 45, "rtc config success to %s", datetime);
      command_type = 0; 
      break;
    case 11: 
      snprintf(log_need, 24, "system hardware test");
      command_type = 0; 
      break;
    case 12: 
      snprintf(log_need, 10, "test ok");
      command_type = 0; 
      break;
    case 13: 
      snprintf(log_need, 35, "test error: tf card not found");
      command_type = 0; 
      break;
    case 14: 
      snprintf(log_need, 35, "cycle switch to %02ds(key press)", get_cycle_time ());
      command_type = 0; 
      break;
  }
}

void set_command_type(uint8_t new_command)
{
  command_type = new_command;
}

void set_command_from(uint8_t new_command_from)
{
  command_from = new_command_from;
}
