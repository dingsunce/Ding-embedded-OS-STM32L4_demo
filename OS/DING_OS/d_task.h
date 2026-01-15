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

#include "d_define.h"
#include "d_error.h"
#include "d_os.h"
#include "d_process.h"

  void    DTask_Init(void);
  void    DTask_Exit(void);
  OsErr_t DTask_Store(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg);
  void    DTask_Run(void);
  void    DTask_RunAll(void);
  void    DTask_CancelMsg(DProcess_t *process, DMsgId_t msgId);
  void    DTask_FlushMsg(DProcess_t *process);
  bool    DTask_IsMsgInTask(DProcess_t *process, DMsgId_t msgId);

#ifdef __cplusplus
}
#endif

#endif