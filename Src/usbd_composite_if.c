#include "usbd_composite_if.h"
#include "usbd_midi_if.h"
#include "usbd_cdc_if.h"


USBD_COMPOSITE_ItfTypeDef USBD_COMPOSITE_Interface_fops_FS =
{
  COMPOSITE_Init_FS,
  COMPOSITE_DeInit_FS,
  COMPOSITE_Control_FS,
  NULL
};


int8_t COMPOSITE_Init_FS(void)
{
	CDC_Init_FS();
	MIDI_Init_FS();

  return (USBD_OK);

}


int8_t COMPOSITE_DeInit_FS(void)
{
	CDC_DeInit_FS();
	MIDI_DeInit_FS();

  return (USBD_OK);
}


int8_t COMPOSITE_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length){
	CDC_Control_FS(cmd, pbuf, length);
}

