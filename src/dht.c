#include "inc/dht.h"
#include "inc/clock.h"
struct dht_st dht;

void dht_init_tim4() {
  TIM4_DeInit();
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);
  TIM4_TimeBaseInit(TIM4_PRESCALER_16, 255); // each 1us, 255us timeout
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  TIM4_Cmd(ENABLE);
}

void dht_start() {
  #ifndef VER_TM1639
    GPIO_Init(PORT_DHT, PIN_DHT_ENABLE, GPIO_MODE_OUT_PP_HIGH_FAST);
  #endif
  GPIO_Init(PORT_DHT, PIN_DHT_DATA, GPIO_MODE_OUT_PP_HIGH_FAST);
  dht.flags = DHT_PENDING;
  for (uint8_t i = 0; i < 5; ++i) dht.data[i] = 0;
  GPIO_WriteLow(PORT_DHT, PIN_DHT_DATA);
  delay_ms_blocking(18);
  GPIO_WriteHigh(PORT_DHT, PIN_DHT_DATA);
  disableInterrupts();
    dht.pos = 0xFE;
    GPIO_Init(PORT_DHT, PIN_DHT_DATA, GPIO_MODE_IN_PU_IT);
    ITC_SetSoftwarePriority(ITC_IRQ_PORTD, ITC_PRIORITYLEVEL_3);
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_ONLY);
    dht_init_tim4();
  enableInterrupts();
  delay_ms_blocking(10); // wait for data
}

void dht_stop() {
  TIM4_Cmd(DISABLE);
  if (! (dht.flags & DHT_CRC_ERR) && ! (dht.flags & DHT_TIMEOUT_ERR) ) {
    dht.flags |= DHT_OK;
  }
  dht.flags &= ~DHT_PENDING;
  GPIO_Init(PORT_DHT, PIN_DHT_DATA, GPIO_MODE_OUT_PP_HIGH_FAST);
}

void onTimer4_OVF() { // dht timeout
  dht.flags |= DHT_TIMEOUT_ERR;
  dht_stop();
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
}

/*
Start:
 MCU: 18ms (LOW) + 40us (HIGH)
 DHT11: 80us (LOW) + 80us (HIGH)

 Data (40 bit) each bit:
 DHT11: 50us (LOW) + (0: 27us HIGH | 1: 70us HIGH)
*/
void irq_gpio_dht() {
  uint8_t t = TIM4_GetCounter();
  if (t < 50) return; // maybe on first
  TIM4_SetCounter(0);
  if (dht.pos++ > 39) return; // note: start at 0xFE
  if (t > 100)
    dht.data[dht.pos/8] |= 1 << (7 - dht.pos % 8);
  if (dht.pos == 39) {
    uint8_t crc = dht.data[0]+dht.data[1]+dht.data[2]+dht.data[3];
    if (crc != dht.data[4] || (dht.data[0] == 0 && dht.data[2]==0)) {
      dht.flags |= DHT_CRC_ERR;
    }
    dht_stop();
  }
}