/*!*****************************************************************************
 * file		app_msg_test.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "app_msg_test.h"
#include "d_message.h"
#include "d_os.h"
#include "d_process.h"

#define TIMER_INSTANT 0
#define TIMER_LATER   1
#define TIMER_CYCLE   2

#define FUNCTION_LATER 0
#define FUNCTION_CYCLE 1

PROCESS(TimerProcess);
PROCESS(FunctionProcess);

PROCESS_HANDLER(TimerProcess, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == TIMER_INSTANT || msgId == TIMER_LATER || msgId == TIMER_CYCLE);

    if (msgId == TIMER_INSTANT)
    {
      DProcess_Poll(&FunctionProcess);
    }
    else if (msgId == TIMER_LATER)
    {
      DMsg_SendLater(&FunctionProcess, FUNCTION_LATER, NULL, 10);
    }
    else if (msgId == TIMER_CYCLE)
    {
      DMsg_Cancel(&FunctionProcess, FUNCTION_CYCLE);
      DMsg_SendLater(&FunctionProcess, FUNCTION_CYCLE, NULL, 10);
    }
  }

  PROCESS_SCHEDULE_END()
}

static bool FunctionInstantOn = false;
static bool FunctionLaterOn = false;
static bool FunctionCycleOn = false;

PROCESS_HANDLER(FunctionProcess, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == SYS_MSG_POLL_PROCESS || msgId == FUNCTION_LATER ||
                         msgId == FUNCTION_CYCLE);

    if (msgId == SYS_MSG_POLL_PROCESS)
    {
      FunctionInstantOn = true;
      FunctionInstantOn = FunctionInstantOn;
    }
    else if (msgId == FUNCTION_LATER)
    {
      FunctionLaterOn = true;
      FunctionLaterOn = FunctionLaterOn;
    }
    else if (msgId == FUNCTION_CYCLE)
    {
      FunctionCycleOn = true;
      FunctionCycleOn = FunctionCycleOn;

      OS_PRINT("MSG Test: 3s cycle timer trggered\n");
    }
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
void app_msg_test_start(void)
{
  DProcess_Start(&TimerProcess);
  DProcess_Start(&FunctionProcess);

  DMsg_SendInstant(&TimerProcess, TIMER_INSTANT, NULL);
  DMsg_SendLater(&TimerProcess, TIMER_LATER, NULL, 10);
  DMsg_SendCycle(&TimerProcess, TIMER_CYCLE, NULL, 3000);

  OS_PRINT("MSG Test Start\n");
}
