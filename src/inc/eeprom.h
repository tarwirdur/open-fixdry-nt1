#ifndef __EEPROM_H
#define __EEPROM_H
#include "common.h"

#define ROM_S_TEMP_MODE_NO_INTERPOLATE (1<<0)
#define ROM_S_SOUND_DISABLED (1<<1)
#define ROM_S_KEY_SOUND_DISABLED (1<<2)

struct eeprom_st {
  uint8_t flags;
  uint8_t target_temp;
  uint16_t timer_minutes;
  uint8_t crc;
};

extern struct eeprom_st eeprom_data;

void eeprom_save();
void eeprom_load();

#endif