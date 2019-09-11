#ifndef  BSP_INIT__
#define  BSP_INIT__


/*
*********************************************************************************************************
*                                               INT DEFINES
*********************************************************************************************************
*/

#define  BSP_INT_ID_WWDG                                   0u   /* Window WatchDog Interrupt                            */
#define  BSP_INT_ID_PVD                                    1u   /* PVD through EXTI Line detection Interrupt            */
#define  BSP_INT_ID_TAMP_STAMP                             2u   /* Tamper and TimeStamp Interrupt                       */
#define  BSP_INT_ID_RTC_WKUP                               3u   /* RTC wakeup Interrupt through the EXTI line           */
#define  BSP_INT_ID_FLASH                                  4u   /* FLASH global Interrupt                               */
#define  BSP_INT_ID_RCC                                    5u   /* RCC global Interrupt                                 */
#define  BSP_INT_ID_EXTI0                                  6u   /* EXTI Line0 Interrupt                                 */
#define  BSP_INT_ID_EXTI1                                  7u   /* EXTI Line1 Interrupt                                 */
#define  BSP_INT_ID_EXTI2                                  8u   /* EXTI Line2 Interrupt                                 */
#define  BSP_INT_ID_EXTI3                                  9u   /* EXTI Line3 Interrupt                                 */
#define  BSP_INT_ID_EXTI4                                 10u   /* EXTI Line4 Interrupt                                 */
#define  BSP_INT_ID_DMA1_CH0                              11u   /* DMA1 Channel 0 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH1                              12u   /* DMA1 Channel 1 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH2                              13u   /* DMA1 Channel 2 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH3                              14u   /* DMA1 Channel 3 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH4                              15u   /* DMA1 Channel 4 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH5                              16u   /* DMA1 Channel 5 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH6                              17u   /* DMA1 Channel 6 global Interrupt                      */
#define  BSP_INT_ID_ADC                                   18u   /* ADC1, ADC2 and ADC3 global Interrupt                 */
#define  BSP_INT_ID_CAN1_TX                               19u   /* CAN1 TX Interrupts                                   */
#define  BSP_INT_ID_CAN1_RX0                              20u   /* CAN1 RX0 Interrupts                                  */
#define  BSP_INT_ID_CAN1_RX1                              21u   /* CAN1 RX1 Interrupt                                   */
#define  BSP_INT_ID_CAN1_SCE                              22u   /* CAN1 SCE Interrupt                                   */
#define  BSP_INT_ID_EXTI9_5                               23u   /* External Line[9:5] Interrupts                        */
#define  BSP_INT_ID_TIM1_BRK_TIM9                         24u   /* TIM1 Break Interrupt and TIM9 global interrupt       */
#define  BSP_INT_ID_TIM1_UP_TIM10                         25u   /* TIM1 Update Interrupt and TIM10 global interrupt     */
#define  BSP_INT_ID_TIM1_TRG_COM_TIM11                    26u   /* TIM1 Trigger & Commutation Int. & TIM11 global Int.  */
#define  BSP_INT_ID_TIM1_CC                               27u   /* TIM1 Capture Compare Interrupt                       */
#define  BSP_INT_ID_TIM2                                  28u   /* TIM2 global Interrupt                                */
#define  BSP_INT_ID_TIM3                                  29u   /* TIM3 global Interrupt                                */
#define  BSP_INT_ID_TIM4                                  30u   /* TIM4 global Interrupt                                */
#define  BSP_INT_ID_I2C1_EV                               31u   /* I2C1 Event Interrupt                                 */
#define  BSP_INT_ID_I2C1_ER                               32u   /* I2C1 Error Interrupt                                 */
#define  BSP_INT_ID_I2C2_EV                               33u   /* I2C2 Event Interrupt                                 */
#define  BSP_INT_ID_I2C2_ER                               34u   /* I2C2 Error Interrupt                                 */
#define  BSP_INT_ID_SPI1                                  35u   /* SPI1 global Interrupt                                */
#define  BSP_INT_ID_SPI2                                  36u   /* SPI2 global Interrupt                                */
#define  BSP_INT_ID_USART1                                37u   /* USART1 global Interrupt                              */
#define  BSP_INT_ID_USART2                                38u   /* USART2 global Interrupt                              */
#define  BSP_INT_ID_USART3                                39u   /* USART3 global Interrupt                              */
#define  BSP_INT_ID_EXTI15_10                             40u   /* External Line[15:10] Interrupts                      */
#define  BSP_INT_ID_RTC_ALARM                             41u   /* RTC Alarms (A and B) through EXTI Line Interrupt     */
#define  BSP_INT_ID_OTG_FS_WKUP                           42u   /* USB WakeUp from suspend through EXTI Line Interrupt  */

