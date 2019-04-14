#include "Infrared.h"
#include "delay.h"
#include "uart.h"
u8 long_press = 0;

#define TIM_PERIOD	 40000

u8 Infrared_Data_Buffer_Count;
u8 Infrared_Result_Flag;
u8 Infrared_data[4];
u32 Infrared_data_Buffer[64];


u16 IR_PRESS_TIMER = 100;
u8 IR_PRESS_COUNT=0;

u16 IR_LONG_PRESS_TIMER = 150;



void Infrared_handle(void)
{
    //int i=0;
		//UART4_Send_Byte(Infrared_data[2]);
		switch (Infrared_data[2])
		{
			case VOL_UP:
				Uart4_SendString("FJDVUP");
				break;
			
			case VOL_DOWN:
				Uart4_SendString("FJDVDN");
				break;
			
			case NEXT:
				//if(!long_press)
					Uart4_SendString("FJDNEXT");
				//else
					//Uart4_SendString("FJDFWD");
				break;
			
			case PREV:
				//if(!long_press)
					Uart4_SendString("FJDPREV");
				//else
					//Uart4_SendString("FJDREV");
				break;
			
			case ENTER:
				Uart4_SendString("FJDENTER");
				break;
			
			case RETURN:
				Uart4_SendString("FJDRETURN");
				break;
			
			case UP:
				Uart4_SendString("FJDUP");
				break;
			
			case DOWN:
				Uart4_SendString("FJDDN");
				break;
			
			case LEFT:
				Uart4_SendString("FJDLEFT");
				break;
			
			case RIGHT:
				Uart4_SendString("FJDRIGHT");
				break;
			
			case POWER:
				Uart4_SendString("FJDPOW");
				break;
			
			case MUTE:
				Uart4_SendString("FJDMUTE");
				break;
			case NN:
				Uart4_SendString("FJDFWD");
				break;
			
			case BB:
				Uart4_SendString("FJDREV");
				break;
			
			case VIDEO:
				Uart4_SendString("FJDMOVIE");
				break;
			
			case MUSIC:
				Uart4_SendString("FJDMUSIC");
				break;
			
			case PHOTO:
				Uart4_SendString("FJDPHOTO");
				break;
			
			case TEXT:
				Uart4_SendString("FJDEBOOK");
				break;
			
			case MODE:
				Uart4_SendString("FJDRPTONE");
				break;
																						
			default:
				//UART4_Send_Byte(Infrared_data[2]);
				break;
		}
}

void Infrared_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);


  EXTI_InitStructure.EXTI_Line = EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //中断事件请求
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //下降沿
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能IO复用功能，使用中断功能重要！！！
  #ifdef  VECT_TAB_RAM  
    /* Set the Vector Table base location at 0x20000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
  #else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */ 
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
  #endif
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel= EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}



u16 TIM_Timer_H;
void EXTI4_IRQHandler(void)
{
	u32 Infrared_Data_Swap;

	
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
  {	   
/*		if(IR_PRESS_TIMER>0 )
		{
				IR_PRESS_COUNT++;
				IR_PRESS_TIMER=100;
				

				if(IR_PRESS_COUNT>20)
				{
					IR_LONG_PRESS_TIMER=150;
					long_press=1;
					IR_PRESS_COUNT=0;
				}
				
				delay_ms(5);

				EXTI_ClearITPendingBit(EXTI_Line4);
		}
		else
		{

			IR_PRESS_TIMER=0;
			long_press=0;
			IR_PRESS_COUNT=0;
		}		*/
		
		if(Infrared_Result_Flag) 
		{
			EXTI_ClearITPendingBit(EXTI_Line4);
			return;
		}
		TIM_ITConfig(TIM2, TIM_IT_Update | TIM_IT_Trigger,ENABLE);
		
		if(Infrared_Data_Buffer_Count==0)
		{
			TIM_SetCounter(TIM2,0);
			TIM_Timer_H=0;
			Infrared_Data_Buffer_Count++;
			EXTI_ClearITPendingBit(EXTI_Line4);
			return;
		}
		else if(Infrared_Data_Buffer_Count<=33)
		{
			Infrared_Data_Swap = TIM_Timer_H*TIM_PERIOD+TIM_GetCounter(TIM2);
	  	TIM_SetCounter(TIM2,0);
			TIM_Timer_H=0;
			Infrared_data_Buffer[Infrared_Data_Buffer_Count++] = Infrared_Data_Swap;
			EXTI_ClearITPendingBit(EXTI_Line4);
		}
	}
}

