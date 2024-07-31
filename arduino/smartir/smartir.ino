#include <Serial.h>
#include "ch552ir.h"
#include "buttonctl.h"
#include "ledctl.h"

ledctl_led led;
buttonctl_button button;
ch552ir_data irData;
ch552ir_data irSaveData;

// void ch552ir_data_test(ch552ir_data* data);


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
    // ch552ir_data_test(&irSaveData);
    buttonctl_clear(&button);
  }

  if(buttonctl_wasPushed(&button)){
    buttonctl_clear(&button);
    ledctl_setPeriod(&led, 500);
    ledctl_write(&led, LEDCTL_ON_UNTIL);
    ledctl_update(&led);
    // ch552ir_data_test(&irSaveData);
  }

  ledctl_update(&led);
}

/*
void ch552ir_record_test(){
  __xdata uint16_t irRawdata_us[128];
  size_t irRawdata_us_len = 0;

  if(ch552ir_available()){
    irRawdata_us_len = ch552ir_record(irRawdata_us, 128);
    if(irRawdata_us_len){
      USBSerial_print(irRawdata_us_len);
      USBSerial_print("[");
      for (size_t i = 0; i < irRawdata_us_len; i++) {
        USBSerial_print(irRawdata_us[i]);
        if(i < irRawdata_us_len - 1){
          USBSerial_print(",");
        }
      }
      USBSerial_println("]");
    } else {
      USBSerial_println("No data received.");
    }
  }
}
*/

/*
void ch552ir_data_test(ch552ir_data* data){
    if(data->format == CH552IR_FORMAT_NEC){
      USBSerial_print("NEC,T=");
      USBSerial_print(data->t);
      for (size_t i = 0; i < data->datalength; i++) {
        USBSerial_print(",");
        USBSerial_print(data->data[i]);
      }
      USBSerial_println("");
    } else if(data->format == CH552IR_FORMAT_AEHA){
      USBSerial_print("AEHA,T=");
      USBSerial_print(data->t);
      for (size_t i = 0; i < data->datalength; i++) {
        USBSerial_print(",");
        USBSerial_print(data->data[i]);
      }
      USBSerial_println("");
    } else {
      USBSerial_println("Invalid data received.");
    }
}
*/
