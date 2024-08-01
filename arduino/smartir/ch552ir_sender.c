#include "Arduino.h"
#include "ch552ir.h"

volatile __data uint8_t ir_subCarrierPhase;
volatile __data bool ir_on;

inline void ch552ir_senderBegin(){
  pinMode(PIN_IRLED, OUTPUT);
  P3_4 = 0;
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

void ch552ir_write(__xdata ch552ir_data* irdata){
  ch552ir_irOscillationStart();
  __xdata unsigned long time_us = 0;
  __xdata unsigned long waitTime_us = 0;

  if(irdata->format == CH552IR_FORMAT_NEC){
    ir_on = true;
    time_us = micros();
    waitTime_us = irdata->t * 16;
    while(micros() - time_us < waitTime_us);

    ir_on = false;
    time_us += waitTime_us;
    waitTime_us = irdata->t * 8;
    while(micros() - time_us < waitTime_us);

  } else if (irdata->format == CH552IR_FORMAT_AEHA){
    ir_on = true;
    time_us = micros();
    waitTime_us = irdata->t * 8;
    while(micros() - time_us < waitTime_us);

    ir_on = false;
    time_us += waitTime_us;
    waitTime_us = irdata->t * 4;
    while(micros() - time_us < waitTime_us);
  }
  if(irdata->format == CH552IR_FORMAT_NEC || irdata->format == CH552IR_FORMAT_AEHA){
    for(uint8_t index = 0; index < irdata->datalength; index++){
      uint8_t data = irdata->data[index];
      for(uint8_t bit = 0; bit < 8; bit++){
        ir_on = true;
        time_us += waitTime_us;
        waitTime_us = irdata->t;
        while(micros() - time_us < waitTime_us);
        ir_on = false;
        time_us += waitTime_us;
        if(data & (1 << bit)){
          waitTime_us = irdata->t * 3;
        }else{
          waitTime_us = irdata->t;
        }
        while(micros() - time_us < waitTime_us);
      }
    }
    ir_on = true;
    time_us += waitTime_us;
    waitTime_us = irdata->t;
    while(micros() - time_us < waitTime_us);
    ir_on = false;
  }

  ch552ir_irOscillationStop();
}



void Timer2Interrupt(void) __interrupt (INT_NO_TMR2){
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