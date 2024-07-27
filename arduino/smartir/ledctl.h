#ifndef LEDCTL_H
#define LEDCTL_H

#include <Arduino.h>

typedef enum enum_ledctl_state {
  LEDCTL_OFF,
  LEDCTL_ON,
  LEDCTL_ON_UNTIL
} ledctl_state;

typedef struct struct_ledctl_led {
  uint8_t pin;
  ledctl_state state;
  unsigned long duration_ms;
  unsigned long changed_ms;
} ledctl_led;

void ledctl_begin(ledctl_led* led, uint8_t pin);
void ledctl_setduration(ledctl_led* led, unsigned long duration_ms);
void ledctl_write(ledctl_led* led, ledctl_state state);
void ledctl_update(ledctl_led* led);

#endif