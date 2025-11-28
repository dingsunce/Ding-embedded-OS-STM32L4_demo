/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
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
*                                      BOARD SUPPORT PACKAGE (BSP)
*
*                                       ST Microelectronics STM32
*                                                on the
*
*                                            STM32L476G-EVAL
*                                           Evaluation Board
*
* Filename      : bsp.h
* Version       : V1.00
* Programmer(s) : FF
*                 SB
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*
*           (2) This file and its dependencies requires IAR v6.20 or later to be compiled.
*
*********************************************************************************************************
*/

#ifndef  BSP_PRESENT
#define  BSP_PRESENT


/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_MODULE
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif


/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_def.h>


/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               INT DEFINES
*********************************************************************************************************
*/

#define  BSP_INT_ID_WWDG                                   0    /* Window Watchdog.                                     */
#define  BSP_INT_ID_PVD_PVM                                1    /* PVD/PVM through EXTI Line Ints.                      */
#define  BSP_INT_ID_RTC_TAMP_STAMP_CSS                     2    /* RTC Tamper or TimeStamp/CSS on LSE Ints.             */
#define  BSP_INT_ID_RTC_WKUP                               3    /* RTC Wakeup Global Int.                               */
#define  BSP_INT_ID_FLASH                                  4    /* FLASH Global Int.                                    */
#define  BSP_INT_ID_RCC                                    5    /* RCC Global Int.                                      */
#define  BSP_INT_ID_EXTI0                                  6    /* EXTI Line0 Int.                                      */
#define  BSP_INT_ID_EXTI1                                  7    /* EXTI Line1 Int.                                      */
#define  BSP_INT_ID_EXTI2                                  8    /* EXTI Line2 Int.                                      */
#define  BSP_INT_ID_EXTI3                                  9    /* EXTI Line3 Int.                                      */
#define  BSP_INT_ID_EXTI4                                 10    /* EXTI Line4 Int.                                      */
#define  BSP_INT_ID_DMA1_CH1                              11    /* DMA1 Channel 1 Global Int.                           */
#define  BSP_INT_ID_DMA1_CH2                              12    /* DMA1 Channel 2 Global Int.                           */
#define  BSP_INT_ID_DMA1_CH3                              13    /* DMA1 Channel 3 Global Int.                           */
#define  BSP_INT_ID_DMA1_CH4                              14    /* DMA1 Channel 4 Global Int.                           */
#define  BSP_INT_ID_DMA1_CH5                              15    /* DMA1 Channel 5 Global Int.                           */
#define  BSP_INT_ID_DMA1_CH6                              16    /* DMA1 Channel 6 Global Int.                           */
#define  BSP_INT_ID_DMA1_CH7                              17    /* DMA1 Channel 7 Global Int.                           */
#define  BSP_INT_ID_ADC1_ADC2                             18    /* ADC1 & ADC2 Global Int.                              */
#define  BSP_INT_ID_CAN1_TX                               19    /* CAN1 TX  Ints.                                       */
#define  BSP_INT_ID_CAN1_RX0                              20    /* CAN1 RX0 Ints.                                       */
#define  BSP_INT_ID_CAN1_RX1                              21    /* CAN1 RX1 Int.                                        */
#define  BSP_INT_ID_CAN1_SCE                              22    /* CAN1 SCE Int.                                        */
#define  BSP_INT_ID_EXTI9_5                               23    /* EXTI Line[9:5] Int.                                  */
#define  BSP_INT_ID_TIM1_BRK_TIM15                        24    /* TIM1 Break  Int. & TIM15 Glb Int.                    */
#define  BSP_INT_ID_TIM1_UP_TIM16                         25    /* TIM1 Update Int. & TIM16 Glb Int.                    */
#define  BSP_INT_ID_TIM1_TRG_COM_TIM17                    26    /* TIM1 Trig & Commutation Ints and TIM17 global Int.   */

