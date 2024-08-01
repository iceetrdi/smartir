#ifndef CH552IR_H
#define CH552IR_H

#include <Arduino.h>

#define PIN_IRLED 34
#define PIN_IRRECEIVER 33

#define CH552IR_NEC_T_US 562
#define CH552IR_AEHA_T_US_TYP 425

typedef enum enum_ch552ir_format {
  CH552IR_FORMAT_UNKNOWN,
  CH552IR_FORMAT_NEC,
  CH552IR_FORMAT_AEHA
} ch552ir_format;

typedef struct struct_ch552ir_data {
  ch552ir_format format;
  unsigned int t;
  uint8_t data[16];
  uint8_t datalength;
} ch552ir_data;

inline void ch552ir_begin();
inline void ch552ir_receiverBegin();
inline void ch552ir_senderBegin();
inline void ch552ir_flush();
inline bool ch552ir_available();
inline void ch552ir_dataInit(__xdata ch552ir_data* data);
void ch552ir_read(__xdata ch552ir_data* data);
void ch552ir_write(__xdata ch552ir_data* irdata);

#endif