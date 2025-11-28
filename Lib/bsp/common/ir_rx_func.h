#ifndef IR_RX_FUNC_H
#define IR_RX_FUNC_H

#include "define.h"

typedef void (*Ir_RcvFunc_t)(u8 *pData, u8 length);

extern void IrRx_Init(Ir_RcvFunc_t rcvFunc);
extern void IrRx_Process(void);

#endif