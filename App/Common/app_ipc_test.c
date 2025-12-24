/*!*****************************************************************************
 * file		app_ipc_test.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "app_ipc_test.h"
#include "d_os.h"

static os_sem_t    *TestIpcSem;
static os_mutex_t  *TestIpcMutex;
static os_event_t  *TestIpcEvent;
static os_mbox_t   *TestIpcMbox;
static os_timer_t  *TestOneShotTimer;
static os_timer_t  *TestCycleTimer;
static os_thread_t *TaskThread1;
static os_thread_t *TaskThread2;

static void    *mbox_msg = NULL;
static uint32_t event_value = 0;
static uint32_t mbox_value = 0;
//-----------------------------------------------------------------------------------------------------------
static void OneShot_TImer(os_timer_t *timer, void *arg)
{
  mbox_value++;
}
//-----------------------------------------------------------------------------------------------------------
static void Cycle_TImer(os_timer_t *timer, void *arg)
{
  mbox_value++;
}
//-----------------------------------------------------------------------------------------------------------
static os_return_t Ipc_Task1(void *p_arg)
{
  (void)p_arg;

  while (!os_thread_should_stop(TaskThread1))
  {
    os_sem_wait(TestIpcSem, OS_WAIT_FOREVER);

    os_event_wait(TestIpcEvent, 0x01, &event_value, OS_WAIT_FOREVER);

    os_mbox_fetch(TestIpcMbox, &mbox_msg, OS_WAIT_FOREVER);

    event_value = *(uint32_t *)mbox_msg;

    os_mutex_lock(TestIpcMutex);

    os_msleep(2000);

    os_mutex_unlock(TestIpcMutex);

    os_msleep(1000);
  }

  OS_RETURN(TaskThread1);
}
//-----------------------------------------------------------------------------------------------------------
static os_return_t Ipc_Task2(void *p_arg)
{
  (void)p_arg;

  while (!os_thread_should_stop(TaskThread2))
  {
    os_msleep(1000);

    os_sem_signal(TestIpcSem);

    os_msleep(1000);

    os_event_set(TestIpcEvent, 0x01);

    os_msleep(1000);

    os_mbox_post(TestIpcMbox, &mbox_value, OS_WAIT_FOREVER);

    os_mutex_lock(TestIpcMutex);

    os_msleep(1000);

    os_mutex_unlock(TestIpcMutex);
  }

  OS_RETURN(TaskThread2);
}
//-----------------------------------------------------------------------------------------------------------
void app_pic_test_start(void)
{
  TestIpcSem = os_sem_create(0);
  TestIpcMutex = os_mutex_create();
  TestIpcEvent = os_event_create();
  TestIpcMbox = os_mbox_create(3);
  TestOneShotTimer = os_timer_create(3000, OneShot_TImer, NULL, true);
  TestCycleTimer = os_timer_create(3000, Cycle_TImer, NULL, false);

  TaskThread1 = os_thread_create("ipc_test_task1", 5, 256, Ipc_Task1, NULL);
  TaskThread2 = os_thread_create("ipc_test_task2", 6, 256, Ipc_Task2, NULL);

  os_timer_start(TestOneShotTimer);
  os_timer_start(TestCycleTimer);
}
