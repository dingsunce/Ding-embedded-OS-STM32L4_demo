/*!*****************************************************************************
 * file		osal.c
 * $Author: sunce.ding
 *******************************************************************************/

#include "osal.h"
#include "d_mem.h"
#include "lib_math.h"
#include <stdlib.h>

static OS_ERR os_err;
static OS_ERR sem_err;
static OS_ERR mutex_err;
static OS_ERR event_err;
static OS_ERR mbox_err;
static OS_ERR tick_err;
static OS_ERR timer_err;
#define TICK_PERIOD_MS (1000 / OS_CFG_TICK_RATE_HZ)
//-----------------------------------------------------------------------------------------------------------
void os_init(void)
{
  OSInit(&os_err);

  CPU_Init();
  Mem_Init();
  Math_Init();

  DMem_Init();
}
//-----------------------------------------------------------------------------------------------------------
void os_start(void)
{
  OSStart(&os_err);
}
//-----------------------------------------------------------------------------------------------------------
void *os_malloc(u16 size)
{
  return DMem_Malloc(size);
}
//-----------------------------------------------------------------------------------------------------------
void os_free(void *ptr)
{
  DMem_Free(ptr);
}
//-----------------------------------------------------------------------------------------------------------
os_thread_t *os_thread_create(char *name, u16 priority, u16 stacksize, os_entry_t entry, void *arg)
{
  OS_TCB *thread = os_malloc(sizeof(OS_TCB));
  if (thread == NULL)
    return NULL;

  void *stack = os_malloc(stacksize * sizeof(CPU_STK));
  if (stack == NULL)
  {
    os_free(thread);
    return NULL;
  }

  OSTaskCreate(thread, name, entry, arg, priority, stack, stacksize / 10, stacksize, 0, 0, 0,
               (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &os_err);

  if (os_err != OS_ERR_NONE)
  {
    os_free(stack);
    os_free(thread);
    thread = NULL;
  }

  return thread;
}
//-----------------------------------------------------------------------------------------------------------
void os_thread_destroy(os_thread_t *thread)
{
  OSTaskDel(thread, &os_err);
  os_free(thread);
}
//-----------------------------------------------------------------------------------------------------------
bool os_thread_should_stop(os_thread_t *thread)
{
  return false;
}
//-----------------------------------------------------------------------------------------------------------
os_mutex_t *os_mutex_create(void)
{
  os_mutex_t *mutex = os_malloc(sizeof(os_mutex_t));
  if (mutex == NULL)
    return NULL;

  OSMutexCreate(mutex, "mutex", &mutex_err);
  if (mutex_err != OS_ERR_NONE)
  {
    os_free(mutex);
    mutex = NULL;
  }

  return mutex;
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_lock(os_mutex_t *mutex)
{
  CPU_TS time_stamp;
  OSMutexPend(mutex, 0, OS_OPT_PEND_BLOCKING, &time_stamp, &mutex_err);
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_unlock(os_mutex_t *mutex)
{
  OSMutexPost(mutex, OS_OPT_POST_NONE, &mutex_err);
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_destroy(os_mutex_t *mutex)
{
  OSMutexDel(mutex, OS_OPT_DEL_ALWAYS, &mutex_err);

  os_free(mutex);
}
//-----------------------------------------------------------------------------------------------------------
os_sem_t *os_sem_create(u16 count)
{
  os_sem_t *sem = os_malloc(sizeof(os_sem_t));
  if (sem == NULL)
    return NULL;

  OSSemCreate(sem, "semaphore", count, &sem_err);
  if (sem_err != OS_ERR_NONE)
  {
    os_free(sem);
    sem = NULL;
  }

  return sem;
}
//-----------------------------------------------------------------------------------------------------------
bool os_sem_wait(os_sem_t *sem, u32 ms)
{
  CPU_TS time_stamp;

  if (ms == OS_WAIT_FOREVER)
    OSSemPend(sem, 0, OS_OPT_PEND_BLOCKING, &time_stamp, &sem_err);
  else
    OSSemPend(sem, ms / TICK_PERIOD_MS, OS_OPT_PEND_BLOCKING, &time_stamp, &sem_err);

  if (sem_err != OS_ERR_NONE)
    return false;

  return true;
}
//-----------------------------------------------------------------------------------------------------------
void os_sem_signal(os_sem_t *sem)
{
  OSSemPost(sem, OS_OPT_POST_1, &sem_err);
}
//-----------------------------------------------------------------------------------------------------------
void os_sem_destroy(os_sem_t *sem)
{
  OSSemDel(sem, OS_OPT_DEL_ALWAYS, &sem_err);

  os_free(sem);
}
//-----------------------------------------------------------------------------------------------------------
os_event_t *os_event_create(void)
{
  os_event_t *event = os_malloc(sizeof(os_event_t));
  if (event == NULL)
    return NULL;

  OSFlagCreate(event, "event", 0, &event_err);
  if (event_err != OS_ERR_NONE)
  {
    os_free(event);
    event = NULL;
  }

  return event;
}
//-----------------------------------------------------------------------------------------------------------
bool os_event_wait(os_event_t *event, u32 mask, u32 *value, u32 ms)
{
  CPU_TS time_stamp;

  if (ms == OS_WAIT_FOREVER)
    *value = OSFlagPend(event, mask, 0, OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_BLOCKING,
                        &time_stamp, &event_err);
  else
    *value = OSFlagPend(event, mask, ms / TICK_PERIOD_MS,
                        OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_BLOCKING, &time_stamp, &event_err);

  if (event_err != OS_ERR_NONE)
    return false;

  *value &= mask;

  return true;
}
//-----------------------------------------------------------------------------------------------------------
void os_event_set(os_event_t *event, u32 value)
{
  OSFlagPost(event, value, OS_OPT_POST_FLAG_SET, &event_err);
}
//-----------------------------------------------------------------------------------------------------------
void os_event_clr(os_event_t *event, u32 value)
{
  OSFlagPost(event, value, OS_OPT_POST_FLAG_CLR, &event_err);
}
//-----------------------------------------------------------------------------------------------------------
void os_event_destroy(os_event_t *event)
{
  OSFlagDel(event, OS_OPT_DEL_ALWAYS, &event_err);

  os_free(event);
}
//-----------------------------------------------------------------------------------------------------------
os_mbox_t *os_mbox_create(u32 size)
{
  os_mbox_t *mbox = os_malloc(sizeof(os_mbox_t));
  if (mbox == NULL)
    return NULL;

  OSQCreate(mbox, "mbox", size, &mbox_err);

  if (mbox_err != OS_ERR_NONE)
  {
    os_free(mbox);
    mbox = NULL;
  }

  return mbox;
}
//-----------------------------------------------------------------------------------------------------------
bool os_mbox_fetch(os_mbox_t *mbox, void **msg, u32 ms)
{
  OS_MSG_SIZE msg_size;
  CPU_TS      time_stamp;

  if (ms == OS_WAIT_FOREVER)
    *msg = OSQPend(mbox, 0, OS_OPT_PEND_BLOCKING, &msg_size, &time_stamp, &mbox_err);
  else
    *msg =
        OSQPend(mbox, ms / TICK_PERIOD_MS, OS_OPT_PEND_BLOCKING, &msg_size, &time_stamp, &mbox_err);

  if (mbox_err != OS_ERR_NONE)
    return false;

  return true;
}
//-----------------------------------------------------------------------------------------------------------
bool os_mbox_post(os_mbox_t *mbox, void *msg, u32 ms)
{
  OSQPost(mbox, msg, 1, OS_OPT_POST_FIFO, &mbox_err);

  if (mbox_err != OS_ERR_NONE)
    return false;

  return true;
}
//-----------------------------------------------------------------------------------------------------------
void os_mbox_destroy(os_mbox_t *mbox)
{
  OSQDel(mbox, OS_OPT_DEL_ALWAYS, &mbox_err);

  os_free(mbox);
}
//-----------------------------------------------------------------------------------------------------------
void os_msleep(u32 ms)
{
  OSTimeDly(ms / TICK_PERIOD_MS, OS_OPT_TIME_DLY, &tick_err);
}
//-----------------------------------------------------------------------------------------------------------
void os_tick_sleep(os_tick_t tick)
{
  OSTimeDly(tick, OS_OPT_TIME_DLY, &tick_err);
}
//-----------------------------------------------------------------------------------------------------------
os_tick_t os_tick_current(void)
{
  return OSTimeGet(&tick_err);
}
//-----------------------------------------------------------------------------------------------------------
u32 os_ms_current(void)
{
  return os_tick_current() / TICK_PERIOD_MS;
}
//-----------------------------------------------------------------------------------------------------------
os_tick_t os_tick_from_ms(u32 ms)
{
  return ms / TICK_PERIOD_MS;
}
//-----------------------------------------------------------------------------------------------------------
os_timer_t *os_timer_create(u32 ms, void (*fn)(os_timer_t *, void *arg), void *arg, bool oneshot)
{
  os_timer_t *timer;

  timer = os_malloc(sizeof(os_timer_t));
  if (timer == NULL)
    return NULL;

  if (oneshot)
    OSTmrCreate(timer, "timer", os_tick_from_ms(ms), 0, OS_OPT_TMR_ONE_SHOT, fn, arg, &timer_err);
  else
    OSTmrCreate(timer, "timer", os_tick_from_ms(ms), os_tick_from_ms(ms), OS_OPT_TMR_PERIODIC, fn,
                arg, &timer_err);

  if (timer_err != OS_ERR_NONE)
  {
    os_free(timer);
    timer = NULL;
  }

  return timer;
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_set(os_timer_t *timer, u32 ms)
{
  timer->Dly = os_tick_from_ms(ms);
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_start(os_timer_t *timer)
{
  OSTmrStart(timer, &timer_err);
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_stop(os_timer_t *timer)
{
  OSTmrStop(timer, OS_OPT_TMR_NONE, NULL, &timer_err);
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_destroy(os_timer_t *timer)
{
  OSTmrDel(timer, &timer_err);
  os_free(timer);
}
