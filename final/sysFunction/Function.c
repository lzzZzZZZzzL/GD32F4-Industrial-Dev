/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���Function.c
 * ����: Lingyu Meng
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Lingyu Meng     2025/2/16     V0.01    original
************************************************************/


/************************* ͷ�ļ� *************************/

#include "Function.h"

#include "ff.h"
#include "diskio.h"
#include "sdcard.h"

/************************* �궨�� *************************/
#define  SFLASH_ID                     0xC84013
#define BUFFER_SIZE                    256
#define TX_BUFFER_SIZE                 BUFFER_SIZE
#define RX_BUFFER_SIZE                 BUFFER_SIZE
#define  FLASH_WRITE_ADDRESS           0x000000
#define  FLASH_READ_ADDRESS            FLASH_WRITE_ADDRESS

typedef struct {
    float ratio;         // ��Ȳ���
    float limit;         // ��ֵ����
} ConfigParams;

/************************ �������� ************************/
uint32_t flash_id = 0;
uint8_t  tx_buffer[TX_BUFFER_SIZE];
uint16_t i = 0, count, result = 0;
uint8_t  is_successful = 0;

FIL fdst;
FATFS fs;
UINT br, bw;
BYTE buffer[128];
BYTE filebuffer[128];

static uint8_t TF_ok_flag=0;//TF��

static uint8_t new_sample_line_count;
static uint8_t new_overlimit_line_count;
static uint8_t new_hide_line_count;

static char write_sample_file_path[64] = {0};
char write_sample_folder_path[] = SAMPLE_FOLDER_PATH;
char write_sample_time[30] = " ";//��ʼΪ��
char write_sample_sample[50] = " ";
char write_sample_name_time[30] = {0};

static char write_overlimit_file_path[64] = {0};
char write_overlimit_folder_path[] = OVERLIMIT_FOLDER_PATH;
char write_overlimit_time[30] = " ";//��ʼΪ��
char write_overlimit_sample[50] = " ";
char write_overlimit_name_time[30] = {0};

static char write_hide_file_path[64] = {0};
char write_hide_folder_path[] = HIDEDATA_FOLDER_PATH;
char write_hide_time[30] = " ";//��ʼΪ��
char write_hide_sample[80] = " ";
char write_hide_name_time[30] = {0};

char write_log_file_path[64] = {0};
char write_log_sample[80] = " ";

extern uint8_t log_sample_flag;
extern uint8_t log_overlimit_flag;
extern uint8_t log_hide_flag;
extern uint8_t log_log_flag;
/************************ �������� ************************/
void Proc5msTask(void);  //5ms��������
void Proc1SecTask(void); //1s��������

ErrStatus memory_compare(uint8_t* src,uint8_t* dst,uint16_t length);

FRESULT create_folder(const char* path);

void Start_Sampling_To_Files(void);

void sample_to_file(char* folder_path, char* file_path, char* name_time, char* time, char* sample);//д��sample�ļ�
void overlimit_to_file(char* folder_path, char* file_path, char* name_time, char* time, char* sample);//д��overlimit�ļ�
void hide_to_file(char* folder_path, char* file_path, char* name_time, char* time, char* sample);//д��hide�ļ�
void log_to_file(char* file_path, char* sample);//д��log�ļ�

