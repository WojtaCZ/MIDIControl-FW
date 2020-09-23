/**
  ******************************************************************************
  * @file    USBD_COMPOSITE.c
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
#include "usbd_cdc.h"
#include  "../Middlewares/ST/STM32_USB_Device_Library/Class/COMPOSITE/Inc/usbd_composite.h"
#include  "../Middlewares/ST/STM32_USB_Device_Library/Class/MIDI/Inc/usbd_midi.h"
#include "usbd_ctlreq.h"
#include "main.h"



static uint8_t  USBD_COMPOSITE_Init(USBD_HandleTypeDef *pdev,
                              uint8_t cfgidx);

static uint8_t  USBD_COMPOSITE_DeInit(USBD_HandleTypeDef *pdev,
                                uint8_t cfgidx);

static uint8_t  USBD_COMPOSITE_Setup(USBD_HandleTypeDef *pdev,
                               USBD_SetupReqTypedef *req);

static uint8_t  USBD_COMPOSITE_DataIn(USBD_HandleTypeDef *pdev,
                                uint8_t epnum);

static uint8_t  USBD_COMPOSITE_DataOut(USBD_HandleTypeDef *pdev,
                                 uint8_t epnum);

static uint8_t  USBD_COMPOSITE_EP0_RxReady(USBD_HandleTypeDef *pdev);

static uint8_t  *USBD_COMPOSITE_GetFSCfgDesc(uint16_t *length);

static uint8_t  *USBD_COMPOSITE_GetHSCfgDesc(uint16_t *length);

static uint8_t  *USBD_COMPOSITE_GetOtherSpeedCfgDesc(uint16_t *length);

uint8_t  *USBD_COMPOSITE_GetDeviceQualifierDescriptor(uint16_t *length);


/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_COMPOSITE_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */

/** @defgroup USBD_COMPOSITE_Private_Variables
  * @{
  */


/* CDC interface class callbacks structure */
USBD_ClassTypeDef  USBD_COMPOSITE =
{
  USBD_COMPOSITE_Init,
  USBD_COMPOSITE_DeInit,
  USBD_COMPOSITE_Setup,
  NULL,                 /* EP0_TxSent, */
  USBD_COMPOSITE_EP0_RxReady,
  USBD_COMPOSITE_DataIn,
  USBD_COMPOSITE_DataOut,
  NULL,
  NULL,
  NULL,
  USBD_COMPOSITE_GetHSCfgDesc,
  USBD_COMPOSITE_GetFSCfgDesc,
  USBD_COMPOSITE_GetOtherSpeedCfgDesc,
  USBD_COMPOSITE_GetDeviceQualifierDescriptor,
};


/* USB CDC device Configuration Descriptor */
__ALIGN_BEGIN uint8_t USBD_COMPOSITE_CfgHSDesc[USB_CDC_CONFIG_DESC_SIZ] __ALIGN_END =
{
  /*Configuration Descriptor*/
  0x09,   /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */
  USB_CDC_CONFIG_DESC_SIZ,                /* wTotalLength:no of returned bytes */
  0x00,
  0x02,   /* bNumInterfaces: 2 interface */
  0x01,   /* bConfigurationValue: Configuration value */
  0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
  0xC0,   /* bmAttributes: self powered */
  0x32,   /* MaxPower 0 mA */

  /*---------------------------------------------------------------------------*/

  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
  /* Interface descriptor type */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x01,   /* bNumEndpoints: One endpoints used */
  0x02,   /* bInterfaceClass: Communication Interface Class */
  0x02,   /* bInterfaceSubClass: Abstract Control Model */
  0x01,   /* bInterfaceProtocol: Common AT commands */
  0x00,   /* iInterface: */

  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x00,   /* bDescriptorSubtype: Header Func Desc */
  0x10,   /* bcdCDC: spec release number */
  0x01,

  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
  0x00,   /* bmCapabilities: D0+D1 */
  0x01,   /* bDataInterface: 1 */

  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,   /* bmCapabilities */

  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc */
  0x00,   /* bMasterInterface: Communication class interface */
  0x01,   /* bSlaveInterface0: Data Class Interface */

  /*Endpoint 2 Descriptor*/
  0x07,                           /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,   /* bDescriptorType: Endpoint */
  CDC_CMD_EP,                     /* bEndpointAddress */
  0x03,                           /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SIZE),     /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SIZE),
  CDC_HS_BINTERVAL,                           /* bInterval: */
  /*---------------------------------------------------------------------------*/

  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
  0x01,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: Two endpoints used */
  0x0A,   /* bInterfaceClass: CDC */
  0x00,   /* bInterfaceSubClass: */
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */

  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  CDC_OUT_EP,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
  0x00,                              /* bInterval: ignore for Bulk transfer */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  CDC_IN_EP,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
  0x00                               /* bInterval: ignore for Bulk transfer */
} ;


