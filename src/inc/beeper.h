#ifndef __BEEPER_H
#define __BEEPER_H
#include "common.h"

void beep_poll();
void beep_async(const uint8_t cnt);

#endif