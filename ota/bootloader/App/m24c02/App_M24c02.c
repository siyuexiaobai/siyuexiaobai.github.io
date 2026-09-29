#include "App_M24c02.h"


#include <string.h>
/**
 * @brief 写入一个字节数据
 * @note 向m24c02指定地址写入一个字节数据，写入后要给芯片留至少5ms的时间留给芯片进行写入，所以需要在写入后延时n（10）ms
 * 
 * @param address：写入地址
 * @param data：写入的数据
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U 
 */
HAL_StatusTypeDef App_M24c02_WriteByte(uint8_t address, uint8_t data)
{

	HAL_StatusTypeDef ret =	Inf_IIC_WriteBytes(M24C02_WRITE_ADDRESS,address,&data,1);
	HAL_Delay(10);
	return ret;
}

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
HAL_StatusTypeDef App_M24c02_WriteBytes(uint8_t address, uint8_t *data, uint16_t data_length){
		//1.当前页剩余空间
   uint16_t remain_page_size =  M24C02_CHIP_PAGE_SIZE -	address % M24C02_CHIP_PAGE_SIZE;
	//2.剩余空间能存下
	if(remain_page_size > data_length){
		return 	Inf_IIC_WriteBytes(M24C02_WRITE_ADDRESS,address,data,data_length);
	}
	//3.跨页写入
	//3.1保存地址偏移量及保存数据偏移量
	uint16_t save_offset = address+remain_page_size;
	uint16_t data_offset = remain_page_size;
	//3.2先写满当前页

	HAL_StatusTypeDef ret =Inf_IIC_WriteBytes(M24C02_WRITE_ADDRESS,address,data,remain_page_size);
	HAL_Delay(10);
	if(ret != HAL_OK){
		printf("(error->)[%s:%d] iic write fail:%d",__FILE__,__LINE__,ret);
		return ret;
		
	}
	//3.3计算剩余长度
	data_length -= remain_page_size;
	//3.4循环写入16个字节
	for(uint16_t i = 0; i < data_length /M24C02_CHIP_PAGE_SIZE; i++){
		
		ret =	Inf_IIC_WriteBytes(M24C02_WRITE_ADDRESS,save_offset ,data+data_offset,M24C02_CHIP_PAGE_SIZE);
			HAL_Delay(10);
		if(ret != HAL_OK){
			printf("(error->)[%s:%d] iic write fail:%d",__FILE__,__LINE__,ret);
			return ret;
		
		}	
		data_offset += M24C02_CHIP_PAGE_SIZE;
		save_offset += M24C02_CHIP_PAGE_SIZE;
		data_length -= M24C02_CHIP_PAGE_SIZE;
		
	}
	//3.5处理最后剩余的数据
	if(data_length >0){
		ret =	Inf_IIC_WriteBytes(M24C02_WRITE_ADDRESS,save_offset,data+data_offset,data_length);
			HAL_Delay(10);
		if(ret != HAL_OK){
			printf("save_address:%d,save_data%s,save_lenth%d\r\n",save_offset,data+data_offset,data_length);
			printf("(error->)[%s:%d] iic write fail:%d,data:%s,save_address:%d\r\n",__FILE__,__LINE__,ret,data+data_offset,save_offset);
			return ret;
		}
		
	}
	return ret;
}


/**
 * @brief 读取一个字节
 * @note	从eeprom的指定地址读取一个字节数据
 * 
 * @param：address 读取地址
 * @param：data 读取数据缓冲区
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U  
 */
