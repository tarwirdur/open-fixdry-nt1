#include "inc/action.h"
#include "config.h"
#include "inc/button.h"
#include "inc/clock.h"
#include "inc/eeprom.h"
#include "inc/beeper.h"
#include "inc/state.h"
#include "inc/display.h"

void handle_user_interaction__settings() {
  btn_poll();
  if (button_state[BTN_PWR] & E_BTN_PRESS) {
    do_powerOff();
    return;
  }

  if (button_state[BTN_MENU] & E_BTN_RELEASE) {
    state.current_action = (state.current_action + 1) % CNT_SETACT;
    return;
  }

  if ((button_state[BTN_UP] & E_BTN_RELEASE) || (button_state[BTN_DOWN] & E_BTN_RELEASE)) {
    switch(state.current_action) {
    case SETACT_TEMPMODE:
      eeprom_data.flags ^= ROM_S_TEMP_MODE_NO_INTERPOLATE;
      break;
    case SETACT_SOUND:
      eeprom_data.flags ^= ROM_S_SOUND_DISABLED;
      break;
    case SETACT_KEYSOUND:
      eeprom_data.flags ^= ROM_S_KEY_SOUND_DISABLED;
      break;
    }
  }
}

void _beep_if_enabled() {
  if (~eeprom_data.flags & ROM_S_KEY_SOUND_DISABLED) {
    beep_async(1);
  }
}


void handle_user_interaction() {
    btn_poll();
    if (button_state[BTN_PWR] & E_BTN_PRESS) {
      if (state.flags & SF_PWR_ENABLED) {
        _beep_if_enabled();
        do_powerOff();
      } else {
        if (GPIO_ReadInputPin(PORT_BTN_MENU, PIN_BTN_MENU) == 0) {
          do_settingsOn();
        } else {
          _beep_if_enabled();
          do_powerOn();
        }
      }
      return;
    }
    if (~state.flags & SF_PWR_ENABLED) { // powered off
      return;
    }
    if (~state.flags & SF_DISPLAY_ENABLED) { // display off mode
      if (
           (button_state[BTN_UP]   & E_BTN_RELEASE)
        || (button_state[BTN_DOWN] & E_BTN_RELEASE)
        || (button_state[BTN_MENU] & E_BTN_RELEASE)
      ) {
        _beep_if_enabled();
        enableDisplay();
      }
      return;
    }

    if (button_state[BTN_MENU] & E_BTN_LONG_PRESS) {
      _beep_if_enabled();
      disableDisplay();
      return;
    }

    if (button_state[BTN_MENU] & E_BTN_RELEASE) {
      _beep_if_enabled();
      if (state.flags & SF_COUNTDOWN_STOPPED) { // restart countdown if disabled
        do_startCountdown();
      }
      state.current_action = (state.current_action + 1) % CNT_ACT;
      delay_ms_async(T_MODE_INACTIVE, 5000);
      return;
    }

    if (button_state[BTN_UP] & (E_BTN_RELEASE | E_BTN_LONG_TRIGGER))
      do_change(UP);
    if (button_state[BTN_DOWN] & (E_BTN_RELEASE | E_BTN_LONG_TRIGGER))
      do_change(DOWN);
}

void do_startCountdown() {
  state.flags &= ~SF_COUNTDOWN_STOPPED;
  state.minutes = eeprom_data.timer_minutes;
  if (state.minutes == 0) {
    state.minutes = 1;
  }
  state.seconds = 59;
}

void do_stopCountdown() {
  state.flags |= SF_COUNTDOWN_STOPPED;
  GPIO_WriteLow(PORT_HEATER, PIN_HEATER);
  delay_ms_async(T_FAN_OFF, FAN_FORCE_COOLING_TIME_MS);
}

void do_powerOff() {
  state.flags = 0;
  clear_screen();
  #ifndef VER_TM1639
    GPIO_WriteLow(PORT_DHT, PIN_DHT_ENABLE);
    GPIO_WriteLow(PORT_DHT, PIN_DHT_DATA);
  #endif
  do_stopCountdown();
  eeprom_save();
}

void do_powerOn() {
  state.flags = SF_PWR_ENABLED;
  state.dht_err_count = MAX_DHT_ERROR_COUNT;
  eeprom_load();
  do_startCountdown();
  enableDisplay();
}

void do_settingsOn() {
  state.flags = SF_SETTINGS_MODE | SF_PWR_ENABLED;
  eeprom_load();
  enableDisplay();
}

void enableDisplay() {
  state.flags |= (SF_DISPLAY_ENABLED);
}

void disableDisplay() {
  state.flags &= ~(SF_DISPLAY_ENABLED);
  state.current_action = ACT_DISPLAY;
  clear_screen();
}

void do_change(ChangeDirection dir) {
  _beep_if_enabled();
  delay_ms_async(T_MODE_INACTIVE, 5000);
  uint8_t minutes = state.minutes % 60;
  uint8_t hours = state.minutes / 60;
  switch (state.current_action) {
    case ACT_DISPLAY:
      return;
    case ACT_SET_TARGET_TEMP:
      if (dir == DOWN) {
        if (eeprom_data.target_temp > 0)
            eeprom_data.target_temp--;
      } else {
        if (eeprom_data.target_temp < 90)
            eeprom_data.target_temp++;
      }
      break;
    case ACT_SET_HOUR:
      state.seconds = 59;
      if (dir == DOWN) {
        if (hours > 0) hours--;
        else hours = 99;
      } else {
        if (hours < 99) hours++;
        else hours = 0;
      }
      state.minutes = hours * 60 + minutes;
      eeprom_data.timer_minutes = state.minutes;
      break;
    case ACT_SET_MINUTE:
      state.seconds = 59;
      if (dir == DOWN) {
        if (minutes > 0) minutes--;
        else minutes = 59;
      } else {
        if (minutes < 59) minutes++;
        else minutes = 0;
      }
      state.minutes = hours * 60 + minutes;
      eeprom_data.timer_minutes = state.minutes;
      break;
  }
}