void parse_config_line(const char *line, ConfigParams *params,int i);
FRESULT read_config_file(ConfigParams *params);
int Get_RTC_DateTimeString_without(char *datetime);
int Get_RTC_DateTimeString(char *datetime);
/************************************************************ 
 * Function :       System_Init
 * Comment  :       ���ڳ�ʼ��MCU
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void System_Init(void)
{
	systick_config();     // ʱ������
  
	LED_Init();
  KEY_Init();
	EXTI_PIN_Init();
  
	USART0_Config();      // ���ڳ�ʼ��
  nvic_irq_enable(USART0_IRQn, 0, 0);//ʹ��USART0�ж�
  usart_interrupt_enable(USART0, USART_INT_RBNE);//�����жϴ�
  
  nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);	// �����ж����ȼ�����
  nvic_irq_enable(SDIO_IRQn, 0, 0);					// ʹ��SDIO�жϣ����ȼ�Ϊ0������ճ�����̣�ע�͵��ܴ����ļ�������д�����ݣ�
  
  ADC_port_init();
  InitTimer();
  OLED_Init();
  spi_flash_init();
  RTC_Init(); 
}


/************************************************************ 
 * Function :       UsrFunction
 * Comment  :       �û�������
 * Parameter:       null
 * Return   :       null
 * Author   :       Liu Tao @ GigaDevice
 * Date     :       2025-05-10 V0.1 original
************************************************************/
void UsrFunction(void)
{
//  printf("team_id %ld\r\n",read_flash_long(FLASH_TEAM));
  static uint8_t power_time = 0;
  power_time = (uint8_t)read_flash(FLASH_POWER);
  save_flash((power_time+1), FLASH_POWER);
	uint16_t k = 5;
	DSTATUS stat = 0;
	
	do
	{
		stat = disk_initialize(0); 			//��ʼ��SD�����豸��0��,�������������,ÿ����������������Ӳ�̡�U �̵ȣ�ͨ����������һ��Ψһ�ı�š�
	}while((stat != 0) && (--k));			//�����ʼ��ʧ�ܣ��������k�Ρ�
    
//    printf("SD Card disk_initialize:%d\r\n",stat);
    f_mount(0, &fs);						 //����SD�����ļ�ϵͳ���豸��0����
//    printf("SD Card f_mount:%d\r\n",stat);

	if(RES_OK == stat)						 //���ع��ؽ����FR_OK ��ʾ�ɹ�����
	{        
    TF_ok_flag=1;
//    printf("\r\nSD Card Initialize Success!\r\n");
    create_folder(SAMPLE_FOLDER);
    create_folder(OVERLIMIT_FOLDER);
    create_folder(LOG_FOLDER);
    create_folder(HIDEDATA_FOLDER);  //�����ļ���
    
    new_sample_line_count=0;//��ʼ������
    new_overlimit_line_count=0;
    new_hide_line_count=0;
    
    Get_RTC_DateTimeString_without(write_sample_name_time);
    snprintf(write_sample_file_path, sizeof(write_sample_file_path), "%s%s%s", SAMPLE_FOLDER_PATH, write_sample_name_time, FORMAT_TXT);//�ļ�����·��
      
    Get_RTC_DateTimeString_without(write_overlimit_name_time);
    snprintf(write_overlimit_file_path, sizeof(write_overlimit_file_path), "%s%s%s", OVERLIMIT_FOLDER_PATH, write_overlimit_name_time, FORMAT_TXT);//�ļ�����·��
    
    Get_RTC_DateTimeString_without(write_hide_name_time);
    snprintf(write_hide_file_path, sizeof(write_hide_file_path), "%s%s%s", HIDEDATA_FOLDER_PATH, write_hide_name_time, FORMAT_TXT);//�ļ�����·��
    
    snprintf(write_log_file_path, sizeof(write_log_file_path), "%s%d%s", HIDEDATA_FOLDER_PATH, power_time, FORMAT_TXT);//�ļ�����·��
	}
  else
  {
//    printf("���س��� res=%d\r\n",stat);
//    return;
  }
  
  if(power_time)   //�ǳ����ϵ��ȡflash�е�����
  {
    set_cycle_time((uint8_t)read_flash(FLASH_CYCLE));   //��ȡflash�е�cycleʱ��
    set_ratio(read_flash(FLASH_RATIO));     //��ȡflash�еı��ֵ
    set_limit(read_flash(FLASH_LIMIT));     //��ȡflash�е���ֵ
    set_ratio(5);
    set_limit(10);
  }
  
  printf("\r\n====system init====\r\n");
//  printf("team_id %ld\r\n",read_flash_long(FLASH_TEAM));
  printf("Device_ID:2025-CIMC-");
  printf("team_id %ld\r\n",read_flash_long(FLASH_TEAM));
  printf("====system ready====\r\n");
  OLED_Clear();
  OLED_ShowString(0, 0, "system idle", 16);
  OLED_Refresh();
  
	while(1)
	{ 
    Proc5msTask();  //5ms��������
    Proc1SecTask(); //1s��������
    //sampling_to_file(write_file_path);//��ʼ������д���ļ�
    //delay_1ms(1000);//��ʱ
	}
}

