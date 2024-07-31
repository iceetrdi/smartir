#include <stdint.h>
#ifndef BUTTONCTL_H
#define BUTTONCTL_H

#include <Arduino.h>

typedef struct struct_buttonctl_button {
  uint8_t pin;
  bool pushing;
  bool pushed;
  bool longPushing;
  bool longPushed;
  unsigned long longPushDuration_ms;
  unsigned long pushtime_ms;
} buttonctl_button;

inline void buttonctl_begin(buttonctl_button* button, uint8_t pin);
inline void buttonctl_clear(buttonctl_button* button);
void buttonctl_update(buttonctl_button* button);
inline bool buttonctl_isPushing(buttonctl_button* button);
inline bool buttonctl_wasPushed(buttonctl_button* button);
inline bool buttonctl_isLongPushing(buttonctl_button* button);
inline bool buttonctl_wasLongPushed(buttonctl_button* button);

#endif