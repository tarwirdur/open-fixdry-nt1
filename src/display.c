#include "inc/display.h"
#include "config.h"
#include "inc/eeprom.h"
#include "inc/state.h"
#include "inc/clock.h"
#include "inc/action.h"
#include "stm8s_gpio.h"

#ifndef VER_TM1639
const uint8_t seg8_chars[12] = {
  0b00000011 /* 0 */,      0b10011111 /* 1 */,
  0b00100101 /* 2 */,      0b00001101 /* 3 */,
  0b10011001 /* 4 */,      0b01001001 /* 5 */,
  0b01000001 /* 6 */,      0b00011111 /* 7 */,
  0b00000001 /* 8 */,      0b00001001 /* 9 */,
  0b11111111 /* empty */, 0b11111100 /* - */,
};
#endif
#ifdef VER_TM1639
const uint8_t seg8_chars[12] = {
  0b11000000 /* 0 */,      0b11111001 /* 1 */,
  0b10100100 /* 2 */,      0b10110000 /* 3 */,
  0b10011001 /* 4 */,      0b10010010 /* 5 */,
  0b10000010 /* 6 */,      0b11111000 /* 7 */,
  0b10000000 /* 8 */,      0b10010000 /* 9 */,
  0b11111111 /* empty */, 0b00111111 /* - */,
};
#endif

void _fill(const uint8_t start_pos, const uint8_t count, const uint8_t seg);
void _two_digits(const uint8_t start_pos, const uint8_t data);
void _shift(const uint8_t b);

void update_display_buffer() {
  switch(state.current_action) {
    case ACT_DISPLAY:
      state.seg_highlight = 0;
      if (state.dht_err_count >= MAX_DHT_ERROR_COUNT) {
        state.seg_highlight = 0b00001111;
      }
      break;
    case ACT_SET_TARGET_TEMP:
      state.seg_highlight = 0b00001100;
      break;
    case ACT_SET_HOUR:
      state.seg_highlight = 0b11000000;
      break;
    case ACT_SET_MINUTE:
      state.seg_highlight = 0b00110000;
      break;
  }

  uint8_t hours = state.minutes / 60;
  uint8_t minutes = state.minutes % 60;

  if (state.dht_err_count < MAX_DHT_ERROR_COUNT) {
    _two_digits(2, state.temp);
    _two_digits(0, state.humidity);
  } else {
    _fill(0, 4, seg8_chars[11]);
  }

  if (state.current_action == ACT_SET_TARGET_TEMP) {
    _two_digits(2, eeprom_data.target_temp);
  }

  if (state.flags & SF_COUNTDOWN_STOPPED) {
    _fill(4, 4, seg8_chars[11]);
  } else {
    if (state.minutes == 1 && state.current_action == ACT_DISPLAY) { // display seconds on last minute
      _two_digits(4, state.seconds);
      _fill(6, 2, seg8_chars[10]);
    } else {
      _two_digits(4, minutes);
      _two_digits(6, hours);
    }
  }
  #ifndef VER_TM1639
    state.display[4] &= ~(1<<0); // ":" between hh and mm
  #else
    state.display[6] &= ~(1<<7); // ":" between hh and mm
  #endif
}

void update_display_buffer__settings() {
  switch(state.current_action) {
    case SETACT_TEMPMODE:
      state.seg_highlight = 0b00000100;
      break;
    case SETACT_SOUND:
      state.seg_highlight = 0b00000010;
      break;
    case SETACT_KEYSOUND:
      state.seg_highlight = 0b00000001;
      break;
  }

  _fill(0, 8, seg8_chars[10]); // clear all

  uint8_t soundmode = 11;
  if (eeprom_data.flags & ROM_S_SOUND_DISABLED) {
    soundmode -= 10;
  }
  if (eeprom_data.flags & ROM_S_KEY_SOUND_DISABLED) {
    soundmode -= 1;
  }
  _two_digits(0, soundmode);

  if (eeprom_data.flags & ROM_S_TEMP_MODE_NO_INTERPOLATE) {
    state.display[2] = seg8_chars[0];
  } else {
    state.display[2] = seg8_chars[1];
  }
}