FRESULT create_folder(const char* path) {//�����ļ���
    FRESULT res = f_mkdir(path);
    if (res == FR_OK) {
//        printf("Folder created: %s\r\n", path);
    } else if (res == FR_EXIST) {
//        printf("Folder already exists: %s\r\n", path);
    } else {
//        printf("Failed to create folder (Error %d): %s\r\n", res, path);
    }
    return res;
}

int Get_RTC_DateTimeString(char *datetime) {//��ȡʱ���
  
    rtc_parameter_struct rtc;
    rtc_current_time_get(&rtc);

   
    uint8_t hour   = bcd_to_byte(rtc.hour) % 24;     // 0-23
    uint8_t minute = bcd_to_byte(rtc.minute) % 60;   // 0-59
    uint8_t second = bcd_to_byte(rtc.second) % 60;   // 0-59
    uint8_t month  = bcd_to_byte(rtc.month);
    month = (month == 0) ? 1 : (month > 12 ? 12 : month); // 1-12
    uint8_t day    = bcd_to_byte(rtc.date);
    day = (day == 0) ? 1 : (day > 31 ? 31 : day);    // 1-31
    uint8_t year   = bcd_to_byte(rtc.year);          // 0-99

    int len = sprintf(datetime, "%04d-%02d-%02d %02d:%02d:%02d",
                        year + 2000, month, day, hour, minute, second);
    
    return (len == 14) ? 0 : -1; 
}


int Get_RTC_DateTimeString_without(char *datetime) {//��ȡ�޷���ʱ���
  
    rtc_parameter_struct rtc;
    rtc_current_time_get(&rtc);

   
    uint8_t hour   = bcd_to_byte(rtc.hour) % 24;     // 0-23
    uint8_t minute = bcd_to_byte(rtc.minute) % 60;   // 0-59
    uint8_t second = bcd_to_byte(rtc.second) % 60;   // 0-59
    uint8_t month  = bcd_to_byte(rtc.month);
    month = (month == 0) ? 1 : (month > 12 ? 12 : month); // 1-12
    uint8_t day    = bcd_to_byte(rtc.date);
    day = (day == 0) ? 1 : (day > 31 ? 31 : day);    // 1-31
    uint8_t year   = bcd_to_byte(rtc.year);          // 0-99

    int len = sprintf(datetime, "%04d%02d%02d%02d%02d%02d",
                     year + 2000, month, day, hour, minute, second);
    
    return (len == 14) ? 0 : -1; 
}


