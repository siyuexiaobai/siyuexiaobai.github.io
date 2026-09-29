#include "Com_Config.h"

enum OTA_UPDATA_E ota_updata = OTA_NOT_NEED_UPDATA;
enum OTA_UPDATA_STEP_E ota_updata_step = OTA_UPDATA_INIT;

EEPROM_CODE_T eeprom_code = {0};
