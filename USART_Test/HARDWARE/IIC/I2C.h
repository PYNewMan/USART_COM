#ifndef __I2C_H
#define __I2C_H			    
#include "stm32f10x.h"
#include "stdbool.h"

#define FALSE 0
#define TRUE  1 

#define SCL_H         GPIO_SetBits(GPIOB, GPIO_Pin_10)
#define SCL_L         GPIO_ResetBits(GPIOB, GPIO_Pin_10)
   
#define SDA_H         GPIO_SetBits(GPIOB, GPIO_Pin_11)
#define SDA_L         GPIO_ResetBits(GPIOB, GPIO_Pin_11)

      
#define SCL_read      GPIOB->IDR  & GPIO_Pin_10
#define SDA_read      GPIOB->IDR  & GPIO_Pin_11


void I2C_Configuration(void);

bool I2C_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite);

void I2C_delay(void);

bool I2C_Start(void);

void I2C_Stop(void);

void I2C_Ack(void);

void I2C_NoAck(void);

bool I2C_WaitAck(void);

void I2C_SendByte(u8 SendByte);

u8 I2C_ReceiveByte(void);

bool I2C_WriteByte(u8 SendByte, u16 WriteAddress, u8 DeviceAddress);

bool I2C_BufferWrite(u8* pBuffer, u8 length, u16 WriteAddress, u8 DeviceAddress);

bool I2C_ReadByte(u8* pBuffer,   u8 length,   u8 DeviceAddress);







void i2c_Start(void);
uint8_t i2c_WaitAck(void);
void i2c_Stop(void);
uint8_t i2c_WaitAck(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_CheckDevice(uint8_t _Address,uint8_t I2C_WR);

//extern void Delay(__IO uint32_t nCount);

#endif