/* USB CDC device Configuration Descriptor */
__ALIGN_BEGIN uint8_t USBD_COMPOSITE_CfgFSDesc[USB_CDC_CONFIG_DESC_SIZ] __ALIGN_END =
{
  0x09,         /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_CDC_CONFIG_DESC_SIZ, /* wTotalLength: Bytes returned */
  0x00,
  0x04,         /*bNumInterfaces: 3 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x02,         /*iConfiguration: Index of string descriptor describing the configuration*/
  0xC0,         /*bmAttributes: bus powered and Supports Remote Wakeup */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
/* 09 bytes */

/*MIDI class interface descriptor*/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  MIDI_INTERFACE_IDX,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x00,   /* bNumEndpoints: Zero endpoints used */
  0x01,   /* bInterfaceClass: Audio */
  0x01,   /* bInterfaceSubClass: Control Device*/
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */

  /*MIDI class specific Audio Controll (AC) interface descriptor*/
  0x09,   /* bLength: AC Descriptor size */
  0x24,   /* bDescriptorType: */
  0x01,   /* bDescriptorSubtype:  */
  0x00,   /* bcdADC (B1):  */
  0x01,   /* bcdADC (B2):  */
  0x09,   /* wTotalLength (B1):  */
  0x00,   /* wTotalLength (B2):  */
  0x01,   /* bInCollection: */
  0x01,   /* baInterfaceNr(0): */


  /*MIDI streaming interface descriptor*/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  MIDI_INTERFACE_IDX+1,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: Two endpoints used */
  0x01,   /* bInterfaceClass: Audio */
  0x03,   /* bInterfaceSubClass: MIDI Streaming*/
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */

  /*MIDI class specific Midi Streaming (MS) interface descriptor*/
  0x07,   /* bLength: MS Descriptor size */
  0x24,   /* bDescriptorType: */
  0x01,   /* bDescriptorSubtype:  */
  0x00,   /* bcdADC (B1):  */
  0x01,   /* bcdADC (B2):  */
  0x41,   /* wTotalLength (B1):  */
  0x00,   /* wTotalLength (B2):  */

  /*MIDI IN Jack*/
  0x06,   /* bLength: Jack Descriptor size */
  0x24,   /* bDescriptorType: */
  0x02,   /* bDescriptorSubtype:  */
  0x01,   /* bJackType:  */
  0x01,   /* bJackID:  */
  0x00,   /* iJack:  */

  /*MIDI IN Jack*/
  0x06,   /* bLength: Jack Descriptor size */
  0x24,   /* bDescriptorType: */
  0x02,   /* bDescriptorSubtype:  */
  0x02,   /* bJackType:  */
  0x02,   /* bJackID:  */
  0x00,   /* iJack:  */

  /*MIDI OUT Jack*/
  0x09,   /* bLength: Jack Descriptor size */
  0x24,   /* bDescriptorType: */
  0x03,   /* bDescriptorSubtype:  */
  0x01,   /* bJackType:  */
  0x03,   /* bJackID:  */
  0x01,   /* bNrInputPins:  */
  0x02,   /* baSourceID:  */
  0x01,   /* BaSourcePin:  */
  0x00,   /* iJack:  */

  /*MIDI OUT Jack*/
  0x09,   /* bLength: Jack Descriptor size */
  0x24,   /* bDescriptorType: */
  0x03,   /* bDescriptorSubtype:  */
  0x02,   /* bJackType:  */
  0x04,   /* bJackID:  */
  0x01,   /* bNrInputPins:  */
  0x01,   /* baSourceID:  */
  0x01,   /* BaSourcePin:  */
  0x00,   /* iJack:  */


  /*Endpoint IN Descriptor*/
  0x09,                              /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,            /* bDescriptorType: Endpoint */
  MIDI_IN_EP,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(USB_MAX_PACKET_SIZE),		 /* wMaxPacketSize: */
  HIBYTE(USB_MAX_PACKET_SIZE),
  0x00,                              /* bInterval: ignore for Bulk transfer */
  0x00,                              /* bRefresh:  */
  0x00,                              /* bSyncAddress:*/

  /*Endpoint IN Descriptor*/
  0x05,                              /* bLength: Endpoint Descriptor size */
  0x25,                              /* bDescriptorType: Endpoint */
  0x01,                              /* bDescriptorSubtype */
  0x01,                              /* bNumEmbMIDIJack:  */
  0x03,                              /* baAssocJackID: */


  /*Endpoint OUT Descriptor*/
  0x09,                              /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,            /* bDescriptorType: Endpoint */
  MIDI_OUT_EP,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(MIDI_OUT_PACKET_SIZE),		 /* wMaxPacketSize: */
  HIBYTE(MIDI_OUT_PACKET_SIZE),
  0x00,                              /* bInterval: ignore for Bulk transfer */
  0x00,                              /* bRefresh:  */
  0x00,                              /* bSyncAddress:*/

  /*Endpoint OUT Descriptor*/
  0x05,                              /* bLength: Endpoint Descriptor size */
  0x25,                              /* bDescriptorType: Endpoint */
  0x01,                              /* bDescriptorSubtype */
  0x01,                              /* bNumEmbMIDIJack:  */
  0x01,                              /* baAssocJackID: */

  /**************		MIDI PART END			****************/

  /**************		CDC PART BEGIN			****************/

/******** IAD should be positioned just before the CDC interfaces ******
		 IAD to associate the two CDC interfaces */

  0x08, /* bLength */
  0x0B, /* bDescriptorType */
  CDC_INTERFACE_IDX, /* bFirstInterface */
  0x02, /* bInterfaceCount */
  0x02, /* bFunctionClass */
  0x02, /* bFunctionSubClass */
  0x01, /* bFunctionProtocol */
  0x00, /* iFunction (Index of string descriptor describing this function) */
			/* 08 bytes */

  /********************  CDC interfaces ********************/

  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
  /* Interface descriptor type */
  CDC_INTERFACE_IDX,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x01,   /* bNumEndpoints: One endpoints used */
  0x02,   /* bInterfaceClass: Communication Interface Class */
  0x02,   /* bInterfaceSubClass: Abstract Control Model */
  0x01,   /* bInterfaceProtocol: Common AT commands */
  0x01,   /* iInterface: */
  /* 09 bytes */

  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x00,   /* bDescriptorSubtype: Header Func Desc */
  0x10,   /* bcdCDC: spec release number */
  0x01,
  /* 05 bytes */

  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
  0x00,   /* bmCapabilities: D0+D1 */
  CDC_INTERFACE_IDX + 1,   /* bDataInterface: 2 */
  /* 05 bytes */

  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,   /* bmCapabilities */
  /* 04 bytes */

  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc */
  CDC_INTERFACE_IDX,   /* bMasterInterface: Communication class interface */
  CDC_INTERFACE_IDX + 1,   /* bSlaveInterface0: Data Class Interface */
  /* 05 bytes */

  /*Endpoint 2 Descriptor*/
  0x07,                          /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,        /* bDescriptorType: Endpoint */
  CDC_CMD_EP,                    /* bEndpointAddress */
  0x03,                          /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SIZE),   /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SIZE),
  0x10,                          /* bInterval: */
  /* 07 bytes */

  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_INTERFACE,       /* bDescriptorType: */
  CDC_INTERFACE_IDX + 1,         /* bInterfaceNumber: Number of Interface */
  0x00,                          /* bAlternateSetting: Alternate setting */
  0x02,                          /* bNumEndpoints: Two endpoints used */
  0x0A,                          /* bInterfaceClass: CDC */
  0x00,                          /* bInterfaceSubClass: */
  0x00,                          /* bInterfaceProtocol: */
  0x00,                          /* iInterface: */
  /* 09 bytes */

  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,        /* bDescriptorType: Endpoint */
  CDC_OUT_EP,                    /* bEndpointAddress */
  0x02,                          /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
  0x00,                          /* bInterval: ignore for Bulk transfer */
  /* 07 bytes */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,        /* bDescriptorType: Endpoint */
  CDC_IN_EP,                     /* bEndpointAddress */
  0x02,                          /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
  0x00,                          /* bInterval */
  /* 07 bytes */
} ;

