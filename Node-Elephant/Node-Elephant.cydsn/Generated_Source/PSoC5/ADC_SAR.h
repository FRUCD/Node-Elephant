/*******************************************************************************
* File Name: ADC_SAR.h
* Version 2.10
*
* Description:
*  Contains the function prototypes, constants and register definition of the
*  ADC SAR Sequencer Component.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_ADC_SAR_H)
    #define CY_ADC_SAR_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"
#include "ADC_SAR_TempBuf_dma.h"
#include "ADC_SAR_FinalBuf_dma.h"
#include "ADC_SAR_SAR.h"

#define ADC_SAR_NUMBER_OF_CHANNELS    (5u)
#define ADC_SAR_SAMPLE_MODE           (0u)
#define ADC_SAR_CLOCK_SOURCE          (0u)

extern int16  ADC_SAR_finalArray[ADC_SAR_NUMBER_OF_CHANNELS];
extern uint32 ADC_SAR_initVar;

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component ADC_SAR_SEQ_v2_10 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */


/***************************************
*        Function Prototypes
***************************************/
void ADC_SAR_Init(void);
void ADC_SAR_Enable(void);
void ADC_SAR_Disable(void);
void ADC_SAR_Start(void);
void ADC_SAR_Stop(void);

uint32 ADC_SAR_IsEndConversion(uint8 retMode);
int16 ADC_SAR_GetResult16(uint16 chan);
int16 ADC_SAR_GetAdcResult(void);
void ADC_SAR_SetOffset(int32 offset);
void ADC_SAR_SetResolution(uint8 resolution);
void ADC_SAR_SetScaledGain(int32 adcGain);
int32 ADC_SAR_CountsTo_mVolts(int16 adcCounts);
int32 ADC_SAR_CountsTo_uVolts(int16 adcCounts);
float32 ADC_SAR_CountsTo_Volts(int16 adcCounts);
void ADC_SAR_Sleep(void);
void ADC_SAR_Wakeup(void);
void ADC_SAR_SaveConfig(void);
void ADC_SAR_RestoreConfig(void);

CY_ISR_PROTO( ADC_SAR_ISR );

/* Obsolete API for backward compatibility.
*  Should not be used in new designs.
*/
void ADC_SAR_SetGain(int32 adcGain);


/**************************************
*    Initial Parameter Constants
**************************************/
#define ADC_SAR_IRQ_REMOVE             (0u)                /* Removes internal interrupt */


/***************************************
*             Registers
***************************************/
#define ADC_SAR_CYCLE_COUNTER_AUX_CONTROL_REG \
                                               (*(reg8 *) ADC_SAR_bSAR_SEQ_ChannelCounter__CONTROL_AUX_CTL_REG)
#define ADC_SAR_CYCLE_COUNTER_AUX_CONTROL_PTR \
                                               ( (reg8 *) ADC_SAR_bSAR_SEQ_ChannelCounter__CONTROL_AUX_CTL_REG)
#define ADC_SAR_CONTROL_REG    (*(reg8 *) \
                                             ADC_SAR_bSAR_SEQ_CtrlReg__CONTROL_REG)
#define ADC_SAR_CONTROL_PTR    ( (reg8 *) \
                                             ADC_SAR_bSAR_SEQ_CtrlReg__CONTROL_REG)
#define ADC_SAR_COUNT_REG      (*(reg8 *) \
                                             ADC_SAR_bSAR_SEQ_ChannelCounter__COUNT_REG)
#define ADC_SAR_COUNT_PTR      ( (reg8 *) \
                                             ADC_SAR_bSAR_SEQ_ChannelCounter__COUNT_REG)
#define ADC_SAR_STATUS_REG     (*(reg8 *) ADC_SAR_bSAR_SEQ_EOCSts__STATUS_REG)
#define ADC_SAR_STATUS_PTR     ( (reg8 *) ADC_SAR_bSAR_SEQ_EOCSts__STATUS_REG)

