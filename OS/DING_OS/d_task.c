/*!*****************************************************************************
 * file		task.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "d_task.h"
#include "d_list.h"
#include "d_mem.h"
#include "d_memb.h"
#include "d_message.h"
#include "d_process.h"
#include "osal.h"

// The task queue.
typedef struct TaskItems
{
  DLIST_HEADER;
  DProcess_t *Process;
  DMsgId_t    MsgId;
  DMsgArg_t   Arg;
} TaskItem_t;

DLIST(TaskElementList);

DMEMB(TaskElementMem, TaskItem_t, DTASK_ITEM_NUM);

#if (DTASK_DEBUG == 1)
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

    if (os_thread_should_stop(TaskThread))
      break;

    DTask_RunAll();
  }

  OS_RETURN(TaskThread);
}
//-----------------------------------------------------------------------------------------------------------
void DTask_Init(void)
{
  DList_Init(TaskElementList);
  DMemb_Init(&TaskElementMem);

  TaskPendingSem = os_sem_create(0);
  TaskListSem = os_sem_create(1);
  TaskThread = os_thread_create("os_task", D_OS_TASK_PRIO, 1024, DTask_Thread, NULL);

  OS_PRINT("TaskThread Start\n");
}
//-----------------------------------------------------------------------------------------------------------
void DTask_Exit(void)
{
  // firstly set the stop flag(if run in linux, linux kernel, windows)
  os_thread_destroy(TaskThread);

  // then wake up the thread to exit
  os_sem_destroy(TaskPendingSem);
  os_sem_destroy(TaskListSem);

  // finnaly wait for thread exit
  os_thread_wait_for_completion(TaskThread);

  OS_PRINT("TaskThread Exit\n");
}
//-----------------------------------------------------------------------------------------------------------
static TaskItem_t *AllocateElement(void)
{
  TaskItem_t *e = (TaskItem_t *)DMemb_Alloc(&TaskElementMem);

#if (DTASK_DEBUG == 1)
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
OsErr_t DTask_Store(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg)
{
  TaskItem_t *e = AllocateElement();
  if (e != NULL)
  {
    e->Process = process;
    e->MsgId = msgId;
    e->Arg = arg;

    os_sem_wait(TaskListSem, OS_WAIT_FOREVER);
    DList_Add(TaskElementList, e);
    os_sem_signal(TaskListSem);

    os_sem_signal(TaskPendingSem);

    return OS_ERR_OK;
  }

  return OS_ERR_ALLOC;
}
//-----------------------------------------------------------------------------------------------------------
static void FreeElement(TaskItem_t *e)
{
  DMemb_Free(&TaskElementMem, e);

#if (DTASK_DEBUG == 1)
  TaskMemAllocCurrentNum--;
#endif
}
//-----------------------------------------------------------------------------------------------------------
void DTask_Run(void)
{
  os_sem_wait(TaskListSem, OS_WAIT_FOREVER);
  TaskItem_t *e = DList_Pop(TaskElementList);
  os_sem_signal(TaskListSem);

  if (e != NULL)
  {
    if (e->Process != NULL && e->MsgId != SYS_MSG_NONE)
      DProcess_HandleMsg(e->Process, e->MsgId, e->Arg);

    if (e->Arg != NULL)
      DMem_Free(e->Arg);

    FreeElement(e);
  }
}
//-----------------------------------------------------------------------------------------------------------
void DTask_RunAll(void)
{
  os_sem_wait(TaskListSem, OS_WAIT_FOREVER);
  TaskItem_t *e = DList_Pop(TaskElementList);
  os_sem_signal(TaskListSem);

  while (e != NULL)
  {
    if (e->Process != NULL && e->MsgId != SYS_MSG_NONE)
      DProcess_HandleMsg(e->Process, e->MsgId, e->Arg);

    if (e->Arg != NULL)
      DMem_Free(e->Arg);

    FreeElement(e);

    os_sem_wait(TaskListSem, OS_WAIT_FOREVER);
    e = DList_Pop(TaskElementList);
    os_sem_signal(TaskListSem);
  }
}
//-----------------------------------------------------------------------------------------------------------
void DTask_CancelMsg(DProcess_t *process, DMsgId_t msgId)
{
  TaskItem_t *e;
  for (e = (TaskItem_t *)DList_Head(TaskElementList); e != NULL;
       e = (TaskItem_t *)DList_ItemNext(e))
  {
    if (e->Process == process && e->MsgId == msgId)
      e->MsgId = SYS_MSG_NONE;
  }
}
//-----------------------------------------------------------------------------------------------------------
bool DTask_IsMsgInTask(DProcess_t *process, DMsgId_t msgId)
{
  TaskItem_t *e;
  for (e = (TaskItem_t *)DList_Head(TaskElementList); e != NULL;
       e = (TaskItem_t *)DList_ItemNext(e))
  {
    if (e->Process == process && e->MsgId == msgId)
      return true;
  }

  return false;
}
//-----------------------------------------------------------------------------------------------------------
void DTask_FlushMsg(DProcess_t *process)
{
  TaskItem_t *e;
  for (e = (TaskItem_t *)DList_Head(TaskElementList); e != NULL;
       e = (TaskItem_t *)DList_ItemNext(e))
  {
    if (e->Process == process)
      e->MsgId = SYS_MSG_NONE;
  }
}