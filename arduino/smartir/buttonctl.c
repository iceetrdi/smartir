#include "stdbool.h"
#include "Arduino.h"
#include "buttonctl.h"

inline void buttonctl_begin(__xdata buttonctl_button* button, uint8_t pin){
  pinMode(pin, INPUT_PULLUP);
  button->pin = pin;
  button->longPushDuration_ms = 1000;
  buttonctl_clear(button);
}

inline void buttonctl_clear(__xdata buttonctl_button* button){
  button->pushing = false;
  button->pushed = false;
  button->longPushing = false;
  button->longPushed = false;
}

void buttonctl_update(__xdata buttonctl_button* button){
  unsigned long now_ms = millis();
  if(!digitalRead(button->pin)){ // ボタンが押されている状態
    if(!button->pushing){ // ボタン押下を検知
      button->pushing = true;
      button->pushtime_ms = now_ms;
    }
    if (now_ms - button->pushtime_ms > button->longPushDuration_ms){ // ボタン長押しを検知
      button->longPushing = true;
    }
  } else { // ボタンが押されていない状態
    if(button->pushing){
      button->pushing = false;
      button->pushed = true;
    }
    if(button->longPushing){
      button->longPushing = false;
      button->longPushed = true;
    }
  }
}

inline bool buttonctl_isPushing(__xdata buttonctl_button* button){ return button->pushing; }

inline bool buttonctl_wasPushed(__xdata buttonctl_button* button){ return button->pushed; }

inline bool buttonctl_isLongPushing(__xdata buttonctl_button* button){ return button->longPushing; }

inline bool buttonctl_wasLongPushed(__xdata buttonctl_button* button){ return button->longPushed; }
