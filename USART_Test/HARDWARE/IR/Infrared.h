#ifndef __INFRARED_H
#define __INFRARED_H

#include "stm32f10x.h"



#define VOL_UP 				0x02
#define VOL_DOWN			0x51
#define NEXT				 	0x52
#define PREV					0x53
#define ENTER					0x07
#define RETURN				0x4a
#define UP						0x06
#define DOWN					0x44
#define LEFT					0x47
#define RIGHT					0x40
#define POWER					0x14
#define MUTE					0x46
#define NN						0x45
#define BB  					0x01
#define MODE 					0x1a
#define VIDEO					0x09
#define MUSIC					0x1d
#define PHOTO					0x1f
#define TEXT					0x0d

extern unsigned char Infrared_data[];
void Infrared_handle(void);
void Infrared_Configuration(void);
void Timer_Configuration(void);
u8 Infrared_data_Infrared(void);
#endif 

