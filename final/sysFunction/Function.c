/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：Function.c
 * 作者: Lingyu Meng
 * 平台: 2025CIMC IHD-V04
 * 版本: Lingyu Meng     2025/2/16     V0.01    original
************************************************************/


/************************* 头文件 *************************/

#include "Function.h"

#include "ff.h"
#include "diskio.h"
#include "sdcard.h"

/************************* 宏定义 *************************/
#define  SFLASH_ID                     0xC84013
#define BUFFER_SIZE                    256
#define TX_BUFFER_SIZE                 BUFFER_SIZE
#define RX_BUFFER_SIZE                 BUFFER_SIZE
#define  FLASH_WRITE_ADDRESS           0x000000
#define  FLASH_READ_ADDRESS            FLASH_WRITE_ADDRESS

typedef struct {
    float ratio;         // 变比参数
    float limit;         // 阈值参数
} ConfigParams;

/************************ 变量定义 ************************/
uint32_t flash_id = 0;
uint8_t  tx_buffer[TX_BUFFER_SIZE];
uint16_t i = 0, count, result = 0;
uint8_t  is_successful = 0;

FIL fdst;
FATFS fs;
UINT br, bw;
BYTE buffer[128];
BYTE filebuffer[128];

static uint8_t TF_ok_flag=0;//TF卡

static uint8_t new_sample_line_count;
static uint8_t new_overlimit_line_count;
static uint8_t new_hide_line_count;

static char write_sample_file_path[64] = {0};
char write_sample_folder_path[] = SAMPLE_FOLDER_PATH;
char write_sample_time[30] = " ";//初始为空
char write_sample_sample[50] = " ";
char write_sample_name_time[30] = {0};

static char write_overlimit_file_path[64] = {0};
char write_overlimit_folder_path[] = OVERLIMIT_FOLDER_PATH;
char write_overlimit_time[30] = " ";//初始为空
char write_overlimit_sample[50] = " ";
char write_overlimit_name_time[30] = {0};

static char write_hide_file_path[64] = {0};
char write_hide_folder_path[] = HIDEDATA_FOLDER_PATH;
char write_hide_time[30] = " ";//初始为空
char write_hide_sample[80] = " ";
char write_hide_name_time[30] = {0};

char write_log_file_path[64] = {0};
char write_log_sample[80] = " ";

extern uint8_t log_sample_flag;
extern uint8_t log_overlimit_flag;
extern uint8_t log_hide_flag;
extern uint8_t log_log_flag;
/************************ 函数定义 ************************/
void Proc5msTask(void);  //5ms处理任务
void Proc1SecTask(void); //1s处理任务

ErrStatus memory_compare(uint8_t* src,uint8_t* dst,uint16_t length);

FRESULT create_folder(const char* path);

void Start_Sampling_To_Files(void);

void sample_to_file(char* folder_path, char* file_path, char* name_time, char* time, char* sample);//写入sample文件
void overlimit_to_file(char* folder_path, char* file_path, char* name_time, char* time, char* sample);//写入overlimit文件
void hide_to_file(char* folder_path, char* file_path, char* name_time, char* time, char* sample);//写入hide文件
void log_to_file(char* file_path, char* sample);//写入log文件

