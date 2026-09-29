#include "App_OTA.h"
#include "Com_Config.h"
#include "App_W25q32.h"
#include <string.h>
enum OTA_UPDATA_STEP_E App_OTA_Erase_chip_flash(void);
enum OTA_UPDATA_STEP_E App_OTA_Write_flash(void);
void App_Jamp_App(void);

typedef void (*jamp_func)(void);

/**
 * @brief 初始化OTA
 *
 * @note OTA升级的B端的主要函数
 *
 */
void APP_OTA_Init(void)
{

	if (eeprom_code.eeprom_t.is_need_update)
	{ // 需要更新
		while (1)
		{
			switch (eeprom_code.eeprom_t.updata_step)
			{
			case OTA_ERASE_CHIP_FLASH:
				eeprom_code.eeprom_t.updata_step = App_OTA_Erase_chip_flash();
				break; // 擦除片上flash
			case OTA_UPDATA_WRITE_FLASH:
				eeprom_code.eeprom_t.updata_step = App_OTA_Write_flash();
				break; // 写入flash
			case OTA_UPDATA_FINISH:
				App_Jamp_App();
				break;
			case OTA_UPDATA_FAIL: //能来到这条分支，恭喜你已经被玩坏了或者只烧录的B代码，A区代码是空的需要你手动烧录A区代码
				//TODO 需要读取APP_START_ADDRESS地址的数据是否是0x2000xxxx及APP_START_ADDRESS+4的地址数据是否是0x0800xxxx是的话要跳转到A区
				printf("OTA UPDATA FAIL ,PLEASE MANUAL UPDATA TO 0x%08x", APP_START_ADDRESS);
				break;
				// 以下直接跳转，下面对应的逻辑在app区执行
			case OTA_UPDATA_INIT:
			case OTA_ERASE_SPI_FLASH:
			case OTA_UPDATA_DOWNLOAD:
			case OTA_UPDATA_CHECK_BIN:
			default:
				App_Jamp_App();
			}
		}
	}
}
/**
 * @brief 擦除片上flash
 * @note
 *
 * @param NULL
 *
 * @return enum OTA_UPDATA_STEP_E
 */

enum OTA_UPDATA_STEP_E App_OTA_Erase_chip_flash(void)
{
	HAL_StatusTypeDef ret = HAL_ERROR;
	uint16_t erase_offset = 0; // 合成新固件的大小
	uint32_t erase_size = eeprom_code.eeprom_t.new_code_size_hh << 24 | eeprom_code.eeprom_t.new_code_size_h << 16 | eeprom_code.eeprom_t.new_code_size_ll << 8 | eeprom_code.eeprom_t.new_code_size_l << 0;

	if (erase_size == 0)
	{ // 新固件为0则跳转到app重写接收
		printf("(error->)[%s:%d] erase size is zero\r\n", __FILE__, __LINE__);
		App_M24c02_Updata_OTA_Updata_Step(OTA_UPDATA_INIT);
		return OTA_UPDATA_INIT;
	}
	// 计算需要擦除的flash页
	erase_offset = erase_size / CHIP_FLASH_PAGE_SIZE;
	if (erase_size % CHIP_FLASH_PAGE_SIZE != 0)
	{
		erase_offset += 1;
	}
	// 解锁flash
	ret = HAL_FLASH_Unlock();
	// 解锁失败跳转app区
	if (ret != HAL_OK)
	{
		printf("(error->)[%s:%d] flash unlock fail\r\n", __FILE__, __LINE__);
		App_M24c02_Updata_OTA_Updata_Step(OTA_UPDATA_INIT);
		return OTA_UPDATA_INIT;
	}
	// 开始擦除
	for (uint16_t i = 0; i < erase_offset; i++)
	{
		FLASH_EraseInitTypeDef erase_init_typedef = {
			.TypeErase = FLASH_TYPEERASE_PAGES,
			.Banks = FLASH_BANK_1,
			.PageAddress = APP_START_ADDRESS + i * CHIP_FLASH_PAGE_SIZE,
			.NbPages = 1};
		uint32_t PageError = 1;
		// 擦除失败，flash里的固件可能已经被破坏了不能跳转到app区，重新擦除flash
		ret = HAL_FLASHEx_Erase(&erase_init_typedef, &PageError);
		if (ret != HAL_OK)
		{
			printf("(error->)[%s:%d] flash erase fail\r\n", __FILE__, __LINE__);
			App_M24c02_Updata_OTA_Updata_Step(OTA_ERASE_CHIP_FLASH);
			return OTA_ERASE_CHIP_FLASH;
		}
	}