__ALIGN_BEGIN uint8_t USBD_COMPOSITE_OtherSpeedCfgDesc[USB_CDC_CONFIG_DESC_SIZ] __ALIGN_END =
{
  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION,
  USB_CDC_CONFIG_DESC_SIZ,
  0x00,
  0x02,   /* bNumInterfaces: 2 interfaces */
  0x01,   /* bConfigurationValue: */
  0x04,   /* iConfiguration: */
  0xC0,   /* bmAttributes: */
  0x32,   /* MaxPower 100 mA */

  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
  /* Interface descriptor type */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x01,   /* bNumEndpoints: One endpoints used */
  0x02,   /* bInterfaceClass: Communication Interface Class */
  0x02,   /* bInterfaceSubClass: Abstract Control Model */
  0x01,   /* bInterfaceProtocol: Common AT commands */
  0x00,   /* iInterface: */

  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x00,   /* bDescriptorSubtype: Header Func Desc */
  0x10,   /* bcdCDC: spec release number */
  0x01,

  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
  0x00,   /* bmCapabilities: D0+D1 */
  0x01,   /* bDataInterface: 1 */

  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,   /* bmCapabilities */

  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc */
  0x00,   /* bMasterInterface: Communication class interface */
  0x01,   /* bSlaveInterface0: Data Class Interface */

  /*Endpoint 2 Descriptor*/
  0x07,                           /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,         /* bDescriptorType: Endpoint */
  CDC_CMD_EP,                     /* bEndpointAddress */
  0x03,                           /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SIZE),     /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SIZE),
  CDC_FS_BINTERVAL,                           /* bInterval: */

  /*---------------------------------------------------------------------------*/

  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
  0x01,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: Two endpoints used */
  0x0A,   /* bInterfaceClass: CDC */
  0x00,   /* bInterfaceSubClass: */
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */

  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  CDC_OUT_EP,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  0x40,                              /* wMaxPacketSize: */
  0x00,
  0x00,                              /* bInterval: ignore for Bulk transfer */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,     /* bDescriptorType: Endpoint */
  CDC_IN_EP,                        /* bEndpointAddress */
  0x02,                             /* bmAttributes: Bulk */
  0x40,                             /* wMaxPacketSize: */
  0x00,
  0x00                              /* bInterval */
};


