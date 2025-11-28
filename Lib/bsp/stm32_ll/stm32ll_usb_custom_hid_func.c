#include "usb_custom_hid_func.h"
#include "usbd_custom_hid_if.h"

#define USB_LENGTH 64

volatile bool UsbRcv = false;
volatile u8 UsbRxBuffer[USB_LENGTH];
volatile u8 UsbTxBuffer[USB_LENGTH];

static UsbCustomHid_RcvFunc_t RcvFunc;

void CustomHid_Init(UsbCustomHid_RcvFunc_t rcvFunc) { RcvFunc = rcvFunc; }

void CustomHid_Process(void) {
  if (UsbRcv) {
    UsbRcv = false;

    if (RcvFunc != NULL) {
      RcvFunc((const u8 *)UsbRxBuffer, USB_LENGTH);
    }
  }
}

OsErr_t CustomHid_Send(const u8 *pData, u16 length) {
  memcpy((void *)UsbTxBuffer, pData, length);
  memset((void *)(UsbTxBuffer + length), 0, USB_LENGTH - length);

  if (USBD_CUSTOM_HID_SendReport_FS((u8 *)UsbTxBuffer, USB_LENGTH) == USBD_OK) {
    return OS_ERR_OK;
  }

  return OS_ERR_FAIL;
}