	printf("OTA_Erase_chip_flash\r\n");
	App_M24c02_Updata_OTA_Updata_Step(ret == HAL_OK ? OTA_UPDATA_WRITE_FLASH : OTA_ERASE_CHIP_FLASH);
	return ret == HAL_OK ? OTA_UPDATA_WRITE_FLASH : OTA_ERASE_CHIP_FLASH;
}

/**
 * @brief 将固件写入到flash
 *
 * @note 从spi-flash读取固件文件写入到片上flash上，每次取512字节，按字为单位写入
 *
 * @return enum OTA_UPDATA_STEP_E
 */
enum OTA_UPDATA_STEP_E App_OTA_Write_flash(void)
{
	HAL_StatusTypeDef ret = HAL_ERROR;
	printf("OTA_Erase_write_flash\r\n");
	// 0.拯救flash
	if (NEW_CODE_SIZE == 0) // 片内flash已经擦除了，于事无补了后期做app1与app2分区可进行app分区切换，但是上一步擦除flash开始时会校验NEW_CODE_SIZE，一般不会成立,如果成立就毁灭吧
	{

		for (uint32_t i = 0; i < 4 * 1024 * 1024 / W25Q32_BIN_READ_SIZE; i++) // 每次找512字节
		{																	  //
			uint8_t read_data = 0;
			App_W25q32_Read_Find_Byte(i * W25Q32_BIN_READ_SIZE, &read_data); // 读取页头数据
			if (read_data == 0xff)											 // 读到空闲数据
			{
				if (i == 0)
				{
					// 无解，上一步是擦除片上app区 flash，eeprom保存的新固件大小为0，读w25q32的首地址也为空，OTA是不可能了，等手段烧录固件吧。
					return OTA_UPDATA_FAIL;
				}
				else
				{
					// 有希望拯救
					uint8_t find_bin_buff[512] = {0};
					ret = App_W25q32_Read_Find_Bytes((i - 1) * 512, find_bin_buff); // 当前i地址对应的页是空白的，需要找上一个双页确定数据尾位置p1 满 p2 半， i=3 or p1 满 p2 满 p3空，i = 3;
					if (ret == HAL_ERROR)
					{
						printf("(error)->[%s:%d] OTA Write Flash fail", __FILE__, __LINE__);
						return OTA_UPDATA_FAIL;
					}
					uint16_t j = 0;
					uint8_t is_find = 0;
					for (; j < 512; j++)
					{
						if (find_bin_buff[j] == 0xff) // 找完数据
						{
							is_find = 1; // p1 满 p2 半， i=3 is_find or p1 满 p2 满 p3空，i = 3 is_find;
							break;
						}
					}

					uint32_t new_code_size = is_find ? (i - 1) * W25Q32_BIN_READ_SIZE + j : i * W25Q32_BIN_READ_SIZE; // p1 满 p2 半， i=3 ,is_find =1 , new_code_size = (3-2 = 1)*512 + j-1 or p1 满 p2 满 p3空，i = 3, is_find = 0,new_code_size = (3-1 = 2)*512;
					eeprom_code.eeprom_t.new_code_size_hh = new_code_size >> 24;
					eeprom_code.eeprom_t.new_code_size_h = new_code_size >> 16;
					eeprom_code.eeprom_t.new_code_size_ll = new_code_size >> 8;
					eeprom_code.eeprom_t.new_code_size_l = new_code_size >> 0;
					App_M24c02_Updata_New_Code_Size(new_code_size);
					break;
				}
			}
		}
	}

