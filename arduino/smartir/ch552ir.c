#include "ch552ir.h"

inline void ch552ir_dataInit(__xdata ch552ir_data* data){
  data->datalength = 0;
  data->format = CH552IR_FORMAT_UNKNOWN;
}
