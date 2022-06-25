#ifndef UART_H_
#define UART_H_

//default to Arduino oscillator
#ifndef F_CPU
    #define F_CPU 16000000UL
    #warning "F_CPU not defined! Assuming 16MHz."
#endif

//default to 115200
#ifndef BAUD
    #define BAUD 115200
#endif

#include <avr/io.h>         //hardware registers
#include <util/setbaud.h>   //BAUD registers calculations
#include <stddef.h>         //size_t type

class UartClass {
  public:
    /**
     * Returns true if a byte has been received and is ready to be read.
     * Will be reset when the data register UDR0 is read.
     * 
     * @return if a byte has been received and is ready to be read
     */
    static bool isReceiveComplete(void) {
      return UCSR0A & (1 << RXC0);
    }
    /**
     * Returns true when a transmission is completed.
     * 
     * @returns if a transmission is completed
     */
    static bool isTransmitComplete(void) {
      return UCSR0A & (1 << TXC0);
    }

    /**
     * Returns true when the data register UDR0 is empty
     * and new data can be written into it.
     * 
     * @returns if the data register UDR0 is empty
     */
    static bool isDataEmpty(void) {
      return UCSR0A & (1 << UDRE0);
    }
  
    UartClass() {}
    /**
     * Transmits a single byte and blocks until the transmission is completed.
     * 
     * @param data byte to transmit
     */
    static void send(uint8_t data) {
      while(!isDataEmpty());
      UDR0 = data;
      //wait for completed transmission so the user can be sure
      //that the byte is completely clocked out
      //and hopefully has been received by someone
      while(!isTransmitComplete());
    }

    /**
     * Transmits the bytes from the given location
     * and blocks until the transmission is completed.
     * 
     * @param data location of the bytes to transmit
     * @param len number of bytes to transmit
     */
    static void send(uint8_t *data, size_t len) {
      while(len--) {
          while(!isDataEmpty());
          UDR0 = *data++;
      }
      while(!isTransmitComplete());
    }

    /**
     * Transmits a string.
     * 
     * @param str string to transmit
     */
    static void send(const char * str) {
      while (*str) send(*str++);
    }

    /**
     * Blocks until a single byte is received and then returns it.
     * 
     * @return the received byte
     */
    static uint8_t receive(void) {
      while(!isReceiveComplete());
      return UDR0;
    }

    /**
     * Blocks until the given number of bytes are received
     * and writes them to the provided location.
     * 
     * @param data location for the received bytes to be written to
     * @param len number of bytes to receive
     */
    static void receive(uint8_t *data, size_t len) {
      while(len--) *data++ = receive();
    }

    static bool isAvailable(void) {
      return isReceiveComplete() && isTransmitComplete();
    }
    
    /**
     * Initializes the UART peripherals to enable transmitting and receiving
     * and redirects stdout and stdin to the UART if NO_UART_STD is not defined.
     * The BAUD rate is set by using setbaud.h.
     */
    static void init(void) {
      //setbaud.h values
      //https://www.nongnu.org/avr-libc/user-manual/group__util__setbaud.html
      UBRR0H = UBRRH_VALUE;
      UBRR0L = UBRRL_VALUE;

      #if USE_2X
          UCSR0A |= _BV(U2X0);
      #else
          UCSR0A &= ~(_BV(U2X0));
      #endif

      UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
      UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0); // Turn on the transmission, reception, and Receive interrupt
    }
};

#endif /* UART_H_ */