/**
  * @brief  USBD_COMPOSITE_Init
  *         Initialize the CDC interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_COMPOSITE_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx){


	uint8_t ret = USBD_CDC_Init(pdev, cfgidx);

	//if(ret != 0) return ret;

	ret = USBD_MIDI_Init(pdev, cfgidx);

	//if(ret != 0) return ret;

	return USBD_OK;
}

/**
  * @brief  USBD_COMPOSITE_Init
  *         DeInitialize the CDC layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_COMPOSITE_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
	USBD_CDC_DeInit(pdev, cfgidx);

	USBD_MIDI_DeInit(pdev, cfgidx);

	return USBD_OK;
}

/**
  * @brief  USBD_COMPOSITE_Setup
  *         Handle the CDC specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_COMPOSITE_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req){
	USBD_CDC_Setup(pdev, req);
}

/**
  * @brief  USBD_COMPOSITE_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  USBD_COMPOSITE_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	if(epnum == CDC_EP_IDX){
		USBD_CDC_DataIn(pdev, epnum);
	}else if(epnum == MIDI_EP_IDX){
		USBD_MIDI_DataIn(pdev, epnum);
	}

}

/**
  * @brief  USBD_COMPOSITE_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  USBD_COMPOSITE_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	if(epnum == CDC_EP_IDX){
		USBD_CDC_DataOut(pdev, epnum);
	}else if(epnum == MIDI_EP_IDX){
		USBD_MIDI_DataOut(pdev, epnum);
	}
}

/**
  * @brief  USBD_COMPOSITE_EP0_RxReady
  *         Handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_COMPOSITE_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	USBD_CDC_EP0_RxReady(pdev);
}


uint8_t  USBD_COMPOSITE_RegisterInterface(USBD_HandleTypeDef   *pdev,
                                    USBD_COMPOSITE_ItfTypeDef *fops)
{
  uint8_t  ret = USBD_FAIL;

  if (fops != NULL)
  {
    pdev->pUserData = fops;
    ret = USBD_OK;
  }

  return ret;
}

/**
  * @brief  USBD_COMPOSITE_GetFSCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_COMPOSITE_GetFSCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_COMPOSITE_CfgFSDesc);
  return USBD_COMPOSITE_CfgFSDesc;
}

/**
  * @brief  USBD_COMPOSITE_GetHSCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_COMPOSITE_GetHSCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_COMPOSITE_CfgHSDesc);
  return USBD_COMPOSITE_CfgHSDesc;
}

/**
  * @brief  USBD_COMPOSITE_GetCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_COMPOSITE_GetOtherSpeedCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_COMPOSITE_OtherSpeedCfgDesc);
  return USBD_COMPOSITE_OtherSpeedCfgDesc;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_COMPOSITE_GetDeviceQualifierDescriptor(uint16_t *length)
{
  *length = sizeof(USBD_COMPOSITE_DeviceQualifierDesc);
  return USBD_COMPOSITE_DeviceQualifierDesc;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
