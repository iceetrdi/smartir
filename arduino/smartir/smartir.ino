#include "ch552ir.h"
#include "buttonctl.h"
#include "ledctl.h"

#define SERIAL_BUFFER_SIZE 64

__xdata ledctl_led led;
__xdata buttonctl_button button;
__xdata ch552ir_data irData;
__xdata ch552ir_data irSaveData;

void ch552ir_serialPrint(__xdata ch552ir_data* irdata);
void ch552ir_serialRead(__xdata ch552ir_data* irdata);
void ch552ir_saveEEPROM(__xdata ch552ir_data* irdata);
void ch552ir_readEEPROM(__xdata ch552ir_data* irdata);

void setup(){
  ch552ir_senderBegin();
  ch552ir_receiverBegin();
  ch552ir_dataInit(&irData);
  ch552ir_readEEPROM(&irSaveData);
  ledctl_begin(&led, 30);
  buttonctl_begin(&button, 32);
  pinMode(15, OUTPUT);
  digitalWrite(15, LOW);
}

void loop(){
  buttonctl_update(&button);

  if(ch552ir_available()){
    ledctl_setPeriod(&led, 250);
    ledctl_write(&led, LEDCTL_ON_UNTIL);
    ledctl_update(&led);
    
    ch552ir_read(&irData);
    ch552ir_serialPrint(&irData);
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
    ch552ir_flush();

    ch552ir_saveEEPROM(&irSaveData);

    buttonctl_clear(&button);
  }

  if(buttonctl_wasPushed(&button)){
    buttonctl_clear(&button);

    ledctl_setPeriod(&led, 250);
    ledctl_write(&led, LEDCTL_ON_UNTIL);
    ledctl_update(&led);

    ch552ir_write(&irSaveData);
    ch552ir_flush();
  }

  ch552ir_serialRead(&irData);
  if(irData.format != CH552IR_FORMAT_UNKNOWN){
    ledctl_setPeriod(&led, 250);
    ledctl_write(&led, LEDCTL_ON_UNTIL);
    ledctl_update(&led);

    ch552ir_write(&irData);
    ch552ir_flush();
  }

  ledctl_update(&led);
}


void ch552ir_serialPrint(__xdata ch552ir_data* irdata){
    if(irdata->format == CH552IR_FORMAT_NEC){
      USBSerial_print("1,");
      USBSerial_print(irdata->t);
      for (uint8_t i = 0; i < irdata->datalength; i++) {
        USBSerial_print(",");
        USBSerial_print(irdata->data[i]);
      }
      USBSerial_println("");
    } else if(irdata->format == CH552IR_FORMAT_AEHA){
      USBSerial_print("2,");
      USBSerial_print(irdata->t);
      for (uint8_t i = 0; i < irdata->datalength; i++) {
        USBSerial_print(",");
        USBSerial_print(irdata->data[i]);
      }
      USBSerial_println("");
    } else {
      // USBSerial_println("Invalid data received.");
    }
}

int ch552ir_atoi(char* str) {
  int res = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    if(str[i] > '0' - 1 && str[i] < '9' + 1 ){
      res = res * 10 + str[i] - '0';
    }
  }
  return res;
}

void ch552ir_serialRead(__xdata ch552ir_data* irdata){
  __xdata char stringData[SERIAL_BUFFER_SIZE];
  __xdata uint8_t stringData_len = 0;

  if(USBSerial_available()){
    for(uint8_t i = 0; i < SERIAL_BUFFER_SIZE; i++){
      stringData[i] = 0;
    }
  }
  
  for(; stringData_len < SERIAL_BUFFER_SIZE; stringData_len++){
    if(!USBSerial_available()) break;
    stringData[stringData_len] = USBSerial_read();
    delay(1);
  }

  irdata->format = CH552IR_FORMAT_UNKNOWN;
  
  if(stringData_len){
    // USBSerial_println(stringData);
    char* token = strtok(stringData, ",");
    uint8_t index = 0;
    while (token != NULL) {
      if (index == 0) {
        irdata->format = (uint8_t)ch552ir_atoi(token);
      } else if (index == 1) {
        irdata->t = (uint16_t)ch552ir_atoi(token);
      } else {
        irdata->data[index - 2] = (uint8_t)ch552ir_atoi(token);
      }
      token = strtok(NULL, ",");
      index++;
    }
    
    irdata->datalength = index - 2;
  }
}

void ch552ir_saveEEPROM(__xdata ch552ir_data* irdata){
  eeprom_write_byte(0, irdata->format);
  eeprom_write_byte(1, (uint8_t)(irdata->t & 0xff));
  eeprom_write_byte(2, (uint8_t)(irdata->t >> 8));
  eeprom_write_byte(3, irdata->datalength);
  for(uint8_t i = 0; i < IR_MAX_DATA_BYTES; i++){
    eeprom_write_byte(i + 4, irdata->data[i]);
  }
}

void ch552ir_readEEPROM(__xdata ch552ir_data* irdata){
  irdata->format = eeprom_read_byte(0);
  irdata->t = eeprom_read_byte(1);
  irdata->t |= eeprom_read_byte(2) << 8;
  irdata->datalength = eeprom_read_byte(3);
  for(uint8_t i = 0; i < IR_MAX_DATA_BYTES; i++){
    irdata->data[i] = eeprom_read_byte(i + 4);
  }
}

