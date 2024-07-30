#include "ledctl.h"

void ledctl_begin(ledctl_led* led, uint8_t pin){
  led->pin = pin;
  led->state = LEDCTL_OFF;
  led->duration_ms = 1000;
  led->changed_ms = millis();
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void ledctl_setduration(ledctl_led* led, unsigned long duration_ms){
  led->duration_ms = duration_ms;
}

void ledctl_write(ledctl_led* led, ledctl_state state){
  led->state = state;
  led->changed_ms = millis();

  if(state == LEDCTL_OFF){
    digitalWrite(led->pin, LOW);
  } else if (state == LEDCTL_ON || state == LEDCTL_ON_UNTIL){
    digitalWrite(led->pin, HIGH);
  }
}

void ledctl_update(ledctl_led* led){
  switch(led->state){
    case LEDCTL_OFF:
    digitalWrite(led->pin, LOW);
    break;

    case LEDCTL_ON:
    digitalWrite(led->pin, HIGH);
    break;

    case LEDCTL_ON_UNTIL:
    if(millis() - led->changed_ms > led->duration_ms){
      digitalWrite(led->pin, LOW);
    }else{
      digitalWrite(led->pin, HIGH);
    }
    break;

    default:
    digitalWrite(led->pin, LOW);
  }
}