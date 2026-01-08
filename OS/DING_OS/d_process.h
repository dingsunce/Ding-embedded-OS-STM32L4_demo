/*!*****************************************************************************
 * file		d_process.h (process implementation.)
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _D_PROCESS_H_
#define _D_PROCESS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "double_list.h"
#include "pt.h"

  struct DProcess;
  typedef u8 (*DProcessHandler)(struct DProcess *, DMsgId_t, DMsgArg_t);

  typedef struct DProcess
  {
    DProcessHandler Handler;
    Pt_t            Pt;
    u8              State;
    u8              NeedPoll;
    Db_List_t       ProcessList;
    Db_List_t       TimerList;
    const char     *name;
  } DProcess_t;

#define PROCESS_HANDLER(name, msgId, arg)                                                          \
  static u8(name##Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg))

#define PROCESS(name)                                                                              \
  PROCESS_HANDLER(name, msgId, arg);                                                               \
  static DProcess_t name = {                                                                       \
      name##Handler, {0}, 0, 0, DB_LIST_INIT((name).ProcessList), DB_LIST_INIT((name).TimerList),  \
      #name};

#define PROCESS_SCHEDULE_BEGIN() PT_BEGIN(&process->Pt)

#define PROCESS_SCHEDULE_END() PT_END(&process->Pt)

#define PROCESS_WAIT_FOR_ANY_MSG() PT_YIELD(&process->Pt)

#define PROCESS_WAIT_FOR_MSG(c) PT_YIELD_UNTIL(&process->Pt, c)

#define PROCESS_EXIT() PT_EXIT(&process->Pt)

  typedef struct DProcessArg
  {
    void *Process;
  } DProcessArg_t;

  void DProcess_Start(DProcess_t *p);

  void DProcess_ReStart(DProcess_t *p);

  void DProcess_ExitProc(DProcess_t *p);

  void DProcess_ExitAllProc(void);

  void DProcess_HandleMsg(DProcess_t *p, DMsgId_t msg, DMsgArg_t data);

  void DProcess_Poll(DProcess_t *p);

  void DProcess_Init(void);

  void DProcess_Exit(void);

  void DProcess_Run(void);

  bool DProcess_IsRunning(DProcess_t *p);

  void *DProcess_CreateArg(DProcess_t *p);

  void DProcess_InitStructure(DProcess_t *p, DProcessHandler handler);

  void DProcess_InitStruct(DProcess_t *p, DProcessHandler handler, const char *name);

#ifdef __cplusplus
}
#endif

#endif
