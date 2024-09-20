#ifndef INC_TYPES_H
#define INC_TYPES_H

#include "external/lm3s5r31.h"

#define I2C_DEVICES_LIMIT 4    // Can not exceed 16 because of addresses limit
#define PACKET_SIZE       256  // Must be as large as any single protocol response
#define COMMAND_LENGTH    4    // Command name size in bytes (must be 4)
#define PROTOCOL_CRC_SIZE 2    // CRC appendix size (must be 2)

typedef uint8_t  INT8U;
typedef int8_t   INT8S;
typedef uint16_t INT16U;
typedef int16_t  INT16S;
typedef uint32_t INT32U;
typedef int32_t  INT32S;
typedef uint64_t INT64U;
typedef int64_t  INT64S;
typedef float    FLT32;
typedef double   FLT64;

#endif

// vim: syntax=c tabstop=4 shiftwidth=4
