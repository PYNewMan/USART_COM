#ifndef __I2C2_H
#define __I2C2_H			    
#include "stm32f10x.h"


#define SCL2_H         GPIO_SetBits(GPIOB, GPIO_Pin_6)
#define SCL2_L         GPIO_ResetBits(GPIOB, GPIO_Pin_6)
   
#define SDA2_H         GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define SDA2_L         GPIO_ResetBits(GPIOB, GPIO_Pin_7)

      
#define SCL2_read      GPIOB->IDR  & GPIO_Pin_6
#define SDA2_read      GPIOB->IDR  & GPIO_Pin_7


void I2C2_Configuration(void);

//bool I2C2_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite);

bool I2C2_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite);

void I2C2_delay(void);

bool I2C2_Start(void);

void I2C2_Stop(void);

void I2C2_Ack(void);

void I2C2_NoAck(void);

bool I2C2_WaitAck(void);

void I2C2_SendByte(u8 SendByte);

u8 I2C2_ReceiveByte(void);

bool I2C2_WriteByte(u8 SendByte, u16 WriteAddress, u8 DeviceAddress);

bool I2C2_BufferWrite(u8* pBuffer, u8 length, u16 WriteAddress, u8 DeviceAddress);

bool I2C2_ReadByte(u8* pBuffer,   u8 length,   u8 DeviceAddress);





//extern void Delay(__IO uint32_t nCount);

#endif

