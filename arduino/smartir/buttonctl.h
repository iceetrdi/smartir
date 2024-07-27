#include <stdint.h>
#ifndef BUTTONCTL_H
#define BUTTONCTL_H

#include <Arduino.h>

typedef struct struct_buttonctl_button {
  uint8_t pin;
  bool push;
  bool pushed;
  bool longPushed;
  unsigned long longPushDuration_ms;
  unsigned long changed_ms;
} buttonctl_button;

void buttonctl_begin(buttonctl_button* button, uint8_t pin);
void buttonctl_update(buttonctl_button* button);
bool buttonctl_isPush(buttonctl_button* button);
bool buttonctl_wasPushed(buttonctl_button* button);

#endif