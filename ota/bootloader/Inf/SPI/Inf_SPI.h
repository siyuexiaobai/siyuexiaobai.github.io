#pragma once 

#include "spi.h"

/**
 * @brief 写入数据
 *
 * @param data 待写入的数据
 * @param data_length 待写入的数据长度
 * @return HAL_StatusTypeDef：  HAL_OK——0x00U,HAL_ERROR——0x01U,HAL_BUSY——0x02U,HAL_TIMEOUT——0x03U
 */
HAL_StatusTypeDef Inf_SPI_WritrBytes(uint8_t *data, uint16_t data_length);

/**
 * @brief 读取数据
 *
 * @param data 读取数据的缓冲区
 * @param data_length 读取数据的长度
 * @return HAL_StatusTypeDef：  HAL_OK——0x00U,HAL_ERROR——0x01U,HAL_BUSY——0x02U,HAL_TIMEOUT——0x03U
 */
HAL_StatusTypeDef Inf_SPI_ReadBytes(uint8_t *data,uint16_t data_length);

/**
 * @brief 读取数据且接收数据
 * 
 * @param send_data 待发送数据
 * @param receive_data 接收数据缓冲区 
 * @param data_length  交互数据的长度
 * @return HAL_StatusTypeDef：  HAL_OK——0x00U,HAL_ERROR——0x01U,HAL_BUSY——0x02U,HAL_TIMEOUT——0x03U
 */
HAL_StatusTypeDef Inf_SPI_Writr_And_Read(uint8_t *send_data,uint8_t *receive_data,uint16_t data_length);
