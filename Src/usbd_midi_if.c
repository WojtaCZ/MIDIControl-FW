#include "usbd_midi_if.h"

#define MIDI_APP_RX_DATA_SIZE  1000
#define MIDI_APP_TX_DATA_SIZE  1000

uint8_t MIDIRxBufferFS[MIDI_APP_RX_DATA_SIZE];
uint8_t MIDITxBufferFS[MIDI_APP_TX_DATA_SIZE];

extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

USBD_MIDI_ItfTypeDef USBD_MIDI_Interface_fops_FS =
{
  MIDI_Init_FS,
  MIDI_DeInit_FS,
  NULL,
  MIDI_Receive_FS
};


int8_t MIDI_Init_FS(void)
{

  /* Set Application Buffers */
  USBD_MIDI_SetTxBuffer(&hUsbDeviceFS, MIDITxBufferFS, 0);
  USBD_MIDI_SetRxBuffer(&hUsbDeviceFS, MIDIRxBufferFS);

  return (USBD_OK);

}


int8_t MIDI_DeInit_FS(void)
{

  return (USBD_OK);
}


int8_t MIDI_Receive_FS(uint8_t* Buf, uint32_t *Len)
{

  USBD_MIDI_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_MIDI_ReceivePacket(&hUsbDeviceFS);

  USB_MIDI_received_handle((char*)Buf, *Len);

  return (USBD_OK);
}


uint8_t MIDI_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;

  USB_MIDI_transmit_handle((char*) Buf, Len);

  USBD_MIDI_HandleTypeDef *hmidi = (USBD_MIDI_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hmidi->TxState != 0){
    return USBD_BUSY;
  }

  USBD_MIDI_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_MIDI_TransmitPacket(&hUsbDeviceFS);
  return result;
}
