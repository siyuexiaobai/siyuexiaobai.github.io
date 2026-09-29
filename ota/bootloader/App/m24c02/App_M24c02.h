#pragma once

#include "Inf_IIC.h"
#include "Com_Config.h"

#define M24C02_CHIP_PAGE_SIZE 16    //芯片页大小
#define M24C02_CHIP_ADDRESS 0x50    //芯片物理地址
#define M24C02_WRITE_ADDRESS (M24C02_CHIP_ADDRESS << 1)     //芯片的写入地址 七位物理地址+一位读写位，读写位读1写0
#define M24C02_READ_ADDRESS (M24C02_CHIP_ADDRESS <<1 |0x01) //芯片的读取地址 七位物理地址+一位读写位，读写位读1写0

/**
 * @brief 写入一个字节数据
 * 
 * @param address  写入地址
 * @param data 写入数据
 * @return HAL_StatusTypeDef ：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U 
 */
HAL_StatusTypeDef App_M24c02_WriteByte(uint8_t address, uint8_t data);


/**
 * @brief 写入多个字节数据
 * @note  向m24c02指定地址写入多个字节数据，16byte一页，超过16字节需要重写地址要不然会从页头覆盖写入
 *				例如：从0地址写入26个字母，abcd efgh ijkl mnop 再继续写的话需要再重写调用写入函数并重新计算写入地址否则 
 *              qrst……会替换掉前面的abcd……，最终变成 qrst uvwx yzkl mnop，此函数以实现自动翻页写入
 * 
 * @param address：写入地址
 * @param data：写入数据
 * @param data_length：写入数据长度
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U  
 */
HAL_StatusTypeDef App_M24c02_WriteBytes(uint8_t address, uint8_t* data, uint16_t data_length);


/**
 * @brief 读取一个字节
 * @note	从eeprom的指定地址读取一个字节数据
 * 
 * @param：address 读取地址
 * @param：data 读取数据缓冲区
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U  
 */
HAL_StatusTypeDef App_M24c02_readByte(uint8_t address,uint8_t *data);


/**
 * @brief 读取多个字节
 * @note	从eeprom的指定地址读取多个字节数据
 * 
 * @param：address 读取地址
 * @param：data 读取数据缓冲区
 * @param：data_length 读取数据长度
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U  
 */
HAL_StatusTypeDef App_M24c02_readBytes(uint8_t address,uint8_t *data,uint16_t data_length);


/**
 * @brief 校验存储数据合法性
 * @note  校验存储的数据有没有被破环 0-是否升级标志，1-升级步骤标志，3、4-校验和（后期可能会改成 3-当前使用APP区1还是APP区2，4、5-校验和）
 * 				校验方式为当前有效数据的异或值
 * @param NULL
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U 
 */
HAL_StatusTypeDef App_M24C02_check_data(void);


/**
 * @brief 初始化M24C02
 * @note	校验M24C02存储的升级标志
 * 
 * @param NULL
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U 
 */
HAL_StatusTypeDef App_M24C02_Init(void);


/**
 * @brief 更新OTA升级步骤
 * @note  更新OTA升级步骤 需要同步更新校验值到eeprom
 * 
 * @param enum OTA_UPDATA_STEP_E  新的步骤
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U   
 */
HAL_StatusTypeDef App_M24c02_Updata_OTA_Updata_Step(enum OTA_UPDATA_STEP_E new_updata_step);


/**
 * @brief 更新OTA升级状态
 * @note  更新OTA升级状态 需要同步更新校验值到eeprom
 * 
 * @param enum OTA_UPDATA_STEP_E  新的状态
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U   
 */
HAL_StatusTypeDef App_M24c02_Updata_OTA_Updata_State(enum OTA_UPDATA_E ota_updata);


/**
 * @brief 更新OTA升级新固件大小
 * @note  更新OTA升级新固件大小
 * 
 * @param new_code_size新固件大小
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U   
 */
HAL_StatusTypeDef App_M24c02_Updata_New_Code_Size(uint32_t new_code_size);
