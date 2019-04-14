#include "sys.h"
#include "uart.h"
#include "usart.h"	 
#include "delay.h"

void uart2_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART2��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	//USART2_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
  //USART2_RX	  GPIOA.3��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3

	//PA1���������485ģʽ����  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //GPIOA1
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP ;//���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�100MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA1
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	#if 1
  //Usart2 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
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
	
  //Usart2 NVIC ����
  //NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	//NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
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
	RS485_TX_EN=1;//�򿪷���
	for(i=0; i<len; i++)
	{
		USART2_Send_Byte(pt[i]);
	}
	delay_ms(10);
	RS485_TX_EN=0;//�򿪽���
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














 
 
 
 

