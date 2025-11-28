/*!*****************************************************************************
 * file		error.c
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _ERROR_H
#define _ERROR_H

#ifdef __cplusplus
extern "C"
{
#endif

  typedef enum OsErr
  {
    OS_ERR_OK,                  // OK
    OS_ERR_FAIL,                // Failed
    OS_ERR_NULL,                // Pointer in NULL
    OS_ERR_SPEED,               // This device does not work in the active speed mode.
    OS_ERR_RANGE,               // Parameter out of range.
    OS_ERR_VALUE,               // Parameter of incorrect value.
    OS_ERR_ADDRESS,             // Address not match.
    OS_ERR_OVERFLOW,            // Timer overflow.
    OS_ERR_MATH,                // Overflow during evaluation.
    OS_ERR_ENABLED,             // Device is enabled.
    OS_ERR_DISABLED,            // Device is disabled.
    OS_ERR_BUSY,                // Device is busy.
    OS_ERR_NOT_AVAILABLE,       // Requested value or method not available.
    OS_ERR_RX_EMPTY,            // No data in receiver.
    OS_ERR_RX_FULL,             // receiver is full.
    OS_ERR_TX_FULL,             // Transmitter is full.
    OS_ERR_TX_EMPTY,            // Transmitter is empty.
    OS_ERR_BUS_OFF,             // Bus not available.
    OS_ERR_OVERRUN,             // Overrun error is detected.
    OS_ERR_FRAMING,             // Framing error is detected.
    OS_ERR_PARITY,              // Parity error is detected.
    OS_ERR_NOISE,               // Noise error is detected.
    OS_ERR_IDLE,                // Idle error is detected.
    OS_ERR_FAULT,               // Fault error is detected.
    OS_ERR_BREAK,               // Break char is received during communication.
    OS_ERR_CRC,                 // CRC error is detected.
    OS_ERR_ARBITRATION,         // A node lost arbitration. This error occurs if two nodes start
                                // transmission at the same time.
    OS_ERR_PROTECT,             // Protection error is detected.
    OS_ERR_UNDERFLOW,           // Underflow error is detected.
    OS_ERR_UNDER_RUN,           // Under run error is detected.
    OS_ERR_COMMON,              // Common error of a device.
    OS_ERR_LIN_SYNC,            // LIN synchronization error is detected.
    OS_ERR_FAILED,              // Requested functionality or Process failed.
    OS_ERR_QFULL,               // Queue is full.
    OS_ERR_QEMPTY,              // Queue is empty.
    OS_ERR_ALLOC,               // Alloc fail.
    OS_ERR_PARAM_MASK,          // Invalid mask.
    OS_ERR_PARAM_MODE,          // Invalid mode.
    OS_ERR_PARAM_INDEX,         // Invalid index.
    OS_ERR_PARAM_DATA,          // Invalid data.
    OS_ERR_PARAM_SIZE,          // Invalid size.
    OS_ERR_PARAM_VALUE,         // Invalid value.
    OS_ERR_PARAM_RANGE,         // Invalid parameter's range or parameters' combination.
    OS_ERR_PARAM_LOW_VALUE,     // Invalid value (LOW part).
    OS_ERR_PARAM_HIGH_VALUE,    // Invalid value (HIGH part).
    OS_ERR_PARAM_ADDRESS,       // Invalid address.
    OS_ERR_PARAM_PARITY,        // Invalid parity.
    OS_ERR_PARAM_WIDTH,         // Invalid width.
    OS_ERR_PARAM_LENGTH,        // Invalid length.
    OS_ERR_PARAM_ADDRESS_TYPE,  // Invalid address type.
    OS_ERR_PARAM_COMMAND_TYPE,  // Invalid command type.
    OS_ERR_PARAM_COMMAND,       // Invalid command.
    OS_ERR_PARAM_RECIPIENT,     // Invalid recipient.
    OS_ERR_PARAM_BUFFER_COUNT,  // Invalid buffer count.
    OS_ERR_PARAM_ID,            // Invalid ID.
    OS_ERR_PARAM_GROUP,         // Invalid group.
    OS_ERR_PARAM_CHIP_SELECT,   // Invalid chip select.
    OS_ERR_PARAM_ATTRIBUTE_SET, // Invalid set of attributes.
    OS_ERR_PARAM_SAMPLE_COUNT,  // Invalid sample count.
    OS_ERR_PARAM_CONDITION,     // Invalid condition.
    OS_ERR_PARAM_TICKS          // Invalid TICKS parameter.
  } OsErr_t;

#ifdef __cplusplus
}
#endif

#endif