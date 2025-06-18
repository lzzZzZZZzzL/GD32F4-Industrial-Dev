/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：SPI_Flash.h
 * 作者: Jianchuan Wang & Tao Liu @ GigaDevice
 * 平台: 2025CIMC IHD-V04
 * 版本: Jianchuan Wang     2025/4/20     V0.01    original
************************************************************/

#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

/************************* 头文件 *************************/

#include "HeaderFiles.h"

#define  SPI_FLASH_PAGE_SIZE           256
#define  SPI_FLASH_SECTOR_SIZE         4096
#define  SPI_FLASH_CS_LOW()            gpio_bit_reset(GPIOB, GPIO_PIN_12)
#define  SPI_FLASH_CS_HIGH()           gpio_bit_set(GPIOB, GPIO_PIN_12)

#define  SFLASH_ID                     0xC84013
#define  FLASH_WRITE_ADDRESS           0x000000
#define  FLASH_READ_ADDRESS            FLASH_WRITE_ADDRESS

/* initialize SPI1 GPIO and parameter */
void spi_flash_init(void);
/* erase the specified flash sector */
void spi_flash_sector_erase(uint32_t sector_addr);
/* erase the entire flash */
void spi_flash_bulk_erase(void);
/* write more than one byte to the flash */
void spi_flash_page_write(uint8_t* pbuffer,uint32_t write_addr,uint16_t num_byte_to_write);
/* write block of data to the flash */
void spi_flash_buffer_write(uint8_t* pbuffer,uint32_t write_addr,uint32_t num_byte_to_write);
/* read a block of data from the flash */
void spi_flash_buffer_read(uint8_t* pbuffer,uint32_t read_addr,uint16_t num_byte_to_read);
/* read flash identification */
uint32_t spi_flash_read_id(void);
/* initiate a read data byte (read) sequence from the flash */
void spi_flash_start_read_sequence(uint32_t read_addr);
/* read a byte from the SPI flash */
uint8_t spi_flash_read_byte(void);
/* send a byte through the SPI interface and return the byte received from the SPI bus */
uint8_t spi_flash_send_byte(uint8_t byte);
/* send a half word through the SPI interface and return the half word received from the SPI bus */
uint16_t spi_flash_send_halfword(uint16_t half_word);
/* enable the write access to the flash */
void spi_flash_write_enable(void);
/* poll the status of the write in progress (wip) flag in the flash's status register */
void spi_flash_wait_for_write_end(void);


void spi_flash_buffer_erase(uint32_t sector_addr,  uint32_t num_byte_to_erase);



#define PARAM_FLASH_START_ADDR    0x000000      // Flash首地址
#define PARAM_FLASH_SECTOR_SIZE   SPI_FLASH_SECTOR_SIZE  // 每次最小擦除单位
#define FLASH_CYCLE         PARAM_FLASH_START_ADDR + 0 * PARAM_FLASH_SECTOR_SIZE //设置每个数据处于不同的地址，保证不会误擦除
#define FLASH_RATIO         PARAM_FLASH_START_ADDR + 1 * PARAM_FLASH_SECTOR_SIZE
#define FLASH_LIMIT         PARAM_FLASH_START_ADDR + 2 * PARAM_FLASH_SECTOR_SIZE
#define FLASH_POWER         PARAM_FLASH_START_ADDR + 3 * PARAM_FLASH_SECTOR_SIZE
#define FLASH_TEAM          PARAM_FLASH_START_ADDR + 4 * PARAM_FLASH_SECTOR_SIZE

//void flash_param_write(uint32_t addr, const void* data, uint16_t size);
//void flash_param_read(uint32_t addr, void* data, uint16_t size);
//void flash_param_erase_sector(uint32_t addr);


void save_flash(float value, uint32_t addr);//保存数据
void save_flash_long(long value, uint32_t addr);
float read_flash(uint32_t addr);//读取数据
long read_flash_long(uint32_t addr);

#endif
