#ifndef PIN_DRV_H
#define PIN_DRV_H

#include "d_define.h"
#include "func.h"

typedef struct
{
  u8 Value;
} PinDrvRam_t;

typedef const struct
{
  PinDrvRam_t *pRam;
  Io_t ShiftLoadIo; // SH_LD: Shift or Load input, When High Data, shifted. When Low data is loaded
                    // from parallel inputs
  Io_t ClockIo;     // CLK: Clock input
  Io_t ClockInhibitIo; // CLK INH: Clock Inhibit, when High No change in output
  Io_t DataIo;         // QH: Serial Output
} PinDrv_t;

/*
#if PIN_SUM > 0
// The ram data used in PinTable.
static PinDrvRam_t HalPinRam;

// The 165 table.
static PinDrv_t PinTable = {
    &HalPinRam,  // pRam
    {
        (IoPort_t)0,  // ShiftLoadIo port
        (IoPin_t)0,   // ShiftLoadIo pin
    },
    {
        (IoPort_t)0,  // ClockIo port
        (IoPin_t)0,   // ClockIo pin
    },
    {
        (IoPort_t)0,  // ClockInhibitIo port
        (IoPin_t)0,   // ClockInhibitIo pin
    },
    {
        (IoPort_t)0,  // OutputIo port
        (IoPin_t)0,   // OutputIo pin
    },
};

Pin_Init(&HcPinTable);


#endif
 */
void Pin_Init(PinDrv_t *pTable);

u8 Pin_GetValue(void);

#endif