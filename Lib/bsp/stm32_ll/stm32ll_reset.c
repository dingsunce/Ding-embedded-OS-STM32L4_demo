#include "message.h"
#include "process.h"
#include "reset_func.h"
#include "stm32ll.h"

#define RESET_MESSAGE 0

PROCESS(ResetProcess);
//-----------------------------------------------------------------------------------------------------------
PROCESS_HANDLER(ResetProcess, msgId, arg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == RESET_MESSAGE);
    Reset_Mcu();
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
void Reset_Init(void)
{
  Process_Start(&ResetProcess);
}
//-----------------------------------------------------------------------------------------------------------
void Reset_Start(u32 delay)
{
  Msg_Cancel(&ResetProcess, RESET_MESSAGE);
  Msg_SendLater(&ResetProcess, RESET_MESSAGE, NULL, delay);
}
//-----------------------------------------------------------------------------------------------------------
void Reset_Cancel(void)
{
  Msg_Cancel(&ResetProcess, RESET_MESSAGE);
}
//-----------------------------------------------------------------------------------------------------------
u32 Reset_GetResetRemainTimer(void)
{
  return Msg_GetRemainTime(&ResetProcess, RESET_MESSAGE);
}
//-----------------------------------------------------------------------------------------------------------
void Reset_Mcu(void)
{
  NVIC_SystemReset();
}