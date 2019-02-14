#include "sys.h"
#include "usart.h"	  
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
/*
_sys_exit(int x) 
{ 
	x = x; 
} 
*/
/*
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
    USART3->DR = (u8) ch;      
	return ch;
}
*/
#endif 

/*使用microLib的方法*/

int fputc(int ch, FILE *f)
{
	USART_SendData(USART3, (uint8_t) ch);

	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART3->SR & USART_FLAG_RXNE));

    return ((int)(USART3->DR & 0x1FF));
}

 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF1[USART_REC_LEN1];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART_RX_BUF2[USART_REC_LEN2]; 
u8 USART_RX_BUF3[USART_REC_LEN3]; 
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA1=0;       //接收状态标记	  
u16 USART_RX_STA2=0;
u16 USART_RX_STA3=0;

u8 USART_RX_CNT3 = 0;
u8 USART_RXready_FLG3 = 0;

u8 USART_RX_CNT2 = 0;
u8 USART_RXready_FLG2 = 0;
  
//串口1初始化，参数：波特率，是否需要重定义。第二个参数为1需要重定义
void uart1_init(u32 bound,char Remap){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 if(Remap)//如果需要重定义
	 {
		 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);	//使能USART1，GPIOB时钟
			GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);//使能端口重定义
			//USART1_TX   GPIOB6
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
			GPIO_Init(GPIOB, &GPIO_InitStructure);
				 
			//USART1_RX	  初始化  B7
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
			GPIO_Init(GPIOB, &GPIO_InitStructure);
	 }
	 else//不需要重定义
	 {
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
			
			//USART1_TX   GPIOA.9
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
			GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
			 
			//USART1_RX	  GPIOA.10初始化
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
			GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  
	 }
  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}


  
//串口2初始化，参数：波特率，是否需要重定义。第二个参数为1需要重定义
void uart2_init(u32 bound,char Remap){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	Remap = 0;//USART2 is no Remap for RCT6
	 if(Remap)//如果需要重定义
	 {
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

			GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);//使能端口重定义
			//USART2_TX   GPIOD5
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
			GPIO_Init(GPIOD, &GPIO_InitStructure);
				 
			//USART2_RX	  初始化  D6
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
			GPIO_Init(GPIOD, &GPIO_InitStructure);
	 }
	 else//不需要重定义
	 {
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
			//USART2_TX   GPIOA.2
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
			GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
			 
			//USART2_RX	  GPIOA.3
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
			GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA3 
	 }
  //Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//子优先级4
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART2, &USART_InitStructure); //初始化串口2
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART2, ENABLE);                    //使能串口2

}


  
//串口3初始化，参数：波特率，是否需要重定义。第二个参数为1需要重定义
void uart3_init(u32 bound,char Remap){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 if(Remap)//如果需要重定义
	 {
	 	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC| RCC_APB2Periph_AFIO,ENABLE);
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
		
			GPIO_PinRemapConfig(GPIO_PartialRemap_USART3 ,ENABLE);//使能端口重定义
			//USART3_TX   GPIOC10
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
			GPIO_Init(GPIOC, &GPIO_InitStructure);
				 
			//USART3_RX	  初始化  C11
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
			GPIO_Init(GPIOC, &GPIO_InitStructure);
	 }
	 else//不需要重定义
	 {

			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
			//USART3_TX   GPIOB10
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
			GPIO_Init(GPIOB, &GPIO_InitStructure);
			 
			//USART3_RX	  GPIOB11
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
			GPIO_Init(GPIOB, &GPIO_InitStructure);
	 }

  //Usart3 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART3, &USART_InitStructure); //初始化串口3
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART3, ENABLE);                    //使能串口3

}



