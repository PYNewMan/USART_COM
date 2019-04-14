#ifndef __UART_H__
#define __UART_H__

#include "sys.h"
#define RS485_TX_EN PAout(1)	//485模式控制：接收为0，发送为1

void uart2_init(u32 bound);
void uart4_init(u32 bound); 
void USART2_Send_Byte(u8 Data);
void Uart2_SendString(char *pt, int len);
void UART4_Send_Byte(u16 Data);
void Uart4_SendString(char *pt);



#endif