#define  BSP_INT_ID_TIM1_CC                               27    /* TIM1 Capture Compare Int.                            */
#define  BSP_INT_ID_TIM2                                  28    /* TIM2 Global Int.                                     */
#define  BSP_INT_ID_TIM3                                  29    /* TIM3 Global Int.                                     */
#define  BSP_INT_ID_TIM4                                  30    /* TIM4 Global Int.                                     */
#define  BSP_INT_ID_I2C1_EV                               31    /* I2C1 Event  Int.                                     */
#define  BSP_INT_ID_I2C1_ER                               32    /* I2C1 Error  Int.                                     */
#define  BSP_INT_ID_I2C2_EV                               33    /* I2C2 Event  Int.                                     */
#define  BSP_INT_ID_I2C2_ER                               34    /* I2C2 Error  Int.                                     */
#define  BSP_INT_ID_SPI1                                  35    /* SPI1 Global Int.                                     */
#define  BSP_INT_ID_SPI2                                  36    /* SPI2 Global Int.                                     */
#define  BSP_INT_ID_USART1                                37    /* USART1 Global Int.                                   */
#define  BSP_INT_ID_USART2                                38    /* USART2 Global Int.                                   */
#define  BSP_INT_ID_USART3                                39    /* USART3 Global Int.                                   */
#define  BSP_INT_ID_EXTI15_10                             40    /* EXTI Line [15:10] Ints.                              */
#define  BSP_INT_ID_RTC_ALARM                             41    /* RTC Alarm EXT Line Int.                              */
#define  BSP_INT_ID_DFSDM4                                42    /* DFSDM4 global Int.                                   */
#define  BSP_INT_ID_TIM8_BRK                              43    /* TIM8 Break Int.                                      */
#define  BSP_INT_ID_TIM8_UP                               44    /* TIM8 Update Int.                                     */
#define  BSP_INT_ID_TIM8_TRG_COM                          45    /* TIM8 Trigger & commutation Int.                      */
#define  BSP_INT_ID_TIM8_CC                               46    /* TIM8 Capture Compare Int.                            */
#define  BSP_INT_ID_ADC3                                  47    /* ADC3 Int.                                            */
#define  BSP_INT_ID_FMC                                   48    /* FMC global Int.                                      */
#define  BSP_INT_ID_SDMMC                                 49    /* SDMMC global Int.                                    */
#define  BSP_INT_ID_TIM5                                  50    /* TIM5 global Int.                                     */
#define  BSP_INT_ID_SPI3                                  51    /* SPI3 global Int.                                     */
#define  BSP_INT_ID_UART4                                 52    /* UART4 global Int.                                    */
#define  BSP_INT_ID_UART5                                 53    /* UART5 global Int.                                    */
#define  BSP_INT_ID_TIM6_DACUNDER                         54    /* TIM6 global Int, DAC11 underrun Ints                 */

#define  BSP_INT_ID_TIM7                                  55    /* TIM7 global Int.                                     */
#define  BSP_INT_ID_DMA2_CH1                              56    /* DMA2 Channel 1 global Int.                           */
#define  BSP_INT_ID_DMA2_CH2                              57    /* DMA2 Channel 2 global Int.                           */
#define  BSP_INT_ID_DMA2_CH3                              58    /* DMA2 Channel 3 global Int.                           */
#define  BSP_INT_ID_DMA2_CH4                              59    /* DMA2 Channel 4 global Int.                           */
#define  BSP_INT_ID_DMA2_CH5                              60    /* DMA2 Channel 5 global Int.                           */
#define  BSP_INT_ID_DFSDM1                                61    /* DFSDM1 global Int.                                   */
#define  BSP_INT_ID_DFSDM2                                62    /* DFSDM2 global Int.                                   */
#define  BSP_INT_ID_DFSDM3                                63    /* DFSDM3 global Int.                                   */
#define  BSP_INT_ID_COMP                                  64    /* COMP 1/2 through EXTI 21/22 Ints.                    */
#define  BSP_INT_ID_LPTIM1                                65    /* LPTIM1 global Int.                                   */
#define  BSP_INT_ID_LPTIM2                                66    /* LPTIM2 global Int.                                   */
#define  BSP_INT_ID_OTG_FS                                67    /* OTG global Int.                                      */
#define  BSP_INT_ID_DMA2_CH6                              68    /* DMA2 Channel 6 global Int.                           */
#define  BSP_INT_ID_DMA2_CH7                              69    /* DMA2 Channel 7 global Int.                           */
#define  BSP_INT_ID_LPUART1                               70    /* LPUART1 global Int.                                  */
#define  BSP_INT_ID_QUADSPI                               71    /* QUADSPI global Int.                                  */
#define  BSP_INT_ID_I2C3_EV                               72    /* I2C3 Event  Int.                                     */
#define  BSP_INT_ID_I2C3_ER                               73    /* I2C3 Error  Int.                                     */
#define  BSP_INT_ID_SAI1                                  74    /* SAI1 global Int.                                     */
#define  BSP_INT_ID_SAI2                                  75    /* SAI2 global Int.                                     */
#define  BSP_INT_ID_SWPMI1                                76    /* SWPMI1 global Int.                                   */
#define  BSP_INT_ID_TSC                                   77    /* TSC global Int.                                      */
#define  BSP_INT_ID_LCD                                   78    /* LCD global Int.                                      */
#define  BSP_INT_ID_AES                                   79    /* AES global Int.                                      */
#define  BSP_INT_ID_RNG                                   80    /* RNG global Int.                                      */
#define  BSP_INT_ID_FPU                                   81    /* FPU global Int.                                      */


