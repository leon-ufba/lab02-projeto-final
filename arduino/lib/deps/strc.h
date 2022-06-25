
#include <avr/io.h>

#ifndef CUSTOM_STRUCT_H
#define CUSTOM_STRUCT_H

struct DataStruct {
	// uint8_t start = '\0';
	uint8_t button0 : 1;
	uint8_t button1 : 1;
	uint8_t button2 : 1;
	uint8_t button3 : 1;
	uint8_t switcher1 : 1;
	uint8_t switcher2 : 1;
	uint8_t switcher3 : 1;
	uint8_t switcher4 : 1;
	uint8_t led1 : 1;
	uint8_t led2 : 1;
	uint8_t led3 : 1;
	uint8_t led4 : 1;
	uint8_t padding : 4;
	uint8_t lcd[32];
	// uint8_t end = '\0';
	// const char *getCharArray() { return (const char *)(&(this->start)); }
};


#endif