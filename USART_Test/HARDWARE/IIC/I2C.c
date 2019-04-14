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
    I2C_SendByte(WriteAddr);//器件地址 
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
	if(!SDA_read)return FALSE;	//SDA线为低电平则总线忙,退出
	SDA_L;
	I2C_delay();
	if(SDA_read) return FALSE;	//SDA线为高电平则总线出错,退出
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

bool I2C_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
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

void I2C_SendByte(u8 SendByte) //数据从高位到低位//
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

u8 I2C_ReceiveByte(void)  //数据从高位到低位//
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

//写入1字节数据       待写入数据    待写入地址       器件类型(24c16或SD2403)
bool I2C_WriteByte(u8 SendByte, u16 WriteAddress, u8 DeviceAddress)
{		
    if(!I2C_Start())return FALSE;
    I2C_SendByte(WriteAddress);//设置高起始地址+器件地址 
    if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
    I2C_SendByte((u8)(WriteAddress & 0x00FF));   //设置低起始地址      
    I2C_WaitAck();	
    I2C_SendByte(SendByte);
    I2C_WaitAck();   
    I2C_Stop(); 
	//注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms)
    //Systick_Delay_1ms(10);
    return TRUE;
}

bool I2C_BufferWrite(u8* pBuffer, u8 length,     u16 WriteAddress, u8 DeviceAddress)
{
    if(!I2C_Start())return FALSE;
    I2C_SendByte(((WriteAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE);//设置高起始地址+器件地址 
    if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
    I2C_SendByte((u8)(WriteAddress & 0x00FF));   //设置低起始地址      
	  I2C_WaitAck();	
	  
		while(length--)
		{
		  I2C_SendByte(* pBuffer);
		  I2C_WaitAck();
          pBuffer++;
		}
	  I2C_Stop();
	  //注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms)
	  //Systick_Delay_1ms(10);
	  return TRUE;
}

//读出1串数据         存放读出数据  待读出长度   器件类型(24c16或SD2403)	
bool I2C_ReadByte(u8* pBuffer,   u8 length,   u8 DeviceAddress)
{		
    u8 n;
    if(!I2C_Start())return FALSE;
    I2C_SendByte(DeviceAddress);//器件地址 
    if(!I2C_WaitAck())
    {
	I2C_Stop(); 
	return FALSE;
    }
	    
    for(n=0;n<length;n++)
    {
	pBuffer[n]=I2C_ReceiveByte();
	if (n!=length-1)        	 //最后一个数据不应答
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
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Start(void)
{
  
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
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
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参：无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	SDA_H;	/* CPU释放SDA总线 */
	I2C_delay();
	SCL_H;	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	I2C_delay();
	if (SDA_read)	/* CPU读取SDA口线状态 */
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
*	函 数 名: i2c_Stop
*	功能说明: CPU发起I2C总线停止信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Stop(void)
{
   
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	SDA_L;
	SCL_H;
	I2C_delay();
	SDA_H;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参：_ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;
 
	/* 先发送字节的高位bit7 */
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
			 SDA_H; // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		I2C_delay();
	}
}



/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参：无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;
	/* 读到第1个bit为数据的bit7 */
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
*	函 数 名: i2c_CheckDevice
*	功能说明: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*	形    参：_Address：设备的I2C总线地址  I2C_WR :0://写控制bit 1: //读控制bit
*	返 回 值: 返回值 0 表示正确， 返回1表示未探测到
*********************************************************************************************************
*/

uint8_t i2c_CheckDevice(uint8_t _Address,uint8_t I2C_WR)
{
	uint8_t ucAck;

	i2c_Start();		/* 发送启动信号 */

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(_Address | I2C_WR);
	ucAck = i2c_WaitAck();	/* 检测设备的ACK应答 */

	i2c_Stop();			/* 发送停止信号 */

	return ucAck;
}




/*
*********************************************************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Ack(void)
{
   
	SDA_L;	/* CPU驱动SDA = 0 */
	I2C_delay();
	SCL_H;	/* CPU产生1个时钟 */
	I2C_delay();
	SCL_L;
	I2C_delay();
	SDA_H;	/* CPU释放SDA总线 */
}

/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_NAck(void)
{
   
	SDA_H;	/* CPU驱动SDA = 1 */
	I2C_delay();
	SCL_H;	/* CPU产生1个时钟 */
	I2C_delay();
	SCL_L;
	I2C_delay();	
}









