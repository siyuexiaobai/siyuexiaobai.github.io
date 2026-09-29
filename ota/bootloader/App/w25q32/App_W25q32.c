#include "App_W25q32.h"

/**
 * @brief 写入数据
 *
 * @param data 待写入的数据
 * @param data_length 待写入的数据长度
 * @return HAL_StatusTypeDef：  HAL_OK——0x00U,HAL_ERROR——0x01U,HAL_BUSY——0x02U,HAL_TIMEOUT——0x03U
 */
HAL_StatusTypeDef App_W25q32_WriteBytes(uint8_t *data, uint16_t data_length);

/**
 * @brief 写入单字节数据
 *
 * @param data 待写入的数据
 * @return HAL_StatusTypeDef：  HAL_OK——0x00U,HAL_ERROR——0x01U,HAL_BUSY——0x02U,HAL_TIMEOUT——0x03U
 */
HAL_StatusTypeDef App_W25q32_WriteByte(uint8_t data);

/**
 * @brief 读取数据
 *
 * @param data 读取数据缓冲区
 * @param data_length 读取长度
 * @return HAL_StatusTypeDef：  HAL_OK——0x00U,HAL_ERROR——0x01U,HAL_BUSY——0x02U,HAL_TIMEOUT——0x03U
 */
HAL_StatusTypeDef App_W25q32_Read_Bytes(uint8_t *data, uint16_t data_length);

/**
 * @brief 读取单字节数据
 *
 * @param data 读取数据缓冲区
 * @return HAL_StatusTypeDef：  HAL_OK——0x00U,HAL_ERROR——0x01U,HAL_BUSY——0x02U,HAL_TIMEOUT——0x03U
 */
HAL_StatusTypeDef App_W25q32_Read_Byte(uint8_t *data);
/**
 * @brief 获取芯片id
 *
 * @param chipID_buff chipID缓冲区
 * @return HAL_StatusTypeDef：  HAL_OK——0x00U,HAL_ERROR——0x01U,HAL_BUSY——0x02U,HAL_TIMEOUT——0x03U
 */
static HAL_StatusTypeDef App_W25q32_Read_ChipID(uint8_t *chipID_buff);
/**
 * @brief 初始化w25q32
 *
 */
void App_W25q32_Init(void)
{
    uint8_t chip_id[3] = {0}; // 0xef,0x4016
    HAL_StatusTypeDef ret = App_W25q32_Read_ChipID(chip_id);
    printf("0x%02x,0x%04x\r\n", chip_id[0], chip_id[1] << 8 | chip_id[2]);
    uint8_t data_buff[W25Q32_BIN_READ_SIZE] = {0};
    App_W25q32_Read_Bin(0, data_buff);
    for (uint16_t i = 0; i < W25Q32_BIN_READ_SIZE; i++)
    {
        if (i != 0 && i % 7 == 0)
        {

            printf("\n");
        }
        printf("%c ", data_buff[i]);
    }
}

/**
 * @brief 写入数据
 *
 * @param data 待写入的数据
 * @param data_length 待写入的数据长度
 */
HAL_StatusTypeDef App_W25q32_WriteBytes(uint8_t *data, uint16_t data_length)
{

    return Inf_SPI_WritrBytes(data, data_length);
}

/**
 * @brief 写入单字节数据
 *
 * @param data 待写入的数据
 * @return HAL_StatusTypeDef：  HAL_OK——0x00U,HAL_ERROR——0x01U,HAL_BUSY——0x02U,HAL_TIMEOUT——0x03U
 */
HAL_StatusTypeDef App_W25q32_WriteByte(uint8_t data)
{
    return App_W25q32_WriteBytes(&data, 1);
}

/**
 * @brief 读取数据
 *
 * @param data 读取数据缓冲区
 * @param data_length 读取长度
 * @return HAL_StatusTypeDef：  HAL_OK——0x00U,HAL_ERROR——0x01U,HAL_BUSY——0x02U,HAL_TIMEOUT——0x03U
 */
HAL_StatusTypeDef App_W25q32_Read_Bytes(uint8_t *data, uint16_t data_length)
{

    return Inf_SPI_ReadBytes(data, data_length);
}

/**
 * @brief 读取单字节数据
 *
 * @param data 读取数据缓冲区
 * @return HAL_StatusTypeDef：  HAL_OK——0x00U,HAL_ERROR——0x01U,HAL_BUSY——0x02U,HAL_TIMEOUT——0x03U
 */
HAL_StatusTypeDef App_W25q32_Read_Byte(uint8_t *data)
{
    return App_W25q32_Read_Bytes(data, 1);
}

