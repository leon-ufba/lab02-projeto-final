#define BAUD 9600
#include <fun.h>
#include <strc.h>

volatile const uint8_t powerPinGroup = D;
volatile const uint8_t controlPinGroup = D;
volatile const uint8_t powerPin = 2;
volatile const uint8_t controlPin = 3;

volatile bool isConnected = false;
volatile bool isPowerBlocked = false;
volatile bool powerState = LOW;

volatile uint8_t received;
volatile bool beaconFlag = false;
volatile uint8_t powerBlockCounter = 0;
volatile uint8_t connectionTimeCounter = 0;

volatile struct DataStruct dataToSend;


volatile uint8_t stm_tData[] = {
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 00:07
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 07:15
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 16:23
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 24:31
    0,   0,   0,   0,                     //led 32:05
	  0,   0,   0,   0,                     //swt 36:39
	  0,   0,   0,   0,   0,   0,   0,   0, //btn 40:47
	255
};

volatile uint8_t stm_rData[] = {
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 00:07
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 07:15
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 16:23
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 24:31
    0,   0,   0,   0,                     //led 32:05
	  0,   0,   0,   0,                     //swt 36:39
	  0,   0,   0,   0,   0,   0,   0,   0, //btn 40:47
	255
};

volatile uint8_t pc_tData[] = {
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 00:07
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 07:15
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 16:23
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 24:31
    0,   0,   0,   0,                     //led 32:05
    0,   0,   0,   0,                     //swt 36:39
    0,   0,   0,   0,   0,   0,   0,   0, //btn 40:47
	255
};

volatile uint8_t pc_rData[] = {
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 00:07
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 07:15
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 16:23
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', //lcd 24:31
    0,   0,   0,   0,                     //led 32:05
	  0,   0,   0,   0,                     //swt 36:39
	  0,   0,   0,   0,   0,   0,   0,   0, //btn 40:47
	255
};

// interruptions
ISR(TIMER1_OVF_vect) {
	loopFlag = true;
	delayFlag = false;
}

volatile uint8_t uartRxCounter = 0;

ISR(USART_RX_vect) {
	pc_rData[uartRxCounter] = UDR0;
	uartRxCounter++;
	if(uartRxCounter == sizeof(pc_rData)) {
		uartRxCounter = 0;
		uartFlag = true;
	}
}

void start() {
	pinMode(B, 2, OUTPUT);
	pinWrite(B, 2, LOW);

	Uart.init();
	SPI.init();
	timerSet();

	sei();
}

void stmConn() {
	SPI.send_recv((uint8_t *)stm_tData, (uint8_t *)stm_rData, sizeof(stm_tData));

	for (uint8_t i = 0; i < sizeof(pc_tData); i++) {
		pc_tData[i] = stm_rData[i];
	}

	Uart.send((uint8_t *)(pc_tData), sizeof(pc_tData));
	// Uart.send((uint8_t *)(stm_rData), sizeof(stm_rData));
	// Uart.send(" || ");
	// Uart.send((uint8_t *)(stm_tData), sizeof(stm_tData));
}

void pcConn() {
	for(uint8_t i = 0; i < sizeof(stm_tData); i++) {
		stm_tData[i] = pc_rData[i];
	}
	// Uart.send((uint8_t *)(stm_tData), sizeof(stm_tData));
}
// ********************************AAAAAAAAAAAAAAAA
// kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkJJJJJJJJJJJJJJJJ
void loop() {
	while (true) {
		if (uartFlag) {
			uartFlag = 0;
			pcConn();
		}
		if (loopFlag) {
			loopFlag = 0;
			timerReset(100);
			stmConn();
		}
	}
}

int main() {
	start();
	loop();
}
