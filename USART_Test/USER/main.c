
char detect_buffer[9]= {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};	

int g_detect_flag = 0;
int UART_RX_NUM = 0;//����λ��

void Infrared_LOOP(void);

void sleep(int num)//
{
	int i = 0;
	
	for(i=0; i<num; i++)
	{
		delay_ms(1000);
	}

}

void Infrared_LOOP(void)
{

  int result=Infrared_data_Infrared();//�������ݽ���
  if(result == 1)
  {
//    Infrared_handle();

  }
  else if(result == 2)
  {
     Infrared_handle(); 
		//Uart4_SendString(rsbuf_enter);		
  }
}


 int main(void)
 {	 		
	delay_init();	    	 				//��ʱ������ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2

	LED_Init();									//���ָʾ�Ƴ�ʼ��
	 
	uart2_init(9600);						//��������
	uart4_init(9600);
	 
	I2C_Configuration(); 				//��ʼ��I2C
	ADAU1701_Init(); 		 				//1701��ʼ��
	 
	TIM3_Int_Init(9999,7199);		//10Khz�ļ���Ƶ�ʣ�������10000Ϊ1000ms  ����ÿ�뷢��������״ָ̬�� 
	 
	Infrared_Configuration(); 	//��������
	Timer_Configuration();			//���ⶨʱ
	//sleep(5);
	Uart4_SendString(rsbuf_return);//enter menu
	 
	while(1)
	{	
		delay_ms(10);
		//delay_ms(1000);delay_ms(1000);
		Infrared_LOOP();
	}
}
 
void TIM3_IRQHandler(void)   	//TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ 
			//ÿ1�뷢�ͻ�ȡ������״ָ̬�� 01 03 00 20 00 01 85 C0
			Uart2_SendString(detect_buffer, 8);
		}
}


void USART2_IRQHandler(void) 
{	
	unsigned char ch;

	//RS485_TX_EN=1;//�򿪽���
		
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
	{		
			ch = USART_ReceiveData(USART2);
      UART_RX_NUM++;
			
      if(UART_RX_NUM == 1)
      {
        if(ch != 0x01)
        {	
          UART_RX_NUM = 0;
        }
      }
      
      if(UART_RX_NUM == 2)
      {	
        if(ch != 0x03)
        {	
          UART_RX_NUM = 0;
        }
      }
    
      if(UART_RX_NUM == 7)//���ݵ���λ�ж�
      {     
          //UART1_sendchar(ch);
          UART_RX_NUM = 0;
          if(g_detect_flag != ch)
          {
            if(ch == 0x44)//0x44��û��⵽��
            {	
							Uart4_SendString(rsbuf_return);//���ͷ���
							//Uart2_SendString(detect_buffer, 8);//test RS485 receive
              g_detect_flag = ch;
							LED2 = 0;
							
            }
            if(ch == 0xF4)//0xF4���⵽��
            {	
							Uart4_SendString(rsbuf_enter);//����ȷ��
							//Uart2_SendString(detect_buffer, 8);
              g_detect_flag = ch;
							LED2 = 1;
            }    
          }  
      }
		}
		USART_ClearFlag(USART2,USART_FLAG_RXNE);
		//USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}

