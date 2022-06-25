#ifndef _MAINCODE_H
#define _MAINCODE_H

extern UART_HandleTypeDef huart1;
extern SPI_HandleTypeDef hspi1;

void mainloop(void);
void lcd_start(void);
void lcd_print(void);

#endif /* _MAINCODE_H */
