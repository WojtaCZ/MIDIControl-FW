/**
  ******************************************************************************
  * @file    usbd_cdc.h
  * @author  MCD Application Team
  * @brief   header file for the usbd_cdc.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_MIDI_H
#define __USB_MIDI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "../Middlewares/ST/STM32_USB_Device_Library/Class/COMPOSITE/Inc/usbd_composite.h"


uint8_t  USBD_MIDI_Init(USBD_HandleTypeDef *pdev,
                              uint8_t cfgidx);

uint8_t  USBD_MIDI_DeInit(USBD_HandleTypeDef *pdev,
                                uint8_t cfgidx);

uint8_t  USBD_MIDI_Setup(USBD_HandleTypeDef *pdev,
                               USBD_SetupReqTypedef *req);

uint8_t  USBD_MIDI_DataIn(USBD_HandleTypeDef *pdev,
                                uint8_t epnum);

uint8_t  USBD_MIDI_DataOut(USBD_HandleTypeDef *pdev,
                                 uint8_t epnum);

uint8_t  USBD_MIDI_SetTxBuffer(USBD_HandleTypeDef   *pdev,
                              uint8_t  *pbuff,
                              uint16_t length);

uint8_t  USBD_MIDI_SetRxBuffer(USBD_HandleTypeDef   *pdev,
                              uint8_t  *pbuff);

uint8_t  USBD_MIDI_ReceivePacket(USBD_HandleTypeDef *pdev);

uint8_t  USBD_MIDI_TransmitPacket(USBD_HandleTypeDef *pdev, uint8_t* Buf, uint16_t Len);


typedef struct
{
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
} USBD_MIDI_LineCodingTypeDef;

typedef struct _USBD_MIDI_Itf
{
  int8_t (* Init)(void);
  int8_t (* DeInit)(void);
  int8_t (* Control)(uint8_t cmd, uint8_t *pbuf, uint16_t length);
  int8_t (* Receive)(uint8_t *Buf, uint32_t *Len);

} USBD_MIDI_ItfTypeDef;


typedef struct
{
  uint32_t data[CDC_DATA_HS_MAX_PACKET_SIZE / 4U];      /* Force 32bits alignment */
  uint8_t  CmdOpCode;
  uint8_t  CmdLength;
  uint8_t  *RxBuffer;
  uint8_t  *TxBuffer;
  uint32_t RxLength;
  uint32_t TxLength;

  __IO uint32_t TxState;
  __IO uint32_t RxState;
}
USBD_MIDI_HandleTypeDef;


uint8_t  USBD_MIDI_RegisterInterface(USBD_HandleTypeDef   *pdev, USBD_MIDI_ItfTypeDef  *fops);


/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /* __USB_CDC_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
