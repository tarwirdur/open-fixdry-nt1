#include "inc/beeper.h"
#include "inc/clock.h"
#include "inc/state.h"

uint8_t _beep_cnt = 0;
uint8_t _beep_state = 0;

void beep_poll() {
  #ifdef ENABLE_BEEPER
  if(delay_ready(T_BEEPER) ) {
    if (_beep_cnt > 0 && _beep_state == 0) {
      _beep_state = 1;
      _beep_cnt--;
      if (state.flags & SF_PWR_ENABLED)
        GPIO_WriteHigh(PORT_BEEPER, PIN_BEEPER);
      delay_ms_async(T_BEEPER, BEEP_LEN);
    } else {
      _beep_state = 0;
      GPIO_WriteLow(PORT_BEEPER, PIN_BEEPER);
      if (_beep_cnt > 0)
        delay_ms_async(T_BEEPER, BEEP_PAUSE);
    }
  }
  #endif
}

void beep_async(const uint8_t cnt) {
    _beep_cnt += cnt;
}