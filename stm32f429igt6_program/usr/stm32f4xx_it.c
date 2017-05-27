/**
  ******************************************************************************
  * @file    FMC_SDRAM/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"

/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */

//SysTick_Handler is defined in bsp_systick.c
#if 0
void SysTick_Handler(void)
{}
#endif

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/

#include "bsp_ov2640.h"

extern uint16_t *g_dma_target_ptr;
extern const uint16_t g_img_height;
extern const uint16_t g_img_width;
extern const uint16_t g_dma_buf_size;
__IO uint32_t current_height = 0;
__IO uint32_t count_dma = 0;
__IO uint32_t count_frames = 0;

void DMA2_Stream1_IRQHandler(void)
{
    if(SET == DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1))
    {
        current_height++;
        
        if(current_height == g_img_height)
        {
            count_dma++;
            current_height = 0;
        }

        OV2640_DMA_Config((uint32_t)(g_dma_target_ptr + g_img_width*current_height), 
            g_dma_buf_size);
        DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
    }
}

void DCMI_IRQHandler(void)
{
    if(DCMI->MISR & DCMI_IT_FRAME)
    {
        current_height = 0;
        count_frames++;
        DCMI->ICR = DCMI_IT_FRAME;
    }
}

#if 0
#include "bsp_sdio_sd.h"
void DMA2_Stream3_IRQHandler(void)
{
    if(DMA2->LISR & DMA_FLAG_TCIF3)
    {
        g_dma_transfer_end = 1;
        DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_TCIF3 | DMA_FLAG_FEIF3);
    }
}

void SDIO_IRQHandler(void)
{
    if((SDIO->STA & SDIO_IT_DATAEND) != 0)
    {
        g_sd_transfer_status = SD_STATUS_OK;
        SDIO->ICR = SDIO_STA_DATAEND;
        g_sd_transfer_end = 1;
    }  
    else if((SDIO->STA & SDIO_IT_DCRCFAIL) != 0)
    {
        SDIO->ICR = SDIO_ICR_DCRCFAILC;
        g_sd_transfer_status = SD_STATUS_DATA_CRC_FAIL;
    }
    else if((SDIO->STA & SDIO_STA_DTIMEOUT) != 0)
    {
        SDIO->ICR = SDIO_ICR_DTIMEOUTC;
        g_sd_transfer_status = SD_STATUS_DATA_TIMEOUT;
    }
    else if((SDIO->STA & SDIO_STA_RXOVERR) != 0)
    {
        SDIO->ICR = SDIO_ICR_RXOVERRC;
        g_sd_transfer_status = SD_STATUS_RX_OVERRUN;
    }
    else if((SDIO->STA & SDIO_STA_TXUNDERR) != 0)
    {
        SDIO->ICR = SDIO_ICR_TXUNDERRC;
        g_sd_transfer_status = SD_STATUS_TX_UNDERRUN;
    }
    else if(SDIO_GetITStatus(SDIO_IT_STBITERR) != RESET)
    {
        SDIO->ICR = SDIO_ICR_STBITERRC;
        g_sd_transfer_status = SD_STATUS_START_BIT_ERR;
    }

    SDIO->MASK &= ~ (SDIO_MASK_DCRCFAILIE | SDIO_MASK_DTIMEOUTIE | 
                     SDIO_MASK_DATAENDIE  | SDIO_MASK_TXFIFOHEIE | 
                     SDIO_MASK_RXFIFOHFIE | SDIO_MASK_TXUNDERRIE | 
                     SDIO_MASK_RXOVERRIE  | SDIO_MASK_STBITERRIE );
}
#endif
/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
