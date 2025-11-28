#ifndef IR_TX_FUNC_H
#define IR_TX_FUNC_H

#include "define.h"

extern void IrTx_Init(void);
extern void IrTx_Send(u8 *pData, u8 length);

#endif