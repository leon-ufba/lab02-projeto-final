#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>
#include <stddef.h>         //size_t type

class SpiClass {
 public:
 
  SpiClass() {}

  static void init() {
    // Set up SPI, MSB first, Master, mode 0, clock fck/128
    DDRB |= (1<<3)|(1<<5);           // SCK, MOSI as outputs
    DDRB &= ~(1<<4);                 // MISO as input
    SPCR |= (1<<MSTR);               // Set as Master
    SPSR |= (0<<SPI2X);              // divided clock
    SPCR |= (1<<SPR1);               // divided clock
    SPCR |= (1<<SPR0);               // divided clock
    SPCR |= (1<<SPE);                // Enable SPI
  }

  static uint8_t send(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
  }

  static void send(uint8_t *data, size_t len) {
    while(len--) {
        SPDR = *data++;
        while (!(SPSR & (1 << SPIF)));
    }
  }
  
  static uint8_t receive() {
    SPDR = 0xFF;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
  }
  
  static void receive(uint8_t* data, size_t len) {
    while(len--) *data++ = receive();
  }


  static void send_recv(uint8_t *tData, uint8_t *rData, size_t len) {
    while(len--) {
        SPDR = *tData++;
        while (!(SPSR & (1 << SPIF)));
        *rData++ = SPDR;
    }
  }

};

#endif /* SPI_H_ */