void sample_to_file(char* folder_path, char* file_path, char* name_time, char* time, char* sample)
{
    FIL file;//�ļ�
    FRESULT res;//�ļ���Ϣ
    UINT bytes_written;//д�����ʽ
    char line[100] = {0};//׼���ϳ���д�������

    if (new_sample_line_count < 10)
    {
        // ���ļ���д��ģʽ + ����ļ��������򴴽�
        res = f_open(&file, file_path, FA_WRITE | FA_OPEN_ALWAYS);
        if (res != FR_OK)
        {
//            printf("[ERROR] File open failed: %d\r\n", res);
            return;
        }
//        printf("%s �ļ��򿪳ɹ�\r\n",file_path);
        // �ؼ����裺�ļ��򿪺�ǿ��ָ�������ļ�β��
        res = f_lseek(&file, f_size(&file));//������ÿ�������������
        if (res != FR_OK)
        {
//            printf("[ERROR] File seek failed: %d\r\n", res);
            f_close(&file);
            return;
        }

        
     // ��ȡʱ���ַ�����д��һ��
        snprintf(line, sizeof(line), "%s %s\r\n", time, sample);//�ϳ���д�������
        res = f_write(&file, line, strlen(line), &bytes_written);
        if (res == FR_OK && bytes_written == strlen(line))
        {
//            printf("[Write OK] %s", line);
            new_sample_line_count++;
        }
        else
        {
//            printf("[ERROR] Write failed: %d\r\n", res);
        }

        // д���ǿ��ˢ�£���֤����д��
        f_sync(&file);
        // ���ر��ļ�
        f_close(&file);
    }
    else
    {
        // ������10�У������������ļ�
        new_sample_line_count = 0;

        snprintf(write_sample_file_path, sizeof(write_sample_file_path), "%s%s%s", folder_path, name_time, FORMAT_TXT);//�ļ�����·��
//        printf("[INFO] Creating new file: %s\r\n", write_sample_file_path);

        // ���ļ����ݹ�һ�Σ���ʼд��һ��
        sample_to_file(folder_path,write_sample_file_path,name_time,time,sample);
    }
}

void overlimit_to_file(char* folder_path, char* file_path, char* name_time, char* time, char* sample)
{
    FIL file;//�ļ�
    FRESULT res;//�ļ���Ϣ
    UINT bytes_written;//д�����ʽ
    char line[100] = {0};//׼���ϳ���д�������

    if (new_overlimit_line_count < 10)
    {
        // ���ļ���д��ģʽ + ����ļ��������򴴽�
        res = f_open(&file, file_path, FA_WRITE | FA_OPEN_ALWAYS);
        if (res != FR_OK)
        {
//            printf("[ERROR] File open failed: %d\r\n", res);
            return;
        }
//        printf("%s �ļ��򿪳ɹ�\r\n",file_path);
        // �ؼ����裺�ļ��򿪺�ǿ��ָ�������ļ�β��
        res = f_lseek(&file, f_size(&file));//������ÿ�������������
        if (res != FR_OK)
        {
//            printf("[ERROR] File seek failed: %d\r\n", res);
            f_close(&file);
            return;
        }

        // ��ȡʱ���ַ�����д��һ��
        snprintf(line, sizeof(line), "%s %s\r\n", time, sample);//�ϳ���д�������
        res = f_write(&file, line, strlen(line), &bytes_written);
        if (res == FR_OK && bytes_written == strlen(line))
        {
//            printf("[Write OK] %s", line);
            new_sample_line_count++;
        }
        else
        {
//            printf("[ERROR] Write failed: %d\r\n", res);
        }

        // д���ǿ��ˢ�£���֤����д��
        f_sync(&file);
        // ���ر��ļ�
        f_close(&file);
    }
    else
    {
        // ������10�У������������ļ�
        new_overlimit_line_count = 0;

        snprintf(write_overlimit_file_path, sizeof(write_overlimit_file_path), "%s%s%s", folder_path, name_time, FORMAT_TXT);//�ļ�����·��
//        printf("[INFO] Creating new file: %s\r\n", write_overlimit_file_path);

        // ���ļ����ݹ�һ�Σ���ʼд��һ��
        overlimit_to_file(folder_path,write_overlimit_file_path,name_time,time,sample);
    }
}

