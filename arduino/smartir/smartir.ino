#include <Serial.h>
#include "ch552ir.h"
#include "buttonctl.h"
#include "ledctl.h"

ledctl_led led;
buttonctl_button button;
ch552ir_data irData;

void setup(){
  ledctl_begin(&led, 30);
  buttonctl_begin(&button, 31);
  irData.datalength = 0;
}

void loop(){
  ledctl_update(&led);
}
