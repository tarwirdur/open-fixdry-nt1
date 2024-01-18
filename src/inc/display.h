#ifndef __DISPLAY_H
#define __DISPLAY_H
#include "common.h"

extern const uint8_t seg8_chars[12];
void update_display_buffer();
void update_display_buffer__settings();
void clear_screen();
void show_next_sign();

#endif