void hide_to_file(char* folder_path, char* file_path, char* name_time, char* time, char* sample)
{
    FIL file;//�ļ�
    FRESULT res;//�ļ���Ϣ
    UINT bytes_written;//д�����ʽ
    char line[100] = {0};//׼���ϳ���д�������

    if (new_hide_line_count < 10)
    {
        // ���ļ���д��ģʽ + ����ļ��������򴴽�
        res = f_open(&file, file_path, FA_WRITE | FA_OPEN_ALWAYS);
        if (res != FR_OK)
        {
//            printf("[ERROR] File open failed: %d\r\n", res);
            return;
        }
//        printf("%s �ļ��򿪳ɹ�\r\n",file_path);
        // �ؼ����裺�ļ��򿪺�ǿ��ָ�������ļ�β��
        res = f_lseek(&file, f_size(&file));//������ÿ�������������
        if (res != FR_OK)
        {
//            printf("[ERROR] File seek failed: %d\r\n", res);
            f_close(&file);
            return;
        }

        // ��ȡʱ���ַ�����д��һ��
        snprintf(line, sizeof(line), "%s %s\r\n", time, sample);//�ϳ���д�������
        res = f_write(&file, line, strlen(line), &bytes_written);
        if (res == FR_OK && bytes_written == strlen(line))
        {
//            printf("[Write OK] %s", line);
            new_sample_line_count++;
        }
        else
        {
//            printf("[ERROR] Write failed: %d\r\n", res);
        }

        // д���ǿ��ˢ�£���֤����д��
        f_sync(&file);
        // ���ر��ļ�
        f_close(&file);
    }
    else
    {
        // ������10�У������������ļ�
        new_hide_line_count = 0;

        snprintf(write_hide_file_path, sizeof(write_hide_file_path), "%s%s%s", folder_path, name_time, FORMAT_TXT);//�ļ�����·��
//        printf("[INFO] Creating new file: %s\r\n", write_hide_file_path);

        // ���ļ����ݹ�һ�Σ���ʼд��һ��
        hide_to_file(folder_path,write_hide_file_path,name_time,time,sample);
    }
}

void log_to_file(char* file_path, char* sample)
{
    FIL file;//�ļ�
    FRESULT res;//�ļ���Ϣ
    UINT bytes_written;//д�����ʽ
    char line[100] = {0};//׼���ϳ���д�������

    // ���ļ���д��ģʽ + ����ļ��������򴴽�
    res = f_open(&file, file_path, FA_WRITE | FA_OPEN_ALWAYS);
    if (res != FR_OK)
    {
        //printf("[ERROR] File open failed: %d\r\n", res);
        return;
    }
//    printf("%s �ļ��򿪳ɹ�\r\n",file_path);
    // �ؼ����裺�ļ��򿪺�ǿ��ָ�������ļ�β��
    res = f_lseek(&file, f_size(&file));//������ÿ�������������
    if (res != FR_OK)
    {
        //printf("[ERROR] File seek failed: %d\r\n", res);
        f_close(&file);
        return;
    }

    // ��ȡʱ���ַ�����д��һ��
    snprintf(line, sizeof(line), "%s\r\n", sample);//�ϳ���д�������
    res = f_write(&file, line, strlen(line), &bytes_written);
    if (res == FR_OK && bytes_written == strlen(line))
    {
        //printf("[Write OK] %s", line);
        new_sample_line_count++;
    }
    else
    {
//        printf("[ERROR] Write failed: %d\r\n", res);
    }

    // д���ǿ��ˢ�£���֤����д��
    f_sync(&file);
    // ���ر��ļ�
    f_close(&file);
}



void parse_config_line(const char *line, ConfigParams *params,int i)
{
    char key[20], value[20];
    if (sscanf(line, "%[^=]=%s", key, value) == 2) {
        if (i == 2) {
            params->ratio = atof(value);
        } else if (i == 5) {
            params->limit = atof(value);
        }
    }
}

FRESULT read_config_file(ConfigParams *params)
{
  FIL file;
  FRESULT res;
  char line[64];
  int i;
//  UINT br;

  // ���ļ�
  res = f_open(&file, CONFIG_FILE_PATH, FA_READ);
  if (res != FR_OK)
  {     
//    printf("config.ini file not found\r\n");
    return res;  // �ļ������ڻ��ʧ��
  }

    // ���ж�ȡ������
  while (f_gets(line, sizeof(line), &file))
  {
    i++;
    parse_config_line(line, params,i);
  }

    // �ر��ļ�
  f_close(&file);
  return FR_OK;
}


