#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x.h"

/*Function Prototype*/ 				  	    													  
void SPIx_Init(void);			 
void SPI2_Init(void);
void SPIx_SetSpeed(u8 SpeedSet); //�]�mSPI�t��
void SPI2_SetSpeed(u8 SpeedSet);    
u8 SPIx_ReadWriteByte(u8 TxData);//SPI�׬y��Ū�g�@�Ӧ줸��
u8 SPI2_ReadWriteByte(u8 TxData);//SPI�׬y��Ū�g�@�Ӧ줸��

		 
#endif


