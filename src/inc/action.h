#ifndef __ACTION_H
#define __ACTION_H
#include "common.h"

typedef enum {DOWN = 0, UP = !DOWN} ChangeDirection;

typedef enum {
  ACT_DISPLAY,
  ACT_SET_TARGET_TEMP,
  ACT_SET_HOUR,
  ACT_SET_MINUTE,
  CNT_ACT
} ActionType;

typedef enum {
  SETACT_TEMPMODE,
  SETACT_SOUND,
  SETACT_KEYSOUND,
  CNT_SETACT
} SettingsActionType;

void handle_user_interaction();
void handle_user_interaction__settings();
void do_powerOn();
void do_settingsOn();
void do_powerOff();
void do_change(ChangeDirection dir);
void do_startCountdown();
void do_stopCountdown();
void enableDisplay();
void disableDisplay();

#endif