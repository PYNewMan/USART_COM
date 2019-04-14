#include "I2C.h"
//#include "sysdef.h"

void I2C_Configuration(void)  //TEA5756_GPIO   PB11(SDA),PB10(SCL)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  /* Configure I2C2 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

}

bool I2C_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
  	if(!I2C_Start())
   {
//		GPIO_ResetBits(GPIOF, GPIO_Pin_7);
		return FALSE;
  	}
    I2C_SendByte(WriteAddr);//������ַ 
    if(!I2C_WaitAck())
	{
		I2C_Stop();
		return FALSE;
	}
		while(NumByteToWrite--)
		{
		  I2C_SendByte(* pBuffer);
		  if(!I2C_WaitAck())
	{
		I2C_Stop();
		return FALSE;
	}
          pBuffer++;
		}
	  I2C_Stop(); 	
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
void I2C_delay(void)
{
  #if TEA5756
   u8 i=1;
  #else
   u8 i=20;
  #endif
   while(i) 
   { 
     i--; 
   } 
       
}

bool I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)return FALSE;	//SDA��Ϊ�͵�ƽ������æ,�˳�
	SDA_L;
	I2C_delay();
	if(SDA_read) return FALSE;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	SDA_L;
	I2C_delay();
	return TRUE;
}

void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
}

void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

bool I2C_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	
	 SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;
      return FALSE;
	}
	SCL_L;
	return TRUE;
}

void I2C_SendByte(u8 SendByte) //���ݴӸ�λ����λ//
{
    u8 i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
      if(SendByte&0x80)
        SDA_H;  
      else 
        SDA_L;   
        SendByte<<=1;
        I2C_delay();
		SCL_H;
        I2C_delay();
    }
    SCL_L;
}

u8 I2C_ReceiveByte(void)  //���ݴӸ�λ����λ//
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      I2C_delay();
	  SCL_H;
      I2C_delay();	
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
}

//д��1�ֽ�����       ��д������    ��д���ַ       ��������(24c16��SD2403)
bool I2C_WriteByte(u8 SendByte, u16 WriteAddress, u8 DeviceAddress)
{		
    if(!I2C_Start())return FALSE;
    I2C_SendByte(WriteAddress);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
    I2C_SendByte((u8)(WriteAddress & 0x00FF));   //���õ���ʼ��ַ      
    I2C_WaitAck();	
    I2C_SendByte(SendByte);
    I2C_WaitAck();   
    I2C_Stop(); 
	//ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms)
    //Systick_Delay_1ms(10);
    return TRUE;
}

bool I2C_BufferWrite(u8* pBuffer, u8 length,     u16 WriteAddress, u8 DeviceAddress)
{
    if(!I2C_Start())return FALSE;
    I2C_SendByte(((WriteAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
    I2C_SendByte((u8)(WriteAddress & 0x00FF));   //���õ���ʼ��ַ      
	  I2C_WaitAck();	
	  
		while(length--)
		{
		  I2C_SendByte(* pBuffer);
		  I2C_WaitAck();
          pBuffer++;
		}
	  I2C_Stop();
	  //ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms)
	  //Systick_Delay_1ms(10);
	  return TRUE;
}

//����1������         ��Ŷ�������  ����������   ��������(24c16��SD2403)	
bool I2C_ReadByte(u8* pBuffer,   u8 length,   u8 DeviceAddress)
{		
    u8 n;
    if(!I2C_Start())return FALSE;
    I2C_SendByte(DeviceAddress);//������ַ 
    if(!I2C_WaitAck())
    {
	I2C_Stop(); 
	return FALSE;
    }
	    
    for(n=0;n<length;n++)
    {
	pBuffer[n]=I2C_ReceiveByte();
	if (n!=length-1)        	 //���һ�����ݲ�Ӧ��
	{
	    I2C_Ack();
	}
    }
    I2C_NoAck();
    I2C_Stop(); 	
    return TRUE;
}





/*
*********************************************************************************************************
*	�� �� ��: i2c_Start
*	����˵��: CPU����I2C���������ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Start(void)
{
  
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
	SDA_H;
	SCL_H;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_L;
	I2C_delay();
}


/*
*********************************************************************************************************
*	�� �� ��: i2c_WaitAck
*	����˵��: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*	��    �Σ���
*	�� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	SDA_H;	/* CPU�ͷ�SDA���� */
	I2C_delay();
	SCL_H;	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	I2C_delay();
	if (SDA_read)	/* CPU��ȡSDA����״̬ */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	SCL_L;
	I2C_delay();
	return re;
}



/*
*********************************************************************************************************
*	�� �� ��: i2c_Stop
*	����˵��: CPU����I2C����ֹͣ�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Stop(void)
{
   
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
	SDA_L;
	SCL_H;
	I2C_delay();
	SDA_H;
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_SendByte
*	����˵��: CPU��I2C�����豸����8bit����
*	��    �Σ�_ucByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;
 
	/* �ȷ����ֽڵĸ�λbit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			SDA_H;
		}
		else
		{
			SDA_L;
		}
		I2C_delay();
		SCL_H;
		I2C_delay();	
		SCL_L;
		if (i == 7)
		{
			 SDA_H; // �ͷ�����
		}
		_ucByte <<= 1;	/* ����һ��bit */
		I2C_delay();
	}
}



/*
*********************************************************************************************************
*	�� �� ��: i2c_ReadByte
*	����˵��: CPU��I2C�����豸��ȡ8bit����
*	��    �Σ���
*	�� �� ֵ: ����������
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;
	/* ������1��bitΪ���ݵ�bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		SCL_H;
		I2C_delay();
		if (SDA_read)
		{
			value++;
		}
		SCL_L;
		I2C_delay();
	}
	return value;
}






/*
*********************************************************************************************************
*	�� �� ��: i2c_CheckDevice
*	����˵��: ���I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
*	��    �Σ�_Address���豸��I2C���ߵ�ַ  I2C_WR :0://д����bit 1: //������bit
*	�� �� ֵ: ����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽
*********************************************************************************************************
*/

uint8_t i2c_CheckDevice(uint8_t _Address,uint8_t I2C_WR)
{
	uint8_t ucAck;

	i2c_Start();		/* ���������ź� */

	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	i2c_SendByte(_Address | I2C_WR);
	ucAck = i2c_WaitAck();	/* ����豸��ACKӦ�� */

	i2c_Stop();			/* ����ֹͣ�ź� */

	return ucAck;
}




/*
*********************************************************************************************************
*	�� �� ��: i2c_Ack
*	����˵��: CPU����һ��ACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Ack(void)
{
   
	SDA_L;	/* CPU����SDA = 0 */
	I2C_delay();
	SCL_H;	/* CPU����1��ʱ�� */
	I2C_delay();
	SCL_L;
	I2C_delay();
	SDA_H;	/* CPU�ͷ�SDA���� */
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_NAck
*	����˵��: CPU����1��NACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_NAck(void)
{
   
	SDA_H;	/* CPU����SDA = 1 */
	I2C_delay();
	SCL_H;	/* CPU����1��ʱ�� */
	I2C_delay();
	SCL_L;
	I2C_delay();	
}









