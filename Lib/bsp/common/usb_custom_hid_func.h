#ifndef USB_CUSTOM_HID_FUNC_H
#define USB_CUSTOM_HID_FUNC_H

#include "define.h"
#include "error.h"

typedef void (*UsbCustomHid_RcvFunc_t)(const u8 *pData, u16 length);

extern void CustomHid_Init(UsbCustomHid_RcvFunc_t rcvFunc);
extern void CustomHid_Process(void);
extern OsErr_t CustomHid_Send(const u8 *pData, u16 length);

#endif
