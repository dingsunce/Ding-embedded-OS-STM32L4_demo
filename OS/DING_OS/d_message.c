/*!*****************************************************************************
 * file		message.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "d_message.h"
#include "d_list.h"
#include "d_mem.h"
#include "d_memb.h"
#include "d_task.h"
#include "double_list.h"
#include "osal.h"

typedef struct MsgTimer
{
  u16         MsgId;
  u32         TimePeriod;
  u32         TimeMatch;
  DMsgArg_t   Arg;
  DProcess_t *Process;
  Db_List_t   ListInTimerTable;
  Db_List_t   ListInProcess;
} MsgTimer_t;

typedef struct MsgTableItem
{
  Db_List_t TimerList;
} MsgTableItem_t;

DMEMB(MsgTimerMem, MsgTimer_t, DMSG_TIMER_NUM);

#if (DMESSAGE_DEBUG == 1)
static u16 MsgTimerMemAllocFailedNum = 0;
static u16 MsgTimerMemAllocCurrentNum = 0;
static u16 MsgTimerMemAllocMaxNum = 0;
#endif

static u32            TimerTick = 0;
static MsgTableItem_t MsgTimerTable[DMSG_TIMER_TABLE_SIZE];
static void           Msg_FreeTimer(MsgTimer_t *timer, bool freeArg);
static void           InsertToTimerTable(MsgTimer_t *timer);

static os_sem_t    *MsgPendingSem;
static os_sem_t    *MsgListSem;
static os_thread_t *MsgThread;
static bool         MsgRunning = false;
//-----------------------------------------------------------------------------------------------------------
static os_return_t Msg_Thread(void *arg)
{
  while (!os_thread_should_stop(MsgThread))
  {
    os_sem_wait(MsgPendingSem, OS_WAIT_FOREVER);

    if (os_thread_should_stop(MsgThread))
      break;

    DMsg_RunOneTick();
  }

  OS_RETURN(MsgThread);
}
//-----------------------------------------------------------------------------------------------------------
static void InitTimerTable(void)
{
  for (u8 i = 0; i < DMSG_TIMER_TABLE_SIZE; i++)
  {
    DbList_Init(&MsgTimerTable[i].TimerList);
  }
}
//-----------------------------------------------------------------------------------------------------------
void DMsg_Init(void)
{
  TimerTick = 0;
  InitTimerTable();
  DMemb_Init(&MsgTimerMem);

  MsgPendingSem = os_sem_create(0);
  MsgListSem = os_sem_create(1);
  MsgThread = os_thread_create("os_msg", D_OS_MSG_PRIO, 256, Msg_Thread, NULL);

  OS_PRINT("MsgThread Start\n");

  MsgRunning = true;
}
//-----------------------------------------------------------------------------------------------------------
void DMsg_Exit(void)
{
  MsgRunning = false;

  os_thread_destroy(MsgThread);

  os_sem_destroy(MsgPendingSem);
  os_sem_destroy(MsgListSem);

  os_thread_wait_for_completion(MsgThread);

  OS_PRINT("MsgThread Exit\n");
}
//-----------------------------------------------------------------------------------------------------------
void DMsg_PostSem(void)
{
  // interrupt would call DMsg_PostSem before the sem is set up, so check the MsgRunning flag
  if (MsgRunning)
    os_sem_signal(MsgPendingSem);
}
//-----------------------------------------------------------------------------------------------------------
void DMsg_RunOneTick(void)
{
  TimerTick++;

  u8              index = TimerTick % DMSG_TIMER_TABLE_SIZE;
  MsgTableItem_t *table = MsgTimerTable + index;

  os_sem_wait(MsgListSem, OS_WAIT_FOREVER);

  Db_List_t *tmp = table->TimerList.next;
  while (tmp != &table->TimerList)
  {
    MsgTimer_t *timer = DContainerOf(tmp, MsgTimer_t, ListInTimerTable);
    tmp = tmp->next;

    if (TimerTick == timer->TimeMatch)
    {
      DTask_Store(timer->Process, timer->MsgId, timer->Arg);

      if (timer->TimePeriod != 0)
      {
        // recalculate TimeMatch and decide which table timer it belongs to
        timer->TimeMatch = TimerTick + timer->TimePeriod;

        DbList_Remove(&timer->ListInTimerTable);
        InsertToTimerTable(timer);
      }
      else
      {
        DbList_Remove(&timer->ListInTimerTable);
        DbList_Remove(&timer->ListInProcess);

        // argument will be released in task
        Msg_FreeTimer(timer, false);
      }

      continue;
    }

    break;
  }

  os_sem_signal(MsgListSem);
}
//-----------------------------------------------------------------------------------------------------------
static MsgTimer_t *AllocateTimer(void)
{
  MsgTimer_t *timer;

  timer = (MsgTimer_t *)DMemb_Alloc(&MsgTimerMem);

#if (DMESSAGE_DEBUG == 1)
  if (timer != NULL)
  {
    MsgTimerMemAllocCurrentNum++;
    if (MsgTimerMemAllocMaxNum < MsgTimerMemAllocCurrentNum)
      MsgTimerMemAllocMaxNum = MsgTimerMemAllocCurrentNum;
  }
  else
  {
    MsgTimerMemAllocFailedNum++;
  }
#endif

  return timer;
}
//-----------------------------------------------------------------------------------------------------------
static void FreeTimer(MsgTimer_t *timer)
{
  DMemb_Free(&MsgTimerMem, timer);

#if (DMESSAGE_DEBUG == 1)
  if (MsgTimerMemAllocCurrentNum > 0)
    MsgTimerMemAllocCurrentNum--;
#endif
}
//-----------------------------------------------------------------------------------------------------------
static u32 RemainTime(MsgTimer_t *timer)
{
  return timer->TimeMatch - TimerTick;
}
//-----------------------------------------------------------------------------------------------------------
static void InsertToTimerTable(MsgTimer_t *timer)
{
  u8              index = timer->TimeMatch % DMSG_TIMER_TABLE_SIZE;
  MsgTableItem_t *table = MsgTimerTable + index;

  /*
    insert before header if table is empty or

    all items in table have a match value smaller than timer
  */
  Db_List_t *ListInsert = &table->TimerList;
  DbList_Init(&timer->ListInTimerTable);

  Db_List_t *tmp = table->TimerList.next;
  while (tmp != &table->TimerList)
  {
    MsgTimer_t *timerInTable = DContainerOf(tmp, MsgTimer_t, ListInTimerTable);
    if (RemainTime(timerInTable) > RemainTime(timer))
    {
      // insert an entry before tmp
      ListInsert = tmp;
      break;
    }

    tmp = tmp->next;
  }

  DbList_InsertBefore(ListInsert, &timer->ListInTimerTable);
}
//-----------------------------------------------------------------------------------------------------------
static OsErr_t Msg_Send(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg, u32 delay, u32 period)
{
  MsgTimer_t *timer = NULL;

  if (process == NULL || (!DProcess_IsRunning(process)))
  {
    if (arg != NULL)
      DMem_Free(arg);

    return OS_ERR_NULL;
  }

  if (delay == 0)
    return DMsg_SendInstant(process, msgId, arg);

  if (delay < OS_TICK_PERIOD_MS)
    delay = OS_TICK_PERIOD_MS;

  timer = AllocateTimer();
  if (timer != NULL)
  {
    timer->MsgId = msgId;
    timer->TimePeriod = period / OS_TICK_PERIOD_MS;
    timer->TimeMatch = TimerTick + delay / OS_TICK_PERIOD_MS;
    timer->Arg = arg;
    timer->Process = process;

    DbList_Init(&timer->ListInProcess);

    os_sem_wait(MsgListSem, OS_WAIT_FOREVER);

    DbList_Add(&process->TimerList, &timer->ListInProcess);
    InsertToTimerTable(timer);

    os_sem_signal(MsgListSem);

    return OS_ERR_OK;
  }

  return OS_ERR_ALLOC;
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t DMsg_SendLater(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg, u32 delay)
{
  return Msg_Send(process, msgId, arg, delay, 0);
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t DMsg_ReSendLater(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg, u32 delay)
{
  DMsg_CancelAll(process, msgId);
  return Msg_Send(process, msgId, arg, delay, 0);
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t DMsg_SendCycle(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg, u32 period)
{
  return Msg_Send(process, msgId, arg, period, period);
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t DMsg_ReSendCycle(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg, u32 period)
{
  DMsg_CancelAll(process, msgId);
  return Msg_Send(process, msgId, arg, period, period);
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t DMsg_SendInstant(DProcess_t *process, DMsgId_t msgId, DMsgArg_t arg)
{
  if (process == NULL || (!DProcess_IsRunning(process)))
  {
    if (arg != NULL)
      DMem_Free(arg);

    return OS_ERR_NULL;
  }

  return DTask_Store(process, msgId, arg);
}
//-----------------------------------------------------------------------------------------------------------
void DMsg_CancelFirst(DProcess_t *process, DMsgId_t msgId)
{
  if (process == NULL)
    return;

  os_sem_wait(MsgListSem, OS_WAIT_FOREVER);

  Db_List_t *tmp = process->TimerList.next;
  while (tmp != &process->TimerList)
  {
    MsgTimer_t *timer = DContainerOf(tmp, MsgTimer_t, ListInProcess);
    tmp = tmp->next;

    if (timer->MsgId == msgId)
    {
      DbList_Remove(&timer->ListInProcess);
      DbList_Remove(&timer->ListInTimerTable);
      Msg_FreeTimer(timer, true);
      break;
    }
  }

  os_sem_signal(MsgListSem);
}
//-----------------------------------------------------------------------------------------------------------
void DMsg_Cancel(DProcess_t *process, DMsgId_t msgId)
{
  DMsg_CancelAll(process, msgId);
}
//-----------------------------------------------------------------------------------------------------------
void DMsg_CancelAll(DProcess_t *process, DMsgId_t msgId)
{
  if (process == NULL)
    return;

  os_sem_wait(MsgListSem, OS_WAIT_FOREVER);

  Db_List_t *tmp = process->TimerList.next;
  while (tmp != &process->TimerList)
  {
    MsgTimer_t *timer = DContainerOf(tmp, MsgTimer_t, ListInProcess);
    tmp = tmp->next;

    if (timer->MsgId == msgId)
    {

      DbList_Remove(&timer->ListInProcess);
      DbList_Remove(&timer->ListInTimerTable);
      Msg_FreeTimer(timer, true);
    }
  }

  os_sem_signal(MsgListSem);

  DTask_CancelMsg(process, msgId);
}
//-----------------------------------------------------------------------------------------------------------
void DMsg_Flush(DProcess_t *process)
{
  if (process == NULL)
    return;

  os_sem_wait(MsgListSem, OS_WAIT_FOREVER);

  Db_List_t *tmp = process->TimerList.next;
  while (tmp != &process->TimerList)
  {
    MsgTimer_t *timer = DContainerOf(tmp, MsgTimer_t, ListInProcess);
    tmp = tmp->next;

    DbList_Remove(&timer->ListInProcess);
    DbList_Remove(&timer->ListInTimerTable);

    Msg_FreeTimer(timer, true);
  }

  os_sem_signal(MsgListSem);

  DTask_FlushMsg(process);
}
//-----------------------------------------------------------------------------------------------------------
static void Msg_FreeTimer(MsgTimer_t *timer, bool freeArg)
{
  if (timer == NULL)
    return;

  if (freeArg && timer->Arg != NULL)
    DMem_Free(timer->Arg);

  FreeTimer(timer);
}
//-----------------------------------------------------------------------------------------------------------
u32 DMsg_GetRemainTime(DProcess_t *process, DMsgId_t msgId)
{
  if (process == NULL)
    return 0;

  Db_List_t *tmp = process->TimerList.next;
  while (tmp != &process->TimerList)
  {
    MsgTimer_t *timer = DContainerOf(tmp, MsgTimer_t, ListInProcess);
    tmp = tmp->next;

    if (timer->MsgId == msgId)
      return timer->TimeMatch - TimerTick;
  }

  return 0;
}
//-----------------------------------------------------------------------------------------------------------
bool DMsg_IsMsgInProcess(DProcess_t *process, DMsgId_t msgId)
{
  if (process == NULL)
    return false;

  Db_List_t *tmp = process->TimerList.next;
  while (tmp != &process->TimerList)
  {
    MsgTimer_t *timer = DContainerOf(tmp, MsgTimer_t, ListInProcess);
    tmp = tmp->next;

    if (timer->MsgId == msgId)
      return true;
  }

  return false;
}