#ifndef CH552IR_H
#define CH552IR_H

#include <Arduino.h>

#define PIN_IRLED 34
#define PIN_IRRECEIVER 33

#define IR_MAX_DATA_BYTES 8

typedef enum enum_ch552ir_format {
  CH552IR_FORMAT_UNKNOWN,
  CH552IR_FORMAT_NEC = 1,
  CH552IR_FORMAT_AEHA = 2
} ch552ir_format;

typedef struct struct_ch552ir_data {
  ch552ir_format format;
  unsigned int t;
  uint8_t datalength;
  uint8_t data[IR_MAX_DATA_BYTES];
} ch552ir_data;

inline void ch552ir_dataInit(__xdata ch552ir_data* data);

inline void ch552ir_receiverBegin();
inline bool ch552ir_available();
inline void ch552ir_flush();
void ch552ir_read(__xdata ch552ir_data* data);

inline void ch552ir_senderBegin();
void ch552ir_write(__xdata ch552ir_data* irdata);

#endif