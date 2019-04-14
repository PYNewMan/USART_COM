#include "dsp.h"
#include "USART.h"
#include "delay.h"
#include "math.h"
#include "stdbool.h"
#include "uart.h"

#define DSP_REG_ADDR  2076
#define DSP_PROG_ADDR 1024
#define DSP_PARA_ADDR 0

#define CORE_CONTROL_REG    2076   
#define SAFELOAD_DATA_0 	0X0810 
#define SAFELOAD_DATA_1 	0X0811	 
#define SAFELOAD_DATA_2 	0X0812	 
#define SAFELOAD_DATA_3 	0X0813		 
#define SAFELOAD_DATA_4 	0X0814
#define SAFELOAD_ADDRESS_0 	0X0815
#define SAFELOAD_ADDRESS_1 	0X0816  
#define SAFELOAD_ADDRESS_2 	0X0817
#define SAFELOAD_ADDRESS_3 	0X0818
#define SAFELOAD_ADDRESS_4 	0X0819 

#define VOLAddress1         0x64 

#define ADAU1701_RESET_H     GPIO_SetBits(GPIOA,GPIO_Pin_1)
#define ADAU1701_RESET_L     GPIO_ResetBits(GPIOA,GPIO_Pin_1) 
extern INT8U ucDspProgram[];
extern INT8U ucDspParam[];
extern INT8U ucHWReg[];
extern INT8U ucOffDsp[];
extern INT8U ucOnDsp[];

//写数据到ADAU1701

