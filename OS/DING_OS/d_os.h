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
  void DOS_Run1ms(void);
  void DOS_Run(void);

#define ContainerOf(ptr, type, member)                                                             \
  ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#define D_OS_VERSION 0x30

#ifndef MEM_HEAP
#define MEM_HEAP 5000
#endif

#if (MEM_HEAP & 0x80000)
#error MAXMEMHEAP is too big to manage!
#endif

#ifndef MEM_ALIGMENT_4BYTES
#define MEM_ALIGMENT_4BYTES 1
#endif

// MSG_TIMER_TABLE_SIZE is recommended to be a prime
#ifndef MSG_TIMER_TABLE_SIZE
#define MSG_TIMER_TABLE_SIZE 13
#endif

#ifndef MSG_TIMER_NUM
#define MSG_TIMER_NUM 100
#endif

#ifndef TASK_ITEM_NUM
#define TASK_ITEM_NUM 100
#endif

#ifndef MESSAGE_DEBUG
#define MESSAGE_DEBUG 0
#endif

// Caution: If MEM_DEBUG is opened, DMem_Free will change the object contents to MEM_FREE('F')
// Do not use the object directly after it is freed
#ifndef MEM_DEBUG
#define MEM_DEBUG 0
#endif

#ifndef TASK_DEBUG
#define TASK_DEBUG 0
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
