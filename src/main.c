#include "config.h"
#include "inc/action.h"
#include "inc/dht.h"
#include "inc/button.h"
#include "inc/display.h"
#include "inc/eeprom.h"
#include "inc/beeper.h"
#include "inc/clock.h"
#include "inc/state.h"

/*
 *   Display: 2x 74hc595: 1st byte - segments (unset bit to pwr) , 2nd - digit number (set bit to pwr)
 *   Digits: 0,1 - humidity; 2,3 - temp; 4,5 minutes; 6,7 - hours
 *                        +-------------------+
 *         Q_BEEPER - PD4 | 1               20| PD3 - ?
 *         BTN_PWR  - PD5 | 2               19| PD2 - "DIO" on H1
 *         BTN_MENU - PD6 | 3               18| PD1 - SWIM, "CLK" on H1
 *                  - RST | 4   STM8S103F3  17| PC7 - Q_HEATER_RELAY
 *         D_SH_CP  - PA1 | 5               16| PC6 - Q_DHT11
 *         D_ST_CP  - PA2 | 6               15| PC5 - DHT11_DATA
 *                  - GND | 7    TSSOP20    14| PC4 - ?
 *                  - VCAP| 8               13| PC3 - Q_FAN
 *                  - VCC | 9               12| PB4 - BTN_UP
 *         D_SER    - PA3 |10               11| PB5 - BTN_DOWN
 *                        +-------------------+
 *
 *
 * Timers:
 *   TIM1 - unused
 *   TIM2 - OVF every 0.5ms. Display update, clock/delay
 *   TIM4 - timer for DHT11
 */

void settings_loop();
void normal_loop();
struct state_st state;
void init();
void iwdg_config();

int main(void)
{
  init();
  eeprom_load();
  do_powerOff();
  enableInterrupts();
  while(1) {
    if (state.flags & SF_SETTINGS_MODE) // power off, then press BTN_PWR+BTN_MODE
      settings_loop();
    else
      normal_loop();
  }
}

uint8_t interpolate_temp() { // 40 -> 40, 48 -> 70
  if (dht.data[2] < 40)
    return dht.data[2];
  uint16_t t = (dht.data[2]-40)*10 + dht.data[3];
  t = t * 43 / 100 + 40;
  if (t > 99) {
    t = 99;
  }
  return t;
}

void settings_loop() {
  reset_seconds_counter();
  handle_user_interaction__settings();
  update_display_buffer__settings();
  GPIO_WriteLow(PORT_HEATER, PIN_HEATER);
  delay_ms_blocking(10);
}

void normal_loop() {
  handle_user_interaction();
  beep_poll();

  if (reset_seconds_counter() > 0) {
    // each second. Dec countdown if enabled and not in time editing mode
    if (state.current_action != ACT_SET_HOUR
        && state.current_action != ACT_SET_MINUTE
        && ~state.flags & SF_COUNTDOWN_STOPPED) {
      state.seconds--;
      if ((state.seconds == 0 && state.minutes == 1) || state.minutes == 0) {
        state.minutes = 0;
        do_stopCountdown();
        if (~eeprom_data.flags & ROM_S_SOUND_DISABLED)
          beep_async(3);
      } else if (state.seconds == 0xFF) {
        state.seconds = 59;
        state.minutes--;
      }
    }
  }

  if (delay_ready(T_DHT) && (state.flags & T_DHT)) {
    delay_ms_async(T_DHT, 1000);
    dht_start();
    if (dht.flags & DHT_OK) {
      state.humidity = dht.data[0];
      if (eeprom_data.flags & ROM_S_TEMP_MODE_NO_INTERPOLATE) {
        state.temp = dht.data[2];
      } else {
        state.temp = interpolate_temp();
      }
      state.dht_err_count = 0;
    } else {
      if (state.dht_err_count < MAX_DHT_ERROR_COUNT) {
        state.dht_err_count++;
      }
    }
  }

  if (
      (~state.flags & SF_COUNTDOWN_STOPPED)
    && (state.flags & SF_PWR_ENABLED)
    && state.dht_err_count < MAX_DHT_ERROR_COUNT) { // timer active, power enabled, temp ok
    if (state.temp > eeprom_data.target_temp) {
      GPIO_WriteLow(PORT_HEATER, PIN_HEATER);
    } else {
      GPIO_WriteHigh(PORT_HEATER, PIN_HEATER);
      GPIO_WriteHigh(PORT_FAN, PIN_FAN);
    }
  } else {
    GPIO_WriteLow(PORT_HEATER, PIN_HEATER);
  }

  if (delay_ready(T_FAN_OFF) && state.flags & SF_COUNTDOWN_STOPPED) {
    GPIO_WriteLow(PORT_FAN, PIN_FAN);
  }

  if (delay_ready(T_MODE_INACTIVE)) { // return to main activity on idle
    state.current_action = ACT_DISPLAY;
  }

  update_display_buffer();
  delay_ms_blocking(10);
}

void init() {
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

  TIM2_DeInit();
  TIM4_DeInit();
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
  TIM2_TimeBaseInit(TIM2_PRESCALER_8, 999); // each 0.5uS, overflow each 500uS
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
  TIM2_Cmd(ENABLE);
  ITC_SetSoftwarePriority(ITC_IRQ_TIM2_OVF, ITC_PRIORITYLEVEL_2);

  GPIO_Init(PORT_DISP, PIN_DISP_ST_CP | PIN_DISP_SER | PIN_DISP_SH_CP, GPIO_MODE_OUT_PP_LOW_FAST);

  GPIO_Init(PORT_BEEPER, PIN_BEEPER, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(PORT_HEATER, PIN_HEATER, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(PORT_FAN, PIN_FAN, GPIO_MODE_OUT_PP_LOW_FAST);

  btn_init();
  iwdg_config();
}

void onTimer2_OVF() { // each 500us
  clock_tick();
  if (state.flags & SF_DISPLAY_ENABLED)
    show_next_sign();
  TIM2_ClearFlag(TIM2_FLAG_UPDATE);
}

void iwdg_config() {
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(IWDG_Prescaler_256);
  IWDG_SetReload(255);
  IWDG_Enable();
  IWDG_ReloadCounter(); // see clock.c: clock_tick, reset_seconds_counter
}