#define ADC_SAR_SAR_DATA_ADDR_0 (ADC_SAR_SAR_ADC_SAR__WRK0)
#define ADC_SAR_SAR_DATA_ADDR_1 (ADC_SAR_SAR_ADC_SAR__WRK1)
#define ADC_SAR_SAR_DATA_ADDR_0_REG (*(reg8 *) \
                                              ADC_SAR_SAR_ADC_SAR__WRK0)
#define ADC_SAR_SAR_DATA_ADDR_1_REG (*(reg8 *) \
                                              ADC_SAR_SAR_ADC_SAR__WRK1)


/**************************************
*       Register Constants
**************************************/

#if(ADC_SAR_IRQ_REMOVE == 0u)

    /* Priority of the ADC_SAR_IRQ interrupt. */
    #define ADC_SAR_INTC_PRIOR_NUMBER          (uint8)(ADC_SAR_IRQ__INTC_PRIOR_NUM)

    /* ADC_SAR_IRQ interrupt number */
    #define ADC_SAR_INTC_NUMBER                (uint8)(ADC_SAR_IRQ__INTC_NUMBER)

#endif   /* End ADC_SAR_IRQ_REMOVE */


/***************************************
*       API Constants
***************************************/

/* Constants for IsEndConversion() "retMode" parameter */
#define ADC_SAR_RETURN_STATUS              (0x01u)
#define ADC_SAR_WAIT_FOR_RESULT            (0x00u)

/* Defines for the Resolution parameter */
#define ADC_SAR_BITS_12    ADC_SAR_SAR__BITS_12
#define ADC_SAR_BITS_10    ADC_SAR_SAR__BITS_10
#define ADC_SAR_BITS_8     ADC_SAR_SAR__BITS_8

#define ADC_SAR_CYCLE_COUNTER_ENABLE    (0x20u)
#define ADC_SAR_BASE_COMPONENT_ENABLE   (0x01u)
#define ADC_SAR_LOAD_COUNTER_PERIOD     (0x02u)
#define ADC_SAR_SOFTWARE_SOC_PULSE      (0x04u)

/* Generic DMA Configuration parameters */
#define ADC_SAR_TEMP_BYTES_PER_BURST     (uint8)(2u)
#define ADC_SAR_TEMP_TRANSFER_COUNT      ((uint16)ADC_SAR_NUMBER_OF_CHANNELS << 1u)
#define ADC_SAR_FINAL_BYTES_PER_BURST    ((uint16)ADC_SAR_NUMBER_OF_CHANNELS << 1u)
#define ADC_SAR_REQUEST_PER_BURST        (uint8)(1u)

#define ADC_SAR_GET_RESULT_INDEX_OFFSET    ((uint8)ADC_SAR_NUMBER_OF_CHANNELS - 1u)

/* Define for Sample Mode  */
#define ADC_SAR_SAMPLE_MODE_FREE_RUNNING    (0x00u)
#define ADC_SAR_SAMPLE_MODE_SW_TRIGGERED    (0x01u)
#define ADC_SAR_SAMPLE_MODE_HW_TRIGGERED    (0x02u)

/* Define for Clock Source  */
#define ADC_SAR_CLOCK_INTERNAL              (0x00u)
#define ADC_SAR_CLOCK_EXTERNAL              (0x01u)


/***************************************
*        Optional Function Prototypes
***************************************/
#if(ADC_SAR_SAMPLE_MODE != ADC_SAR_SAMPLE_MODE_HW_TRIGGERED)
    void ADC_SAR_StartConvert(void);
    void ADC_SAR_StopConvert(void);
#endif /* ADC_SAR_SAMPLE_MODE != ADC_SAR_SAMPLE_MODE_HW_TRIGGERED */

#endif  /* !defined(CY_ADC_SAR_H) */

/* [] END OF FILE */
