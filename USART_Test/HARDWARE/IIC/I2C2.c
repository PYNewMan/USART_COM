#include "I2C2.h"


void I2C2_Configuration(void)  //TEA5756_GPIO   PB7(SDA),PB6(SCL)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  /* Configure I2C1 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

}


bool I2C2_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
  	if(!I2C2_Start())
   {
//		GPIO_ResetBits(GPIOF, GPIO_Pin_7);
		return FALSE;
  	}
    I2C2_SendByte(WriteAddr);//������ַ 
    if(!I2C2_WaitAck())
	{
		I2C2_Stop();
		return FALSE;
	}
		while(NumByteToWrite--)
		{
		  I2C2_SendByte(* pBuffer);
		  if(!I2C2_WaitAck())
	{
		I2C2_Stop();
		return FALSE;
	}
          pBuffer++;
		}
	  I2C2_Stop(); 	
	  return TRUE;
}

/**
  * @brief  Reads a block of data from the EEPROM.
  * @param pBuffer : pointer to the buffer that receives the data read 
  *   from the EEPROM.
  * @param ReadAddr : EEPROM's internal address to read from.
  * @param NumByteToRead : number of bytes to read from the EEPROM.
  * @retval : None
  */

void I2C2_delay(void)
{	
   u8 i=100; //��������Ż��ٶ�	����������͵�5����д��
   while(i) 
   { 
     i--; 
   } 
}

bool I2C2_Start(void)
{
	SDA2_H;
	SCL2_H;
	I2C2_delay();
	if(!SDA2_read)return FALSE;	//SDA��Ϊ�͵�ƽ������æ,�˳�
	SDA2_L;
	I2C2_delay();
	if(SDA2_read) return FALSE;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	SDA2_L;
	I2C2_delay();
	return TRUE;
}

void I2C2_Stop(void)
{
	SCL2_L;
	I2C2_delay();
	SDA2_L;
	I2C2_delay();
	SCL2_H;
	I2C2_delay();
	SDA2_H;
	I2C2_delay();
}

 void I2C2_Ack(void)
{	
	SCL2_L;
	I2C2_delay();
	SDA2_L;
	I2C2_delay();
	SCL2_H;
	I2C2_delay();
	SCL2_L;
	I2C2_delay();
}

void I2C2_NoAck(void)
{	
	SCL2_L;
	I2C2_delay();
	SDA2_H;
	I2C2_delay();
	SCL2_H;
	I2C2_delay();
	SCL2_L;
	I2C2_delay();
}

bool I2C2_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
{
	SCL2_L;
	I2C2_delay();
	SDA2_H;			
	I2C2_delay();
	
	 SCL2_H;
	I2C2_delay();
	if(SDA2_read)
	{
      SCL2_L;
      return FALSE;
	}
	SCL2_L;
	return TRUE;
}

void I2C2_SendByte(u8 SendByte) //���ݴӸ�λ����λ//
{
    u8 i=8;
    while(i--)
    {
        SCL2_L;
        I2C2_delay();
      if(SendByte&0x80)
        SDA2_H;  
      else 
        SDA2_L;   
        SendByte<<=1;
        I2C2_delay();
	SCL2_H;
        I2C2_delay();
    }
    SCL2_L;
}

u8 I2C2_ReceiveByte(void)  //���ݴӸ�λ����λ//
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    SDA2_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL2_L;
      I2C2_delay();
	  SCL2_H;
      I2C2_delay();	
      if(SDA2_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL2_L;
    return ReceiveByte;
}

//д��1�ֽ�����       ��д������    ��д���ַ       ��������(24c16��SD2403)
bool I2C2_WriteByte(u8 SendByte, u16 WriteAddress, u8 DeviceAddress)
{		
    if(!I2C2_Start())return FALSE;
    I2C2_SendByte(WriteAddress);//���ø���ʼ��ַ+������ַ 
    if(!I2C2_WaitAck()){I2C2_Stop(); return FALSE;}
    I2C2_SendByte((u8)(WriteAddress & 0x00FF));   //���õ���ʼ��ַ      
    I2C2_WaitAck();	
    I2C2_SendByte(SendByte);
    I2C2_WaitAck();   
    I2C2_Stop(); 
	//ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms)
    //Systick_Delay_1ms(10);
    return TRUE;
}

bool I2C2_BufferWrite(u8* pBuffer, u8 length,     u16 WriteAddress, u8 DeviceAddress)
{
    if(!I2C2_Start())return FALSE;
    I2C2_SendByte(((WriteAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C2_WaitAck()){I2C2_Stop(); return FALSE;}
    I2C2_SendByte((u8)(WriteAddress & 0x00FF));   //���õ���ʼ��ַ      
	  I2C2_WaitAck();	
	  
		while(length--)
		{
		  I2C2_SendByte(* pBuffer);
		  I2C2_WaitAck();
          pBuffer++;
		}
	  I2C2_Stop();
	  //ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms)
	  //Systick_Delay_1ms(10);
	  return TRUE;
}

//����1������         ��Ŷ�������  ����������   ��������(24c16��SD2403)	
bool I2C2_ReadByte(u8* pBuffer,   u8 length,   u8 DeviceAddress)
{		
    if(!I2C2_Start())return FALSE;
    I2C2_SendByte(DeviceAddress);//������ַ 
    if(!I2C2_WaitAck())
	{
		I2C2_Stop(); 
		return FALSE;
	}
	    
		while(length--)
		{
		  *pBuffer = I2C2_ReceiveByte();
     	  if(length == 1)
		  	I2C2_NoAck();
     	  else 
		  	I2C2_Ack(); 
          pBuffer++;
        
		}
	  I2C2_Stop(); 	
	  return TRUE;
}
