#ifndef __STATE_H
#define __STATE_H
#include "common.h"

typedef enum  {
  SF_PWR_ENABLED = (1<<0),
  SF_SETTINGS_MODE = (1<<1),
  SF_DISPLAY_ENABLED = (1<<2),
  SF_COUNTDOWN_STOPPED = (1<<3)
} StateFlag_bitmask;

struct state_st {
  uint8_t flags;
  uint8_t display[8];
  uint8_t seg_highlight;
  uint8_t temp;
  uint8_t humidity;
  uint16_t minutes;
  uint8_t seconds;
  uint8_t dht_err_count;
  uint8_t current_action;
};
extern struct state_st state;

#endif