#ifndef __CONFIG_H
#define __CONFIG_H
#define VER_TM1639
// === GPIO ===

// display
#ifndef VER_TM1639
  #define PORT_DISP       GPIOA
  #define PIN_DISP_ST_CP  GPIO_PIN_2
  #define PIN_DISP_SER    GPIO_PIN_3
  #define PIN_DISP_SH_CP  GPIO_PIN_1
#endif
#ifdef VER_TM1639
  // TM1639
  #define PORT_TM1639_DIO GPIOB
  #define PORT_TM1639_CLK GPIOB
  #define PORT_TM1639_STB GPIOC
  #define PIN_TM1639_DIO GPIO_PIN_5
  #define PIN_TM1639_CLK GPIO_PIN_4
  #define PIN_TM1639_STB GPIO_PIN_7
#endif

// beeper
#ifndef VER_TM1639
  #define PORT_BEEPER     GPIOD
  #define PIN_BEEPER      GPIO_PIN_4
#endif
#ifdef VER_TM1639
  #define PORT_BEEPER   GPIOC
  #define PIN_BEEPER    GPIO_PIN_4
#endif

// heater relay
#ifndef VER_TM1639
  #define PORT_HEATER     GPIOC
  #define PIN_HEATER      GPIO_PIN_7
#endif

#ifdef VER_TM1639
  #define PORT_HEATER   GPIOD
  #define PIN_HEATER    GPIO_PIN_6
#endif

// fan
#ifndef VER_TM1639
  #define PORT_FAN        GPIOC
  #define PIN_FAN         GPIO_PIN_3
#endif
#ifdef VER_TM1639
  #define PORT_FAN      GPIOD
  #define PIN_FAN       GPIO_PIN_5
#endif
// DHT
#ifndef VER_TM1639
  #define PORT_DHT        GPIOC
  #define PIN_DHT_DATA    GPIO_PIN_5
  #define PIN_DHT_ENABLE  GPIO_PIN_6
#endif
#ifdef VER_TM1639
  #define PORT_DHT      GPIOD
  #define PIN_DHT_DATA  GPIO_PIN_1
#endif

// buttons
#ifndef VER_TM1639
  #define PORT_BTN_PWR    GPIOD
  #define PIN_BTN_PWR     GPIO_PIN_5
  #define PORT_BTN_MENU   GPIOD
  #define PIN_BTN_MENU    GPIO_PIN_6
  #define PORT_BTN_UP     GPIOB
  #define PIN_BTN_UP      GPIO_PIN_4
  #define PORT_BTN_DOWN   GPIOB
  #define PIN_BTN_DOWN    GPIO_PIN_5
#endif
#ifdef VER_TM1639
  #define PORT_BTN_PWR    GPIOA
  #define PIN_BTN_PWR     GPIO_PIN_3
  #define PORT_BTN_MENU   GPIOC
  #define PIN_BTN_MENU    GPIO_PIN_3
  #define PORT_BTN_UP     GPIOA
  #define PIN_BTN_UP      GPIO_PIN_2
  #define PORT_BTN_DOWN   GPIOA
  #define PIN_BTN_DOWN    GPIO_PIN_1
#endif

// === SETTINGS ===
#define ENABLE_BEEPER
#define MAX_DHT_ERROR_COUNT (5)
#define FAN_FORCE_COOLING_TIME_MS (30000)

#define BTN_CYCLES_PRESS  (5)
#define BTN_MIN_CYCLES_LONGPRESS (100)
#define BTN_LONG_PRESS_DIVIDER (10)

#define ENABLE_EEPROM
#define EEPROM_ADDR (uint32_t)(0x4005)

#define DEFAULT_TARGET_TEMP   (40)
#define DEFAULT_TIMER_MINUTES (4*60)

#define BEEP_LEN 50
#define BEEP_PAUSE 200

#endif
