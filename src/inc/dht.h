#ifndef __DHT_H
#define __DHT_H

#include "common.h"

typedef enum {
  DHT_PENDING     = (1<<0),
  DHT_OK          = (1<<1),
  DHT_CRC_ERR     = (1<<2),
  DHT_TIMEOUT_ERR = (1<<3)
} DhtFlagMask;

struct dht_st {
  uint8_t flags;
  uint8_t pos;
  uint8_t data[5];
};
extern struct dht_st dht;

void dht_init_tim4();
void dht_start();
void onTimer4_OVF();
void irq_gpio_dht();

#endif