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
#ifndef __USB_COMPOSITE_H
#define __USB_COMPOSITE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"



#define USB_MAX_PACKET_SIZE							512U

#define MIDI_INTERFACE_IDX 0x0			// Index of MIDI interface
#define CDC_INTERFACE_IDX 0x2			// Index of CDC interface

// endpoints numbers

#define MIDI_EP_IDX                     0x03
#define CDC_CMD_EP_IDX                  0x02
#define CDC_EP_IDX                      0x01

#define IN_EP_DIR						0x80 // Adds a direction bit

#define MIDI_OUT_EP                     MIDI_EP_IDX                     /* EP1 for BULK OUT */
#define MIDI_IN_EP                      (MIDI_EP_IDX | IN_EP_DIR)       /* EP1 for BULK IN */
#define CDC_CMD_EP                      (CDC_CMD_EP_IDX | IN_EP_DIR)   /* EP2 for CDC commands */
#define CDC_OUT_EP                      CDC_EP_IDX                     /* EP3 for data OUT */
#define CDC_IN_EP                       (CDC_EP_IDX | IN_EP_DIR)       /* EP3 for data IN */

#ifndef CDC_HS_BINTERVAL
#define CDC_HS_BINTERVAL                          0x10U
#endif /* CDC_HS_BINTERVAL */

#ifndef CDC_FS_BINTERVAL
#define CDC_FS_BINTERVAL                          0x10U
#endif /* CDC_FS_BINTERVAL */

/* CDC Endpoints parameters: you can fine tune these values depending on the needed baudrates and performance. */
#define CDC_DATA_HS_MAX_PACKET_SIZE                 512U  /* Endpoint IN & OUT Packet size */
#define CDC_DATA_FS_MAX_PACKET_SIZE                 128U  /* Endpoint IN & OUT Packet size */
#define CDC_CMD_PACKET_SIZE                         8U  /* Control Endpoint Packet size */

#define USB_CDC_CONFIG_DESC_SIZ                     167U
#define CDC_DATA_HS_IN_PACKET_SIZE                  CDC_DATA_HS_MAX_PACKET_SIZE
#define CDC_DATA_HS_OUT_PACKET_SIZE                 CDC_DATA_HS_MAX_PACKET_SIZE

#define CDC_DATA_FS_IN_PACKET_SIZE                  CDC_DATA_FS_MAX_PACKET_SIZE
#define CDC_DATA_FS_OUT_PACKET_SIZE                 CDC_DATA_FS_MAX_PACKET_SIZE

#define MIDI_OUT_PACKET_SIZE						0x04
#define MIDI_IN_PACKET_SIZE							0x04

extern USBD_ClassTypeDef  USBD_COMPOSITE;
#define USBD_COMPOSITE_CLASS    &USBD_COMPOSITE

typedef struct _USBD_COMPOSITE_Itf
{
  int8_t (* Init)(void);
  int8_t (* DeInit)(void);
  int8_t (* Control)(uint8_t cmd, uint8_t *pbuf, uint16_t length);
  int8_t (* Receive)(uint8_t *Buf, uint32_t *Len);

} USBD_COMPOSITE_ItfTypeDef;

uint8_t  USBD_COMPOSITE_RegisterInterface(USBD_HandleTypeDef   *pdev, USBD_COMPOSITE_ItfTypeDef *fops);


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
