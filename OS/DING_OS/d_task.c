/*!*****************************************************************************
 * file		task.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "d_task.h"
#include "d_mem.h"
#include "memb.h"
#include "message.h"
#include "osal.h"
#include "process.h"
#include "s_list.h"

// The task queue.
typedef struct TaskItems
{
  LIST_HEADER;
  Process_t *Process;
  MsgId_t    MsgId;
  MsgArg_t   Arg;
} TaskItem_t;

LIST(TaskElementList);

MEMB(TaskElementMem, TaskItem_t, TASK_ITEM_NUM);

#if (TASK_DEBUG == 1)
static u16 TaskMemAllocFailedNum = 0;
static u16 TaskMemAllocCurrentNum = 0;
static u16 TaskMemAllocMaxNum = 0;
#endif

static os_sem_t    *TaskPendingSem;
static os_sem_t    *TaskListSem;
static os_thread_t *TaskThread;
//-----------------------------------------------------------------------------------------------------------
static os_return_t DTask_Thread(void *arg)
{
  while (!os_thread_should_stop(TaskThread))
  {
    os_sem_wait(TaskPendingSem, OS_WAIT_FOREVER);

    DTask_RunAll();
  }

  OS_RETURN(TaskThread);
}
//-----------------------------------------------------------------------------------------------------------
void DTask_Init(void)
{
  List_Init(TaskElementList);
  Memb_Init(&TaskElementMem);

  TaskPendingSem = os_sem_create(0);
  TaskListSem = os_sem_create(1);
  TaskThread = os_thread_create("os_task", D_OS_TASK_PRIO, 1024, DTask_Thread, NULL);
}
//-----------------------------------------------------------------------------------------------------------
void DTask_Exit(void)
{
  os_thread_destroy(TaskThread);

  os_sem_destroy(TaskPendingSem);
  os_sem_destroy(TaskListSem);
}
//-----------------------------------------------------------------------------------------------------------
static TaskItem_t *AllocateElement(void)
{
  TaskItem_t *e = (TaskItem_t *)Memb_Alloc(&TaskElementMem);

#if (TASK_DEBUG == 1)
  if (e != NULL)
  {
    TaskMemAllocCurrentNum++;
    if (TaskMemAllocMaxNum < TaskMemAllocCurrentNum)
      TaskMemAllocMaxNum = TaskMemAllocCurrentNum;
  }
  else
  {
    TaskMemAllocFailedNum++;
  }
#endif

  return e;
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t DTask_Store(Process_t *process, MsgId_t msgId, MsgArg_t arg)
{
  TaskItem_t *e = AllocateElement();
  if (e != NULL)
  {
    e->Process = process;
    e->MsgId = msgId;
    e->Arg = arg;

    os_sem_wait(TaskListSem, OS_WAIT_FOREVER);
    List_Add(TaskElementList, e);
    os_sem_signal(TaskListSem);

    os_sem_signal(TaskPendingSem);

    return OS_ERR_OK;
  }

  return OS_ERR_ALLOC;
}
//-----------------------------------------------------------------------------------------------------------
static void FreeElement(TaskItem_t *e)
{
  Memb_Free(&TaskElementMem, e);

#if (TASK_DEBUG == 1)
  TaskMemAllocCurrentNum--;
#endif
}
//-----------------------------------------------------------------------------------------------------------
void DTask_Run(void)
{
  os_sem_wait(TaskListSem, OS_WAIT_FOREVER);
  TaskItem_t *e = List_Pop(TaskElementList);
  os_sem_signal(TaskListSem);

  if (e != NULL)
  {
    if (e->Process != NULL && e->MsgId != SYS_MSG_NONE)
      Process_HandleMsg(e->Process, e->MsgId, e->Arg);

    if (e->Arg != NULL)
      DMem_Free(e->Arg);

    FreeElement(e);
  }
}
//-----------------------------------------------------------------------------------------------------------
void DTask_RunAll(void)
{
  os_sem_wait(TaskListSem, OS_WAIT_FOREVER);
  TaskItem_t *e = List_Pop(TaskElementList);
  os_sem_signal(TaskListSem);

  while (e != NULL)
  {
    if (e->Process != NULL && e->MsgId != SYS_MSG_NONE)
      Process_HandleMsg(e->Process, e->MsgId, e->Arg);

    if (e->Arg != NULL)
      DMem_Free(e->Arg);

    FreeElement(e);

    os_sem_wait(TaskListSem, OS_WAIT_FOREVER);
    e = List_Pop(TaskElementList);
    os_sem_signal(TaskListSem);
  }
}
//-----------------------------------------------------------------------------------------------------------
void DTask_CancelMsg(Process_t *process, MsgId_t msgId)
{
  TaskItem_t *e;
  for (e = (TaskItem_t *)List_Head(TaskElementList); e != NULL; e = (TaskItem_t *)List_ItemNext(e))
  {
    if (e->Process == process && e->MsgId == msgId)
      e->MsgId = SYS_MSG_NONE;
  }
}
//-----------------------------------------------------------------------------------------------------------
bool DTask_IsMsgInTask(Process_t *process, MsgId_t msgId)
{
  TaskItem_t *e;
  for (e = (TaskItem_t *)List_Head(TaskElementList); e != NULL; e = (TaskItem_t *)List_ItemNext(e))
  {
    if (e->Process == process && e->MsgId == msgId)
      return true;
  }

  return false;
}
//-----------------------------------------------------------------------------------------------------------
void DTask_FlushMsg(Process_t *process)
{
  TaskItem_t *e;
  for (e = (TaskItem_t *)List_Head(TaskElementList); e != NULL; e = (TaskItem_t *)List_ItemNext(e))
  {
    if (e->Process == process)
      e->MsgId = SYS_MSG_NONE;
  }
}