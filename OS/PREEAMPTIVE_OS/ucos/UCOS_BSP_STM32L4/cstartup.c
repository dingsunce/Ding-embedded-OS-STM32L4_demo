/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2004-2015; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*
*                                           EXCEPTION VECTORS
*
*                                       ST Microelectronics STM32
*                                                on the
*
*                                            STM32L476G-EVAL
*                                           Evaluation Board
*
* Filename      : cstartup.c
* Version       : V1.00
* Programmer(s) : FT
*                 FF
*                 SB
*********************************************************************************************************
*/

#include <includes.h>
#include <stdint.h>
#include <stdio.h>
#include "system_stm32l4xx.h"
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/

typedef  union {
    CPU_FNCT_VOID   Fnct;
    void           *Ptr;
} APP_INTVECT_ELEM;

/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#pragma language=extended
#pragma segment="CSTACK"

static  void  App_Reset_ISR       (void);

static  void  App_NMI_ISR         (void);

static  void  App_Fault_ISR       (void);

static  void  App_BusFault_ISR    (void);

static  void  App_UsageFault_ISR  (void);

static  void  App_MemFault_ISR    (void);

static  void  App_Spurious_ISR    (void);

extern  void  __iar_program_start (void);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                  EXCEPTION / INTERRUPT VECTOR TABLE
*
* Note(s) : (1) The Cortex-M4 may have up to 256 external interrupts, which are the final entries in the
*               vector table.  The STM32L4xxx has 81 external interrupt vectors.
*
*           (2) Interrupts vector 2-13 are implemented in this file as infinite loop for debuging
*               purposes only. The application might implement a recover procedure if it is needed.
*
*           (3) OS_CPU_PendSVHandler() and OS_CPU_SysTickHandler() are implemented in the generic OS
*               port.
*********************************************************************************************************
*/

