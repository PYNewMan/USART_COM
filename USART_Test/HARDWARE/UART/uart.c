#include "sys.h"
#include "uart.h"
#include "usart.h"	 
#include "delay.h"

void uart2_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART2，GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	//USART2_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
   
  //USART2_RX	  GPIOA.3初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3

	//PA1推挽输出，485模式控制  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //GPIOA1
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP ;//输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度100MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA1
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	
  USART_Cmd(USART2, ENABLE);                    //使能串口2	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
	
	USART_Init(USART2, &USART_InitStructure); //初始化串口2
	#if 1
  //Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
#endif
	RS485_TX_EN=0;
}

void uart4_init(u32 bound){
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//NVIC_InitTypeDef NVIC_InitStructure;
	 	
	//USART4
	USART_InitStructure.USART_BaudRate = bound;                 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
	USART_InitStructure.USART_StopBits = USART_StopBits_1;      
	USART_InitStructure.USART_Parity = USART_Parity_No;         
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;     

			
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4 , ENABLE);

	/* Configure USART4 Tx (PC.10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/* Configure USART4 Rx (PC.11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
	USART_Init(UART4, &USART_InitStructure);
	USART_Cmd(UART4, ENABLE);
	
  //Usart2 NVIC 配置
  //NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	//NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}

//*****************************************************************************

//=====================================================================
void USART2_Send_Byte(u8 Data)
{ 	
   	while (!(USART2->SR & USART_FLAG_TXE));
				USART2->DR = (Data & (uint16_t)0x01FF);
}


//====================================================================
void Uart2_SendString(char *pt, int len)
{	
	int i = 0;
	//delay_ms(10);
	RS485_TX_EN=1;//打开发送
	for(i=0; i<len; i++)
	{
		USART2_Send_Byte(pt[i]);
	}
	delay_ms(10);
	RS485_TX_EN=0;//打开接收
}

void UART4_Send_Byte(u16 Data)
{ 
   	while (!(UART4->SR & USART_FLAG_TXE));
				UART4->DR = (Data & (uint16_t)0x01FF);	   
}

void Uart4_SendString(char *pt)
{
    while(*pt)
        UART4_Send_Byte(*pt++);	
}














 
 
 
 