/*
*********************************************************************************************************
*                                             PERIPH DEFINES
*********************************************************************************************************
*/
                                                                /* --------------------- AHB1 BUS --------------------- */
#define  BSP_AHB1_REG1_MASK          DEF_BIT_05

#define  BSP_PERIPH_ID_TSC          (BSP_AHB1_REG1_MASK | 16u)
#define  BSP_PERIPH_ID_CRC          (BSP_AHB1_REG1_MASK | 12u)
#define  BSP_PERIPH_ID_FLASH        (BSP_AHB1_REG1_MASK |  8u)
#define  BSP_PERIPH_ID_DMA2         (BSP_AHB1_REG1_MASK |  1u)
#define  BSP_PERIPH_ID_DMA1         (BSP_AHB1_REG1_MASK |  0u)

                                                                /* --------------------- AHB2 BUS --------------------- */
#define  BSP_AHB2_REG1_MASK          DEF_BIT_06

#define  BSP_PERIPH_ID_RNG          (BSP_AHB2_REG1_MASK | 18u)
#define  BSP_PERIPH_ID_AES          (BSP_AHB2_REG1_MASK | 16u)
#define  BSP_PERIPH_ID_ADC          (BSP_AHB2_REG1_MASK | 13u)
#define  BSP_PERIPH_ID_OTGFS        (BSP_AHB2_REG1_MASK | 12u)
#define  BSP_PERIPH_ID_GPIOH        (BSP_AHB2_REG1_MASK |  7u)
#define  BSP_PERIPH_ID_GPIOG        (BSP_AHB2_REG1_MASK |  6u)
#define  BSP_PERIPH_ID_GPIOF        (BSP_AHB2_REG1_MASK |  5u)
#define  BSP_PERIPH_ID_GPIOE        (BSP_AHB2_REG1_MASK |  4u)
#define  BSP_PERIPH_ID_GPIOD        (BSP_AHB2_REG1_MASK |  3u)
#define  BSP_PERIPH_ID_GPIOC        (BSP_AHB2_REG1_MASK |  2u)
#define  BSP_PERIPH_ID_GPIOB        (BSP_AHB2_REG1_MASK |  1u)
#define  BSP_PERIPH_ID_GPIOA        (BSP_AHB2_REG1_MASK |  0u)

                                                                /* --------------------- AHB3 BUS --------------------- */
#define  BSP_AHB3_REG1_MASK          DEF_BIT_07

#define  BSP_PERIPH_ID_QSPI         (BSP_AHB3_REG1_MASK |  8u)
#define  BSP_PERIPH_ID_FMC          (BSP_AHB3_REG1_MASK |  0u)

                                                                /* ------------------ APB1 BUS REG 1 ------------------ */
#define  BSP_APB1_REG1_MASK          DEF_BIT_08

