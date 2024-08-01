#include "ch552ir.h"
#include "buttonctl.h"
#include "ledctl.h"

__xdata ledctl_led led;
__xdata buttonctl_button button;
__xdata ch552ir_data irData;
__xdata ch552ir_data irSaveData;

inline void ch552ir_data_test(__xdata ch552ir_data* data);

void setup(){
  ch552ir_begin();
  ch552ir_dataInit(&irData);
  ledctl_begin(&led, 30);
  buttonctl_begin(&button, 32);
  pinMode(15, OUTPUT);
  digitalWrite(15, LOW);
}

void loop(){
  buttonctl_update(&button);

  if(ch552ir_available()){
    ch552ir_read(&irData);
    ch552ir_data_test(&irData);
    ch552ir_flush();
  }

  if(buttonctl_isLongPushing(&button)){
    ledctl_setPeriod(&led, 200);
    ledctl_write(&led, LEDCTL_BLINK);
    ch552ir_flush();
    while (!ch552ir_available()) {
      ledctl_update(&led);
    }
    ledctl_setPeriod(&led, 1000);
    ledctl_write(&led, LEDCTL_ON_UNTIL);
    ledctl_update(&led);
    ch552ir_read(&irSaveData);
    ch552ir_data_test(&irSaveData);
    buttonctl_clear(&button);
  }

  if(buttonctl_wasPushed(&button)){
    buttonctl_clear(&button);
    ledctl_setPeriod(&led, 500);
    ledctl_write(&led, LEDCTL_ON_UNTIL);
    ledctl_update(&led);
    ch552ir_write(&irSaveData);
    ch552ir_data_test(&irSaveData);
  }

  ledctl_update(&led);
}


inline void ch552ir_data_test(__xdata ch552ir_data* irdata){
    if(irdata->format == CH552IR_FORMAT_NEC){
      USBSerial_print("NEC,T=");
      USBSerial_print(irdata->t);
      for (uint8_t i = 0; i < irdata->datalength; i++) {
        USBSerial_print(",");
        USBSerial_print(irdata->data[i]);
      }
      USBSerial_println("");
    } else if(irdata->format == CH552IR_FORMAT_AEHA){
      USBSerial_print("AEHA,T=");
      USBSerial_print(irdata->t);
      for (uint8_t i = 0; i < irdata->datalength; i++) {
        USBSerial_print(",");
        USBSerial_print(irdata->data[i]);
      }
      USBSerial_println("");
    } else {
      USBSerial_println("Invalid data received.");
    }
}

