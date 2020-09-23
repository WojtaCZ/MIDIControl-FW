/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_MIDI_IF_H__
#define __USBD_MIDI_IF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "../Middlewares/ST/STM32_USB_Device_Library/Class/MIDI/Inc/usbd_midi.h"

extern USBD_MIDI_ItfTypeDef USBD_MIDI_Interface_fops_FS;
extern USBD_HandleTypeDef hUsbDeviceFS;

uint8_t MIDI_Transmit_FS(uint8_t* Buf, uint16_t Len);
int8_t MIDI_Init_FS(void);
int8_t MIDI_DeInit_FS(void);
int8_t MIDI_Receive_FS(uint8_t* pbuf, uint32_t *Len);


extern void USB_MIDI_received_handle(char * buff, uint32_t len);
extern void USB_MIDI_transmit_handle(char * buff, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
