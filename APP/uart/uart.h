#ifndef APP_UART_UART_H_
#define APP_UART_UART_H_

#include "DSP2833x_Device.h"     // DSP2833x ͷ�ļ�
#include "DSP2833x_Examples.h"   // DSP2833x �������ͷ�ļ�
#include "leds.h"

#define     BUFFER_SIZE       4
#define     FRAME_HEADER_1      0x2C
#define     FRAME_HEADER_2      0x12
#define     FRAME_TAIL          0x5B

void SCIb_SendByte(int dat);
void UARTb_Init(Uint32 baud);
void UARTb_SendByte(int a);
void UARTb_SendString(char * msg);

__interrupt void seriblRxISR(void);

#endif /* APP_UART_UART_H_ */
