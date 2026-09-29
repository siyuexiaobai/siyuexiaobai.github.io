#pragma once

#include "Inf_SPI.h"
#include "gpio.h"

#define W25Q32_CS_HIGH HAL_GPIO_WritePin(W25Q32_CS_GPIO_Port,W25Q32_CS_Pin,GPIO_PIN_SET)
#define W25Q32_CS_LOW HAL_GPIO_WritePin(W25Q32_CS_GPIO_Port,W25Q32_CS_Pin,GPIO_PIN_RESET)

#define W25Q32_BIN_READ_SIZE 512
#define W25Q32_BIN_BASH_ADDRESS 0

#define W25Q32_CHIP_ID_CMD 0x9F
#define W25Q32_READ_DATA_CMD  0x03

/**
 * @brief ???w25q32
 *
 */
void App_W25q32_Init(void);


/**
 * @brief ??bin??
 * @note ????512?????????flash
 * @param read_address ????
 * @param data_buff     ???????
 * @return HAL_StatusTypeDef ?HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U   
 */
HAL_StatusTypeDef App_W25q32_Read_Bin(uint32_t read_address, uint8_t *data_buff);


/**
 * @brief ??????
 * 
 * @note???spi-flash ————>>>chip on flash???chip on flash?0x08000000??????eeprom???????0???
 * ???????????????????
 * 
 * @param read_address ????
 * @param data_buff ???????
 * @return HAL_StatusTypeDef ?HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U  
 */
HAL_StatusTypeDef App_W25q32_Read_Find_Byte(uint32_t read_address, uint8_t *data_buff);


/**
 * @brief ????????512???????
 * 
 * @param read_address ????
 * @param data_buff ???????
 * @return HAL_StatusTypeDef ?HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U  
 */
HAL_StatusTypeDef App_W25q32_Read_Find_Bytes(uint32_t read_address, uint8_t *data_buff);

