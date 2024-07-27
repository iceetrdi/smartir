#ifndef CH552IR_H
#define CH552IR_H

#include <Arduino.h>

#define CH552IR_NEC_T_US 562
#define CH552IR_AEHA_T_US_TYP 425

typedef enum enum_ch552ir_format {
  CH552IR_FORMAT_NEC,
  CH552IR_FORMAT_AEHA
} ch552ir_format;

typedef struct struct_ch552ir_data {
  ch552ir_format format;
  unsigned int t;
  uint8_t data[16];
  size_t datalength;
} ch552ir_data;

void ch552ir_begin();
bool ch552ir_available();
void ch552ir_read(ch552ir_data* data);
void ch552ir_write(ch552ir_data* data);

#endif