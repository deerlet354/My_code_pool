#ifndef _INTERNAL_FLASH_H_
#define _INTERNAL_FLASH_H_

#include "main.h"
#include <string.h>

/* STM32G031G8U6 Flash 参数 (64KB容量) */
#define FLASH_BASE_ADDR        0x08000000U
//#define FLASH_PAGE_SIZE        0x0800U        // G0系列固定为 2KB/页
#define FLASH_TOTAL_SIZE       0x00010000U    // 64KB
#define FLASH_TOTAL_PAGES      32U            // 64KB / 2KB
#define FLASH_SAVE_ADDR        0x0800F800U    // 放在最后1页(页31)，避开前17KB代码区
#define FLASH_DATA_SIZE        24U           // 按需修改，需是8的倍数最佳

/* 页计算宏 */
#define GET_PAGE_NUMBER(addr)  (((addr) - FLASH_BASE_ADDR) / FLASH_PAGE_SIZE)

/* API 声明 */
HAL_StatusTypeDef Flash_Write(uint32_t SAVE_Address, uint8_t *data, uint16_t num);
void              Flash_Read (uint32_t Address, uint8_t *data, uint16_t num);

/* 状态枚举 (保留原设计) */
typedef enum {
    FLASH_OK      = 0x00,
    FLASH_ERROR   = 0x01,
    FLASH_BUSY    = 0x02,
    FLASH_TIMEOUT = 0x03
} Flash_Status;


void Save_cali_data(void);
void Get_cali_data(void);
	
#endif /* _INTERNAL_FLASH_H_ */
