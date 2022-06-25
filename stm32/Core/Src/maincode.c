#include <stdint.h>
#include "main.h"
#include "maincode.h"
#include "liquidcrystal_i2c.h"

uint8_t rData [] = {
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 00:07
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 07:15
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 16:23
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 24:31
	  0,   0,   0,   0,                     //led 32:05
	  0,   0,   0,   0,                     //swt 36:39
	  0,   0,   0,   0,   0,   0,   0,   0, //btn 40:47
	255
};

uint8_t tData [] =  {
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 00:07
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 07:15
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 16:23
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 24:31
	  0,   0,   0,   0,                     //led 32:05
	  0,   0,   0,   0,                     //swt 36:39
	  0,   0,   0,   0,   0,   0,   0,   0, //btn 40:47
	255
};

uint8_t screen[33] = "Starting...                     ";
int8_t sec = 0;
int8_t playing = 0;

void lcd_start() {
	uint8_t rows = 2;
	HD44780_Init(rows);
	lcd_print();
}

void lcd_print() {
	HD44780_SetCursor(0,0);
	HD44780_PrintStr((const char*)screen);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
	if(playing) {
		sec--;
		if(sec <= 0) {
			playing = 0;
			sec = 0;
		}
	}
	uint8_t d0 = (sec / 10) + 48;
	uint8_t d1 = (sec % 10) + 48;
	for (uint8_t i = 0; i < sizeof(screen)-1; i++) screen[i] = ' ';
	screen[0] = d0;
	screen[1] = d1;
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {

	if(rData[40] && !tData[40]) { // +5s
		sec += 5;
		if(sec > 60) sec = 60;
	}

	if(rData[41] && !tData[41]) { // -5s
		sec -= 5;
		if(sec < 0) sec = 0;
	}

	if(rData[42] && !tData[42]) { // pause / cancel
		if(!playing) sec = 0;
		else playing = 0;
	}

	if(rData[43] && !tData[43]) { // play
		playing = 1;
	}

	for (uint8_t i = 0; i < sizeof(tData); i++) {
		tData[i] = (i < sizeof(screen) - 1) ? screen[i] : rData[i];
	}
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, !rData[39]);
	tData[32] = !HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin);

}

void mainloop(void) {

	HAL_SPI_TransmitReceive_IT(&hspi1, tData, rData, sizeof(tData));
	lcd_print();

}