/**
 * @brief 获取芯片id
 *
 * @param chipID_buff chipID缓冲区
 * @return HAL_StatusTypeDef：  HAL_OK——0x00U,HAL_ERROR——0x01U,HAL_BUSY——0x02U,HAL_TIMEOUT——0x03U
 */
static HAL_StatusTypeDef App_W25q32_Read_ChipID(uint8_t *chipID_buff)
{
    W25Q32_CS_LOW;
    uint8_t cmd = W25Q32_CHIP_ID_CMD;
    HAL_StatusTypeDef ret = App_W25q32_WriteByte(cmd);
    ret = Inf_SPI_ReadBytes(chipID_buff, 3);
    W25Q32_CS_HIGH;
    return ret;
}


/**
 * @brief 读取bin文件
 * @note 每次读取512字节用于写入到片内flash
 * @param read_address 读取地址
 * @param data_buff     读取数据缓冲区
 * @return HAL_StatusTypeDef ：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U   
 */
HAL_StatusTypeDef App_W25q32_Read_Bin(uint32_t read_address, uint8_t *data_buff)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    W25Q32_CS_LOW;
    uint8_t read_cmd[4] = {W25Q32_READ_DATA_CMD, read_address >> 24, read_address >> 16, read_address >> 8};
    ret = App_W25q32_WriteBytes(read_cmd, 4);
    if (ret != HAL_OK)
    {
        printf("(error)->[%s:%d]app read bin  send cmd fail\r\n", __FILE__, __LINE__);
        W25Q32_CS_HIGH;
        return ret;
    }
    ret = App_W25q32_Read_Bytes(data_buff, W25Q32_BIN_READ_SIZE);
    if (ret != HAL_OK)
    {
        printf("(error)->[%s:%d]app read bin  send cmd ok but read fail\r\n", __FILE__, __LINE__);
        W25Q32_CS_HIGH;
        return ret;
    }
    W25Q32_CS_HIGH;
    return ret;
}


/**
 * @brief 读取一个字节
 * 
 * @note用于在spi-flash ————>>>chip on flash的时候chip on flash的0x08000000已经擦除了但eeprom的新固件大小为0的时候
 * 两页两页读取首地址的数据反推新固件大小
 * 
 * @param read_address 读取地址
 * @param data_buff 读取数据缓冲区
 * @return HAL_StatusTypeDef ：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U  
 */
HAL_StatusTypeDef App_W25q32_Read_Find_Byte(uint32_t read_address, uint8_t *data_buff)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    W25Q32_CS_LOW;
    uint8_t read_cmd[4] = {W25Q32_READ_DATA_CMD, read_address >> 24, read_address >> 16, read_address >> 8};
    ret = App_W25q32_WriteBytes(read_cmd, 4);
    if (ret != HAL_OK)
    {
        printf("(error)->[%s:%d]app read bin  send cmd fail\r\n", __FILE__, __LINE__);
        W25Q32_CS_HIGH;
        return ret;
    }
    ret = App_W25q32_Read_Bytes(data_buff, 1);
    if (ret != HAL_OK)
    {
        printf("(error)->[%s:%d]app read bin  send cmd ok but read fail\r\n", __FILE__, __LINE__);
        W25Q32_CS_HIGH;
        return ret;
    }
    W25Q32_CS_HIGH;
    return ret;
}

/**
 * @brief 读取最后一页不足512字节的固件数据
 * 
 * @param read_address 读取地址
 * @param data_buff 读取数据缓冲区
 * @return HAL_StatusTypeDef ：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U  
 */
HAL_StatusTypeDef App_W25q32_Read_Find_Bytes(uint32_t read_address, uint8_t *data_buff)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    W25Q32_CS_LOW;
    uint8_t read_cmd[4] = {W25Q32_READ_DATA_CMD, read_address >> 24, read_address >> 16, read_address >> 8};
    ret = App_W25q32_WriteBytes(read_cmd, 4);
    if (ret != HAL_OK)
    {
        printf("(error)->[%s:%d]app read bin  send cmd fail\r\n", __FILE__, __LINE__);
        W25Q32_CS_HIGH;
        return ret;
    }
    ret = App_W25q32_Read_Bytes(data_buff, 512);
    if (ret != HAL_OK)
    {
        printf("(error)->[%s:%d]app read bin  send cmd ok but read fail\r\n", __FILE__, __LINE__);
        W25Q32_CS_HIGH;
        return ret;
    }
    W25Q32_CS_HIGH;
    return ret;
}
