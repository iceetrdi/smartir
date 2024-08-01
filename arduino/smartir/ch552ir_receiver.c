#include "Arduino.h"
#include "ch552ir.h"

#define PIN_IRRECEIVER 33
#define BIT_IRRECEIVER P3_3
#define INTERRUPT_IRRECEIVER 1
#define INTERRUPT_EN_IRRECEIVER EX1
#define IR_TIMEOUT 10000

#define IR_PULSE_OFFSET_US 80
#define IR_RAWDATA_US_LEN 136

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

void ch552ir_read(__xdata ch552ir_data* data){
  __xdata uint16_t irRawdata_us[IR_RAWDATA_US_LEN];
  __xdata uint32_t lastChangeTime_us = irRxSignal_us;
  __xdata uint32_t now_us;
  uint8_t lastStaste = BIT_IRRECEIVER;
  uint8_t irRawdata_us_len;
  bool isIrReceiveTimeout = false;

  INTERRUPT_EN_IRRECEIVER = 0;

  for(irRawdata_us_len = 0; irRawdata_us_len < IR_RAWDATA_US_LEN; irRawdata_us_len++){
    while (BIT_IRRECEIVER == lastStaste) {
      if( micros() - lastChangeTime_us > IR_TIMEOUT ){
        isIrReceiveTimeout = true;
        break;
      }
    }
    if(isIrReceiveTimeout) break;
    now_us = micros();
    irRawdata_us[irRawdata_us_len] = now_us - lastChangeTime_us;
    lastChangeTime_us = now_us;
    lastStaste = BIT_IRRECEIVER;
  }

  for(uint8_t i = 0; i < irRawdata_us_len; i++){
    if(i%2){
      irRawdata_us[i] += IR_PULSE_OFFSET_US;
    }else{
      irRawdata_us[i] -= IR_PULSE_OFFSET_US;
    }
  }

  INTERRUPT_EN_IRRECEIVER = 1;

  __xdata uint16_t irLeader_us = irRawdata_us[0] + irRawdata_us[1];

  if(irLeader_us >  10790 && irLeader_us <  16185 && irRawdata_us[0] < irRawdata_us[1] * 3){
    data->format = CH552IR_FORMAT_NEC;
    data->t = irLeader_us / 24;
  }else if(irLeader_us >  4200 && irLeader_us <  7200){
    data->format = CH552IR_FORMAT_AEHA;
    data->t = irLeader_us / 12;
  }else{
    data->format = CH552IR_FORMAT_UNKNOWN;
  }

  if(data->format == CH552IR_FORMAT_NEC || data->format == CH552IR_FORMAT_AEHA){
    for(uint8_t i = 0; i < IR_MAX_DATA_BYTES; i++){
      data->data[i] = 0;
    }
    data->datalength = 0;

    for(uint8_t i = 2; i < irRawdata_us_len; i+=2){
      uint8_t bit_position = (i - 2) / 2;
      uint8_t byte_index = bit_position / 8;
      uint8_t bit_index = bit_position % 8;
      if (irRawdata_us[i + 1] > data->t * 2) {
        data->data[byte_index] |= (1 << bit_index);
      }
      data->datalength = byte_index;
    }
  }

}
