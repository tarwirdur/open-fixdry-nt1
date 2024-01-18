#include "inc/eeprom.h"
#include "config.h"

#define CRC_SALT (179)

uint8_t _crc(struct eeprom_st * state);
void _default_data(struct eeprom_st * target);

struct eeprom_st eeprom_data;

void eeprom_load() {
  #ifdef ENABLE_EEPROM
    for(uint8_t i = 0; i < (uint8_t)sizeof(struct eeprom_st); ++i) {
      ((uint8_t*)&eeprom_data)[i] = FLASH_ReadByte(EEPROM_ADDR+i);
    }
    if (_crc(&eeprom_data) != eeprom_data.crc) {
      _default_data(&eeprom_data);
    }
  #else
    _default_data(&eeprom_data);
  #endif
}

void eeprom_save() {
#ifdef ENABLE_EEPROM
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  eeprom_data.crc = _crc(&eeprom_data);
  for(uint8_t i = 0; i < sizeof(struct eeprom_st); ++i) {
    uint8_t w = ((uint8_t*)&eeprom_data)[i];
    uint8_t r = FLASH_ReadByte(EEPROM_ADDR + i);
    if (r != w) {
      FLASH_ProgramByte(EEPROM_ADDR + i, w);
    }
  }
  FLASH_Lock(FLASH_MEMTYPE_DATA);
#endif
}

void _default_data(struct eeprom_st * target) {
  target->flags = 0x00;
  target->target_temp = DEFAULT_TARGET_TEMP;
  target->timer_minutes = DEFAULT_TIMER_MINUTES;
}

uint8_t _crc(struct eeprom_st * data) {
  uint8_t crc = CRC_SALT; // not zero
  for(uint8_t i = 0; i < sizeof(struct eeprom_st) - 1; ++i) { // -1 = without crc
    crc += ((uint8_t*)data)[i];
  }
  return crc;
}
