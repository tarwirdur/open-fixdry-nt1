#ifndef __COMMON_H
#define __COMMON_H
#include "../config.h"
#ifndef __SDCC
  // hacks for clangd
  #define __CLANGD
  #define STM8S103
  #define __SDCC
  #define __SDCC_VERSION_MAJOR 4
  #define __trap
#endif
#include "stm8s.h"

#ifndef __CLANGD
  #include "../stm8s_it.h"    /* SDCC patch: required by SDCC for interrupts */
#endif

#endif