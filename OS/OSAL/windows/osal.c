/*!*****************************************************************************
 * file		osal.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "osal.h"
#include "d_mem.h"

#define MS_PER_SECOND (1000)
#define NS_PER_MS     (1000 * 1000)
#define NS_PER_SECOND (1000 * 1000 * 1000)

#define URESOLUTION 1 // 1 ms resolution
//-----------------------------------------------------------------------------------------------------------
void os_init(void)
{
  DMem_Init();
}
//-----------------------------------------------------------------------------------------------------------
void os_start(void)
{
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
  os_thread_t *thread = os_malloc(sizeof(*thread));
  if (thread == NULL)
    return NULL;

  HANDLE handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)entry, (LPVOID)arg, 0, NULL);
  if (handle == NULL)
  {
    os_free(thread);
    return NULL;
  }

  thread->handle = handle;
  thread->should_stop = FALSE;

  if (priority < 5)
    SetThreadPriority(handle, THREAD_PRIORITY_BELOW_NORMAL);
  else if (priority >= 15)
    SetThreadPriority(handle, THREAD_PRIORITY_TIME_CRITICAL);

  return thread;
}
//-----------------------------------------------------------------------------------------------------------
void os_thread_destroy(os_thread_t *thread)
{
  thread->should_stop = TRUE;
}
//-----------------------------------------------------------------------------------------------------------
bool os_thread_should_stop(os_thread_t *thread)
{
  return thread->should_stop;
}
//-----------------------------------------------------------------------------------------------------------
os_mutex_t *os_mutex_create(void)
{
  return CreateMutex(NULL, FALSE, NULL);
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_lock(os_mutex_t *mutex)
{
  WaitForSingleObject(mutex, INFINITE);
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_unlock(os_mutex_t *mutex)
{
  ReleaseMutex(mutex);
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_destroy(os_mutex_t *mutex)
{
  CloseHandle(mutex);
}
//-----------------------------------------------------------------------------------------------------------
os_sem_t *os_sem_create(u16 count)
{
  os_sem_t *sem;

  sem = (os_sem_t *)os_malloc(sizeof(*sem));

  InitializeConditionVariable(&sem->condition);
  InitializeCriticalSection(&sem->lock);
  sem->count = count;

  return sem;
}
//-----------------------------------------------------------------------------------------------------------
bool os_sem_wait(os_sem_t *sem, u32 ms)
{
  BOOL success = TRUE;

  EnterCriticalSection(&sem->lock);
  while (sem->count == 0)
  {
    success = SleepConditionVariableCS(&sem->condition, &sem->lock, ms);
    if (!success && GetLastError() == ERROR_TIMEOUT)
      goto timeout;
  }

  sem->count--;

timeout:
  LeaveCriticalSection(&sem->lock);
  return success;
}
//-----------------------------------------------------------------------------------------------------------
void os_sem_signal(os_sem_t *sem)
{
  EnterCriticalSection(&sem->lock);
  sem->count++;
  LeaveCriticalSection(&sem->lock);
  WakeAllConditionVariable(&sem->condition);
}
//-----------------------------------------------------------------------------------------------------------
void os_sem_destroy(os_sem_t *sem)
{
  WakeAllConditionVariable(&sem->condition);

  os_free(sem);
}
//-----------------------------------------------------------------------------------------------------------
os_event_t *os_event_create(void)
{
  os_event_t *event;

  event = (os_event_t *)os_malloc(sizeof(*event));

  InitializeConditionVariable(&event->condition);
  InitializeCriticalSection(&event->lock);
  event->flags = 0;

  return event;
}
//-----------------------------------------------------------------------------------------------------------
bool os_event_wait(os_event_t *event, u32 mask, u32 *value, u32 ms)
{
  BOOL success = TRUE;

  EnterCriticalSection(&event->lock);
  while ((event->flags & mask) == 0)
  {
    success = SleepConditionVariableCS(&event->condition, &event->lock, ms);
    if (!success && GetLastError() == ERROR_TIMEOUT)
      break;
  }

  *value = event->flags & mask;
  LeaveCriticalSection(&event->lock);

  return success;
}
//-----------------------------------------------------------------------------------------------------------
void os_event_set(os_event_t *event, u32 value)
{
  EnterCriticalSection(&event->lock);
  event->flags |= value;
  LeaveCriticalSection(&event->lock);
  WakeAllConditionVariable(&event->condition);
}
//-----------------------------------------------------------------------------------------------------------
void os_event_clr(os_event_t *event, u32 value)
{
  EnterCriticalSection(&event->lock);
  event->flags &= ~value;
  LeaveCriticalSection(&event->lock);
}
//-----------------------------------------------------------------------------------------------------------
void os_event_destroy(os_event_t *event)
{
  WakeAllConditionVariable(&event->condition);

  os_free(event);
}
//-----------------------------------------------------------------------------------------------------------
os_mbox_t *os_mbox_create(u32 size)
{
  os_mbox_t *mbox;

  mbox = (os_mbox_t *)os_malloc(sizeof(*mbox) + size * sizeof(void *));

  InitializeConditionVariable(&mbox->condition);
  InitializeCriticalSection(&mbox->lock);

  mbox->r = 0;
  mbox->w = 0;
  mbox->count = 0;
  mbox->size = size;

  return mbox;
}
//-----------------------------------------------------------------------------------------------------------
bool os_mbox_fetch(os_mbox_t *mbox, void **msg, u32 ms)
{
  BOOL success = TRUE;

  EnterCriticalSection(&mbox->lock);
  while (mbox->count == 0)
  {
    success = SleepConditionVariableCS(&mbox->condition, &mbox->lock, ms);
    if (!success && GetLastError() == ERROR_TIMEOUT)
      goto timeout;
  }

  *msg = mbox->msg[mbox->r++];
  if (mbox->r == mbox->size)
    mbox->r = 0;

  mbox->count--;

timeout:
  LeaveCriticalSection(&mbox->lock);
  WakeAllConditionVariable(&mbox->condition);

  return success;
}
//-----------------------------------------------------------------------------------------------------------
bool os_mbox_post(os_mbox_t *mbox, void *msg, u32 ms)
{
  BOOL success = TRUE;

  EnterCriticalSection(&mbox->lock);
  while (mbox->count == mbox->size)
  {
    success = SleepConditionVariableCS(&mbox->condition, &mbox->lock, ms);
    if (!success && GetLastError() == ERROR_TIMEOUT)
      goto timeout;
  }

  mbox->msg[mbox->w++] = msg;
  if (mbox->w == mbox->size)
    mbox->w = 0;

  mbox->count++;

timeout:
  LeaveCriticalSection(&mbox->lock);
  WakeAllConditionVariable(&mbox->condition);

  return success;
}
//-----------------------------------------------------------------------------------------------------------
void os_mbox_destroy(os_mbox_t *mbox)
{
  WakeAllConditionVariable(&mbox->condition);

  os_free(mbox);
}
//-----------------------------------------------------------------------------------------------------------
void os_msleep(u32 ms)
{
  Sleep(ms);
}
//-----------------------------------------------------------------------------------------------------------
static uint64_t os_get_ticks_per_ms(void)
{
  static uint64_t ticksPerMs;
  if (ticksPerMs == 0)
  {
    LARGE_INTEGER performanceFrequency;
    QueryPerformanceFrequency(&performanceFrequency);
    // QuadPart is tick number per second
    ticksPerMs = performanceFrequency.QuadPart / 1000;

    return ticksPerMs;
  }

  return ticksPerMs;
}
//-----------------------------------------------------------------------------------------------------------
os_tick_t os_tick_current(void)
{
  LARGE_INTEGER currentCount;
  QueryPerformanceCounter(&currentCount);

  return currentCount.QuadPart;
}
//-----------------------------------------------------------------------------------------------------------
u32 os_ms_current(void)
{
  return (u32)(os_tick_current() / os_get_ticks_per_ms());
}
//-----------------------------------------------------------------------------------------------------------
os_tick_t os_tick_from_ms(u32 ms)
{
  return os_get_ticks_per_ms() * ms;
}
//-----------------------------------------------------------------------------------------------------------
void os_tick_sleep(os_tick_t tick)
{
  os_msleep(tick / os_get_ticks_per_ms());
}
//-----------------------------------------------------------------------------------------------------------
static VOID CALLBACK timer_callback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1,
                                    DWORD_PTR dw2)
{
  os_timer_t *timer = (os_timer_t *)dwUser;

  if (timer->fn)
    timer->fn(timer, timer->arg);
}
//-----------------------------------------------------------------------------------------------------------
os_timer_t *os_timer_create(u32 ms, void (*fn)(os_timer_t *, void *arg), void *arg, bool oneshot)
{
  os_timer_t *timer;

  timer = (os_timer_t *)os_malloc(sizeof(*timer));

  timer->fn = fn;
  timer->arg = arg;
  timer->ms = ms;
  timer->oneshot = oneshot;

  return timer;
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_set(os_timer_t *timer, u32 ms)
{
  timer->ms = ms;
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_start(os_timer_t *timer)
{
  timeBeginPeriod(URESOLUTION);

  timer->timerID = timeSetEvent(timer->ms, URESOLUTION, timer_callback, (DWORD_PTR)timer,
                                (timer->oneshot) ? TIME_ONESHOT : TIME_PERIODIC);
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_stop(os_timer_t *timer)
{
  timeKillEvent(timer->timerID);

  timeEndPeriod(URESOLUTION);
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_destroy(os_timer_t *timer)
{
  os_free(timer);
}
