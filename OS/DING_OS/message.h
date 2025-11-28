/*!*****************************************************************************
 * file		message.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _MESSAGE_H
#define _MESSAGE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "d_os.h"
#include "define.h"
#include "error.h"
#include "d_task.h"

#define SYS_MSG_START_PROGRESS   0xf000
#define SYS_MSG_POLL_PROCESS     0xf001
#define SYS_MSG_EXIT_PROCESS     0xf002
#define SYS_MSG_CONTINUE_PROCESS 0xf003
#define SYS_MSG_TEST_PROCESS     0xf004
#define SYS_MSG_PROGRESS_EXITED  0xf005
#define SYS_MSG_NONE             0xffff // reversed for sys use

#define MSG_IMMEDIATE ((MsgDelay_t)0)
#define MSG_MSEC(m)   ((MsgDelay_t)(m))
#define MSG_SEC(s)    ((MsgDelay_t)((s) * (MsgDelay_t)1000))
#define MSG_MIN(m)    ((MsgDelay_t)((m) * (MsgDelay_t)1000 * (MsgDelay_t)60))
#define MSG_HOUR(h)   ((MsgDelay_t)((h) * (MsgDelay_t)1000 * (MsgDelay_t)60) * (MsgDelay_t)60)

#define MSG_NO_ARG NULL

  void    Msg_Init(void);
  void    Msg_Run1ms(void);
  void    Msg_PostSem(void);
  void    Msg_Cancel(Process_t *process, MsgId_t msgId);
  void    Msg_CancelFirst(Process_t *process, MsgId_t msgId);
  void    Msg_CancelAll(Process_t *process, MsgId_t msgId);
  void    Msg_Flush(Process_t *process);
  OsErr_t Msg_SendLater(Process_t *process, MsgId_t msgId, MsgArg_t arg, u32 delay);
  OsErr_t Msg_ReSendLater(Process_t *process, MsgId_t msgId, MsgArg_t arg, u32 delay);
  OsErr_t Msg_SendCycle(Process_t *process, MsgId_t msgId, MsgArg_t arg, u32 period);
  OsErr_t Msg_ReSendCycle(Process_t *process, MsgId_t msgId, MsgArg_t arg, u32 period);
  OsErr_t Msg_SendInstant(Process_t *process, MsgId_t msgId, MsgArg_t arg);
  u32     Msg_GetRemainTime(Process_t *process, MsgId_t msgId);
  bool    Msg_IsMsgInProcess(Process_t *process, MsgId_t msgId);

#ifdef __cplusplus
}
#endif

#endif
