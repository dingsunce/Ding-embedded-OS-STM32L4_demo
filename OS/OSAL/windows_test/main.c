#include "SysTick.h"
#include "app_ipc_test.h"
#include "app_msg_test.h"
#include "d_os.h"
#include "osal.h"

static void App_TaskStart(void *p_arg)
{
  (void)p_arg;

  /*
    init D_OS and other app in preemptive os thread

    systick is not working only if we have started preemptive os like ucos, free rtos
 */
  DOS_Init();

  app_msg_test_start();

  app_pic_test_start();

  while (1)
  {
    /* Task body, always written as an infinite loop.           */
    os_usleep(1000000);
  }
}

static void onems_timer(os_timer_t *timer, void *arg)
{
  SysTick_On();
}

int main(int argc, char **argv)
{
  os_init();
  os_thread_create("os_task", 3, 256, App_TaskStart, NULL);
  os_start(); // start first thread

  os_timer_t *timer = os_timer_create(1000, onems_timer, NULL, false);
  os_timer_start(timer);

  while (1)
  {
  }

  return 1;
}
