################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/adc.c \
../Src/bluetooth.c \
../Src/crc.c \
../Src/devStatus.c \
../Src/dma.c \
../Src/gpio.c \
../Src/i2c.c \
../Src/main.c \
../Src/midiControl.c \
../Src/msgDecoder.c \
../Src/oled.c \
../Src/rtc.c \
../Src/ssd1306.c \
../Src/ssd1306_fonts.c \
../Src/stm32g4xx_hal_msp.c \
../Src/stm32g4xx_it.c \
../Src/sys.c \
../Src/syscalls.c \
../Src/system_stm32g4xx.c \
../Src/tim.c \
../Src/ucpd.c \
../Src/usart.c \
../Src/usb_device.c \
../Src/usbd_cdc_if.c \
../Src/usbd_composite_if.c \
../Src/usbd_conf.c \
../Src/usbd_desc.c \
../Src/usbd_midi_if.c \
../Src/ws2812.c 

OBJS += \
./Src/adc.o \
./Src/bluetooth.o \
./Src/crc.o \
./Src/devStatus.o \
./Src/dma.o \
./Src/gpio.o \
./Src/i2c.o \
./Src/main.o \
./Src/midiControl.o \
./Src/msgDecoder.o \
./Src/oled.o \
./Src/rtc.o \
./Src/ssd1306.o \
./Src/ssd1306_fonts.o \
./Src/stm32g4xx_hal_msp.o \
./Src/stm32g4xx_it.o \
./Src/sys.o \
./Src/syscalls.o \
./Src/system_stm32g4xx.o \
./Src/tim.o \
./Src/ucpd.o \
./Src/usart.o \
./Src/usb_device.o \
./Src/usbd_cdc_if.o \
./Src/usbd_composite_if.o \
./Src/usbd_conf.o \
./Src/usbd_desc.o \
./Src/usbd_midi_if.o \
./Src/ws2812.o 

C_DEPS += \
./Src/adc.d \
./Src/bluetooth.d \
./Src/crc.d \
./Src/devStatus.d \
./Src/dma.d \
./Src/gpio.d \
./Src/i2c.d \
./Src/main.d \
./Src/midiControl.d \
./Src/msgDecoder.d \
./Src/oled.d \
./Src/rtc.d \
./Src/ssd1306.d \
./Src/ssd1306_fonts.d \
./Src/stm32g4xx_hal_msp.d \
./Src/stm32g4xx_it.d \
./Src/sys.d \
./Src/syscalls.d \
./Src/system_stm32g4xx.d \
./Src/tim.d \
./Src/ucpd.d \
./Src/usart.d \
./Src/usb_device.d \
./Src/usbd_cdc_if.d \
./Src/usbd_composite_if.d \
./Src/usbd_conf.d \
./Src/usbd_desc.d \
./Src/usbd_midi_if.d \
./Src/ws2812.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DUSE_FULL_LL_DRIVER '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32G431xx -I"D:/STM/MIDIControl/Inc" -I"D:/STM/MIDIControl/Drivers/STM32G4xx_HAL_Driver/Inc" -I"D:/STM/MIDIControl/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy" -I"D:/STM/MIDIControl/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"D:/STM/MIDIControl/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"D:/STM/MIDIControl/Drivers/CMSIS/Device/ST/STM32G4xx/Include" -I"D:/STM/MIDIControl/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