void TIM2_IRQHandler(void)
{	
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{
		TIM_Timer_H++;
		if(TIM_Timer_H>1)
		{
			if(Infrared_Data_Buffer_Count>=33)
			{
				Infrared_Result_Flag=1;
				Infrared_Data_Buffer_Count=0;
			}
			else Infrared_Data_Buffer_Count=0;
			TIM_ITConfig(TIM2,TIM_IT_Update | TIM_IT_Trigger,DISABLE);
		}
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update); 
	}
}


void Timer_Configuration(void)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;

 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

 	TIM_DeInit(TIM2);//重设为缺省值
 	TIM_TimeBaseStructure.TIM_Period=TIM_PERIOD;		 //ARR的值,40ms
 	TIM_TimeBaseStructure.TIM_Prescaler=0;
 	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; //采样分频
 	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
 	TIM_TimeBaseStructure.TIM_RepetitionCounter =0;//重复计价次数
 	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
 	TIM_PrescalerConfig(TIM2,72,TIM_PSCReloadMode_Immediate);//时钟分频系数72，所以定时器时钟为1000000
 	TIM_ClearFlag(TIM2,TIM_FLAG_Update);//清除中断，以免一启用就中断
 	//TIM_ARRPreloadConfig(TIM2, DISABLE);//禁止ARR预装载缓冲器
 	TIM_ITConfig(TIM2,TIM_IT_Update | TIM_IT_Trigger,DISABLE);
 	TIM_Cmd(TIM2,ENABLE);//TIM1总中断，开启
        
        
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能IO复用功能，使用中断功能重要！！！
	#ifdef  VECT_TAB_RAM  
	  /* Set the Vector Table base location at 0x20000000 */ 
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH  */
	  /* Set the Vector Table base location at 0x08000000 */ 
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif
        

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  //NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;//更新事件
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
 


u8 Infrared_data_Infrared(void)
{	
	u8 I_Js,I_Jss;
	u32 Infrared_data_Buffer_Swap=0;
	u8 Infrared_data_Swap0=0,Infrared_data_Swap1=0;
	if(Infrared_Result_Flag==0) 
		return 0;
#if 0
		for(I_Js=0;I_Js<36;I_Js++)
			Uart4_Printf("Buffer[%ld] = %ld \n\r",I_Js,Infrared_data_Buffer[I_Js]);
#endif
		
	if(Infrared_data_Buffer[1]<50000) 
	{
		Infrared_Result_Flag=0;
		return 0;
	}
    
      IR_PRESS_COUNT=0;

	for(I_Js=0;I_Js<4;I_Js++)
		for(I_Jss=0;I_Jss<8;I_Jss++)
		{
			Infrared_data_Buffer_Swap=Infrared_data_Buffer[I_Js*8+I_Jss+2];
			if(Infrared_data_Buffer_Swap>2000&&Infrared_data_Buffer_Swap<2500)//位1 2.25ms
				Infrared_data[I_Js] |= 1<<I_Jss;
			else 
			if(Infrared_data_Buffer_Swap>1000&&Infrared_data_Buffer_Swap<1500)//位0 1.125ms
			Infrared_data[I_Js] &= ~(1<<I_Jss);
			else 
			{
				Infrared_Result_Flag=0;
				return 0;
			}
		}
		
#if 0		
		for(I_Js=0;I_Js<4;I_Js++)
			Uart4_Printf("Infrared[%ld] = %ld \n\r",I_Js,Infrared_data[I_Js]);
#endif		
		Infrared_data_Swap0=~Infrared_data[1];
		Infrared_data_Swap1=~Infrared_data[3];
		if((Infrared_data[0]==144)&&(Infrared_data[0]==Infrared_data_Swap0)&&(Infrared_data[2]==Infrared_data_Swap1))
		{
			Infrared_Result_Flag=0;
            IR_PRESS_TIMER=100;
          
			return 1;
        }
        else if(Infrared_data[0]==0x00 && Infrared_data[1]==0xff)   //new ir
        {
            Infrared_Result_Flag=0;
            IR_PRESS_TIMER=100;
			return 2;
        }
          
		Infrared_Result_Flag=0;
		return 0;

}




