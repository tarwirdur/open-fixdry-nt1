#ifndef __BUTTON_H
#define __BUTTON_H
#include "common.h"

typedef enum {
  E_BTN_PRESS        = (1<<0),
  E_BTN_LONG_PRESS   = (1<<1),
  E_BTN_RELEASE      = (1<<2),
  E_BTN_LONG_RELEASE = (1<<3),
  E_BTN_LONG_TRIGGER = (1<<4)
} BtnEvtTypeMask;

typedef enum {
  BTN_PWR,
  BTN_MENU,
  BTN_UP,
  BTN_DOWN,
  CNT_BTN
} BtnNum;

extern uint8_t button_state[CNT_BTN];

void btn_init();
void btn_poll();

#endif