HAL_StatusTypeDef App_M24c02_readByte(uint8_t address,uint8_t *data)
{
		return Inf_IIC_readBytes(M24C02_READ_ADDRESS,address,data,1);
}	

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
HAL_StatusTypeDef App_M24c02_readBytes(uint8_t address,uint8_t *data,uint16_t data_length){
	 HAL_StatusTypeDef ret  =Inf_IIC_readBytes(M24C02_READ_ADDRESS,address,data,data_length);
	 if(ret != HAL_OK){
		 printf("(error->)[%s:%d],iic read fail:%d\r\n",__FILE__,__LINE__,ret);
	 }
	 return ret;
}
/**
 * @brief 初始化M24C02
 * @note	校验M24C02存储的升级标志
 * 
 * @param NULL
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U 
 */
HAL_StatusTypeDef App_M24C02_Init(void){
	HAL_StatusTypeDef ret = HAL_ERROR;
	ret = App_M24c02_readBytes(0,eeprom_code.eeprom_a,sizeof(eeprom_code.eeprom_a)); 
	if(ret != HAL_OK){
		printf("(error->)[%s:%d]M24C02 Init fail\r\n",__FILE__,__LINE__);
		return ret;
	}
	// 首次使用或者还未有任何更新
	if((eeprom_code.eeprom_t.eeprom_check_code_h << 8 | eeprom_code.eeprom_t.eeprom_check_code_l) == 0){ 
			printf("[%s:%d],eeprom first used\r\n",__FILE__,__LINE__);
			uint8_t need_rewrite_flash = 0;
			for(uint8_t i = 0; i < EEPROM_DATA_LENGTH; i++){
						if(eeprom_code.eeprom_a[i] != 0){
							need_rewrite_flash = 1;
							break;
						}
			}
			if(need_rewrite_flash){
				memset((char *) eeprom_code.eeprom_a,0,EEPROM_DATA_LENGTH);
				App_M24c02_WriteBytes(0,eeprom_code.eeprom_a,EEPROM_DATA_LENGTH);
			}
	}else{ //非首次使用  
		uint16_t temp = 0; //校验值
		for(uint8_t i = 0; i < EEPROM_DATA_LENGTH-2; i++){
			temp ^= eeprom_code.eeprom_a[i];
		}
		if(temp != (eeprom_code.eeprom_t.eeprom_check_code_h << 8 | eeprom_code.eeprom_t.eeprom_check_code_l)){ 
				printf("eeprom check fail,rewrite init value\r\n");
				memset((char *) eeprom_code.eeprom_a,0,EEPROM_DATA_LENGTH);
				App_M24c02_WriteBytes(0,eeprom_code.eeprom_a,EEPROM_DATA_LENGTH);
		}
	}
	return ret;
}


/**
 * @brief 校验存储数据合法性
 * @note  校验存储的数据有没有被破环 0-是否升级标志，1-升级步骤标志，3、4-校验和（后期可能会改成 3-当前使用APP区1还是APP区2，4、5-校验和）
 * 				校验方式为当前有效数据的异或值
 * @param NULL
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U 
 */
HAL_StatusTypeDef App_M24C02_check_data(void)
{

		App_M24c02_readBytes(0,eeprom_code.eeprom_a,EEPROM_DATA_LENGTH); 
		uint16_t temp = 0; //校验值
		for(uint8_t i = 0; i < EEPROM_DATA_LENGTH-2; i++){
			temp ^= eeprom_code.eeprom_a[i];
		}
		if(temp != (eeprom_code.eeprom_t.eeprom_check_code_h << 8 | eeprom_code.eeprom_t.eeprom_check_code_l)){ 
				return HAL_ERROR;
		}
		return HAL_OK;
}
/**
 * @brief 更新OTA升级步骤
 * @note  更新OTA升级步骤 需要同步更新校验值到eeprom
 * 
 * @param enum OTA_UPDATA_STEP_E  新的步骤
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U   
 */
