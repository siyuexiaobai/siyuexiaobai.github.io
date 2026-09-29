#ifndef __INF_IIC_H__
#define __INF_IIC_H__

#include "i2c.h"


/**
 * @brief 写入多字节数据
 * 
 * @param chip_address 芯片的物理地址
 * @param address 要写入的内存地址
 * @param data 要写入的数据
 * @param data_length 要写入的数据的长度
 * @return HAL_StatusTypeDef ：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U
 */
HAL_StatusTypeDef Inf_IIC_WriteBytes(uint8_t chip_address,uint8_t address,uint8_t *data, uint16_t data_length);

/**
 * @brief 读取多字节数据
 * 
 * @param chip_address 芯片的物理地址
 * @param address 要读取的内存地址
 * @param data 要读取数据的缓冲区
 * @param data_length 要读取数据的长度
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U
 */
HAL_StatusTypeDef Inf_IIC_readBytes(uint8_t chip_address, uint8_t address,uint8_t *data,uint16_t data_length);
#endif