	// FLASH_Type_Program
	uint8_t bin_buff[W25Q32_BIN_READ_SIZE] = {0};
	uint32_t write_address_offset = APP_START_ADDRESS;
	// 1.先读写取整512字节的
	ret = HAL_FLASH_Unlock();
	if (ret != HAL_OK)
	{
		printf("(error->)[%s:%d] flash unlock fail\r\n", __FILE__, __LINE__);
		App_M24c02_Updata_OTA_Updata_Step(OTA_UPDATA_WRITE_FLASH);
		return OTA_UPDATA_WRITE_FLASH;
	}
	for (uint16_t i = 0; i < NEW_CODE_SIZE / W25Q32_BIN_READ_SIZE; i++) // 循环每次读512字节
	{
		ret = App_W25q32_Read_Bin(W25Q32_BIN_BASH_ADDRESS + i * W25Q32_BIN_READ_SIZE, bin_buff);
		if (ret == HAL_ERROR)
		{
			printf("(error)->[%s:%d] OTA Write Flash fail", __FILE__, __LINE__);
			HAL_FLASH_Lock();
			return OTA_UPDATA_WRITE_FLASH;
		}
		for (uint16_t j = 0; j < W25Q32_BIN_READ_SIZE; j += 4)
		{
			uint32_t data_temp = bin_buff[j + 3] << 24 | bin_buff[j + 2] << 16 | bin_buff[j + 1] << 8 | bin_buff[j];
			ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, write_address_offset, data_temp);
			if (ret == HAL_ERROR)
			{
				printf("(error)->[%s:%d] OTA Write Flash fail", __FILE__, __LINE__);
				HAL_FLASH_Lock();
				return OTA_UPDATA_WRITE_FLASH;
			}
			write_address_offset += 4; // 08000000 + i * 512 + j * 4
		}
	}
	// 2.读写剩余或者固件大小不足512字节的，如果固件大小不足512字节第一步是不运行的
	if (NEW_CODE_SIZE % W25Q32_BIN_READ_SIZE != 0) // 有剩余或者固件大小不足512字节
	{
		memset((char *)bin_buff, 0, W25Q32_BIN_READ_SIZE); // 清空缓冲区
		if (NEW_CODE_SIZE / W25Q32_BIN_READ_SIZE > 0)	   // 剩余
		{
			ret = App_W25q32_Read_Bin((NEW_CODE_SIZE / W25Q32_BIN_READ_SIZE) * W25Q32_BIN_READ_SIZE, bin_buff); // (526/512 = 1)*512 = 512,从512第二页（0-511）首地址开始读
			if (ret == HAL_ERROR)
			{
				printf("(error)->[%s:%d] OTA Write Flash fail", __FILE__, __LINE__);
				HAL_FLASH_Lock();
				return OTA_UPDATA_WRITE_FLASH;
			}
		}
		else // 固件总大小不足512
		{
			ret = App_W25q32_Read_Bin(W25Q32_BIN_BASH_ADDRESS, bin_buff); // 从0地址开始读
			if (ret == HAL_ERROR)
			{
				printf("(error)->[%s:%d] OTA Write Flash fail", __FILE__, __LINE__);
				HAL_FLASH_Lock();
				return OTA_UPDATA_WRITE_FLASH;
			}
		}
		// 3.四字节一组处理
		for (uint8_t i = 0; i < NEW_CODE_SIZE % W25Q32_BIN_READ_SIZE / 4; i++) // bin size: 113, i: 0-28  %：1
		{
			uint32_t data_temp = bin_buff[i * 4] | bin_buff[i * 4 + 1] << 8 | bin_buff[i * 4 + 2] << 16 | bin_buff[i * 4 + 3] << 24;
			ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, write_address_offset, data_temp);
			if (ret == HAL_ERROR)
			{
				printf("(error)->[%s:%d] OTA Write Flash fail", __FILE__, __LINE__);
				HAL_FLASH_Lock();
				return OTA_UPDATA_WRITE_FLASH;
			}
			write_address_offset += 4;
		}
		// 4.最后一帧凑不够4字节补0xff处理
		uint8_t temp_index = NEW_CODE_SIZE % 4; // 处理最后一帧数据 113 - ((113/4 = 28) * 4= 112) = 1
		if (temp_index == 3)
		{
			// 113 - 3 = 110											111												112
			uint32_t last_data = bin_buff[NEW_CODE_SIZE % W25Q32_BIN_READ_SIZE - 3] << 0 | bin_buff[NEW_CODE_SIZE % W25Q32_BIN_READ_SIZE - 2] << 8 | bin_buff[NEW_CODE_SIZE % W25Q32_BIN_READ_SIZE - 1] << 16 | 0xff000000;
			ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, write_address_offset, last_data);
			if (ret == HAL_ERROR)
			{
				printf("(error)->[%s:%d] OTA Write Flash fail", __FILE__, __LINE__);
				HAL_FLASH_Lock();
				return OTA_UPDATA_WRITE_FLASH;
			}
		}
		else if (temp_index == 2)
		{
			// 113 - 3 = 110											111												112
			uint32_t last_data = bin_buff[NEW_CODE_SIZE % W25Q32_BIN_READ_SIZE - 2] << 0 | bin_buff[NEW_CODE_SIZE % W25Q32_BIN_READ_SIZE - 1] << 8 | 0xffff0000;
			ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, write_address_offset, last_data);
			if (ret == HAL_ERROR)
			{
				printf("(error)->[%s:%d] OTA Write Flash fail", __FILE__, __LINE__);
				HAL_FLASH_Lock();
				return OTA_UPDATA_WRITE_FLASH;
			}
		}
		else if (temp_index == 1)
		{
			uint32_t last_data = bin_buff[NEW_CODE_SIZE % W25Q32_BIN_READ_SIZE - 1] << 0 | 0xffffff00;
			ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, write_address_offset, last_data);
			if (ret == HAL_ERROR)
			{
				printf("(error)->[%s:%d] OTA Write Flash fail", __FILE__, __LINE__);
				HAL_FLASH_Lock();
				return OTA_UPDATA_WRITE_FLASH;
			}
		}
	}
	HAL_FLASH_Lock();

	App_M24c02_Updata_OTA_Updata_Step(OTA_UPDATA_FINISH);
	return OTA_UPDATA_FINISH;
}

