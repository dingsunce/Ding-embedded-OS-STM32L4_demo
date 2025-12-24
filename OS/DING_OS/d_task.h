/*!*****************************************************************************
 * file		d_task.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _d_task_H
#define _d_task_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "d_os.h"
#include "define.h"
#include "error.h"
#include "process.h"

  void    DTask_Init(void);
  void    DTask_Exit(void);
  OsErr_t DTask_Store(Process_t *process, MsgId_t msgId, MsgArg_t arg);
  void    DTask_Run(void);
  void    DTask_RunAll(void);
  void    DTask_CancelMsg(Process_t *process, MsgId_t msgId);
  void    DTask_FlushMsg(Process_t *process);
  bool    DTask_IsMsgInTask(Process_t *process, MsgId_t msgId);

#ifdef __cplusplus
}
#endif

#endif