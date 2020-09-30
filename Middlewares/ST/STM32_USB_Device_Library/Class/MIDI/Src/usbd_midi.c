/**
  ******************************************************************************
  * @file    USBD_MIDI.c
  * @author  MCD Application Team
  * @brief   This file provides the high layer firmware functions to manage the
  *          following functionalities of the USB CDC Class:
  *           - Initialization and Configuration of high and low layer
  *           - Enumeration as CDC Device (and enumeration for each implemented memory interface)
  *           - OUT/IN data transfer
  *           - Command IN transfer (class requests management)
  *           - Error management
  *
  *  @verbatim
  *
  *          ===================================================================
  *                                CDC Class Driver Description
  *          ===================================================================
  *           This driver manages the "Universal Serial Bus Class Definitions for Communications Devices
  *           Revision 1.2 November 16, 2007" and the sub-protocol specification of "Universal Serial Bus
  *           Communications Class Subclass Specification for PSTN Devices Revision 1.2 February 9, 2007"
  *           This driver implements the following aspects of the specification:
  *             - Device descriptor management
  *             - Configuration descriptor management
  *             - Enumeration as CDC device with 2 data endpoints (IN and OUT) and 1 command endpoint (IN)
  *             - Requests management (as described in section 6.2 in specification)
  *             - Abstract Control Model compliant
  *             - Union Functional collection (using 1 IN endpoint for control)
  *             - Data interface class
  *
  *           These aspects may be enriched or modified for a specific user application.
  *
  *            This driver doesn't implement the following aspects of the specification
  *            (but it is possible to manage these features with some modifications on this driver):
  *             - Any class-specific aspect relative to communication classes should be managed by user application.
  *             - All communication classes other than PSTN are not managed
  *
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include  "../Middlewares/ST/STM32_USB_Device_Library/Class/MIDI/Inc/usbd_midi.h"
#include "usbd_ctlreq.h"
#include "usbd_midi_if.h"
/**
  * @brief  USBD_MIDI_Init
  *         Initialize the CDC interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */

uint8_t  USBD_MIDI_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    USBD_LL_OpenEP(pdev, MIDI_IN_EP, USBD_EP_TYPE_BULK, MIDI_IN_PACKET_SIZE);

    pdev->ep_in[MIDI_IN_EP & 0xFU].is_used = 1U;

    USBD_LL_OpenEP(pdev, MIDI_OUT_EP, USBD_EP_TYPE_BULK, MIDI_OUT_PACKET_SIZE);

    pdev->ep_out[MIDI_OUT_EP & 0xFU].is_used = 1U;

    /* Prepare Out endpoint to receive next packet */
    USBD_LL_PrepareReceive(pdev, MIDI_OUT_EP, (uint8_t *)MIDIRxBufferFS, MIDI_OUT_PACKET_SIZE);

  return USBD_OK;
}

/**
  * @brief  USBD_MIDI_Init
  *         DeInitialize the CDC layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
uint8_t  USBD_MIDI_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = 0U;

  /* Close EP IN */
  USBD_LL_CloseEP(pdev, MIDI_IN_EP);
  pdev->ep_in[MIDI_IN_EP & 0xFU].is_used = 0U;

  /* Close EP OUT */
  USBD_LL_CloseEP(pdev, MIDI_OUT_EP);
  pdev->ep_out[MIDI_OUT_EP & 0xFU].is_used = 0U;

  /* DeInit  physical Interface components */
  if (pdev->pClassData != NULL)
  {
    ((USBD_MIDI_ItfTypeDef *)pdev->pUserData)->DeInit();
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }

  return ret;
}