#define  BSP_PERIPH_ID_LPTIM1       (BSP_APB1_REG1_MASK | 31u)
#define  BSP_PERIPH_ID_OPAMP        (BSP_APB1_REG1_MASK | 30u)
#define  BSP_PERIPH_ID_DAC1         (BSP_APB1_REG1_MASK | 29u)
#define  BSP_PERIPH_ID_PWR          (BSP_APB1_REG1_MASK | 28u)
#define  BSP_PERIPH_ID_CAN1         (BSP_APB1_REG1_MASK | 25u)
#define  BSP_PERIPH_ID_I2C3         (BSP_APB1_REG1_MASK | 23u)
#define  BSP_PERIPH_ID_I2C2         (BSP_APB1_REG1_MASK | 22u)
#define  BSP_PERIPH_ID_I2C1         (BSP_APB1_REG1_MASK | 21u)
#define  BSP_PERIPH_ID_UART5        (BSP_APB1_REG1_MASK | 20u)
#define  BSP_PERIPH_ID_UART4        (BSP_APB1_REG1_MASK | 19u)
#define  BSP_PERIPH_ID_USART3       (BSP_APB1_REG1_MASK | 18u)
#define  BSP_PERIPH_ID_USART2       (BSP_APB1_REG1_MASK | 17u)
#define  BSP_PERIPH_ID_SPI3         (BSP_APB1_REG1_MASK | 15u)
#define  BSP_PERIPH_ID_SPI2         (BSP_APB1_REG1_MASK | 14u)
#define  BSP_PERIPH_ID_WWDG         (BSP_APB1_REG1_MASK | 11u)
#define  BSP_PERIPH_ID_LCD          (BSP_APB1_REG1_MASK |  9u)
#define  BSP_PERIPH_ID_TIM7         (BSP_APB1_REG1_MASK |  5u)
#define  BSP_PERIPH_ID_TIM6         (BSP_APB1_REG1_MASK |  4u)
#define  BSP_PERIPH_ID_TIM5         (BSP_APB1_REG1_MASK |  3u)
#define  BSP_PERIPH_ID_TIM4         (BSP_APB1_REG1_MASK |  2u)
#define  BSP_PERIPH_ID_TIM3         (BSP_APB1_REG1_MASK |  1u)
#define  BSP_PERIPH_ID_TIM2         (BSP_APB1_REG1_MASK |  0u)

                                                                /* ------------------ APB1 BUS REG 2 ------------------ */
#define  BSP_APB1_REG2_MASK          DEF_BIT_09

#define  BSP_PERIPH_ID_LPTIM2       (BSP_APB1_REG2_MASK |  5u)
#define  BSP_PERIPH_ID_SWPMI1       (BSP_APB1_REG2_MASK |  2u)
#define  BSP_PERIPH_ID_LPUART1      (BSP_APB1_REG2_MASK |  0u)

                                                                /* --------------------- APB2 BUS --------------------- */
#define  BSP_APB2_REG1_MASK          DEF_BIT_10

#define  BSP_PERIPH_ID_DFSDM        (BSP_APB2_REG1_MASK |  24u)
#define  BSP_PERIPH_ID_SAI2         (BSP_APB2_REG1_MASK |  22u)
#define  BSP_PERIPH_ID_SAI1         (BSP_APB2_REG1_MASK |  21u)
#define  BSP_PERIPH_ID_TIM17        (BSP_APB2_REG1_MASK |  18u)
#define  BSP_PERIPH_ID_TIM16        (BSP_APB2_REG1_MASK |  17u)
#define  BSP_PERIPH_ID_TIM15        (BSP_APB2_REG1_MASK |  16u)
#define  BSP_PERIPH_ID_USART1       (BSP_APB2_REG1_MASK |  14u)
#define  BSP_PERIPH_ID_TIM8         (BSP_APB2_REG1_MASK |  13u)
#define  BSP_PERIPH_ID_SPI1         (BSP_APB2_REG1_MASK |  12u)
#define  BSP_PERIPH_ID_TIM1         (BSP_APB2_REG1_MASK |  11u)
#define  BSP_PERIPH_ID_SDMMC        (BSP_APB2_REG1_MASK |  10u)
#define  BSP_PERIPH_ID_FW           (BSP_APB2_REG1_MASK |   7u)
#define  BSP_PERIPH_ID_SYSCFG       (BSP_APB2_REG1_MASK |   0u)


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void        BSP_Init                        (void);

void        BSP_Tick_Init                   (void);

void        BSP_IntDisAll                   (void);

CPU_INT32U  BSP_CPU_ClkFreq                 (void);


/*
*********************************************************************************************************
*                                           INTERRUPT SERVICES
*********************************************************************************************************
*/

void        BSP_IntInit                     (void);

void        BSP_IntEn                       (CPU_DATA       int_id);

void        BSP_IntDis                      (CPU_DATA       int_id);

void        BSP_IntClr                      (CPU_DATA       int_id);

void        BSP_IntVectSet                  (CPU_DATA       int_id,
                                             CPU_FNCT_VOID  isr);

void        BSP_IntPrioSet                  (CPU_DATA       int_id,
                                             CPU_INT08U     prio);