void _two_digits(const uint8_t start_pos, const uint8_t data) {
  state.display[start_pos] = seg8_chars[data % 10];
  state.display[start_pos+1] = seg8_chars[data / 10 % 10];
}

void _fill(const uint8_t start_pos, const uint8_t count, const uint8_t seg) {
  for(uint8_t i = 0; i<count; ++i)
    state.display[start_pos+i] = seg;
}

#ifndef VER_TM1639
  uint16_t _current_sign = 0;
  void show_next_sign() {
    uint8_t data;
    if ((tick_counter / 32) % 16 > 10 && (state.seg_highlight & 1<<_current_sign)) { // blink on highlight
      data = 0xFF;
    } else {
      data = state.display[_current_sign];
    }
    // send next display symbol
    GPIO_WriteLow(PORT_DISP, PIN_DISP_SH_CP);
    _shift(data);
    _shift(1<<_current_sign);
    GPIO_WriteHigh(PORT_DISP, PIN_DISP_SH_CP);
    _current_sign = (_current_sign + 1) % 8;
  }

  void clear_screen() {
    GPIO_WriteLow(PORT_DISP, PIN_DISP_SH_CP);
    _shift(0xFF);
    _shift(0x00);
    GPIO_WriteHigh(PORT_DISP, PIN_DISP_SH_CP);
  }

  void _shift(const uint8_t b) {
    static uint8_t i;
    for(i = 0; i<8; ++i) {
      if (b & 1<<i) {
        GPIO_WriteHigh(PORT_DISP, PIN_DISP_SER);
      } else {
        GPIO_WriteLow(PORT_DISP, PIN_DISP_SER);
      }
      GPIO_WriteHigh(PORT_DISP, PIN_DISP_ST_CP);
      GPIO_WriteLow(PORT_DISP, PIN_DISP_ST_CP);
    }
  }
#endif

#ifdef VER_TM1639
  void tm1639_sendbyte(const uint8_t b);
  void tm1639_sendcmd(const uint8_t b);

  void clear_screen() {
    tm1639_sendcmd(0b10000000);
  }

  void show_next_sign() {
  }


  void tm1639_sendbyte(const uint8_t b) {
    static uint8_t i;
    for(i = 0; i<8; ++i) {
      GPIO_WriteLow(PORT_TM1639_CLK, PIN_TM1639_CLK);
      if (b & 1<<i) {
        GPIO_WriteHigh(PORT_TM1639_DIO, PIN_TM1639_DIO);
      } else {
        GPIO_WriteLow(PORT_TM1639_DIO, PIN_TM1639_DIO);
      }
      GPIO_WriteHigh(PORT_TM1639_CLK, PIN_TM1639_CLK);
    }
  }
  void tm1639_sendcmd(const uint8_t b) {
    GPIO_WriteLow(PORT_TM1639_STB, PIN_TM1639_STB);
    tm1639_sendbyte(b);
    GPIO_WriteHigh(PORT_TM1639_STB, PIN_TM1639_STB);
  }

  void tm1639_sendcmd2(const uint8_t b1, const uint8_t b2) {
    GPIO_WriteLow(PORT_TM1639_STB, PIN_TM1639_STB);
    tm1639_sendbyte(b1);
    tm1639_sendbyte(b2);
    GPIO_WriteHigh(PORT_TM1639_STB, PIN_TM1639_STB);
  }

  uint8_t _blink_state  = 1;
  void tm1639_sendData() {
    if (delay_ready(T_BLINK)) {
      delay_ms_async(T_BLINK, (_blink_state == 1) ? 200 : 100);
      _blink_state = ! _blink_state;
    }
    for(int sign = 0; sign < 8; sign++) {
      uint8_t data = state.display[7-sign];
      data = ~data;
      if (state.seg_highlight & 1<<(7-sign) && _blink_state) {
        data = 0;
      }
      tm1639_sendcmd2(0b11000000+sign*2, data  & 0x0F );
      tm1639_sendcmd2(0b11000000+sign*2+1, data >> 4);
      tm1639_sendcmd(0x40);
      tm1639_sendcmd(0b10001010);
    }
  }


#endif