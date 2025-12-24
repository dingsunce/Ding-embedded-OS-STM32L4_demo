/*!*****************************************************************************
 * file		process.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "process.h"
#include "DList.h"
#include "d_mem.h"
#include "message.h"
#include "osal.h"

#define PROCESS_STATE_NONE    0
#define PROCESS_STATE_RUNNING 1

DLIST(MyProcessList);

static volatile u8 PollRequested;
static void        Do_Poll(void);

static os_sem_t    *ProcessPendingSem;
static os_sem_t    *ProcessListSem;
static os_thread_t *ProcessThread;
//-----------------------------------------------------------------------------------------------------------
static os_return_t Process_Thread(void *arg)
{
  while (!os_thread_should_stop(ProcessThread))
  {
    os_sem_wait(ProcessPendingSem, OS_WAIT_FOREVER);

    Process_Run();
  }

  OS_RETURN(ProcessThread);
}
//-----------------------------------------------------------------------------------------------------------
void Process_Init(void)
{
  // prevent reiniting the process
  Process_DoExitAll();

  ProcessPendingSem = os_sem_create(0);
  ProcessListSem = os_sem_create(1);
  ProcessThread = os_thread_create("os_process", D_OS_PROCESS_PRIO, 256, Process_Thread, NULL);
}
//-----------------------------------------------------------------------------------------------------------
void Process_Exit(void)
{
  os_sem_destroy(ProcessPendingSem);
  os_sem_destroy(ProcessListSem);

  os_thread_destroy(ProcessThread);
}
//-----------------------------------------------------------------------------------------------------------
void Process_InitStructure(Process_t *p, ProcessHandler handler)
{
  if (p->Handler != NULL) // already Initialized Process
    return;

  p->Handler = handler;
  PT_INIT(&p->Pt);

  os_sem_wait(ProcessListSem, OS_WAIT_FOREVER);

  DList_Init(&p->ProcessList);
  DList_Init(&p->TimerList);

  os_sem_signal(ProcessListSem);

  p->State = PROCESS_STATE_NONE;
  p->NeedPoll = false;
}
//-----------------------------------------------------------------------------------------------------------
void Process_InitStruct(Process_t *p, ProcessHandler handler, const char *name)
{
  if (p->Handler != NULL) // already Initialized Process
    return;

  Process_InitStructure(p, handler);

  p->name = name;
}
//-----------------------------------------------------------------------------------------------------------
void Process_Start(Process_t *p)
{
  if (p->Handler == NULL) // it is a UnInitialized Process
    return;

  if (p->State & PROCESS_STATE_RUNNING)
    return;

  os_sem_wait(ProcessListSem, OS_WAIT_FOREVER);

  DList_Init(&p->ProcessList);
  DList_Init(&p->TimerList);
  DList_Add(&MyProcessList, &p->ProcessList);

  os_sem_signal(ProcessListSem);

  p->State = PROCESS_STATE_RUNNING;
  PT_INIT(&p->Pt);

  Process_HandleMsg(p, SYS_MSG_START_PROGRESS, NULL);
}
//-----------------------------------------------------------------------------------------------------------
void Process_ReStart(Process_t *p)
{
  Process_DoExit(p);
  PT_INIT(&p->Pt);
  Process_Start(p);
}
//-----------------------------------------------------------------------------------------------------------
void Process_HandleMsg(Process_t *p, MsgId_t msg, MsgArg_t data)
{
  if ((p->State & PROCESS_STATE_RUNNING) && p->Handler != NULL)
  {
    u8 ret = p->Handler(p, msg, data);
    if (ret == PT_EXITED || ret == PT_ENDED)
      Process_DoExit(p);
  }
}
//-----------------------------------------------------------------------------------------------------------
void Process_DoExit(Process_t *p)
{
  if (Process_IsRunning(p))
  {
    p->State = PROCESS_STATE_NONE;

    os_sem_signal(ProcessListSem);
    DList_Remove(&p->ProcessList);
    os_sem_signal(ProcessListSem);

    Msg_Flush(p);
  }
}
//-----------------------------------------------------------------------------------------------------------
void Process_DoExitAll(void)
{
  DList_t *tmp = MyProcessList.next;
  while (tmp != &MyProcessList)
  {
    Process_t *p = ContainerOf(tmp, Process_t, ProcessList);
    tmp = tmp->next;
    Process_DoExit(p);
  }

  DList_Init(&MyProcessList);
}
//-----------------------------------------------------------------------------------------------------------
static void Do_Poll(void)
{
  PollRequested = 0;
  DList_t *tmp = MyProcessList.next;
  while (tmp != &MyProcessList)
  {
    Process_t *p = ContainerOf(tmp, Process_t, ProcessList);
    tmp = tmp->next;

    if (p->NeedPoll)
    {
      p->NeedPoll = 0;
      DTask_Store(p, SYS_MSG_POLL_PROCESS, NULL);
    }
  }
}
//-----------------------------------------------------------------------------------------------------------
void Process_Run(void)
{
  if (PollRequested)
    Do_Poll();
}
//-----------------------------------------------------------------------------------------------------------
void Process_Poll(Process_t *p)
{
  if (p != NULL)
  {
    if (p->State == PROCESS_STATE_RUNNING)
    {
      p->NeedPoll = 1;
      PollRequested = 1;

      os_sem_signal(ProcessPendingSem);
    }
  }
}
//-----------------------------------------------------------------------------------------------------------
bool Process_IsRunning(Process_t *p)
{
  return p->State == PROCESS_STATE_RUNNING;
}
//-----------------------------------------------------------------------------------------------------------
void *CreateProcessArg(Process_t *p)
{
  ProcessArg_t *arg = (ProcessArg_t *)DMem_Malloc(sizeof(ProcessArg_t));
  if (arg != NULL)
    arg->Process = p;

  return arg;
}