void        BSP_IntHandlerWWDG              (void);
void        BSP_IntHandlerPVD_PVM           (void);
void        BSP_IntHandlerRTC_TAMP_STAMP_CSS(void);
void        BSP_IntHandlerRTC_WKUP          (void);
void        BSP_IntHandlerFLASH             (void);
void        BSP_IntHandlerRCC               (void);
void        BSP_IntHandlerEXTI0             (void);
void        BSP_IntHandlerEXTI1             (void);
void        BSP_IntHandlerEXTI2             (void);
void        BSP_IntHandlerEXTI3             (void);
void        BSP_IntHandlerEXTI4             (void);
void        BSP_IntHandlerDMA1_CH1          (void);
void        BSP_IntHandlerDMA1_CH2          (void);
void        BSP_IntHandlerDMA1_CH3          (void);
void        BSP_IntHandlerDMA1_CH4          (void);
void        BSP_IntHandlerDMA1_CH5          (void);
void        BSP_IntHandlerDMA1_CH6          (void);
void        BSP_IntHandlerDMA1_CH7          (void);
void        BSP_IntHandlerADC1_ADC2         (void);
void        BSP_IntHandlerCAN1_TX           (void);
void        BSP_IntHandlerCAN1_RX0          (void);
void        BSP_IntHandlerCAN1_RX1          (void);
void        BSP_IntHandlerCAN1_SCE          (void);
void        BSP_IntHandlerEXTI9_5           (void);
void        BSP_IntHandlerTIM1_BRK_TIM15    (void);
void        BSP_IntHandlerTIM1_UP_TIM16     (void);
void        BSP_IntHandlerTIM1_TRG_COM_TIM17(void);
void        BSP_IntHandlerTIM1_CC           (void);
void        BSP_IntHandlerTIM2              (void);
void        BSP_IntHandlerTIM3              (void);
void        BSP_IntHandlerTIM4              (void);
void        BSP_IntHandlerI2C1_EV           (void);
void        BSP_IntHandlerI2C1_ER           (void);
void        BSP_IntHandlerI2C2_EV           (void);
void        BSP_IntHandlerI2C2_ER           (void);
void        BSP_IntHandlerSPI1              (void);
void        BSP_IntHandlerSPI2              (void);
void        BSP_IntHandlerUSART1            (void);
void        BSP_IntHandlerUSART2            (void);
void        BSP_IntHandlerUSART3            (void);
void        BSP_IntHandlerEXTI15_10         (void);
void        BSP_IntHandlerRTC_ALARM         (void);
void        BSP_IntHandlerDFSDM4            (void);
void        BSP_IntHandlerTIM8_BRK          (void);
void        BSP_IntHandlerTIM8_UP           (void);
void        BSP_IntHandlerTIM8_TRG_COM      (void);
void        BSP_IntHandlerTIM8_CC           (void);
void        BSP_IntHandlerADC3              (void);
void        BSP_IntHandlerFMC               (void);
void        BSP_IntHandlerSDMMC             (void);
void        BSP_IntHandlerTIM5              (void);
void        BSP_IntHandlerSPI3              (void);
void        BSP_IntHandlerUART4             (void);
void        BSP_IntHandlerUART5             (void);
void        BSP_IntHandlerTIM6_DACUNDER     (void);
void        BSP_IntHandlerTIM7              (void);
void        BSP_IntHandlerDMA2_CH1          (void);
void        BSP_IntHandlerDMA2_CH2          (void);
void        BSP_IntHandlerDMA2_CH3          (void);
void        BSP_IntHandlerDMA2_CH4          (void);
void        BSP_IntHandlerDMA2_CH5          (void);
void        BSP_IntHandlerDFSDM1            (void);
void        BSP_IntHandlerDFSDM2            (void);
void        BSP_IntHandlerDFSDM3            (void);
void        BSP_IntHandlerCOMP              (void);
void        BSP_IntHandlerLPTIM1            (void);
void        BSP_IntHandlerLPTIM2            (void);
void        BSP_IntHandlerOTG_FS            (void);
void        BSP_IntHandlerDMA2_CH6          (void);
void        BSP_IntHandlerDMA2_CH7          (void);
void        BSP_IntHandlerLPUART1           (void);
void        BSP_IntHandlerQUADSPI           (void);
void        BSP_IntHandlerI2C3_EV           (void);
void        BSP_IntHandlerI2C3_ER           (void);
void        BSP_IntHandlerSAI1              (void);
void        BSP_IntHandlerSAI2              (void);
void        BSP_IntHandlerSWPMI1            (void);
void        BSP_IntHandlerTSC               (void);
void        BSP_IntHandlerLCD               (void);
void        BSP_IntHandlerAES               (void);
void        BSP_IntHandlerRNG               (void);
void        BSP_IntHandlerFPU               (void);


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */

