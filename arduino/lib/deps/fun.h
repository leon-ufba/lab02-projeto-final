#include <avr/io.h>
#include <avr/interrupt.h>
#include <uart.h>
#include <spi.h>

#ifndef CUSTOM_IO_H
#define CUSTOM_IO_H

#define INPUT		0
#define OUTPUT	1
#define LOW		0
#define HIGH	1
#define B	1
#define C	2
#define D	3

#define TIME_FILL	0xF424

volatile bool delayFlag = false;
volatile bool loopFlag = true;
volatile bool uartFlag = true;
volatile UartClass Uart;
volatile SpiClass SPI;

// configura um PIN como entrada ou saída
void pinMode(uint8_t x, uint8_t index, uint8_t value) {
  switch (x) {
    case (B):
      if (value)	DDRB |=  (1 << index);
      else				DDRB &= ~(1 << index);
      break;
    case (C):
      if (value)	DDRC |=  (1 << index);
      else				DDRC &= ~(1 << index);
      break;
    case (D):
      if (value)	DDRD |=  (1 << index);
      else				DDRD &= ~(1 << index);
      break;
    default:
      break;
  }
}

// muda o valor do PIN para alto ou baixo
void pinWrite(uint8_t x, uint8_t index, uint8_t value) {
  switch (x) {
    case (B):
      if (value)	PORTB |=  (1 << index);
      else				PORTB &= ~(1 << index);
      break;
    case (C):
      if (value)	PORTC |=  (1 << index);
      else				PORTC &= ~(1 << index);
      break;
    case (D):
      if (value)	PORTD |=  (1 << index);
      else				PORTD &= ~(1 << index);
      break;
    default:
      break;
  }
}

// lê o valor do PIN
uint8_t pinRead(uint8_t pinGroup, uint8_t index) {
  return (pinGroup & (1 << index));
}

// lê o valor do resultado da conversão ADC
uint16_t adcRead(uint8_t ADCchannel) {
  ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);	// seleciona o canal do ADC sem alterar alguns bits
  ADCSRA |= (1<<ADSC);													// modo de conversão simples
  while( ADCSRA & (1<<ADSC) );									// espera a conversão ser concluída
  return ADC;
}

// configura o ADC
void adcSet() {
  ADMUX |= (1<<REFS0);																	// 10-bits // AVcc com capacitor externo no pin AREF
  ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);	// pré-escalar de 128 e habilita o ADC
}

// configura o TIMER1
void timerSet(){
  TCCR1A = 0;												// operação normal pinos OC1A e OC1B desconectados
  TCCR1B = 0;												// limpa o registrador
  TCCR1B |= (1<<CS10)|(1 << CS12);	// configura prescaler para 1024: CS12 = 1 e CS10 = 1
  TCNT1 = TIME_FILL;								// incia timer com valor para que estouro ocorra em 1 segundo // 65536-(16MHz/1024/1Hz) = 49911 = 0xC2F7
  TIMSK1 |= (1 << TOIE1);						// habilita a interrupção do TIMER1
}

// reinicia o contador do TIMER1 para o valor desejado
void timerReset() {
  TCNT1 = TIME_FILL;
}

void timerReset(uint16_t dt) {
  uint32_t rdt = 65536 - ((16000 * (uint32_t)dt) >> 10);
  TCNT1 = rdt;
}

void delay(uint16_t dt = TIME_FILL) {
  if(dt == TIME_FILL) timerReset();
	else timerReset(dt);
	delayFlag = true;
	while (delayFlag) {
	};
}

#endif