void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		if((USART_RX_STA1&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA1&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA1=0;//接收错误,重新开始
				else USART_RX_STA1|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA1|=0x4000;
				else
					{
					USART_RX_BUF3[USART_RX_STA1&0X3FFF]=Res ;
					USART_RX_STA1++;
					if(USART_RX_STA1>(USART_REC_LEN1-1))USART_RX_STA1=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 




void USART2_IRQHandler(void)                	//串口2中断服务程序
	{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
		{
		Res =USART_ReceiveData(USART2);	//读取接收到的数据
			if(USART_RX_CNT2 < USART_REC_LEN2)
			{
				USART_RX_BUF2[USART_RX_CNT2]=Res ;
				USART_RX_CNT2++;
				
				TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除定时器溢出中断
				TIM_SetCounter(TIM3,0);//当接收到一个新的字节，将定时器3复位为0，重新计时（相当于喂狗）
				TIM_Cmd(TIM3,ENABLE);//开始计时			
			}
			else
			{
				USART_RX_CNT2 = 0;
			}
     } 
} 

/*
void USART2_IRQHandler(void)                	//串口2中断服务程序
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART2);	//读取接收到的数据
		
		if((USART_RX_STA2&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA2&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA2=0;//接收错误,重新开始
				else USART_RX_STA2|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA2|=0x4000;
				else
					{
					USART_RX_BUF2[USART_RX_STA2&0X3FFF]=Res ;
					USART_RX_STA2++;
					if(USART_RX_STA2>(USART_REC_LEN2-1))USART_RX_STA2=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 

*/


void USART3_IRQHandler(void)                	//串口3中断服务程序
	{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
		{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
			if(USART_RX_CNT3 < USART_REC_LEN3)
			{
				USART_RX_BUF3[USART_RX_CNT3]=Res ;
				USART_RX_CNT3++;
				
				TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除定时器溢出中断
				TIM_SetCounter(TIM3,0);//当接收到一个新的字节，将定时器3复位为0，重新计时（相当于喂狗）
				TIM_Cmd(TIM3,ENABLE);//开始计时			
			}
			else
			{
				USART_RX_CNT3 = 0;
			}
     } 
} 



/*
void USART3_IRQHandler(void)                	//串口3中断服务程序
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
		
		if((USART_RX_STA3&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA3&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA3=0;//接收错误,重新开始
				else USART_RX_STA3|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA3|=0x4000;
				else
					{
					USART_RX_BUF3[USART_RX_STA3&0X3FFF]=Res ;
					USART_RX_STA3++;
					if(USART_RX_STA3>(USART_REC_LEN3-1))USART_RX_STA3=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
*/
	

void Uart1_send_buf(unsigned char *abyte,u8 len)
{
	unsigned char i;
	for(i=0;i<len;i++)
	{
		USART_SendData(USART1,*abyte);						// output */
		abyte++;
		delay_us(200);
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TC))
		{
		}
	}
}
void Uart2_send_buf(unsigned char *abyte,u8 len)
{
	unsigned char i;
	for(i=0;i<len;i++)
	{
		USART_SendData(USART2,*abyte);						// output */
		abyte++;
		delay_us(200);
		while(!USART_GetFlagStatus(USART2, USART_FLAG_TC))
		{
		}
	}
}

void Uart3_send_buf(unsigned char *abyte,u8 len)
{
	unsigned char i;
	for(i=0;i<len;i++)
	{
		USART_SendData(USART3,*abyte);						// output */
		abyte++;
		delay_us(200);
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TC))
		{
		}
	}
}

//收到什么数据，发送什么数据。收到的数据要背正确截断，例如0x0d  0x0a
void Uart1_test(void)
{
	u16 len;
		if(USART_RX_STA1&0x8000)
		{
			len=USART_RX_STA1&0x3fff;//得到此次接收到的数据长度
			Uart1_send_buf(USART_RX_BUF1,len);
			USART_RX_STA1=0;
		}
}

//收到什么数据，发送什么数据。收到的数据要背正确截断，例如0x0d  0x0a
void Uart2_test(void)
{
	u16 len;
		if(USART_RX_STA2&0x8000)
		{
			len=USART_RX_STA2&0x3fff;//得到此次接收到的数据长度
			Uart2_send_buf(USART_RX_BUF2,len);
			USART_RX_STA2=0;
		}
}

//收到什么数据，发送什么数据。收到的数据要背正确截断，例如0x0d  0x0a
void Uart3_test(void)
{
	//u16 len;
		if(USART_RXready_FLG3)
		{
			//len=USART_RX_STA3&0x3fff;//得到此次接收到的数据长度
			Uart3_send_buf(USART_RX_BUF3,USART_RX_CNT3);
			USART_RXready_FLG3=0;
			USART_RX_CNT3 = 0;
		}
}


#endif	

