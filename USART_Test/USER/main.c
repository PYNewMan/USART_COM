
char detect_buffer[9]= {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};	

int g_detect_flag = 0;
int UART_RX_NUM = 0;//接收位数

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

  int result=Infrared_data_Infrared();//红外数据解析
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
	delay_init();	    	 				//延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2

	LED_Init();									//检测指示灯初始化
	 
	uart2_init(9600);						//串口配置
	uart4_init(9600);
	 
	I2C_Configuration(); 				//初始化I2C
	ADAU1701_Init(); 		 				//1701初始化
	 
	TIM3_Int_Init(9999,7199);		//10Khz的计数频率，计数到10000为1000ms  用于每秒发送人体检测状态指令 
	 
	Infrared_Configuration(); 	//红外配置
	Timer_Configuration();			//红外定时
	//sleep(5);
	Uart4_SendString(rsbuf_return);//enter menu
	 
	while(1)
	{	
		delay_ms(10);
		//delay_ms(1000);delay_ms(1000);
		Infrared_LOOP();
	}
}
 
void TIM3_IRQHandler(void)   	//TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
			//每1秒发送获取人体检测状态指令 01 03 00 20 00 01 85 C0
			Uart2_SendString(detect_buffer, 8);
		}
}


void USART2_IRQHandler(void) 
{	
	unsigned char ch;

	//RS485_TX_EN=1;//打开接收
		
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
    
      if(UART_RX_NUM == 7)//根据第七位判断
      {     
          //UART1_sendchar(ch);
          UART_RX_NUM = 0;
          if(g_detect_flag != ch)
          {
            if(ch == 0x44)//0x44表没检测到人
            {	
							Uart4_SendString(rsbuf_return);//发送返回
							//Uart2_SendString(detect_buffer, 8);//test RS485 receive
              g_detect_flag = ch;
							LED2 = 0;
							
            }
            if(ch == 0xF4)//0xF4表检测到人
            {	
							Uart4_SendString(rsbuf_enter);//发送确定
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

