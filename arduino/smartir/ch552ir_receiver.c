#include "Arduino.h"
#include "ch552ir.h"

#define PIN_IRRECEIVER 33
#define BIT_IRRECEIVER P3_3
#define INTERRUPT_IRRECEIVER 1
#define INTERRUPT_EN_IRRECEIVER EX1
#define IR_TIMEOUT 10000

#define IR_PULSE_OFFSET_US 80

volatile __data bool irRxSignal;
volatile __data unsigned long irRxSignal_us;

void ch552ir_receiverInterrupt(){
  irRxSignal = true;
  irRxSignal_us = micros();
}

inline void ch552ir_receiverBegin(){
  irRxSignal = false;
  irRxSignal_us = 0;
  pinMode(PIN_IRRECEIVER, INPUT);
  attachInterrupt(INTERRUPT_IRRECEIVER, ch552ir_receiverInterrupt, FALLING);
}

inline bool ch552ir_available(){
  return irRxSignal;
}

inline void ch552ir_flush(){
  irRxSignal = false;
  irRxSignal_us = 0;
}

inline uint8_t ch552ir_record(__xdata uint16_t data[], uint8_t data_len){
  // unsigned long startTime_us = irRxSignal_us;
  __xdata unsigned long lastChangeTime_us = irRxSignal_us;
  __xdata unsigned long now_us;
  uint8_t lastStaste = BIT_IRRECEIVER;
  bool dataAvailable = false;
  uint8_t len;

  INTERRUPT_EN_IRRECEIVER = 0;

  for(len = 0; len < data_len; len++){
    while (BIT_IRRECEIVER == lastStaste) {
      if( micros() - lastChangeTime_us > IR_TIMEOUT ){
        goto ch552ir_record_end;
      }
    }
    now_us = micros();
    data[len] = now_us - lastChangeTime_us;
    lastChangeTime_us = now_us;
    dataAvailable = true;
    lastStaste = BIT_IRRECEIVER;
  }

  ch552ir_record_end:

  for(uint8_t i=0; i<len; i++){
    if(i%2){
      data[i] += IR_PULSE_OFFSET_US;
    }else{
      data[i] -= IR_PULSE_OFFSET_US;
    }
  }

  INTERRUPT_EN_IRRECEIVER = 1;
  ch552ir_flush();

  if(!dataAvailable){
    return 0;
  }
  
  return len;
}

void ch552ir_read(__xdata ch552ir_data* data){
  __xdata uint16_t irRawdata_us[128];
  uint8_t irRawdata_us_len = ch552ir_record(irRawdata_us, 128);
  __xdata uint16_t irLeader_us = irRawdata_us[0] + irRawdata_us[1];
  if(irLeader_us >  10790 && irLeader_us <  16185){
    data->format = CH552IR_FORMAT_NEC;
    data->t = irLeader_us / 24;
  }else if(irLeader_us >  4200 && irLeader_us <  7200){
    data->format = CH552IR_FORMAT_AEHA;
    data->t = irLeader_us / 12;
  }else{
    data->format = CH552IR_FORMAT_UNKNOWN;
  }

  if(data->format == CH552IR_FORMAT_NEC || data->format == CH552IR_FORMAT_AEHA){
    for(uint8_t i=0; i<16; i++){
      data->data[i] = 0;
    }
    data->datalength = 0;

    for(uint8_t i=2; i<irRawdata_us_len; i+=2){
      uint8_t bit_position = (i - 2) / 2;
      uint8_t byte_index = bit_position / 8;
      uint8_t bit_index = bit_position % 8;
      if (irRawdata_us[i + 1] > data->t * 2) {
        data->data[byte_index] |= (1 << bit_index);
      }
      data->datalength = byte_index + 1;
    }
  }

}