void parse_config_line(const char *line, ConfigParams *params,int i);
FRESULT read_config_file(ConfigParams *params);
int Get_RTC_DateTimeString_without(char *datetime);
int Get_RTC_DateTimeString(char *datetime);
/************************************************************ 
 * Function :       System_Init
 * Comment  :       用于初始化MCU
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void System_Init(void)
{
	systick_config();     // 时钟配置
  
	LED_Init();
  KEY_Init();
	EXTI_PIN_Init();
  
	USART0_Config();      // 串口初始化
  nvic_irq_enable(USART0_IRQn, 0, 0);//使能USART0中断
  usart_interrupt_enable(USART0, USART_INT_RBNE);//接收中断打开
  
  nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);	// 设置中断优先级分组
  nvic_irq_enable(SDIO_IRQn, 0, 0);					// 使能SDIO中断，优先级为0（复制粘贴例程，注释掉能创建文件但不能写入数据）
  
  ADC_port_init();
  InitTimer();
  OLED_Init();
  spi_flash_init();
  RTC_Init(); 
}


/************************************************************ 
 * Function :       UsrFunction
 * Comment  :       用户程序功能
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
		stat = disk_initialize(0); 			//初始化SD卡（设备号0）,物理驱动器编号,每个物理驱动器（如硬盘、U 盘等）通常都被分配一个唯一的编号。
	}while((stat != 0) && (--k));			//如果初始化失败，重试最多k次。
    
//    printf("SD Card disk_initialize:%d\r\n",stat);
    f_mount(0, &fs);						 //挂载SD卡的文件系统（设备号0）。
//    printf("SD Card f_mount:%d\r\n",stat);

	if(RES_OK == stat)						 //返回挂载结果（FR_OK 表示成功）。
	{        
    TF_ok_flag=1;
//    printf("\r\nSD Card Initialize Success!\r\n");
    create_folder(SAMPLE_FOLDER);
    create_folder(OVERLIMIT_FOLDER);
    create_folder(LOG_FOLDER);
    create_folder(HIDEDATA_FOLDER);  //创建文件夹
    
    new_sample_line_count=0;//初始化行数
    new_overlimit_line_count=0;
    new_hide_line_count=0;
    
    Get_RTC_DateTimeString_without(write_sample_name_time);
    snprintf(write_sample_file_path, sizeof(write_sample_file_path), "%s%s%s", SAMPLE_FOLDER_PATH, write_sample_name_time, FORMAT_TXT);//文件名和路径
      
    Get_RTC_DateTimeString_without(write_overlimit_name_time);
    snprintf(write_overlimit_file_path, sizeof(write_overlimit_file_path), "%s%s%s", OVERLIMIT_FOLDER_PATH, write_overlimit_name_time, FORMAT_TXT);//文件名和路径
    
    Get_RTC_DateTimeString_without(write_hide_name_time);
    snprintf(write_hide_file_path, sizeof(write_hide_file_path), "%s%s%s", HIDEDATA_FOLDER_PATH, write_hide_name_time, FORMAT_TXT);//文件名和路径
    
    snprintf(write_log_file_path, sizeof(write_log_file_path), "%s%d%s", HIDEDATA_FOLDER_PATH, power_time, FORMAT_TXT);//文件名和路径
	}
  else
  {
//    printf("挂载出错 res=%d\r\n",stat);
//    return;
  }
  
  if(power_time)   //非初次上电读取flash中的设置
  {
    set_cycle_time((uint8_t)read_flash(FLASH_CYCLE));   //读取flash中的cycle时间
    set_ratio(read_flash(FLASH_RATIO));     //读取flash中的变比值
    set_limit(read_flash(FLASH_LIMIT));     //读取flash中的阈值
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
    Proc5msTask();  //5ms处理任务
    Proc1SecTask(); //1s处理任务
    //sampling_to_file(write_file_path);//开始将数据写入文件
    //delay_1ms(1000);//延时
	}
}

FRESULT create_folder(const char* path) {//创建文件夹
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

int Get_RTC_DateTimeString(char *datetime) {//获取时间戳
  
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


int Get_RTC_DateTimeString_without(char *datetime) {//获取无符号时间戳
  
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
    FIL file;//文件
    FRESULT res;//文件信息
    UINT bytes_written;//写入的形式
    char line[100] = {0};//准备合成需写入的数据

    if (new_sample_line_count < 10)
    {
        // 打开文件，写入模式 + 如果文件不存在则创建
        res = f_open(&file, file_path, FA_WRITE | FA_OPEN_ALWAYS);
        if (res != FR_OK)
        {
//            printf("[ERROR] File open failed: %d\r\n", res);
            return;
        }
//        printf("%s 文件打开成功\r\n",file_path);
        // 关键步骤：文件打开后，强制指针跳到文件尾部
        res = f_lseek(&file, f_size(&file));//现在是每次在最下面加入
        if (res != FR_OK)
        {
//            printf("[ERROR] File seek failed: %d\r\n", res);
            f_close(&file);
            return;
        }

        
     // 获取时间字符串，写入一行
        snprintf(line, sizeof(line), "%s %s\r\n", time, sample);//合成需写入的数据
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

        // 写完后强制刷新，保证物理写入
        f_sync(&file);
        // 最后关闭文件
        f_close(&file);
    }
    else
    {
        // 行数满10行，重新生成新文件
        new_sample_line_count = 0;

        snprintf(write_sample_file_path, sizeof(write_sample_file_path), "%s%s%s", folder_path, name_time, FORMAT_TXT);//文件名和路径
//        printf("[INFO] Creating new file: %s\r\n", write_sample_file_path);

        // 新文件，递归一次，开始写第一行
        sample_to_file(folder_path,write_sample_file_path,name_time,time,sample);
    }
}

void overlimit_to_file(char* folder_path, char* file_path, char* name_time, char* time, char* sample)
{
    FIL file;//文件
    FRESULT res;//文件信息
    UINT bytes_written;//写入的形式
    char line[100] = {0};//准备合成需写入的数据

    if (new_overlimit_line_count < 10)
    {
        // 打开文件，写入模式 + 如果文件不存在则创建
        res = f_open(&file, file_path, FA_WRITE | FA_OPEN_ALWAYS);
        if (res != FR_OK)
        {
//            printf("[ERROR] File open failed: %d\r\n", res);
            return;
        }
//        printf("%s 文件打开成功\r\n",file_path);
        // 关键步骤：文件打开后，强制指针跳到文件尾部
        res = f_lseek(&file, f_size(&file));//现在是每次在最下面加入
        if (res != FR_OK)
        {
//            printf("[ERROR] File seek failed: %d\r\n", res);
            f_close(&file);
            return;
        }

        // 获取时间字符串，写入一行
        snprintf(line, sizeof(line), "%s %s\r\n", time, sample);//合成需写入的数据
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

        // 写完后强制刷新，保证物理写入
        f_sync(&file);
        // 最后关闭文件
        f_close(&file);
    }
    else
    {
        // 行数满10行，重新生成新文件
        new_overlimit_line_count = 0;

        snprintf(write_overlimit_file_path, sizeof(write_overlimit_file_path), "%s%s%s", folder_path, name_time, FORMAT_TXT);//文件名和路径
//        printf("[INFO] Creating new file: %s\r\n", write_overlimit_file_path);

        // 新文件，递归一次，开始写第一行
        overlimit_to_file(folder_path,write_overlimit_file_path,name_time,time,sample);
    }
}

void hide_to_file(char* folder_path, char* file_path, char* name_time, char* time, char* sample)
{
    FIL file;//文件
    FRESULT res;//文件信息
    UINT bytes_written;//写入的形式
    char line[100] = {0};//准备合成需写入的数据

    if (new_hide_line_count < 10)
    {
        // 打开文件，写入模式 + 如果文件不存在则创建
        res = f_open(&file, file_path, FA_WRITE | FA_OPEN_ALWAYS);
        if (res != FR_OK)
        {
//            printf("[ERROR] File open failed: %d\r\n", res);
            return;
        }
//        printf("%s 文件打开成功\r\n",file_path);
        // 关键步骤：文件打开后，强制指针跳到文件尾部
        res = f_lseek(&file, f_size(&file));//现在是每次在最下面加入
        if (res != FR_OK)
        {
//            printf("[ERROR] File seek failed: %d\r\n", res);
            f_close(&file);
            return;
        }

        // 获取时间字符串，写入一行
        snprintf(line, sizeof(line), "%s %s\r\n", time, sample);//合成需写入的数据
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

        // 写完后强制刷新，保证物理写入
        f_sync(&file);
        // 最后关闭文件
        f_close(&file);
    }
    else
    {
        // 行数满10行，重新生成新文件
        new_hide_line_count = 0;

        snprintf(write_hide_file_path, sizeof(write_hide_file_path), "%s%s%s", folder_path, name_time, FORMAT_TXT);//文件名和路径
//        printf("[INFO] Creating new file: %s\r\n", write_hide_file_path);

        // 新文件，递归一次，开始写第一行
        hide_to_file(folder_path,write_hide_file_path,name_time,time,sample);
    }
}

void log_to_file(char* file_path, char* sample)
{
    FIL file;//文件
    FRESULT res;//文件信息
    UINT bytes_written;//写入的形式
    char line[100] = {0};//准备合成需写入的数据

    // 打开文件，写入模式 + 如果文件不存在则创建
    res = f_open(&file, file_path, FA_WRITE | FA_OPEN_ALWAYS);
    if (res != FR_OK)
    {
        //printf("[ERROR] File open failed: %d\r\n", res);
        return;
    }
//    printf("%s 文件打开成功\r\n",file_path);
    // 关键步骤：文件打开后，强制指针跳到文件尾部
    res = f_lseek(&file, f_size(&file));//现在是每次在最下面加入
    if (res != FR_OK)
    {
        //printf("[ERROR] File seek failed: %d\r\n", res);
        f_close(&file);
        return;
    }

    // 获取时间字符串，写入一行
    snprintf(line, sizeof(line), "%s\r\n", sample);//合成需写入的数据
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

    // 写完后强制刷新，保证物理写入
    f_sync(&file);
    // 最后关闭文件
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

  // 打开文件
  res = f_open(&file, CONFIG_FILE_PATH, FA_READ);
  if (res != FR_OK)
  {     
//    printf("config.ini file not found\r\n");
    return res;  // 文件不存在或打开失败
  }

    // 逐行读取并解析
  while (f_gets(line, sizeof(line), &file))
  {
    i++;
    parse_config_line(line, params,i);
  }

    // 关闭文件
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
    save_flash(params.ratio, FLASH_RATIO); //保存到flash
    save_flash(params.limit, FLASH_LIMIT);
	}
}

/************************************************************ 
 * Function :       Proc5msTask
 * Comment  :       利用Timer1实现5ms处理任务
 * Parameter:       null
 * Return   :       null
 * Author   :       
 * Date     :       
************************************************************/
static  void  Proc5msTask(void)
{  
  static unsigned char OLED_Refresh_Cnt = 0;
  
  if(get_5ms_Flag())  //判断1ms标志状态
  {
    LED_Flicker(500);   //LED以一秒为周期闪烁
    clear_5ms_flag();   //清除1ms标志
    OLED_Refresh_Cnt ++;
    //printf("1msCnt = %d\r\n", OLED_Refresh_Cnt);
    if(OLED_Refresh_Cnt >= 24)
    {
      OLED_Refresh_Cnt = 0;
      OLED_Refresh();
    }
    scan_key();     //检测按键
    LED_Flicker(100);   //LED以一秒为周期闪烁
    //cycle(cycle_time);
  }
}

