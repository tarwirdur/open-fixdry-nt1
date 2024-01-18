#include "inc/clock.h"

uint16_t tick_counter = 0; // % 2000 = 1s
uint16_t _countdown_timers[CNT_MsDelayCountdownTimer]; // not necessary to be initilized
uint8_t _seconds_counter = 0;

void clock_tick() { // called each 500us by TIM2 OVF
  tick_counter = (tick_counter+1) % 2000;
  if (tick_counter == 0) {
    _seconds_counter++;
  }
  if (_seconds_counter < 2) {
    IWDG_ReloadCounter();
  }
  if (tick_counter % 2 == 1) return;
  for (uint8_t i=0; i < CNT_MsDelayCountdownTimer; ++i) {
    if (_countdown_timers[i] > 0) _countdown_timers[i]--;
  }
}

uint8_t reset_seconds_counter() {
  uint8_t s;
  disableInterrupts();
    s = _seconds_counter;
    _seconds_counter = 0;
  enableInterrupts();
  return s;
}

bool delay_ready(const MsDelayCountdownTimer timer) {
  disableInterrupts();
    bool status = (_countdown_timers[timer] == 0);
  enableInterrupts();
  return status;
}

void delay_ms_async(const MsDelayCountdownTimer timer, const uint16_t cnt) {
  disableInterrupts();
    _countdown_timers[timer] = cnt;
  enableInterrupts();
}

void delay_ms_blocking(const uint16_t cnt) {
  delay_ms_async(T_BLOCKING, cnt);
  while(! delay_ready(T_BLOCKING)) {}
}