void usart_read_config(void)
{
	FRESULT res;
	ConfigParams params;
	
	res=read_config_file(&params);
	if(res==FR_OK)
	{
		printf("Ratio=%.2f\r\nLimit=%.2f\r\nConfig read success\r\n",params.ratio,params.limit);
    save_flash(params.ratio, FLASH_RATIO); //���浽flash
    save_flash(params.limit, FLASH_LIMIT);
	}
}

/************************************************************ 
 * Function :       Proc5msTask
 * Comment  :       ����Timer1ʵ��5ms��������
 * Parameter:       null
 * Return   :       null
 * Author   :       
 * Date     :       
************************************************************/
static  void  Proc5msTask(void)
{  
  static unsigned char OLED_Refresh_Cnt = 0;
  
  if(get_5ms_Flag())  //�ж�1ms��־״̬
  {
    LED_Flicker(500);   //LED��һ��Ϊ������˸
    clear_5ms_flag();   //���1ms��־
    OLED_Refresh_Cnt ++;
    //printf("1msCnt = %d\r\n", OLED_Refresh_Cnt);
    if(OLED_Refresh_Cnt >= 24)
    {
      OLED_Refresh_Cnt = 0;
      OLED_Refresh();
    }
    scan_key();     //��ⰴ��
    LED_Flicker(100);   //LED��һ��Ϊ������˸
    //cycle(cycle_time);
  }
}

/************************************************************ 
 * Function :       Proc1sTask
 * Comment  :       ����Timer4ʵ��1s��������
 * Parameter:       null
 * Return   :       null
 * Author   :       
 * Date     :       
************************************************************/
static  void  Proc1SecTask(void)
{  
  if(get_1s_Flag()) //�ж�1s��־״̬
  {
    clear_1s_flag();  //���1s��־
    DataProcess(get_cycle_time(), get_is_hide(), get_sampling_flag());   //��ʾ����
    if(TF_ok_flag)
    {
      if(log_sample_flag)
      {
        Get_RTC_DateTimeString_without(write_sample_name_time);//�õ��޷��ŵ�ʱ���
        Get_RTC_DateTimeString(write_sample_time);//�õ��޷��ŵ�ʱ���
        sample_need(write_sample_sample);           //��ȡ��Ҫ���ַ���
//        printf("write_sample_sample %s\r\n",write_sample_sample);
//        snprintf(write_sample_sample, sizeof(write_sample_sample), "���Բ���%.6s", write_sample_name_time);//��Ҫ��Ϊ��ȡ����������
        sample_to_file(write_sample_folder_path,write_sample_file_path,write_sample_name_time,write_sample_time,write_sample_sample);//��ʼ������д���ļ�
        log_sample_flag=0;
      }
      
      if(log_overlimit_flag)
      {
        Get_RTC_DateTimeString_without(write_overlimit_name_time);//�õ��޷��ŵ�ʱ���
        Get_RTC_DateTimeString(write_overlimit_time);//�õ��޷��ŵ�ʱ���
        overlimit_need(write_overlimit_sample);
        overlimit_to_file(write_overlimit_folder_path,write_overlimit_file_path,write_overlimit_name_time,write_overlimit_time,write_overlimit_sample);//��ʼ������д���ļ�
        log_overlimit_flag=0;
      }
      
      if(log_hide_flag)
      {
        Get_RTC_DateTimeString_without(write_hide_name_time);//�õ�ʱ�������Ҫ��ΪRTC��ȡ
        Get_RTC_DateTimeString(write_hide_time);//�õ�ʱ�������Ҫ��ΪRTC��ȡ
        hide_need(write_hide_sample);
        hide_to_file(write_hide_folder_path,write_hide_file_path,write_hide_name_time,write_hide_time,write_hide_sample);//��ʼ������д���ļ�
        log_hide_flag=0;
      }
      
      if(log_log_flag)
      {
        log_need(write_log_sample); 
        log_to_file(write_log_file_path,write_log_sample);//��ʼ������д���ļ�
        log_log_flag=0;
      }
    }
  }    
}

//�����ļ�����log��־λ
void set_log_log_flag(void)
{
  log_log_flag = 1;
}
/****************************End*****************************/