float m_cDynFrequency[12]={31,62,125,250,500,1000,2000,4000,8000,16000,20000};
unsigned char const PreEq[5][10]={
 								 {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//标准
								 {0x03,0x01,0x00,0x82,0x84,0x84,0x82,0x00,0x00,0x02},//流行
								 {0x00,0x00,0x00,0x04,0x04,0x04,0x00,0x02,0x03,0x04},//爵士
								 //{0x00,0x08,0x08,0x04,0x00,0x00,0x00,0x00,0x02,0x02},//古典
								 //{0x82,0x00,0x02,0x04,0x82,0x82,0x00,0x00,0x04,0x04},//摇滚
                                                                  {0x82,0x00,0x02,0x04,0x82,0x82,0x00,0x00,0x04,0x04},//摇滚
                                                                  {0x00,0x08,0x08,0x04,0x00,0x00,0x00,0x00,0x02,0x02},//古典							 
								};



void ADAU1701_GPIO_Configuration(void)
{
      GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
  	
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //通用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    ADAU1701_RESET_H;
    delay_ms(100);
    ADAU1701_RESET_L;
    delay_ms(500);
    ADAU1701_RESET_H;
}

u8 I2cWriteDsp(INT8U ucChipAddr, INT16U uiSubAddr, INT8U *ucData, INT16U uiLen)
{

	INT16U i;
	INT8U ucAddrHi, ucAddrLo;

	ucAddrHi = (INT8U)(uiSubAddr>>8);
	ucAddrLo = (INT8U)(uiSubAddr);
	
	if(!I2C_Start())
		return FALSE;
	I2C_SendByte(ucChipAddr);
	if(!I2C_WaitAck())
	{
		I2C_Stop(); 
		return FALSE;
	}
	I2C_SendByte(ucAddrHi);
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_SendByte(ucAddrLo);
	if(!I2C_WaitAck());
	for (i=0; i<uiLen; i++)
	{
		I2C_SendByte(ucData[i]);
		if(!I2C_WaitAck());
	}
	I2C_Stop();
	return TRUE;
}

//从ADAU1701中读取数据
u8 I2cReadDsp(INT8U ucChipAddr, INT16U uiSubAddr, INT8U *ucData, INT16U uiLen)
{

	INT16U i;
	INT8U ucAddrHi, ucAddrLo;

	ucAddrHi = (INT8U)(uiSubAddr>>8);
	ucAddrLo = (INT8U)(uiSubAddr);

	if(!I2C_Start())return FALSE;
	I2C_SendByte(ucChipAddr);
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_SendByte(ucAddrHi);
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_SendByte(ucAddrLo);
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	SDA_L;
	I2C_delay();
	SDA_H;
	I2C_Start();
	I2C_SendByte(ucChipAddr+1);
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	for (i=0; i<(uiLen-1); i++)
	{
		ucData[i] = I2C_ReceiveByte();
		I2C_Ack();
	}
	ucData[uiLen-1] = I2C_ReceiveByte();
	I2C_NoAck();
	I2C_Stop();
	return TRUE;

}

void ReadInit(unsigned char ChipADR,unsigned short ReadAddress , unsigned short ReadSet)
{
	unsigned char ReadSetDat[2];
	ReadSetDat[0]=ReadSet>>8;
	ReadSetDat[1]=ReadSet;
	I2cWriteDsp(ChipADR,ReadAddress, ReadSetDat,2);		
} 


unsigned char m_clevel[22];
/*
从1701读出频点值  
ChipADR -- 为器件地址
*/
void RdWave(unsigned char ChipADR)  
{
	unsigned char	ReadDataTemp[3],ci=0;
	signed char	temp;
	signed char	temps;
	for(ci=0;ci<14;ci++)
	{
		    ReadInit(ChipADR,0x081a ,(ci*0x0c+0x0e22));
		    I2cReadDsp(ChipADR,0x081a,ReadDataTemp,3);
			temp=((signed char)(96.33*((((unsigned long)ReadDataTemp[0]<<16)|\
				  ((unsigned short)ReadDataTemp[1]<<8)|ReadDataTemp[2])/524288.00-1)))+96;
			if(temp>96)
				temp=96;
			if(temp<=48)
				temp=0;
			else
				temp-=48;
                        
			m_clevel[ci]=temp;
	}
      
        temps = m_clevel[4];
        m_clevel[4] = m_clevel[7];
        m_clevel[7] = temps;
        temps = m_clevel[5];
        m_clevel[5] = m_clevel[6];
        m_clevel[6] = temps;
    

        
}


const float  m_cVolTable[256]= {
                                    80,     72,     64,     58,     52,     48,     44,     42,     40,     38,
                                    36,     34.5,   33,     32,     31,     30,     28.5,   27,     26,     25,
                                    24,     23.6,   23.2,   22.8,   22.4,   22,     21,     19.2,   18.7,   18.3,
                                    18,     17.5,   17,     16.6,   16.2,   15.9,   15.5,   15,     14.6,   14.2,
                                    14,     13.4,   13,     12.6,   12.2,   12,     11.4,   11,     10.6,   10.2,
                                    10,     9.7,    9.4,    9.1,    8.8,    8.6,    8.1,    7.7,    7.4,    7.2,
                                    7,      6.7,    6.4,    6.2,    6,      5.8,    5.5,    5.3,    5,      4.8,
                                    4.7,    4.4,    4.1,    3.8,    3.7,    3.6,    3.2,    3,      2.8,    2.7,
                                    2.6,    2.4,    2.2,    2,      1.9,    1.8,    1.4,    1.2,    1.1,    1,
                                    0.9,    0.8,    0.7,    0.6,    0.5,    0.4,    0.3,    0.2,    0.1,    0
                                };





u8 SafeParam(unsigned char ChipADR,unsigned short safe_param, unsigned char *param_data)	
{	
	unsigned char safe_param_lo;
	unsigned char safe_param_hi;
	unsigned char param_data_H;
	safe_param_lo=(unsigned char)safe_param;	
	safe_param_hi=(unsigned char)(safe_param>>8); 	
	param_data_H=0;


	I2C_Start();
	I2C_SendByte(ChipADR);	
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_SendByte(safe_param_hi); 
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_SendByte(safe_param_lo);	
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_SendByte(param_data_H);	 
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_SendByte(param_data[0]);	 
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_SendByte(param_data[1]);	  
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_SendByte(param_data[2]);	  
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_SendByte(param_data[3]);	 
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_Stop();
	return 1;	
}

u8 SafeAddr(unsigned char ChipADR,unsigned short safe_addr, unsigned short param_addr)
{
	unsigned char safe_addr_lo;	
	unsigned char safe_addr_hi;	
	unsigned char param_addr_lo;
	unsigned char param_addr_hi;
			  
	safe_addr_lo=(unsigned char)safe_addr;  	   			
	safe_addr_hi=(unsigned char)(safe_addr>>8);    	
	param_addr_lo=(unsigned char)param_addr;  	
	param_addr_hi=(unsigned char)(param_addr>>8); 	

	I2C_Start();
	I2C_SendByte(ChipADR);		
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_SendByte(safe_addr_hi);    
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_SendByte(safe_addr_lo);	
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_SendByte(param_addr_hi);	
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_SendByte(param_addr_lo);	  
	if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	I2C_Stop();
	return 1;
} 

void VolCtrl(unsigned char ChipADR,float cVolData)  
{
	unsigned char corecontrol[2] = {0x00,0x3c};
	unsigned long temp;
	unsigned char ParamVol[4];
	unsigned char VolDataGain[4];
        
	temp=(unsigned long)(0x800000*pow(0.891250,cVolData));
	VolDataGain[0]=temp>>24;
	VolDataGain[1]=temp>>16;
	VolDataGain[2]=temp>>8;
	VolDataGain[3]=temp;
	ParamVol[0]=0;
	ParamVol[1]=0;
	ParamVol[2]=0x08;
	ParamVol[3]=0;
	SafeParam(ChipADR,SAFELOAD_DATA_0, VolDataGain);
	delay_us(1000);			   	
	SafeAddr(ChipADR,SAFELOAD_ADDRESS_0, VOLAddress1);
	delay_us(1000);			   	
    SafeParam(ChipADR,SAFELOAD_DATA_0+1, ParamVol);			   	
	delay_us(1000);			   	
	SafeAddr(ChipADR,SAFELOAD_ADDRESS_0+1, VOLAddress1+1);
	delay_us(1000);			   	
	I2cWriteDsp(ChipADR,CORE_CONTROL_REG, corecontrol, 2);
}


 void To523(float param_dec, unsigned char *param_hex,u8 Sign)
	
{	
		signed  long  param223;
		signed  long  param227;
		signed  long  MoveMath=0x01;
			
		param223 =param_dec * (MoveMath << 23);		
		if(!Sign)
		param227 = param223 + (MoveMath << 27); 
		else
		param227=param223;
		param_hex[3]=(unsigned char)param227;  	
		param_hex[2]=(unsigned char)(param227>>8); 
		param_hex[1]=(unsigned char)(param227>>16); 
		param_hex[0]=(unsigned char)(param227>>24); 
		if(!Sign)
		param_hex[0] = param_hex[0] ^ 0x08;   	
}

void EQCtrl(unsigned char ChipADR,unsigned char Segment , unsigned char Size)
{
	//Size 0~48,0: -24db 24:0db  48:24db
	float A,w0,alpha,a0,a1,a2,b0,b1,b2;
	unsigned char ci;
	unsigned char corecontrol[2] = {0x00,0x3c};
	unsigned short EQAddress;
	unsigned char Param1[5][4];
	unsigned char t;
	signed char  Step=0;

	if(Size<50)
		Step=Size-24;
	A=pow(10,(((float)Step)/40));
	w0=m_cDynFrequency[Segment]/7639.437;
	alpha=sin(w0)/2.82;
	a0=1+alpha/A;
	b0 = (1+alpha*A)/a0;
	b1 = (-(2*cos(w0)))/a0;
	b2 = (1-alpha*A)/a0;
	a1=-(-2*cos(w0))/a0;
	a2=-(1-alpha/A)/a0;
	To523(b0, &Param1[0][0],1);
	To523(b1, &Param1[0][0]+4,1);
	To523(b2, &Param1[0][0]+8,1);
	To523(a1, &Param1[0][0]+12,1);
	To523(a2, &Param1[0][0]+16,1);
	for(ci=0;ci<2;ci++)
	{
		if(ci==0)
			EQAddress=Segment*5;
		else
			EQAddress=50+Segment*5;
		for(t=0;t<5;t++)
		{
			SafeParam(ChipADR,SAFELOAD_DATA_0+t, &Param1[t][0]);			   	
			SafeAddr(ChipADR,SAFELOAD_ADDRESS_0+t, EQAddress+t);
		}
		I2cWriteDsp(ChipADR,CORE_CONTROL_REG, corecontrol, 2);
	}
}

extern u8 Vol_Value;
u8 ADAU1701_Init(void)
{
  u8 Error_Conut;
  while(!DspInit(0x68))
  {
    if(Error_Conut++>30) 
			return 0;
  }
  return 1;
}

u8 DspInit(INT8U ucDspAddr)
{
	if(I2cWriteDsp(ucDspAddr, DSP_REG_ADDR, ucOffDsp, 2)==0)
	{
		//Uart1_Printf("\n\r ucOffDsp error");
		return FALSE;
	}


		
	if(I2cWriteDsp(ucDspAddr, DSP_PROG_ADDR, ucDspProgram, 5120)==0)
	{
		//Uart1_Printf("\n\r ucDspProgram error");
		return FALSE;
	}

	if(I2cWriteDsp(ucDspAddr, DSP_PARA_ADDR, ucDspParam, 4096)==0)
	{
		//Uart1_Printf("\n\r ucDspParam error");
		return FALSE;
	}

	if(I2cWriteDsp(ucDspAddr, DSP_REG_ADDR, ucHWReg, 24)==0)
	{
		//Uart1_Printf("\n\r ucHWReg error");
		return FALSE;
	}

	if(I2cWriteDsp(ucDspAddr, DSP_REG_ADDR, ucOnDsp, 2)==0)
	{
		//Uart1_Printf("\n\r ucOnDsp error");
		return FALSE;
	}
	//Uart1_Printf("\n\r DspInit OK");
		return TRUE;
}
