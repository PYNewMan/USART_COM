


char rsbuf_enter[10] = "FJDENTER";

char detect_buffer[9]= {0x09,0x99};	

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



void USART2_IRQHandler(void) 
{	
       USART2_IRQHandl(uuuuu_chenyaoBin)
    
     
		}
		USART_ClearFlag(USART2,USART_FLAG_RXNE);
		//USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}

