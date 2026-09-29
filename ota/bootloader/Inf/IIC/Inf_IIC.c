#include "Inf_IIC.h"



/**
 * @brief 写入多个字节数据
 * @note 向指定地址写入多个字节数据
 * 
 * @param chip_address：芯片的写地址（往哪个芯片写）
 * @param address：写入的内存地址（往芯片哪个地址写）
 * @param data：写入的数据
 * @param data：写入的长度
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U
 */
HAL_StatusTypeDef Inf_IIC_WriteBytes(uint8_t chip_address,uint8_t address,uint8_t *data, uint16_t data_length)
{
	return HAL_I2C_Mem_Write(&hi2c2,chip_address,address,I2C_MEMADD_SIZE_8BIT,data,data_length,100);
}


/**
 * @brief 读取多个字节数据
 * @note 向指定地址读取多个字节数据
 * 
 * @param chip_address：芯片的读地址（往哪个芯片读）
 * @param address：读取的内存地址（往芯片哪个地址读）
 * @param data：读取的数据缓冲区
 * @param data_length：读取的个数
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U
 */
HAL_StatusTypeDef Inf_IIC_readBytes(uint8_t chip_address, uint8_t address,uint8_t *data,uint16_t data_length){
	return HAL_I2C_Mem_Read(&hi2c2,chip_address,address,I2C_MEMADD_SIZE_8BIT,data,data_length,100);
}
