/*!*****************************************************************************
 * file		d_os.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "d_os.h"
#include "d_mem.h"
#include "d_message.h"
#include "d_task.h"
#include "d_tick.h"
//-----------------------------------------------------------------------------------------------------------
void DOS_Init(void)
{
  DTick_Init();
  DMem_Init();
  DProcess_Init();
  DMsg_Init();
  DTask_Init();
}
//-----------------------------------------------------------------------------------------------------------
void DOS_RunOneTick(void)
{
  /*
    call DOS_RunOneTick in main while loop per tick time,
    if we did not embed DOS in a preemptive os like rt-thread
  */
  DMsg_RunOneTick();
}
//-----------------------------------------------------------------------------------------------------------
void DOS_Run(void)
{
  /*
    call DOS_Run in main while loop as quickly as possible ,
    if we did not embed DOS in a preemptive os like rt-thread
  */
  DProcess_Run();
  DTask_Run();
}
//-----------------------------------------------------------------------------------------------------------
void DOS_Exit(void)
{
  DTask_Exit();
  DProcess_Exit();
  DMsg_Exit();
  DMem_Exit();
}