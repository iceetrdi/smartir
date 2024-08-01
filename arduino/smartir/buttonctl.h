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

inline void buttonctl_begin(__xdata buttonctl_button* button, uint8_t pin);
inline void buttonctl_clear(__xdata buttonctl_button* button);
void buttonctl_update(__xdata buttonctl_button* button);
inline bool buttonctl_isPushing(__xdata buttonctl_button* button);
inline bool buttonctl_wasPushed(__xdata buttonctl_button* button);
inline bool buttonctl_isLongPushing(__xdata buttonctl_button* button);
inline bool buttonctl_wasLongPushed(__xdata buttonctl_button* button);

#endif