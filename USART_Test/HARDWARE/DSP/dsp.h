#ifndef _DSP_H_
#define _DSP_H_


#include "I2C.h"

#define INT8U u8
#define INT16U u16
#define INT32 u32

#define DSP_ADDR0  0x68
#define DSP_ADDR1  0x6a
#define DSP_ADDR2  0x6c
#define DSP_ADDR3  0x6e

#define FALSE 0

#define IIC_INT_EN  1  //1: 中断方式，0: 模拟IIC

#if (IIC_INT_EN)   //1: 中断方式，0: 模拟IIC

//IIC初始化
extern void Dev1161Init(void);

extern const  float  m_cVolTable[];
extern const unsigned char PreEq[5][10];
extern const float  m_cVolTable[256];

#else			   //1: 中断方式，0: 模拟IIC

#define I2C_NOP 1
#define I2C_ACK 0

sbit I2C_SCL = P0^3;
sbit I2C_SDA = P0^2;

extern void I2C_delay(void);                // 起始条件
extern void I2C_Start(void);                // 停止条件
extern void I2C_Stop(void);                 // 主机接收 应答位，如果应答错误，向串口发送0xAE
extern bool I2C_WaitAck(void);              // 主机发送应答位

//extern void I2C_SendAck(bit bAck);          // 写一个字节，先高位，后低位
//extern void I2C_Write(unsigned char dat);

// 读取一个字节，先高位，后低位
//extern unsigned char I2C_Read(void);

#endif
extern unsigned char m_clevel[22];
//写数据到ADAU1701
u8 I2cWriteDsp(INT8U ucChipAddr, INT16U uiSubAddr, INT8U* ucData, INT16U uiLen);

//从ADAU1701中读取数据
u8 I2cReadDsp(INT8U ucChipAddr, INT16U uiSubAddr, INT8U* ucData, INT16U uiLen);

u8 ADAU1701_Init(void);

u8 DspInit(INT8U ucDspAddr);

void ReadInit(unsigned char ChipADR,unsigned short ReadAddress , unsigned short ReadSet);

void RdWave(unsigned char ChipADR);

void VolCtrl(unsigned char ChipADR,float cVolData);


void EQCtrl(unsigned char ChipADR,unsigned char Segment , unsigned char Size);

#endif