#define  BSP_INT_ID_TIM8_BRK_TIM12                        43u   /* TIM8 Break Interrupt and TIM12 global Interrupt      */
#define  BSP_INT_ID_TIM8_UP_TIM13                         44u   /* TIM8 Update Interrupt and TIM13 global Interrupt     */
#define  BSP_INT_ID_TIM8_TRG_COM_TIM14                    45u   /* TIM8 Trigger/Commutation and TIM14 global Interrupt  */
#define  BSP_INT_ID_TIM8_CC                               46u   /* TIM8 Capture Compare Interrupt                       */
#define  BSP_INT_ID_DMA1_STREAM7                          47u   /* DMA1 Stream7 Interrupt                               */
#define  BSP_INT_ID_FSMC                                  48u   /* FSMC global Interrupt                                */
#define  BSP_INT_ID_SDIO                                  49u   /* SDIO global Interrupt                                */

#define  BSP_INT_ID_TIM5                                  50u   /* TIM5 global Interrupt                                */
#define  BSP_INT_ID_SPI3                                  51u   /* SPI3 global Interrupt                                */
#define  BSP_INT_ID_USART4                                52u   /* USART4 global Interrupt                              */
#define  BSP_INT_ID_USART5                                53u   /* USART5 global Interrupt                              */
#define  BSP_INT_ID_TIM6_DAC                              54u   /* TIM6 global Interrupt, DAC1 & DAC2 underrun err int. */
#define  BSP_INT_ID_TIM7                                  55u   /* TIM7 global Interrupt                                */
#define  BSP_INT_ID_DMA2_CH0                              56u   /* DMA2 Channel 0 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH1                              57u   /* DMA2 Channel 1 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH2                              58u   /* DMA2 Channel 2 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH3                              59u   /* DMA2 Channel 3 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH4                              60u   /* DMA2 Channel 4 global Interrupt                      */

#define  BSP_INT_ID_ETH                                   61u   /* ETH  global Interrupt                                */
#define  BSP_INT_ID_ETH_WKUP                              62u   /* ETH  WakeUp from EXTI line interrupt                 */
#define  BSP_INT_ID_CAN2_TX                               63u   /* CAN2 TX Interrupts                                   */
#define  BSP_INT_ID_CAN2_RX0                              64u   /* CAN2 RX0 Interrupts                                  */
#define  BSP_INT_ID_CAN2_RX1                              65u   /* CAN2 RX1 Interrupt                                   */
#define  BSP_INT_ID_CAN2_SCE                              66u   /* CAN2 SCE Interrupt                                   */
#define  BSP_INT_ID_OTG_FS                                67u   /* OTG global Interrupt                                 */

#define  BSP_INT_ID_DMA2_CH5                              68u   /* DMA2 Channel 5 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH6                              69u   /* DMA2 Channel 6 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH7                              70u   /* DMA2 Channel 7 global Interrupt                      */
#define  BSP_INT_ID_USART6                                71u   /* USART5 global Interrupt                              */
#define  BSP_INT_ID_I2C3_EV                               72u   /* I2C3 Event  Interrupt                                */
#define  BSP_INT_ID_I2C3_ER                               73u   /* I2C3 Error  Interrupt                                */
#define  BSP_INT_ID_OTG_HS_EP1_OUT                        74u   /* OTG HS EP1 OUT global Interrupt                      */
#define  BSP_INT_ID_OTG_HS_EP1_IN                         75u   /* OTG HS EP1 IN global Interrupt                       */
#define  BSP_INT_ID_OTG_HS_WKUP                           76u   /* OTG HS Wakeup Interrupt                              */
#define  BSP_INT_ID_OTG_HS                                77u   /* OTG HS global Interrupt                              */
#define  BSP_INT_ID_DCMI                                  78u   /* DCMI global Interrupt                                */
#define  BSP_INT_ID_CRYP                                  79u   /* CRYPT crypto global Interrupt                        */
#define  BSP_INT_ID_HASH_RNG                              80u   /* HASH and RNG global Interrupt                        */
#define  BSP_INT_ID_FPU                                   81u   /* FPU global Interrupt                                 */

#define  BSP_INT_SRC_NBR                                  82u

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void        BSP_Init       (void);

void        BSP_IntDisAll  (void);

CPU_INT32U  BSP_CPU_ClkFreq(void);

void        BSP_Tick_Init  (void);


/*
*********************************************************************************************************
*                                           INTERRUPT SERVICES
*********************************************************************************************************
*/

void        BSP_IntInit                     (void);

void        BSP_IntEn                       (CPU_DATA       int_id);

void        BSP_IntDis                      (CPU_DATA       int_id);


void        BSP_IntVectSet                  (CPU_DATA       int_id,
                                             CPU_FNCT_VOID  isr);

void        BSP_IntPrioSet                  (CPU_DATA       int_id,
                                             CPU_INT08U     prio);

CPU_INT32U  BSP_CPU_ClkFreq (void);
void BSP_Tick_Init (void);
/*
*********************************************************************************************************
*                                     PERIPHERAL POWER/CLOCK SERVICES
*********************************************************************************************************
*/

CPU_INT32U   BSP_PeriphClkFreqGet(CPU_DATA  pwr_clk_id);

void         BSP_PeriphEn        (CPU_DATA  pwr_clk_id);

void         BSP_PeriphDis       (CPU_DATA  pwr_clk_id);

#endif

