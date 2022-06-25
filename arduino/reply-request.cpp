#define BAUD 115200
#include "fun.h"


volatile const uint8_t LEDR = 4;
volatile const uint8_t LEDG = 5;
volatile uint8_t received;
volatile uint8_t pinValue;
volatile bool delayFlag = false;

// interruptions
ISR(TIMER1_OVF_vect) {
  loopFlag = true;
  delayFlag = false;
}

void delay() {
  timerReset();
  delayFlag = true;
  while (delayFlag) {
  };
}

void start() {
  Uart.init();
  pinMode(D, LEDR, OUTPUT);
  pinMode(D, LEDG, OUTPUT);
  pinWrite(D, LEDR, LOW);
  pinWrite(D, LEDG, LOW);
  timerSet();
}

void loop() {
  while (true) {
    if (Uart.isReceiveComplete()) {  // se byte pronto para leitura
      received = Uart.receive();
      switch (received) {  // verifica qual numero recebido
        case 1:
          Uart.send("Pong 1\n");
          pinValue = pinRead(D, LEDR);
          pinWrite(D, LEDR, !pinValue); // inverte estado do LED
          break;
        case 2:
          Uart.send("Pong 2\n");
          pinValue = pinRead(D, LEDR);
          pinWrite(D, LEDG, !pinValue);  // inverte estado do LED
          break;
        case 90:
          Uart.send("Pong 90\n");
          break;
        default:
          Uart.send("Pong 404\n");
      }
    }
    // função que só é executada a cada 1000ms
    if (loopFlag) {
      loopFlag = 0;
      timerReset();  // reset the TIMER1
      Uart.send("ONLINE\n");
    }
  }
}