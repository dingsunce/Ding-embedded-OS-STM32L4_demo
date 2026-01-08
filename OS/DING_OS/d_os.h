/*!*****************************************************************************
 * file		d_os.h
 * $Author: sunce.ding
 *******************************************************************************/

#ifndef OS_H_
#define OS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "osal.h"

  void DOS_Init(void);
  void DOS_Exit(void);
  void DOS_RunOneTick(void);
  void DOS_Run(void);

#define DContainerOf(ptr, type, member)                                                            \
  ((type *)((char *)(ptr) - (uintptr_t)(&((type *)0)->member)))

#define D_OS_VERSION 0x10

#ifndef DMEM_HEAP
#define DMEM_HEAP 20480
#endif

#if (DMEM_HEAP & 0x80000)
#error MAXMEMHEAP is too big to manage!
#endif

#ifndef DMEM_ALIGMENT_4BYTES
#define DMEM_ALIGMENT_4BYTES 1
#endif

// DMSG_TIMER_TABLE_SIZE is recommended to be a prime
#ifndef DMSG_TIMER_TABLE_SIZE
#define DMSG_TIMER_TABLE_SIZE 17
#endif

#ifndef DMSG_TIMER_NUM
#define DMSG_TIMER_NUM 100
#endif

#ifndef DTASK_ITEM_NUM
#define DTASK_ITEM_NUM 100
#endif

#ifndef DMESSAGE_DEBUG
#define DMESSAGE_DEBUG 0
#endif

#ifndef DMEM_DEBUG
#define DMEM_DEBUG 0
#endif

#ifndef DTASK_DEBUG
#define DTASK_DEBUG 0
#endif

#ifndef D_OS_PRIO
#define D_OS_PRIO 10
#endif

// msg priority > process priority > task priority
#if OS_INVERTED_PRIORITY == 0

#define D_OS_MSG_PRIO     D_OS_PRIO
#define D_OS_PROCESS_PRIO D_OS_PRIO + 1
#define D_OS_TASK_PRIO    D_OS_PRIO + 2

#else

#define D_OS_MSG_PRIO     D_OS_PRIO + 2
#define D_OS_PROCESS_PRIO D_OS_PRIO + 1
#define D_OS_TASK_PRIO    D_OS_PRIO

#endif

#ifdef __cplusplus
}
#endif

#endif