__root  const  APP_INTVECT_ELEM  __vector_table[] @ ".intvec" = {
    { .Ptr = (void *)__sfe( "CSTACK" )},                        /*   0, SP start value.                                         */
    App_Reset_ISR,                                              /*   1, PC start value.                                         */
    App_NMI_ISR,                                                /*   2, NMI.                                                    */
    App_Fault_ISR,                                              /*   3, Hard Fault.                                             */
    App_MemFault_ISR,                                           /*   4, Memory Management.                                      */
    App_BusFault_ISR,                                           /*   5, Bus Fault.                                              */
    App_UsageFault_ISR,                                         /*   6, Usage Fault.                                            */
    App_Spurious_ISR,                                           /*   7, Reserved.                                               */
    App_Spurious_ISR,                                           /*   8, Reserved.                                               */
    App_Spurious_ISR,                                           /*   9, Reserved.                                               */
    App_Spurious_ISR,                                           /*  10, Reserved.                                               */
    App_Spurious_ISR,                                           /*  11, SVCall.                                                 */
    App_Spurious_ISR,                                           /*  12, Debug Monitor.                                          */
    App_Spurious_ISR,                                           /*  13, Reserved.                                               */
    OS_CPU_PendSVHandler,                                       /*  14, PendSV Handler.                                         */
    OS_CPU_SysTickHandler,                                      /*  15, uC/OS-III Tick ISR Handler.                             */

    BSP_IntHandlerWWDG,                                         /*  16, INTISR[  0]  Window Watchdog.                           */
    BSP_IntHandlerPVD_PVM,                                      /*  17, INTISR[  1]  PVD/PVM through EXTI Line Ints.            */
    BSP_IntHandlerRTC_TAMP_STAMP_CSS,                           /*  18, INTISR[  2]  RTC Tamper or TimeStamp/CSS on LSE Ints.   */
    BSP_IntHandlerRTC_WKUP,                                     /*  19, INTISR[  3]  RTC Wakeup Global Int.                     */
    BSP_IntHandlerFLASH,                                        /*  20, INTISR[  4]  FLASH Global Int.                          */
    BSP_IntHandlerRCC,                                          /*  21, INTISR[  5]  RCC Global Int.                            */
    BSP_IntHandlerEXTI0,                                        /*  22, INTISR[  6]  EXTI Line0 Int.                            */
    BSP_IntHandlerEXTI1,                                        /*  23, INTISR[  7]  EXTI Line1 Int.                            */
    BSP_IntHandlerEXTI2,                                        /*  24, INTISR[  8]  EXTI Line2 Int.                            */
    BSP_IntHandlerEXTI3,                                        /*  25, INTISR[  9]  EXTI Line3 Int.                            */
    BSP_IntHandlerEXTI4,                                        /*  26, INTISR[ 10]  EXTI Line4 Int.                            */
    BSP_IntHandlerDMA1_CH1,                                     /*  27, INTISR[ 11]  DMA1 Channel 1 Global Int.                 */
    BSP_IntHandlerDMA1_CH2,                                     /*  28, INTISR[ 12]  DMA1 Channel 2 Global Int.                 */
    BSP_IntHandlerDMA1_CH3,                                     /*  29, INTISR[ 13]  DMA1 Channel 3 Global Int.                 */
    BSP_IntHandlerDMA1_CH4,                                     /*  30, INTISR[ 14]  DMA1 Channel 4 Global Int.                 */
    BSP_IntHandlerDMA1_CH5,                                     /*  31, INTISR[ 15]  DMA1 Channel 5 Global Int.                 */
    BSP_IntHandlerDMA1_CH6,                                     /*  32, INTISR[ 16]  DMA1 Channel 6 Global Int.                 */
    BSP_IntHandlerDMA1_CH7,                                     /*  33, INTISR[ 17]  DMA1 Channel 7 Global Int.                 */
    BSP_IntHandlerADC1_ADC2,                                    /*  34, INTISR[ 18]  ADC1 & ADC2 Global Int.                    */
    BSP_IntHandlerCAN1_TX,                                      /*  35, INTISR[ 19]  CAN1 TX  Ints.                             */
    BSP_IntHandlerCAN1_RX0,                                     /*  36, INTISR[ 20]  CAN1 RX0 Ints.                             */
    BSP_IntHandlerCAN1_RX1,                                     /*  37, INTISR[ 21]  CAN1 RX1 Int.                              */
    BSP_IntHandlerCAN1_SCE,                                     /*  38, INTISR[ 22]  CAN1 SCE Int.                              */
    BSP_IntHandlerEXTI9_5,                                      /*  39, INTISR[ 23]  EXTI Line[9:5] Int.                        */
    BSP_IntHandlerTIM1_BRK_TIM15,                               /*  40, INTISR[ 24]  TIM1 Break  Int. & TIM15 Glb Int.          */
    BSP_IntHandlerTIM1_UP_TIM16,                                /*  41, INTISR[ 25]  TIM1 Update Int. & TIM16 Glb Int.          */
    BSP_IntHandlerTIM1_TRG_COM_TIM17,                           /*  42, INTISR[ 26]  TIM1 Trig & Commutation Ints...            */
                                                                /*  .................and TIM17 global Int.                      */

    BSP_IntHandlerTIM1_CC,                                      /*  43, INTISR[ 27]  TIM1 Capture Compare Int.                  */
    BSP_IntHandlerTIM2,                                         /*  44, INTISR[ 28]  TIM2 Global Int.                           */
    BSP_IntHandlerTIM3,                                         /*  45, INTISR[ 29]  TIM3 Global Int.                           */
    BSP_IntHandlerTIM4,                                         /*  46, INTISR[ 30]  TIM4 Global Int.                           */
    BSP_IntHandlerI2C1_EV,                                      /*  47, INTISR[ 31]  I2C1 Event  Int.                           */
    BSP_IntHandlerI2C1_ER,                                      /*  48, INTISR[ 32]  I2C1 Error  Int.                           */
    BSP_IntHandlerI2C2_EV,                                      /*  49, INTISR[ 33]  I2C2 Event  Int.                           */
    BSP_IntHandlerI2C2_ER,                                      /*  50, INTISR[ 34]  I2C2 Error  Int.                           */
    BSP_IntHandlerSPI1,                                         /*  51, INTISR[ 35]  SPI1 Global Int.                           */
    BSP_IntHandlerSPI2,                                         /*  52, INTISR[ 36]  SPI2 Global Int.                           */
    BSP_IntHandlerUSART1,                                       /*  53, INTISR[ 37]  USART1 Global Int.                         */
    BSP_IntHandlerUSART2,                                       /*  54, INTISR[ 38]  USART2 Global Int.                         */
    BSP_IntHandlerUSART3,                                       /*  55, INTISR[ 39]  USART3 Global Int.                         */
    BSP_IntHandlerEXTI15_10,                                    /*  56, INTISR[ 40]  EXTI Line [15:10] Ints.                    */
    BSP_IntHandlerRTC_ALARM,                                    /*  57, INTISR[ 41]  RTC Alarm EXT Line Int.                    */
    BSP_IntHandlerDFSDM4,                                       /*  58, INTISR[ 42]  DFSDM4 global Int.                         */
    BSP_IntHandlerTIM8_BRK,                                     /*  59, INTISR[ 43]  TIM8 Break Int.                            */
    BSP_IntHandlerTIM8_UP,                                      /*  60, INTISR[ 44]  TIM8 Update Int.                           */
    BSP_IntHandlerTIM8_TRG_COM,                                 /*  61, INTISR[ 45]  TIM8 Trigger & commutation Int.            */
    BSP_IntHandlerTIM8_CC,                                      /*  62, INTISR[ 46]  TIM8 Capture Compare Int.                  */
    BSP_IntHandlerADC3,                                         /*  63, INTISR[ 47]  ADC3 Int.                                  */
    BSP_IntHandlerFMC,                                          /*  64, INTISR[ 48]  FMC global Int.                            */
    BSP_IntHandlerSDMMC,                                        /*  65, INTISR[ 49]  SDMMC global Int.                          */
    BSP_IntHandlerTIM5,                                         /*  66, INTISR[ 50]  TIM5 global Int.                           */
    BSP_IntHandlerSPI3,                                         /*  67, INTISR[ 51]  SPI3 global Int.                           */
    BSP_IntHandlerUART4,                                        /*  68, INTISR[ 52]  UART4 global Int.                          */
    BSP_IntHandlerUART5,                                        /*  69, INTISR[ 53]  UART5 global Int.                          */
    BSP_IntHandlerTIM6_DACUNDER,                                /*  70, INTISR[ 54]  TIM6 global Int, DAC11 underrun ...        */
                                                                /*  ................ Ints.                                      */

    BSP_IntHandlerTIM7,                                         /*  71, INTISR[ 55]  TIM7 global Int.                           */
    BSP_IntHandlerDMA2_CH1,                                     /*  72, INTISR[ 56]  DMA2 Channel 1 global Int.                 */
    BSP_IntHandlerDMA2_CH2,                                     /*  73, INTISR[ 57]  DMA2 Channel 2 global Int.                 */
    BSP_IntHandlerDMA2_CH3,                                     /*  74, INTISR[ 58]  DMA2 Channel 3 global Int.                 */
    BSP_IntHandlerDMA2_CH4,                                     /*  75, INTISR[ 59]  DMA2 Channel 4 global Int.                 */
    BSP_IntHandlerDMA2_CH5,                                     /*  76, INTISR[ 60]  DMA2 Channel 5 global Int.                 */
    BSP_IntHandlerDFSDM1,                                       /*  77, INTISR[ 61]  DFSDM1 global Int.                         */
    BSP_IntHandlerDFSDM2,                                       /*  78, INTISR[ 62]  DFSDM2 global Int.                         */
    BSP_IntHandlerDFSDM3,                                       /*  79, INTISR[ 63]  DFSDM3 global Int.                         */
    BSP_IntHandlerCOMP,                                         /*  80, INTISR[ 64]  COMP 1/2 through EXTI 21/22 Ints.          */
    BSP_IntHandlerLPTIM1,                                       /*  81, INTISR[ 65]  LPTIM1 global Int.                         */
    BSP_IntHandlerLPTIM2,                                       /*  82, INTISR[ 66]  LPTIM2 global Int.                         */
    BSP_IntHandlerOTG_FS,                                       /*  83, INTISR[ 67]  OTG global Int.                            */
    BSP_IntHandlerDMA2_CH6,                                     /*  84, INTISR[ 68]  DMA2 Channel 6 global Int.                 */
    BSP_IntHandlerDMA2_CH7,                                     /*  85, INTISR[ 69]  DMA2 Channel 7 global Int.                 */
    BSP_IntHandlerLPUART1,                                      /*  86, INTISR[ 70]  LPUART1 global Int.                        */
    BSP_IntHandlerQUADSPI,                                      /*  87, INTISR[ 71]  QUADSPI global Int.                        */
    BSP_IntHandlerI2C3_EV,                                      /*  88, INTISR[ 72]  I2C3 Event  Int.                           */
    BSP_IntHandlerI2C3_ER,                                      /*  89, INTISR[ 73]  I2C3 Error  Int.                           */
    BSP_IntHandlerSAI1,                                         /*  90, INTISR[ 74]  SAI1 global Int.                           */
    BSP_IntHandlerSAI2,                                         /*  91, INTISR[ 75]  SAI2 global Int.                           */
    BSP_IntHandlerSWPMI1,                                       /*  92, INTISR[ 76]  SWPMI1 global Int.                         */
    BSP_IntHandlerTSC,                                          /*  93, INTISR[ 77]  TSC global Int.                            */
    BSP_IntHandlerLCD,                                          /*  94, INTISR[ 78]  LCD global Int.                            */
    BSP_IntHandlerAES,                                          /*  95, INTISR[ 79]  AES global Int.                            */
    BSP_IntHandlerRNG,                                          /*  96, INTISR[ 80]  RNG global Int.                            */
    BSP_IntHandlerFPU,                                          /*  97, INTISR[ 81]  FPU global Int.                            */
};


/*
*********************************************************************************************************
*                                            App_Reset_ISR()
*
* Description : Handle Reset.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_Reset_ISR (void)
{
    SystemInit();

    __iar_program_start();
}

/*
*********************************************************************************************************
*                                            App_NMI_ISR()
*
* Description : Handle Non-Maskable Interrupt (NMI).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : (1) Since the NMI is not being used, this serves merely as a catch for a spurious
*                   exception.
*********************************************************************************************************
*/

static  void  App_NMI_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                             App_Fault_ISR()
*
* Description : Handle hard fault.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_Fault_ISR (void)
{
    unsigned  char hardware_hault = 1;
   
    while (DEF_TRUE) {
      hardware_hault = hardware_hault
        ;
    }
}


/*
*********************************************************************************************************
*                                           App_BusFault_ISR()
*
* Description : Handle bus fault.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_BusFault_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                          App_UsageFault_ISR()
*
* Description : Handle usage fault.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_UsageFault_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                           App_MemFault_ISR()
*
* Description : Handle memory fault.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_MemFault_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                                           App_Spurious_ISR()
*
* Description : Handle spurious interrupt.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_Spurious_ISR (void)
{
    while (DEF_TRUE) {
        ;
    }
}
