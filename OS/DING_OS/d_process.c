/*!*****************************************************************************
 * file		d_process.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "d_process.h"
#include "d_mem.h"
#include "d_message.h"
#include "double_list.h"
#include "osal.h"

#define PROCESS_STATE_NONE    0
#define PROCESS_STATE_RUNNING 1

DB_LIST(MyProcessList);

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

    if (os_thread_should_stop(ProcessThread))
      break;

    DProcess_Run();
  }

  OS_RETURN(ProcessThread);
}
//-----------------------------------------------------------------------------------------------------------
void DProcess_Init(void)
{
  // prevent reiniting the process
  DProcess_ExitAllProc();

  DbList_Init(&MyProcessList);

  ProcessPendingSem = os_sem_create(0);
  ProcessListSem = os_sem_create(1);
  ProcessThread = os_thread_create("os_process", D_OS_PROCESS_PRIO, 256, Process_Thread, NULL);

  OS_PRINT("ProcessThread Start\n");
}
//-----------------------------------------------------------------------------------------------------------
void DProcess_Exit(void)
{
  os_thread_destroy(ProcessThread);

  os_sem_destroy(ProcessPendingSem);
  os_sem_destroy(ProcessListSem);

  os_thread_wait_for_completion(ProcessThread);

  OS_PRINT("ProcessThread Exit\n");
}
//-----------------------------------------------------------------------------------------------------------
void DProcess_InitStructure(DProcess_t *p, DProcessHandler handler)
{
  if (p->Handler != NULL) // already Initialized Process
    return;

  p->Handler = handler;
  PT_INIT(&p->Pt);

  os_sem_wait(ProcessListSem, OS_WAIT_FOREVER);

  DbList_Init(&p->ProcessList);
  DbList_Init(&p->TimerList);

  os_sem_signal(ProcessListSem);

  p->State = PROCESS_STATE_NONE;
  p->NeedPoll = false;
}
//-----------------------------------------------------------------------------------------------------------
void DProcess_InitStruct(DProcess_t *p, DProcessHandler handler, const char *name)
{
  // already Initialized Process
  if (p->Handler != NULL)
    return;

  DProcess_InitStructure(p, handler);

  p->name = name;
}
//-----------------------------------------------------------------------------------------------------------
void DProcess_Start(DProcess_t *p)
{
  // it is a UnInitialized Process
  if (p->Handler == NULL)
    return;

  if (p->State & PROCESS_STATE_RUNNING)
    return;

  os_sem_wait(ProcessListSem, OS_WAIT_FOREVER);

  DbList_Init(&p->ProcessList);
  DbList_Init(&p->TimerList);
  DbList_Add(&MyProcessList, &p->ProcessList);

  os_sem_signal(ProcessListSem);

  p->State = PROCESS_STATE_RUNNING;
  PT_INIT(&p->Pt);

  DProcess_HandleMsg(p, SYS_MSG_START_PROGRESS, NULL);
}
//-----------------------------------------------------------------------------------------------------------
void DProcess_ReStart(DProcess_t *p)
{
  DProcess_ExitProc(p);
  PT_INIT(&p->Pt);
  DProcess_Start(p);
}
//-----------------------------------------------------------------------------------------------------------
void DProcess_HandleMsg(DProcess_t *p, DMsgId_t msg, DMsgArg_t data)
{
  if ((p->State & PROCESS_STATE_RUNNING) && p->Handler != NULL)
  {
    u8 ret = p->Handler(p, msg, data);
    if (ret == PT_EXITED || ret == PT_ENDED)
      DProcess_ExitProc(p);
  }
}
//-----------------------------------------------------------------------------------------------------------
void DProcess_ExitProc(DProcess_t *p)
{
  if (DProcess_IsRunning(p))
  {
    p->State = PROCESS_STATE_NONE;

    os_sem_wait(ProcessListSem, OS_WAIT_FOREVER);
    DbList_Remove(&p->ProcessList);
    os_sem_signal(ProcessListSem);

    DMsg_Flush(p);
  }
}
//-----------------------------------------------------------------------------------------------------------
void DProcess_ExitAllProc(void)
{
  Db_List_t *tmp = MyProcessList.next;
  while (tmp != &MyProcessList)
  {
    DProcess_t *p = DContainerOf(tmp, DProcess_t, ProcessList);
    tmp = tmp->next;
    DProcess_ExitProc(p);
  }
}
//-----------------------------------------------------------------------------------------------------------
static void Do_Poll(void)
{
  PollRequested = 0;
  Db_List_t *tmp = MyProcessList.next;
  while (tmp != &MyProcessList)
  {
    DProcess_t *p = DContainerOf(tmp, DProcess_t, ProcessList);
    tmp = tmp->next;

    if (p->NeedPoll)
    {
      p->NeedPoll = 0;
      DTask_Store(p, SYS_MSG_POLL_PROCESS, NULL);
    }
  }
}
//-----------------------------------------------------------------------------------------------------------
void DProcess_Run(void)
{
  if (PollRequested)
    Do_Poll();
}
//-----------------------------------------------------------------------------------------------------------
void DProcess_Poll(DProcess_t *p)
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
bool DProcess_IsRunning(DProcess_t *p)
{
  return p->State == PROCESS_STATE_RUNNING;
}
//-----------------------------------------------------------------------------------------------------------
void *DProcess_CreateArg(DProcess_t *p)
{
  DProcessArg_t *arg = (DProcessArg_t *)DMem_Malloc(sizeof(DProcessArg_t));
  if (arg != NULL)
    arg->Process = p;

  return arg;
}
