/*!*****************************************************************************
 * file		d_message.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _D_MESSAGE_H
#define _D_MESSAGE_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "d_os.h"
#include "d_task.h"
#include "define.h"
#include "error.h"

#define SYS_MSG_START_PROGRESS   0xf000
#define SYS_MSG_POLL_PROCESS     0xf001
#define SYS_MSG_EXIT_PROCESS     0xf002
#define SYS_MSG_CONTINUE_PROCESS 0xf003
#define SYS_MSG_TEST_PROCESS     0xf004
#define SYS_MSG_PROGRESS_EXITED  0xf005
#define SYS_MSG_NONE             0xffff // reversed for sys use

#define MSG_IMMEDIATE ((DMsgDelay_t)0)
#define MSG_MSEC(m)   ((DMsgDelay_t)(m))
#define MSG_SEC(s)    ((DMsgDelay_t)((s) * (DMsgDelay_t)1000))
#define MSG_MIN(m)    ((DMsgDelay_t)((m) * (DMsgDelay_t)1000 * (DMsgDelay_t)60))
#define MSG_HOUR(h)   ((DMsgDelay_t)((h) * (DMsgDelay_t)1000 * (DMsgDelay_t)60) * (DMsgDelay_t)60)

#define MSG_NO_ARG NULL

  void    DMsg_Init(void);
  void    DMsg_Exit(void);
  void    DMsg_RunOneTick(void);
  void    DMsg_PostSem(void);
  void    DMsg_Cancel(DProcess_t *process, DMsgId_t msgId);
  void    DMsg_CancelFirst(DProcess_t *process, DMsgId_t msgId);
  void    DMsg_CancelAll(DProcess_t *process, DMsgId_t msgId);
  void    DMsg_Flush(DProcess_t *process);
  OsErr_t DMsg_SendLater(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg, u32 delay);
  OsErr_t DMsg_ReSendLater(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg, u32 delay);
  OsErr_t DMsg_SendCycle(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg, u32 period);
  OsErr_t DMsg_ReSendCycle(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg, u32 period);
  OsErr_t DMsg_SendInstant(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg);
  u32     DMsg_GetRemainTime(DProcess_t *process, DMsgId_t msgId);
  bool    DMsg_IsMsgInProcess(DProcess_t *process, DMsgId_t msgId);

#ifdef __cplusplus
}
#endif

#endif
