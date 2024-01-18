#include "inc/button.h"

uint8_t button_state[CNT_BTN];
uint8_t cycles_press[CNT_BTN];
uint8_t cycles_release[CNT_BTN];
uint8_t poll_counter = 0;

void btn_init() {
  for(uint8_t i=0; i<CNT_BTN; ++i) {
    button_state[i]=0;
    cycles_press[i]=0;
    cycles_release[i]=0;
  }
  GPIO_Init(PORT_BTN_PWR , PIN_BTN_PWR , GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(PORT_BTN_MENU, PIN_BTN_MENU, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(PORT_BTN_UP  , PIN_BTN_UP  , GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(PORT_BTN_DOWN, PIN_BTN_DOWN, GPIO_MODE_IN_PU_NO_IT);
}

void _btn_poll(uint8_t i, GPIO_TypeDef *port, GPIO_Pin_TypeDef pin) {
  button_state[i] = 0;
  uint8_t is_pressed = GPIO_ReadInputPin(port, pin) == 0;
  if (is_pressed) {
    if (cycles_press[i] < 255)
      cycles_press[i]++;
    if (cycles_press[i] >= BTN_CYCLES_PRESS)
      cycles_release[i] = 0;
    if (cycles_press[i] == BTN_CYCLES_PRESS)
      button_state[i] |= E_BTN_PRESS;
    if (cycles_press[i] >= BTN_MIN_CYCLES_LONGPRESS) {
      button_state[i] |= E_BTN_LONG_PRESS;
      if (poll_counter == 0)
        button_state[i] |= E_BTN_LONG_TRIGGER;
    }
  } else {
    if (cycles_release[i] < 255)
      cycles_release[i]++;
    if (cycles_release[i] >= BTN_CYCLES_PRESS) {
      if (cycles_press[i] >= BTN_CYCLES_PRESS) {
        if (cycles_press[i] < BTN_MIN_CYCLES_LONGPRESS) {
          button_state[i] |= E_BTN_RELEASE;
        } else {
          button_state[i] |= E_BTN_LONG_RELEASE;
        }
      }
      cycles_press[i] = 0;
    }
  }
}

void btn_poll() {
  poll_counter = (poll_counter + 1) % BTN_LONG_PRESS_DIVIDER;
  _btn_poll(BTN_PWR , PORT_BTN_PWR , PIN_BTN_PWR);
  _btn_poll(BTN_MENU, PORT_BTN_MENU, PIN_BTN_MENU);
  _btn_poll(BTN_UP  , PORT_BTN_UP  , PIN_BTN_UP);
  _btn_poll(BTN_DOWN, PORT_BTN_DOWN, PIN_BTN_DOWN);
}