/************************************************************ 
 * Function :       Proc1sTask
 * Comment  :       利用Timer4实现1s处理任务
 * Parameter:       null
 * Return   :       null
 * Author   :       
 * Date     :       
************************************************************/
static  void  Proc1SecTask(void)
{  
  if(get_1s_Flag()) //判断1s标志状态
  {
    clear_1s_flag();  //清除1s标志
    DataProcess(get_cycle_time(), get_is_hide(), get_sampling_flag());   //显示数据
    if(TF_ok_flag)
    {
      if(log_sample_flag)
      {
        Get_RTC_DateTimeString_without(write_sample_name_time);//得到无符号的时间戳
        Get_RTC_DateTimeString(write_sample_time);//得到无符号的时间戳
        sample_need(write_sample_sample);           //获取需要的字符串
//        printf("write_sample_sample %s\r\n",write_sample_sample);
//        snprintf(write_sample_sample, sizeof(write_sample_sample), "测试采样%.6s", write_sample_name_time);//需要改为获取到采样数据
        sample_to_file(write_sample_folder_path,write_sample_file_path,write_sample_name_time,write_sample_time,write_sample_sample);//开始将数据写入文件
        log_sample_flag=0;
      }
      
      if(log_overlimit_flag)
      {
        Get_RTC_DateTimeString_without(write_overlimit_name_time);//得到无符号的时间戳
        Get_RTC_DateTimeString(write_overlimit_time);//得到无符号的时间戳
        overlimit_need(write_overlimit_sample);
        overlimit_to_file(write_overlimit_folder_path,write_overlimit_file_path,write_overlimit_name_time,write_overlimit_time,write_overlimit_sample);//开始将数据写入文件
        log_overlimit_flag=0;
      }
      
      if(log_hide_flag)
      {
        Get_RTC_DateTimeString_without(write_hide_name_time);//得到时间戳，需要改为RTC获取
        Get_RTC_DateTimeString(write_hide_time);//得到时间戳，需要改为RTC获取
        hide_need(write_hide_sample);
        hide_to_file(write_hide_folder_path,write_hide_file_path,write_hide_name_time,write_hide_time,write_hide_sample);//开始将数据写入文件
        log_hide_flag=0;
      }
      
      if(log_log_flag)
      {
        log_need(write_log_sample); 
        log_to_file(write_log_file_path,write_log_sample);//开始将数据写入文件
        log_log_flag=0;
      }
    }
  }    
}

//其他文件设置log标志位
void set_log_log_flag(void)
{
  log_log_flag = 1;
}
/****************************End*****************************/
