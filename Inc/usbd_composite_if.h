/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_COMPOSITE_IF_H__
#define __USBD_COMPOSITE_IF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "../Middlewares/ST/STM32_USB_Device_Library/Class/COMPOSITE/Inc/usbd_composite.h"

extern USBD_COMPOSITE_ItfTypeDef USBD_COMPOSITE_Interface_fops_FS;
extern USBD_HandleTypeDef hUsbDeviceFS;

uint8_t COMPOSITE_Transmit_FS(uint8_t* Buf, uint16_t Len);
int8_t COMPOSITE_Init_FS(void);
int8_t COMPOSITE_DeInit_FS(void);
int8_t COMPOSITE_Receive_FS(uint8_t* pbuf, uint32_t *Len);
int8_t COMPOSITE_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);


#ifdef __cplusplus
}
#endif

#endif