HAL_StatusTypeDef App_M24c02_Updata_OTA_Updata_Step(enum OTA_UPDATA_STEP_E new_updata_step){
			//获取校验值
			uint16_t check_temp = eeprom_code.eeprom_t.eeprom_check_code_h << 8 |eeprom_code.eeprom_t.eeprom_check_code_l;
			//注销旧步骤的校验值
			check_temp ^= eeprom_code.eeprom_t.updata_step; 
			//设置新步骤的校验值
			check_temp ^= new_updata_step;
			//更新步骤
			eeprom_code.eeprom_t.updata_step = new_updata_step;
			//更新校验值
			eeprom_code.eeprom_t.eeprom_check_code_h = check_temp >>8;
			eeprom_code.eeprom_t.eeprom_check_code_l = check_temp;
			//写入eeprom
			return	App_M24c02_WriteBytes(0,eeprom_code.eeprom_a,EEPROM_DATA_LENGTH);
}

/**
 * @brief 更新OTA升级状态
 * @note  更新OTA升级状态 需要同步更新校验值到eeprom
 * 
 * @param enum OTA_UPDATA_STEP_E  新的状态
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U   
 */
HAL_StatusTypeDef App_M24c02_Updata_OTA_Updata_State(enum OTA_UPDATA_E ota_updata){
			//获取校验值
			uint16_t check_temp = eeprom_code.eeprom_t.eeprom_check_code_h << 8 |eeprom_code.eeprom_t.eeprom_check_code_l;
			//注销旧状态的校验值
			check_temp ^= eeprom_code.eeprom_t.is_need_update; 
			//设置新状态的校验值
			check_temp ^= ota_updata;
			//更新状态
			eeprom_code.eeprom_t.is_need_update = ota_updata;
			//更新校验值
			eeprom_code.eeprom_t.eeprom_check_code_h = check_temp >>8;
			eeprom_code.eeprom_t.eeprom_check_code_l = check_temp;
			//写入eeprom
			return	App_M24c02_WriteBytes(0,eeprom_code.eeprom_a,EEPROM_DATA_LENGTH);
}

/**
 * @brief 更新OTA升级新固件大小
 * @note  更新OTA升级新固件大小
 * 
 * @param new_code_size新固件大小
 * 
 * @return HAL_StatusTypeDef：HAL_OK=0x00U,HAL_ERROR=0x01U,HAL_BUSY=0x02U,HAL_TIMEOUT=0x03U   
 */
HAL_StatusTypeDef App_M24c02_Updata_New_Code_Size(uint32_t new_code_size){
			//获取校验值
			uint16_t check_temp = eeprom_code.eeprom_t.eeprom_check_code_h << 8 |eeprom_code.eeprom_t.eeprom_check_code_l;
			//注销旧大小的校验值
			check_temp ^= eeprom_code.eeprom_t.new_code_size_hh; 
			check_temp ^= eeprom_code.eeprom_t.new_code_size_h;
			check_temp ^= eeprom_code.eeprom_t.new_code_size_ll;
			check_temp ^= eeprom_code.eeprom_t.new_code_size_l;

			//获取新固件大小
			eeprom_code.eeprom_t.new_code_size_hh = (new_code_size >> 24); 
			eeprom_code.eeprom_t.new_code_size_h = (new_code_size >> 16); 
			eeprom_code.eeprom_t.new_code_size_ll = (new_code_size >> 8); 
			eeprom_code.eeprom_t.new_code_size_l = (new_code_size >> 0); 
				//设置新固件大小校验值
			check_temp ^= eeprom_code.eeprom_t.new_code_size_hh;
			check_temp ^= eeprom_code.eeprom_t.new_code_size_h;
			check_temp ^= eeprom_code.eeprom_t.new_code_size_ll;
			check_temp ^= eeprom_code.eeprom_t.new_code_size_l;
			//获取新的校验值
			eeprom_code.eeprom_t.eeprom_check_code_h = check_temp >>8;
			eeprom_code.eeprom_t.eeprom_check_code_l = check_temp;
			//写入eeprom
			return	App_M24c02_WriteBytes(0,eeprom_code.eeprom_a,EEPROM_DATA_LENGTH);
}
