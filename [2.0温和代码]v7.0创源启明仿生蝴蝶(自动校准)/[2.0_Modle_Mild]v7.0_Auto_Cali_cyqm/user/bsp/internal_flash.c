#include "internal_flash.h"
#include "motor.h"

uint8_t FLASH_DATA[FLASH_DATA_SIZE];

extern uint8_t M_pos[4];
extern uint8_t E_pos[4];

HAL_StatusTypeDef Flash_Write(uint32_t SAVE_Address, uint8_t *data, uint16_t num)
{
    FLASH_EraseInitTypeDef EraseInitStruct = {0};
    uint32_t PageError = 0;
    uint32_t addr = SAVE_Address;
    uint64_t write_data;
    HAL_StatusTypeDef status = HAL_OK;

    /* 地址与参数校验 */
    if (SAVE_Address < FLASH_BASE_ADDR || 
        SAVE_Address >= (FLASH_BASE_ADDR + FLASH_TOTAL_SIZE) || 
        (SAVE_Address & 0x7U) != 0U)  // G0双字编程必须8字节对齐
    {
        return HAL_ERROR;
    }
    if (data == NULL || num == 0)
    {
        return HAL_ERROR;
    }

    /* 解锁 Flash */
    HAL_FLASH_Unlock();

    /* 配置擦除参数 (G0 HAL 使用 Page 页号 + Banks 字段，非 PageAddress) */
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page      = GET_PAGE_NUMBER(SAVE_Address);
    EraseInitStruct.NbPages   = 1;
    EraseInitStruct.Banks     = FLASH_BANK_1;  // G0单Bank架构必填

    /* 执行擦除 */
    status = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
    if (status != HAL_OK)
    {
        HAL_FLASH_Lock();
        return status;
    }

    /* 按 8字节(64位双字) 编程写入 */
    for (uint16_t i = 0; i < num; i += 8)
    {
        write_data = 0;
        /* 拼装64位数据，自动处理末尾不足8字节的情况 */
        for (uint8_t j = 0; j < 8 && (i + j) < num; j++)
        {
            write_data |= ((uint64_t)data[i + j] << (j * 8));
        }

        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, write_data);
        if (status != HAL_OK)
        {
            break;  // 编程失败立即终止
        }
        addr += 8;
    }

    /* 6. 锁定 Flash */
    HAL_FLASH_Lock();
    return status;
}

//Flash 读取函数 (XIP模式直接指针访问)
void Flash_Read(uint32_t Address, uint8_t *data, uint16_t num)
{
    if (Address < FLASH_BASE_ADDR || Address >= (FLASH_BASE_ADDR + FLASH_TOTAL_SIZE))
    {
        return;
    }
    memcpy(data, (const uint8_t *)Address, num);
}

void Save_cali_data()
{
    for(uint8_t i = 0; i < 4; i++) 
	{
        FLASH_DATA[i]     = E_pos[i];
        FLASH_DATA[i + 4] = M_pos[i];
    }

    for(uint8_t i = 0; i < 4; i++) 
	{
        FLASH_DATA[8  + i] = (uint8_t)Wings_Data.Wings_motor[i].encoder_dir;
        FLASH_DATA[12 + i] = (uint8_t)Wings_Data.Wings_motor[i].motor_dir;
    }
	
    uint16_t offset = 16;
    for(uint8_t i = 0; i < 4; i++) 
	{
        FLASH_DATA[offset]     = (uint8_t)((Wings_Data.Wings_motor[i].MOTOR_MIDPOINT >> 8) & 0xFF);
        FLASH_DATA[offset + 1] = (uint8_t)( Wings_Data.Wings_motor[i].MOTOR_MIDPOINT       & 0xFF);
        offset += 2;
    }

    Flash_Write(FLASH_SAVE_ADDR, FLASH_DATA, FLASH_DATA_SIZE);
}

void Get_cali_data()
{
    Flash_Read(FLASH_SAVE_ADDR, FLASH_DATA, FLASH_DATA_SIZE);

    for(uint8_t i = 0; i < 4; i++) 
	{
        E_pos[i]     = FLASH_DATA[i];
        M_pos[i]     = FLASH_DATA[i + 4];
    }

    for(uint8_t i = 0; i < 4; i++) 
	{
        Wings_Data.Wings_motor[i].encoder_dir = (int8_t)FLASH_DATA[8  + i];
        Wings_Data.Wings_motor[i].motor_dir   = (int8_t)FLASH_DATA[12 + i];
    }

    uint16_t offset = 16;
    for(uint8_t i = 0; i < 4; i++) 
	{
        uint16_t high = (uint16_t)FLASH_DATA[offset]     << 8;
        uint16_t low  = (uint16_t)FLASH_DATA[offset + 1];
        Wings_Data.Wings_motor[i].MOTOR_MIDPOINT = (int16_t)(high | low);
        offset += 2;
    }
	
}