/**
  * @brief  USBD_MIDI_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
uint8_t  USBD_MIDI_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_MIDI_HandleTypeDef *hmidi = (USBD_MIDI_HandleTypeDef *)pdev->pClassData;
  PCD_HandleTypeDef *hpcd = pdev->pData;

  if (pdev->pClassData != NULL)
  {
    if ((pdev->ep_in[epnum].total_length > 0U) && ((pdev->ep_in[epnum].total_length % hpcd->IN_ep[epnum].maxpacket) == 0U))
    {

      pdev->ep_in[epnum].total_length = 0U;
      USBD_LL_Transmit(pdev, epnum, NULL, 0U);
    }
    else
    {
      hmidi->TxState = 0U;
    }
    return USBD_OK;
  }
 else
  {
    return USBD_FAIL;
  }
}

/**
  * @brief  USBD_MIDI_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
uint8_t  USBD_MIDI_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{


/* USBD_MIDI_HandleTypeDef   *hmidi = (USBD_MIDI_HandleTypeDef *) pdev->pClassData;


  hmidi->RxLength = USBD_LL_GetRxDataSize(pdev, epnum);


  if (pdev->pClassData != NULL)
  {*/

	uint32_t rec = USBD_LL_GetRxDataSize(pdev, epnum);


	  //((USBD_MIDI_ItfTypeDef *)pdev->pUserData)->Receive(hmidi->RxBuffer, &hmidi->RxLength);
	  MIDI_Receive_FS(MIDIRxBufferFS, &rec);

	  USBD_LL_PrepareReceive(pdev, MIDI_OUT_EP, (uint8_t *)MIDIRxBufferFS, MIDI_OUT_PACKET_SIZE);


    return USBD_OK;/*
  }
  else
  {
    return USBD_FAIL;
  }*/




}

uint8_t  USBD_MIDI_TransmitPacket(USBD_HandleTypeDef *pdev, uint8_t* Buf, uint16_t Len)
{
  USBD_MIDI_HandleTypeDef   *hmidi = (USBD_MIDI_HandleTypeDef *) pdev->pClassData;

  if (pdev->pClassData != NULL)
  {
    if (hmidi->TxState == 0U)
    {
      /* Tx Transfer in progress */
      hmidi->TxState = 1U;

      /* Update the packet total length */
      pdev->ep_in[MIDI_IN_EP & 0xFU].total_length = Len;

      /* Transmit next packet */
      USBD_LL_Transmit(pdev, MIDI_IN_EP, (uint8_t *)Buf, (uint16_t)Len);

     return USBD_OK;
    }
    else
    {
      return USBD_BUSY;
    }
  }
  else
  {
    return USBD_FAIL;
  }
}



uint8_t  USBD_MIDI_ReceivePacket(USBD_HandleTypeDef *pdev)
{
 // USBD_MIDI_HandleTypeDef   *hmidi = (USBD_MIDI_HandleTypeDef *) pdev->pClassData;
  //hmidi->RxBuffer = MIDIRxBufferFS;

  /* Suspend or Resume USB Out process */
  /*if (pdev->pClassData != NULL)
  {
    if (pdev->dev_speed == USBD_SPEED_HIGH)
    {*/
      /* Prepare Out endpoint to receive next packet */
     /* USBD_LL_PrepareReceive(pdev,
                             MIDI_OUT_EP,
                             hmidi->RxBuffer,
							 MIDI_OUT_PACKET_SIZE);
    }
    else
    {*/
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev,
                             MIDI_OUT_EP,
							 MIDIRxBufferFS,
							 MIDI_OUT_PACKET_SIZE);
   /* }
    return USBD_OK;
  }
  else
  {
    return USBD_FAIL;
  }*/
}


/**
  * @brief  USBD_MIDI_SetTxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Tx Buffer
  * @retval status
  */
uint8_t  USBD_MIDI_SetTxBuffer(USBD_HandleTypeDef   *pdev, uint8_t  *pbuff, uint16_t length)
{
  USBD_MIDI_HandleTypeDef   *hmidi = (USBD_MIDI_HandleTypeDef *) pdev->pClassData;

  hmidi->TxBuffer = pbuff;
  hmidi->TxLength = length;

  return USBD_OK;
}


/**
  * @brief  USBD_MIDI_SetRxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Rx Buffer
  * @retval status
  */
uint8_t  USBD_MIDI_SetRxBuffer(USBD_HandleTypeDef *pdev, uint8_t  *pbuff)
{
  USBD_MIDI_HandleTypeDef   *hmidi = (USBD_MIDI_HandleTypeDef *) pdev->pClassData;

  hmidi->RxBuffer = pbuff;

  return USBD_OK;
}


/**
* @brief  USBD_MIDI_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CD  Interface callback
  * @retval status
  */
uint8_t  USBD_MIDI_RegisterInterface(USBD_HandleTypeDef   *pdev, USBD_MIDI_ItfTypeDef  *fops)
{
  uint8_t  ret = USBD_FAIL;

  if (fops != NULL)
  {
    pdev->pUserData = fops;
    ret = USBD_OK;
  }

  return ret;
}
