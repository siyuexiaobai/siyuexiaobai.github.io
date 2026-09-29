#pragma once

#include "main.h"

// OTA 的app起始地址
#define APP_START_ADDRESS 0x08004000

// eeprom 存储的数据字节数
#define EEPROM_DATA_LENGTH 8

// 片上flash的页大小
#define CHIP_FLASH_PAGE_SIZE 0x800

// 新的固件长度计算宏
#define NEW_CODE_SIZE (eeprom_code.eeprom_t.new_code_size_hh << 24 | eeprom_code.eeprom_t.new_code_size_h << 16 | eeprom_code.eeprom_t.new_code_size_ll << 8 | eeprom_code.eeprom_t.new_code_size_ll)

/**
 * @brief 每个属性代表eeprom内存储的数据
 *
 */
typedef struct
{
	uint8_t is_need_update;		 // 升级标志位
	uint8_t updata_step;		 // 更新步骤
	uint8_t new_code_size_hh;	 // 新固件数据大小最高位
	uint8_t new_code_size_h;	 // 新固件数据大小次高位
	uint8_t new_code_size_ll;	 // 新固件数据大小次低位
	uint8_t new_code_size_l;	 // 新固件数据大小最低位
	uint8_t eeprom_check_code_h; // 校验码高位
	uint8_t eeprom_check_code_l; // 校验码低位
} eeprom_data_t;


/**
 * @brief 公用体，eeprom_t方便修改eeprom的内容，eeprom_a方便读写eeprom
 * 
 */
typedef union
{
	eeprom_data_t eeprom_t;
	uint8_t eeprom_a[sizeof(eeprom_data_t)];
} EEPROM_CODE_T;

/**
 *
 *OTA更新状态
 *
 */
enum OTA_UPDATA_E
{
	OTA_NOT_NEED_UPDATA = 0,
	OTA_NEED_UPDATA,
	MAX_NEED
};

/**
 *
 *OTA更新步骤
 *
 */
enum OTA_UPDATA_STEP_E
{
	OTA_UPDATA_INIT,		// 初始化
	OTA_ERASE_SPI_FLASH,	// 擦除spi-flash
	OTA_UPDATA_DOWNLOAD,	// 下载OTA固件
	OTA_UPDATA_CHECK_BIN,	// 校验固件
	OTA_ERASE_CHIP_FLASH,	// 擦除片上flash
	OTA_UPDATA_WRITE_FLASH, // 开始升级
	OTA_UPDATA_FINISH,		// 升级完成
	OTA_UPDATA_FAIL,		// 升级失败
};

/************外部链接变量**************** */
extern enum OTA_UPDATA_E ota_updata;
extern enum OTA_UPDATA_STEP_E ota_updata_step;
extern uint32_t new_code_size;
extern EEPROM_CODE_T eeprom_code;
