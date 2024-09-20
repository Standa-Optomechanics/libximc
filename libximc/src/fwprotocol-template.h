#ifndef INC_FWPROTOCOL_H
#define INC_FWPROTOCOL_H

typedef struct
{
  uint32_t	    Cmd;
  uint8_t		SendBytes;
  uint8_t		ReceiveBytes;
} CmdLengthStr;

extern CmdLengthStr CmdLengths[];

#pragma pack(push, 1)

/* @@GENERATED_CODE@@ */

#pragma pack(pop)

#endif // INC_FWPROTOCOL_H

// vim: ts=4 shiftwidth=4
