#include "stdbool.h"
#include "Arduino.h"
#include "ch552ir.h"

#define PIN_IRLED 34
#define PIN_IRRECEIVER 33
#define BIT_IRRECEIVER P3_3
#define INTERRUPT_IRRECEIVER 1
#define INTERRUPT_EN_IRRECEIVER EX1
#define IR_TIMEOUT 10000

#define IR_PULSE_OFFSET_US 80

/*
  受信
*/

volatile __data bool irRxSignal;
volatile __data unsigned long irRxSignal_us;

void ch552ir_receiverInterrupt(){
  irRxSignal = true;
  irRxSignal_us = micros();
}

bool ch552ir_available(){
  return irRxSignal;
}

void ch552ir_flush(){
  irRxSignal = false;
  irRxSignal_us = 0;
}

size_t ch552ir_record(uint16_t data[], size_t data_len){
  unsigned long startTime_us = irRxSignal_us;
  unsigned long lastChangeTime_us = irRxSignal_us;
  unsigned long now_us;
  uint8_t lastStaste = BIT_IRRECEIVER;
  bool dataAvailable = false;
  size_t len;

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

  for(size_t i=0; i<len; i++){
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

void ch552ir_read(ch552ir_data* data){
  __xdata uint16_t irRawdata_us[128];
  size_t irRawdata_us_len = ch552ir_record(irRawdata_us, 128);

  uint16_t irLeader_us = irRawdata_us[0] + irRawdata_us[1];
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
    for(size_t i=0; i<16; i++){
      data->data[i] = 0;
    }
    data->datalength = 0;

    for(size_t i=2; i<irRawdata_us_len; i+=2){
      int bit_position = (i - 2) / 2;
      int byte_index = bit_position / 8;
      int bit_index = bit_position % 8;
      if (irRawdata_us[i + 1] > data->t * 2) {
        data->data[byte_index] |= (1 << bit_index);
      }
      data->datalength = byte_index + 1;
    }
  }

}

/*
  送信
*/

volatile __data uint8_t ir_subCarrierPhase;
volatile __data bool ir_on;

void Timer2Interrupt(void) __interrupt (INT_NO_TMR2)
{
  TF2 = 0;
  if(!ir_subCarrierPhase && ir_on){
    P3_4 = 1;
  } else {
    P3_4 = 0;
  }
  ir_subCarrierPhase++;
  if(ir_subCarrierPhase > 2){
    ir_subCarrierPhase = 0;
  }
}

void ch552ir_irOscillationStart(){
  const uint16_t timer2_reloadValue_38k = 0xffff - 210;
  ir_subCarrierPhase = 0;
  ir_on = false;
  TR2 = 0;
  T2MOD |= bTMR_CLK | bT2_CLK;
  RCAP2H = (uint8_t)(timer2_reloadValue_38k >> 8);
  RCAP2L = (uint8_t)(timer2_reloadValue_38k & 0xff); 
  TH2 = RCAP2H;
  TL2 = RCAP2L;
  TF2 = 0;
  ET2 = 1;
  EA = 1;
  TR2 = 1;
}

void ch552ir_irOscillationStop(){
  TR2 = 0;
  P3_4 = 0;
}

void ch552ir_write(ch552ir_data* data){
  ch552ir_irOscillationStart();
  unsigned long time_us;
  unsigned long waitTime_us;

  if(data->format == CH552IR_FORMAT_NEC){
    ir_on = true;
    time_us = micros();
    waitTime_us = data->t * 16;
    while(micros() - time_us < waitTime_us);

    ir_on = false;
    time_us += waitTime_us;
    waitTime_us = data->t * 8;
    while(micros() - time_us < waitTime_us);

  } else if (data->format == CH552IR_FORMAT_AEHA){
    ir_on = true;
    time_us = micros();
    waitTime_us = data->t * 8;
    while(micros() - time_us < waitTime_us);

    ir_on = false;
    time_us += waitTime_us;
    waitTime_us = data->t * 4;
    while(micros() - time_us < waitTime_us);
  }
  if(data->format == CH552IR_FORMAT_NEC || data->format == CH552IR_FORMAT_AEHA){
    for(size_t index = 0; index < data->datalength; index++){
      for(uint8_t bit = 0; bit < 8; bit++){
        ir_on = true;
        time_us += waitTime_us;
        waitTime_us = data->t;
        while(micros() - time_us < waitTime_us);
        ir_on = false;
        time_us += waitTime_us;
        if(data->data[index] & 1 << bit){
          waitTime_us = data->t * 3;
        }else{
          waitTime_us = data->t;
        }
        while(micros() - time_us < waitTime_us);
      }
    }
  }
}

/*
  送受信共通部
*/

void ch552ir_begin(){
  irRxSignal = false;
  irRxSignal_us = 0;
  pinMode(PIN_IRRECEIVER, INPUT);
  pinMode(PIN_IRLED, OUTPUT);
  attachInterrupt(INTERRUPT_IRRECEIVER, ch552ir_receiverInterrupt, FALLING);
}

void ch552ir_dataInit(ch552ir_data* data){
  data->datalength = 0;
  data->format = CH552IR_FORMAT_NEC;
}
