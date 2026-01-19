#ifndef _POUT_DRV_H
#define _POUT_DRV_H

#include "d_define.h"
#include "func.h"

typedef const struct
{
  u8   Id;
  Io_t DataIo;       // DS
  Io_t ShiftClockIo; // SH_CP
  Io_t StoreClockIo; // ST_CP
} PoutDrv_t;

/*
#if POUT_SUM > 0
// The pout table.
static PoutDrv_t PoutTable[POUT_SUM] = {
    {
        0,                // Id
        {
            (IoPort_t)0,  // DS Io port
            (IoPin_t)0,   // DS Io pin
        },
        {
            (IoPort_t)0,  // STCP Io port
            (IoPin_t)0,   // STCP Io pin
        },
        {
            (IoPort_t)0,  // SHCP Io port
            (IoPin_t)0,   // SHCP Io pin
        },
    },
};

Pout_Init(PoutTable, POUT_SUM);

#endif
*/
void Pout_Init(PoutDrv_t *pTable, u8 sum);

void Pout_WriteData(u8 id, u8 *pData, u8 length);

#endif