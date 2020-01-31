#include "ws2812.h"

static uint8_t LEDbuffer[STRIP_NUMBER][LED_BUFFER_SIZE];
extern TIM_HandleTypeDef htim17;
extern TIM_HandleTypeDef htim15;

void ws2812_init(void) {
	fillBufferBlack(0);
	fillBufferBlack(1);
	HAL_TIM_PWM_Start_DMA(&htim17, TIM_CHANNEL_1, (uint32_t *) LEDbuffer[0],
			LED_BUFFER_SIZE);

	HAL_TIM_PWM_Start_DMA(&htim15, TIM_CHANNEL_1, (uint32_t *) LEDbuffer[1],
				LED_BUFFER_SIZE);
}

void setLEDcolor(uint8_t strip, uint32_t LEDnumber, uint8_t RED, uint8_t GREEN, uint8_t BLUE) {
	uint8_t tempBuffer[24];
	uint32_t i;
	uint32_t LEDindex;
	LEDindex = LEDnumber % LED_NUMBER;

	for (i = 0; i < 8; i++) // GREEN data
		tempBuffer[i] = ((GREEN << i) & 0x80) ? WS2812_1 : WS2812_0;
	for (i = 0; i < 8; i++) // RED
		tempBuffer[8 + i] = ((RED << i) & 0x80) ? WS2812_1 : WS2812_0;
	for (i = 0; i < 8; i++) // BLUE
		tempBuffer[16 + i] = ((BLUE << i) & 0x80) ? WS2812_1 : WS2812_0;

	for (i = 0; i < 24; i++)
		LEDbuffer[strip][RESET_SLOTS_BEGIN + LEDindex * 24 + i] = tempBuffer[i];

}

void setWHOLEcolor(uint8_t strip, uint8_t RED, uint8_t GREEN, uint8_t BLUE) {
	uint32_t index;

	for (index = 0; index < LED_NUMBER; index++)
		setLEDcolor(strip, index, RED, GREEN, BLUE);
}

void fillBufferBlack(uint8_t strip) {
	/*Fill LED buffer - ALL OFF*/
	uint32_t index, buffIndex;
	buffIndex = 0;

	for (index = 0; index < RESET_SLOTS_BEGIN; index++) {
		LEDbuffer[strip][buffIndex] = WS2812_RESET;
		buffIndex++;
	}
	for (index = 0; index < LED_DATA_SIZE; index++) {
		LEDbuffer[strip][buffIndex] = WS2812_0;
		buffIndex++;
	}
	buffIndex++;
	for (index = 0; index < RESET_SLOTS_END; index++) {
		LEDbuffer[strip][buffIndex] = 0;
		buffIndex++;
	}
}

void fillBufferWhite(uint8_t strip) {
	/*Fill LED buffer - ALL OFF*/
	uint32_t index, buffIndex;
	buffIndex = 0;

	for (index = 0; index < RESET_SLOTS_BEGIN; index++) {
		LEDbuffer[strip][buffIndex] = WS2812_RESET;
		buffIndex++;
	}
	for (index = 0; index < LED_DATA_SIZE; index++) {
		LEDbuffer[strip][buffIndex] = WS2812_1;
		buffIndex++;
	}
	buffIndex++;
	for (index = 0; index < RESET_SLOTS_END; index++) {
		LEDbuffer[strip][buffIndex] = 0;
		buffIndex++;
	}
}
