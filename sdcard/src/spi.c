#include "spi.h"

SPI_InitTypeDef  SPI1_InitStructure;
SPI_InitTypeDef  SPI2_InitStructure;


/*SPI1*/
void SPIx_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  	//Enable Clock
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );	
 	//Pin Configuration
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�ƥα�����X
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
	//SPI Configuration
	SPI1_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //�]�mSPI��V�Ϊ����V����ƼҦ�:SPI�]�m�����u���V�����u
	SPI1_InitStructure.SPI_Mode = SPI_Mode_Master;		//�]�mSPI�u�@�Ҧ�:�]�m���DSPI
	SPI1_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//�]�mSPI���ƾڤj�p:SPI�o�e����8�줸�V���c
	SPI1_InitStructure.SPI_CPOL = SPI_CPOL_High;		//��ܤF��C������í�A:�����a�Ű�
	SPI1_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//��Ʈ����ĤG�Ӯ����u
	SPI1_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�H���ѵw��]NSS�޸}�^�٬O�n��]�ϥ�SSI�줸�^�޲z:����NSS�H����SSI�줸����
	SPI1_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//�w�q��C�ǿ�t�v�w���W����:��C�ǿ�t�v�w���W�Ȭ�256
	SPI1_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//���w��ƶǿ�qMSB���٬OLSB��}�l:��ƶǿ�qMSB��}�l
	SPI1_InitStructure.SPI_CRCPolynomial = 7;	//CRC�ȭp�⪺�h����
	SPI_Init(SPI1, &SPI1_InitStructure);  //�ھ�SPI_InitStruct�����w���Ѽƪ�l�ƥ~�]SPIx�H�s��
 
	SPI_Cmd(SPI1, ENABLE); //�ϯ�SPI�~�]
	
	SPIx_ReadWriteByte(0xff);//�Ұʶǿ�		 
}   


/*SPI2*/
void SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  	//Enable Clock
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB|RCC_APB1Periph_SPI2, ENABLE );	
 	//Pin Configuration
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�ƥα�����X
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	//SPI Configuration
	SPI2_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  
	SPI2_InitStructure.SPI_Mode = SPI_Mode_Master;		
	SPI2_InitStructure.SPI_DataSize = SPI_DataSize_8b;		
	SPI2_InitStructure.SPI_CPOL = SPI_CPOL_High;		//CPOL=1
	SPI2_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//CPHA=1
	SPI2_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS software
	SPI2_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//�w�q��C�ǿ�t�v�w���W����:��C�ǿ�t�v�w���W�Ȭ�256
	SPI2_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//���w��ƶǿ�qMSB���٬OLSB��}�l:��ƶǿ�qMSB��}�l
	SPI2_InitStructure.SPI_CRCPolynomial = 7;	//CRC�ȭp�⪺�h����
	SPI_Init(SPI2, &SPI2_InitStructure);  //�ھ�SPI_InitStruct�����w���Ѽƪ�l�ƥ~�]SPIx�H�s��
 
	SPI_Cmd(SPI2, ENABLE); //�ϯ�SPI�~�]
	
	SPI2_ReadWriteByte(0xff);//�Ұʶǿ�		 
}   
//SPI �t�׳]�m���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2���W   (SPI 36M@sys 72M)
//SPI_BaudRatePrescaler_8   8���W   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_16  16���W  (SPI 4.5M@sys 72M)
//SPI_BaudRatePrescaler_256 256���W (SPI 281.25K@sys 72M)
  
void SPIx_SetSpeed(u8 SpeedSet)
{
	SPI1_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
  	SPI_Init(SPI1, &SPI1_InitStructure);
	SPI_Cmd(SPI1,ENABLE);
} 

//SPI �t�׳]�m���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2���W   (SPI 36M@sys 72M)
//SPI_BaudRatePrescaler_8   8���W   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_16  16���W  (SPI 4.5M@sys 72M)
//SPI_BaudRatePrescaler_256 256���W (SPI 281.25K@sys 72M)
  
void SPI2_SetSpeed(u8 SpeedSet)
{
	SPI2_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
  	SPI_Init(SPI2, &SPI2_InitStructure);
	SPI_Cmd(SPI2,ENABLE);
}


u8 SPIx_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) 
		{
			retry++;
			if(retry>200)return 0;
		}			  

	SPI_I2S_SendData(SPI1, TxData);
	retry=0;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
		{
			retry++;
			if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI1); 
}






u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //�ˬd���w��SPI�лx�줸�]�m�P�_:�o�e�w�s�żлx�줸
		{
			retry++;
			if(retry>200)return 0;
		}			  

	SPI_I2S_SendData(SPI2, TxData); //�q�L�~�]SPIx�o�e�@�Ӹ��
	retry=0;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); //�ˬd���w��SPI�лx�줸�]�m�P�_:�����w�s�D�żлx�줸
		{
			retry++;
			if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI2); //��^�q�LSPIx�̪񱵦������
}
