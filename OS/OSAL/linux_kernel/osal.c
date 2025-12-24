/*!*****************************************************************************
 * file		osal.c
 * $Author: sunce.ding
 *******************************************************************************/

#include "osal.h"
#include "d_mem.h"
#include <linux/delay.h>
#include <linux/jiffies.h>
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
  struct task_struct *thread = kthread_run(entry, arg, name);
  if (IS_ERR(thread))
    return NULL;

  return thread;
}
//-----------------------------------------------------------------------------------------------------------
void os_thread_destroy(os_thread_t *thread)
{
  // notifying the thread that it should exit
  kthread_stop(thread);
}
//-----------------------------------------------------------------------------------------------------------
bool os_thread_should_stop(os_thread_t *thread)
{
  return kthread_should_stop();
}
//-----------------------------------------------------------------------------------------------------------
os_mutex_t *os_mutex_create(void)
{
  os_mutex_t *mutex = os_malloc(sizeof(os_mutex_t));
  if (mutex == NULL)
    return NULL;

  mutex_init(mutex);

  return mutex;
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_lock(os_mutex_t *mutex)
{
  mutex_lock(mutex);
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_unlock(os_mutex_t *mutex)
{
  mutex_unlock(mutex);
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_destroy(os_mutex_t *mutex)
{
  mutex_unlock(mutex);
  os_free(mutex);
}
//-----------------------------------------------------------------------------------------------------------
os_sem_t *os_sem_create(u16 count)
{
  os_sem_t *sem = os_malloc(sizeof(os_sem_t));
  if (sem == NULL)
    return NULL;

  sema_init(sem, count);

  return sem;
}
//-----------------------------------------------------------------------------------------------------------
bool os_sem_wait(os_sem_t *sem, u32 ms)
{
  bool ret = true;

  if (ms == OS_WAIT_FOREVER)
    down(sem);
  else if (down_timeout(sem, msecs_to_jiffies(ms)))
    ret = false;

  return ret;
}
//-----------------------------------------------------------------------------------------------------------
void os_sem_signal(os_sem_t *sem)
{
  up(sem);
}
//-----------------------------------------------------------------------------------------------------------
void os_sem_destroy(os_sem_t *sem)
{
  os_sem_signal(sem);
  os_free(sem);
}
//-----------------------------------------------------------------------------------------------------------
os_event_t *os_event_create(void)
{
  os_event_t *event;

  event = (os_event_t *)os_malloc(sizeof(*event));
  if (event == NULL)
    return NULL;

  sema_init(&event->condition, 0);
  mutex_init(&event->lock);
  event->flags = 0;

  return event;
}
//-----------------------------------------------------------------------------------------------------------
bool os_event_wait(os_event_t *event, u32 mask, u32 *value, u32 ms)
{
  bool success = true;

  while ((event->flags & mask) == 0)
  {
    success = os_sem_wait(&event->condition, ms);
    if (!success)
      goto timeout;
  }

timeout:

  *value = event->flags & mask;

  return success;
}
//-----------------------------------------------------------------------------------------------------------
void os_event_set(os_event_t *event, u32 value)
{
  mutex_lock(&event->lock);
  event->flags |= value;
  mutex_unlock(&event->lock);

  os_sem_signal(&event->condition);
}
//-----------------------------------------------------------------------------------------------------------
void os_event_clr(os_event_t *event, u32 value)
{
  mutex_lock(&event->lock);
  event->flags &= ~value;
  mutex_unlock(&event->lock);
}
//-----------------------------------------------------------------------------------------------------------
void os_event_destroy(os_event_t *event)
{
  os_sem_signal(&event->condition);

  os_free(event);
}
//-----------------------------------------------------------------------------------------------------------
os_mbox_t *os_mbox_create(u32 size)
{
  os_mbox_t *mbox;

  mbox = (os_mbox_t *)os_malloc(sizeof(*mbox) + size * sizeof(void *));

  sema_init(&mbox->condition, 0);
  mutex_init(&mbox->lock);

  mbox->r = 0;
  mbox->w = 0;
  mbox->count = 0;
  mbox->size = size;

  return mbox;
}
//-----------------------------------------------------------------------------------------------------------
bool os_mbox_fetch(os_mbox_t *mbox, void **msg, u32 ms)
{
  bool success = os_sem_wait(&mbox->condition, ms);
  if (!success)
    goto timeout;

  mutex_lock(&mbox->lock);

  *msg = mbox->msg[mbox->r++];
  if (mbox->r == mbox->size)
    mbox->r = 0;

  mbox->count--;

  mutex_unlock(&mbox->lock);

timeout:

  return success;
}
//-----------------------------------------------------------------------------------------------------------
bool os_mbox_post(os_mbox_t *mbox, void *msg, u32 ms)
{
  bool success = true;

  if (mbox->count == mbox->size)
    return false;

  mutex_lock(&mbox->lock);

  mbox->msg[mbox->w++] = msg;
  if (mbox->w == mbox->size)
    mbox->w = 0;

  mbox->count++;

  mutex_unlock(&mbox->lock);

  os_sem_signal(&mbox->condition);

  return success;
}
//-----------------------------------------------------------------------------------------------------------
void os_mbox_destroy(os_mbox_t *mbox)
{
  os_sem_signal(&mbox->condition);

  os_free(mbox);
}
//-----------------------------------------------------------------------------------------------------------
void os_msleep(u32 ms)
{
  msleep(ms);
}
//-----------------------------------------------------------------------------------------------------------
u32 os_ms_current(void)
{
  return jiffies_to_msecs(os_tick_current());
}
//-----------------------------------------------------------------------------------------------------------
os_tick_t os_tick_current(void)
{
  return jiffies;
}
//-----------------------------------------------------------------------------------------------------------
os_tick_t os_tick_from_ms(u32 ms)
{
  return msecs_to_jiffies(ms);
}
//-----------------------------------------------------------------------------------------------------------
void os_tick_sleep(os_tick_t tick)
{
  msleep(jiffies_to_msecs(tick));
}
//-----------------------------------------------------------------------------------------------------------
static void expired(struct timer_list *t)
{
  os_timer_t *timer = from_timer(timer, t, kernel_timer);
  if (timer->fn)
    timer->fn(timer, timer->arg);

  if (!timer->oneshot)
    os_timer_start(timer);
}
//-----------------------------------------------------------------------------------------------------------
os_timer_t *os_timer_create(u32 ms, void (*fn)(os_timer_t *, void *arg), void *arg, bool oneshot)
{
  os_timer_t *timer = (os_timer_t *)os_malloc(sizeof(*timer));
  if (timer == NULL)
    return NULL;

  timer->fn = fn;
  timer->arg = arg;
  timer->ms = ms;
  timer->oneshot = oneshot;

  timer_setup(&timer->kernel_timer, expired, 0);

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
  mod_timer(&timer->kernel_timer, os_tick_current() + msecs_to_jiffies(timer->ms));
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_stop(os_timer_t *timer)
{
  del_timer_sync(&timer->kernel_timer);
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_destroy(os_timer_t *timer)
{
  del_timer_sync(&timer->kernel_timer);
  os_free(timer);
}
