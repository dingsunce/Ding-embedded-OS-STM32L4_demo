/*!*****************************************************************************
 * file		osal.c
 * $Author: sunce.ding
 *******************************************************************************/

#define _GNU_SOURCE /* For pthread_setname_mp() */

#include "osal.h"
#include "d_mem.h"
/* #include "options.h" */

#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include <sys/syscall.h>

#define TIMER_PRIO 15

#define MS_PER_SECOND (1000)
#define NS_PER_MS     (1000 * 1000)
#define NS_PER_SECOND (1000 * 1000 * 1000)

#define TICK_PERIOD_MS 1
//-----------------------------------------------------------------------------------------------------------
void os_init(void)
{
  // pthread_cancel() will cancel the thread immediately
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

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
  int            result;
  pthread_t     *thread;
  pthread_attr_t attr;

  thread = os_malloc(sizeof(*thread));
  if (thread == NULL)
    return NULL;

  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN + stacksize);

  // set priority needs root privilege
  struct sched_param param = {.sched_priority = priority};
  pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
  pthread_attr_setschedparam(&attr, &param);

  result = pthread_create(thread, &attr, (void *)entry, arg);
  if (result != 0)
  {
    os_free(thread);
    return NULL;
  }

  pthread_setname_np(*thread, name);
  return thread;
}
//-----------------------------------------------------------------------------------------------------------
void os_thread_destroy(os_thread_t *thread)
{
  // The pthread_cancel() function sends a cancelation request to the thread thread
  pthread_cancel((pthread_t)thread);
}
//-----------------------------------------------------------------------------------------------------------
bool os_thread_should_stop(os_thread_t *thread)
{
  // pthread_testcancel() creates a cancelation point within the calling thread
  pthread_testcancel();

  return false;
}
//-----------------------------------------------------------------------------------------------------------
os_mutex_t *os_mutex_create(void)
{
  int                 result;
  pthread_mutex_t    *mutex;
  pthread_mutexattr_t mattr;

  mutex = os_malloc(sizeof(*mutex));
  if (mutex == NULL)
    return NULL;

  pthread_mutexattr_init(&mattr);
  pthread_mutexattr_setprotocol(&mattr, PTHREAD_PRIO_INHERIT);
  pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);

  result = pthread_mutex_init(mutex, &mattr);
  if (result != 0)
  {
    os_free(mutex);
    return NULL;
  }

  return mutex;
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_lock(os_mutex_t *_mutex)
{
  pthread_mutex_t *mutex = _mutex;
  pthread_mutex_lock(mutex);
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_unlock(os_mutex_t *_mutex)
{
  pthread_mutex_t *mutex = _mutex;
  pthread_mutex_unlock(mutex);
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_destroy(os_mutex_t *_mutex)
{
  pthread_mutex_t *mutex = _mutex;

  os_mutex_unlock(mutex);
  pthread_mutex_destroy(mutex);

  os_free(mutex);
}
//-----------------------------------------------------------------------------------------------------------
os_sem_t *os_sem_create(u16 count)
{
  os_sem_t           *sem;
  pthread_mutexattr_t mattr;
  pthread_condattr_t  cattr;

  sem = os_malloc(sizeof(*sem));
  if (sem == NULL)
    return NULL;

  pthread_condattr_init(&cattr);
  pthread_condattr_setclock(&cattr, CLOCK_MONOTONIC);
  pthread_cond_init(&sem->cond, &cattr);
  pthread_mutexattr_init(&mattr);
  pthread_mutexattr_setprotocol(&mattr, PTHREAD_PRIO_INHERIT);
  pthread_mutex_init(&sem->mutex, &mattr);
  sem->count = count;

  return sem;
}
//-----------------------------------------------------------------------------------------------------------
bool os_sem_wait(os_sem_t *sem, u32 ms)
{
  struct timespec ts;
  int             error = 0;
  uint64_t        nsec = (uint64_t)ms * NS_PER_MS;

  if (ms != OS_WAIT_FOREVER)
  {
    clock_gettime(CLOCK_MONOTONIC, &ts);
    nsec += ts.tv_nsec;

    ts.tv_sec += nsec / NS_PER_SECOND;
    ts.tv_nsec = nsec % NS_PER_SECOND;
  }

  pthread_mutex_lock(&sem->mutex);
  while (sem->count == 0)
  {
    if (ms != OS_WAIT_FOREVER)
    {
      error = pthread_cond_timedwait(&sem->cond, &sem->mutex, &ts);
      if (error)
        goto timeout;
    }
    else
    {
      error = pthread_cond_wait(&sem->cond, &sem->mutex);
    }
  }

  sem->count--;

timeout:
  pthread_mutex_unlock(&sem->mutex);
  return (error == 0);
}
//-----------------------------------------------------------------------------------------------------------
void os_sem_signal(os_sem_t *sem)
{
  pthread_mutex_lock(&sem->mutex);
  sem->count++;
  pthread_mutex_unlock(&sem->mutex);

  pthread_cond_signal(&sem->cond);
}
//-----------------------------------------------------------------------------------------------------------
void os_sem_destroy(os_sem_t *sem)
{
  pthread_cond_broadcast(&sem->cond);

  pthread_cond_destroy(&sem->cond);
  pthread_mutex_destroy(&sem->mutex);

  os_free(sem);
}
//-----------------------------------------------------------------------------------------------------------
os_event_t *os_event_create(void)
{
  os_event_t         *event;
  pthread_mutexattr_t mattr;
  pthread_condattr_t  cattr;

  event = (os_event_t *)os_malloc(sizeof(*event));
  if (event == NULL)
    return NULL;

  pthread_condattr_init(&cattr);
  pthread_condattr_setclock(&cattr, CLOCK_MONOTONIC);
  pthread_cond_init(&event->cond, &cattr);
  pthread_mutexattr_init(&mattr);
  pthread_mutexattr_setprotocol(&mattr, PTHREAD_PRIO_INHERIT);
  pthread_mutex_init(&event->mutex, &mattr);
  event->flags = 0;

  return event;
}
//-----------------------------------------------------------------------------------------------------------
bool os_event_wait(os_event_t *event, u32 mask, u32 *value, u32 ms)
{
  struct timespec ts;
  int             error = 0;
  uint64_t        nsec = (uint64_t)ms * NS_PER_MS;

  if (ms != OS_WAIT_FOREVER)
  {
    clock_gettime(CLOCK_MONOTONIC, &ts);
    nsec += ts.tv_nsec;

    ts.tv_sec += nsec / NS_PER_SECOND;
    ts.tv_nsec = nsec % NS_PER_SECOND;
  }

  pthread_mutex_lock(&event->mutex);

  while ((event->flags & mask) == 0)
  {
    if (ms != OS_WAIT_FOREVER)
    {
      error = pthread_cond_timedwait(&event->cond, &event->mutex, &ts);
      if (error)
        goto timeout;
    }
    else
    {
      error = pthread_cond_wait(&event->cond, &event->mutex);
    }
  }

timeout:
  *value = event->flags & mask;
  pthread_mutex_unlock(&event->mutex);

  return (error == 0);
}
//-----------------------------------------------------------------------------------------------------------
void os_event_set(os_event_t *event, u32 value)
{
  pthread_mutex_lock(&event->mutex);
  event->flags |= value;
  pthread_mutex_unlock(&event->mutex);

  pthread_cond_signal(&event->cond);
}
//-----------------------------------------------------------------------------------------------------------
void os_event_clr(os_event_t *event, u32 value)
{
  pthread_mutex_lock(&event->mutex);
  event->flags &= ~value;
  pthread_mutex_unlock(&event->mutex);

  pthread_cond_signal(&event->cond);
}
//-----------------------------------------------------------------------------------------------------------
void os_event_destroy(os_event_t *event)
{
  pthread_cond_broadcast(&event->cond);

  pthread_cond_destroy(&event->cond);
  pthread_mutex_destroy(&event->mutex);
  os_free(event);
}
//-----------------------------------------------------------------------------------------------------------
os_mbox_t *os_mbox_create(u32 size)
{
  os_mbox_t          *mbox;
  pthread_mutexattr_t mattr;
  pthread_condattr_t  cattr;

  mbox = (os_mbox_t *)os_malloc(sizeof(*mbox) + size * sizeof(void *));
  if (mbox == NULL)
    return NULL;

  pthread_condattr_init(&cattr);
  pthread_condattr_setclock(&cattr, CLOCK_MONOTONIC);
  pthread_cond_init(&mbox->cond, &cattr);
  pthread_mutexattr_init(&mattr);
  pthread_mutexattr_setprotocol(&mattr, PTHREAD_PRIO_INHERIT);
  pthread_mutex_init(&mbox->mutex, &mattr);

  mbox->r = 0;
  mbox->w = 0;
  mbox->count = 0;
  mbox->size = size;

  return mbox;
}
//-----------------------------------------------------------------------------------------------------------
bool os_mbox_fetch(os_mbox_t *mbox, void **msg, u32 ms)
{
  struct timespec ts;
  int             error = 0;
  uint64_t        nsec = (uint64_t)ms * NS_PER_MS;

  if (ms != OS_WAIT_FOREVER)
  {
    clock_gettime(CLOCK_MONOTONIC, &ts);
    nsec += ts.tv_nsec;

    ts.tv_sec += nsec / NS_PER_SECOND;
    ts.tv_nsec = nsec % NS_PER_SECOND;
  }

  pthread_mutex_lock(&mbox->mutex);

  while (mbox->count == 0)
  {
    if (ms != OS_WAIT_FOREVER)
    {
      error = pthread_cond_timedwait(&mbox->cond, &mbox->mutex, &ts);
      if (error)
        goto timeout;
    }
    else
    {
      error = pthread_cond_wait(&mbox->cond, &mbox->mutex);
    }
  }

  *msg = mbox->msg[mbox->r++];
  if (mbox->r == mbox->size)
    mbox->r = 0;

  mbox->count--;

timeout:
  pthread_mutex_unlock(&mbox->mutex);
  pthread_cond_signal(&mbox->cond);

  return (error == 0);
}
//-----------------------------------------------------------------------------------------------------------
bool os_mbox_post(os_mbox_t *mbox, void *msg, u32 ms)
{
  struct timespec ts;
  int             error = 0;
  uint64_t        nsec = (uint64_t)ms * NS_PER_MS;

  if (ms != OS_WAIT_FOREVER)
  {
    clock_gettime(CLOCK_MONOTONIC, &ts);
    nsec += ts.tv_nsec;

    ts.tv_sec += nsec / NS_PER_SECOND;
    ts.tv_nsec = nsec % NS_PER_SECOND;
  }

  pthread_mutex_lock(&mbox->mutex);

  while (mbox->count == mbox->size)
  {
    if (ms != OS_WAIT_FOREVER)
    {
      error = pthread_cond_timedwait(&mbox->cond, &mbox->mutex, &ts);
      if (error)
        goto timeout;
    }
    else
    {
      error = pthread_cond_wait(&mbox->cond, &mbox->mutex);
    }
  }

  mbox->msg[mbox->w++] = msg;
  if (mbox->w == mbox->size)
    mbox->w = 0;

  mbox->count++;

timeout:
  pthread_mutex_unlock(&mbox->mutex);
  pthread_cond_signal(&mbox->cond);

  return (error == 0);
}
//-----------------------------------------------------------------------------------------------------------
void os_mbox_destroy(os_mbox_t *mbox)
{
  pthread_cond_broadcast(&mbox->cond);

  pthread_cond_destroy(&mbox->cond);
  pthread_mutex_destroy(&mbox->mutex);
  os_free(mbox);
}
//-----------------------------------------------------------------------------------------------------------
os_tick_t os_tick_from_ms(u32 ms)
{
  return ms / TICK_PERIOD_MS;
}
//-----------------------------------------------------------------------------------------------------------
os_tick_t os_ms_from_tick(os_tick_t tick)
{
  return tick * TICK_PERIOD_MS;
}
//-----------------------------------------------------------------------------------------------------------
void os_msleep(u32 ms)
{
  struct timespec ts;
  struct timespec remain;

  ts.tv_sec = ms / MS_PER_SECOND;
  ts.tv_nsec = (ms % MS_PER_SECOND) * NS_PER_MS;
  while (clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, &remain) != 0)
  {
    ts = remain;
  }
}
//-----------------------------------------------------------------------------------------------------------
u32 os_ms_current(void)
{
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * MS_PER_SECOND + ts.tv_nsec / NS_PER_MS;
}
//-----------------------------------------------------------------------------------------------------------
os_tick_t os_tick_current(void)
{
  return os_tick_from_ms(os_ms_current());
}
//-----------------------------------------------------------------------------------------------------------
void os_tick_sleep(os_tick_t tick)
{
  os_msleep(os_ms_from_tick(tick));
}
//-----------------------------------------------------------------------------------------------------------
static void expired(union sigval sv)
{
  os_timer_t *timer = (os_timer_t *)sv.sival_ptr;
  if (timer->fn)
    timer->fn(timer, timer->arg);
}
//-----------------------------------------------------------------------------------------------------------
os_timer_t *os_timer_create(u32 ms, void (*fn)(os_timer_t *, void *arg), void *arg, bool oneshot)
{
  os_timer_t     *timer;
  struct sigevent sev;

  timer = (os_timer_t *)os_malloc(sizeof(*timer));
  if (timer == NULL)
    return NULL;

  timer->fn = fn;
  timer->arg = arg;
  timer->ms = ms;
  timer->oneshot = oneshot;

  sev.sigev_notify = SIGEV_THREAD;      /* Notify via thread */
  sev.sigev_notify_function = &expired; /* Thread start function */
  sev.sigev_value.sival_ptr = timer;    /* Argument passed to threadFunc() */
  sev.sigev_notify_attributes = NULL;   /* Default thread attributes */

  if (timer_create(CLOCK_MONOTONIC, &sev, &timer->timerid) == -1)
  {
    os_free(timer);
    return NULL;
  }

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
  struct itimerspec its;

  its.it_value.tv_sec = timer->ms / MS_PER_SECOND;
  its.it_value.tv_nsec = (timer->ms % MS_PER_SECOND) * NS_PER_MS;
  its.it_interval.tv_sec = (timer->oneshot) ? 0 : its.it_value.tv_sec;
  its.it_interval.tv_nsec = (timer->oneshot) ? 0 : its.it_value.tv_nsec;
  timer_settime(timer->timerid, 0, &its, NULL);
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_stop(os_timer_t *timer)
{
  struct itimerspec its;

  its.it_value.tv_sec = 0;
  its.it_value.tv_nsec = 0;
  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec = 0;
  timer_settime(timer->timerid, 0, &its, NULL);
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_destroy(os_timer_t *timer)
{
  timer_delete(timer->timerid);
  os_free(timer);
}
