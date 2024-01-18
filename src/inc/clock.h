#ifndef __CLOCK_H
#define __CLOCK_H
#include "common.h"

typedef enum {
  T_BLOCKING,
  T_FAN_OFF,
  T_MODE_INACTIVE,
  T_DHT,
  T_BEEPER,
  CNT_MsDelayCountdownTimer
} MsDelayCountdownTimer;

extern uint16_t tick_counter;
void delay_ms_blocking(const uint16_t cnt);
bool delay_ready(const MsDelayCountdownTimer timer);
uint8_t reset_seconds_counter();
void delay_ms_async(const MsDelayCountdownTimer timer, const uint16_t cnt);
void clock_tick();

#endif