#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN1  			20  	//定义最大接收字节数 20
#define USART_REC_LEN2              20
#define USART_REC_LEN3              20
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF1[USART_REC_LEN1]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u8  USART_RX_BUF2[USART_REC_LEN2];
extern u8  USART_RX_BUF3[USART_REC_LEN3];
extern u16 USART_RX_STA1;         		//接收状态标记	
extern u16 USART_RX_STA2;
extern u16 USART_RX_STA3;

extern u8 USART_RX_CNT3;
extern u8 USART_RXready_FLG3;

extern u8 USART_RX_CNT2;
extern u8 USART_RXready_FLG2;

void uart1_init(u32 bound,char Remap);
void uart3_init(u32 bound,char Remap);
void uart2_init(u32 bound,char Remap);
void Uart1_send_buf(unsigned char *abyte,u8 len);
void Uart2_send_buf(unsigned char *abyte,u8 len);	
void Uart3_send_buf(unsigned char *abyte,u8 len);	

void Uart3_test(void);

#endif


