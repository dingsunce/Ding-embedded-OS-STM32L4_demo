#ifndef HEX_DRV_H
#define HEX_DRV_H

#include "d_define.h"
#include "func.h"

#define HEX_IO_SUM       4
#define HEX_DRV_TIMEBASE 10 // 10ms

typedef struct
{
  u8  Position;
  u16 DebounceTime;
  u8  LastPosition;
} HexDrvRam_t;

typedef const struct
{
  u8           Id;
  HexDrvRam_t *pRam;
  Io_t         Io[HEX_IO_SUM]; // Hex 1,2,4,8
  void (*CallBackFunc)(u8 id, u8 position);
} HexDrv_t;

/*
#if HEX_SUM > 0
// The ram data used in HexTable.
static HexDrvRam_t HexRam[HEX_SUM];

// The hex table.
static HexDrv_t HexTable[HEX_SUM] = {
    {
        0,       // Id
        HexRam,  // pRam
        {{
             (IoPort_t)0,  // Io port hex 1
             (IoPin_t)0,   // Io pin hex 1
         },
         {
             (IoPort_t)0,  // Io port hex 2
             (IoPin_t)0,   // Io pin hex 2
         },
         {
             (IoPort_t)0,  // Io port hex 4
             (IoPin_t)0,   // Io pin hex 4
         },
         {
             (IoPort_t)0,  // Io port hex 8
             (IoPin_t)0,   // Io pin hex 8
         }},
        CallbackFunc,  // CallBackFunc
    },
};


Hex_Init(HexTable, HEX_SUM);

#endif
*/
void Hex_Init(HexDrv_t *pTable, u8 sum);

u8 Hex_GetCurrentPosition(u8 hexId);

#endif