/**
 * @brief  跳转到app区
 * @note
 *
 * @param NULL
 *
 * @return NULL
 */
void App_Jamp_App(void)
{

	printf("OTA jamp app\r\n");
	uint32_t app_msp;
	uint32_t app_reset;
	jamp_func jamp_to_app;

	if (eeprom_code.eeprom_t.updata_step == OTA_UPDATA_FINISH)
	{
		App_M24c02_Updata_OTA_Updata_State(OTA_NOT_NEED_UPDATA);
		App_M24c02_Updata_New_Code_Size(0);
	}
	HAL_Delay(100);
	app_msp = *(volatile uint32_t *)APP_START_ADDRESS; // 获取MSP的地址0x2000xxxx

	app_reset = *(volatile uint32_t *)(APP_START_ADDRESS + 4); // 获取复位向量的地址0x2000xxxx

	if (app_msp < 0x20000000 || app_msp > 0x20010000)
	{
		eeprom_code.eeprom_t.updata_step = OTA_UPDATA_WRITE_FLASH;
		printf("Invalid MSP: 0x%08X\r\n", app_msp);
		return;
	}
	if (app_reset < APP_START_ADDRESS)
	{
		eeprom_code.eeprom_t.updata_step = OTA_UPDATA_WRITE_FLASH;
		printf("Invalid Reset: 0x%08X\r\n", app_reset);
		return;
	}

	__disable_irq(); // 关闭中断
	// 重置滴答时钟
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;

	// 设置跳转地址
	SCB->VTOR = APP_START_ADDRESS;

	// 设置msp
	__set_MSP(app_msp);

	// 获取复位函数
	jamp_to_app = (jamp_func)app_reset;
	// 如果是直接跳转就不执行下面的清理工作

	jamp_to_app(); // 执行复位中断函数
}
