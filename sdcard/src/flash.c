#include "flash.h" 
#include "spi.h"
#include "pervia.h"
//4Kbytes���@��Sector
//16�ӺϰϬ�1��Block
//W25X16
//�e�q��2M�줸��,�@��32��Block,512��Sector 

//��l��SPI FLASH��IO�f
int* ptr = NULL;

void SPI_Flash_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	SPI_FLASH_CS_CLK,ENABLE);	 

    GPIO_InitStructure.GPIO_Pin = SPI_FLASH_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_FLASH_CS_PORT, &GPIO_InitStructure);
	GPIO_SetBits(SPI_FLASH_CS_PORT,SPI_FLASH_CS_PIN);
	Clr_SPI_FLASH_CS;
	SPIx_Init();
	Set_SPI_FLASH_CS; 

	//�]�mSD Card��CS�åB�԰�
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}  

//Ū��SPI_FLASH�����A�H�s��
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:�w�]0,���A�H�s���O�@�줸,�t�XWP�ϥ�
//TB,BP2,BP1,BP0:FLASH�ϰ쨾�g�]�m
//WEL:�g�ϯ���w
//BUSY:���аO�줸(1,��;0,�Ŷ�)
//�q�{:0x00
u8 SPI_Flash_ReadSR(void)   
{  
	u8 byte=0;   
	Clr_SPI_FLASH_CS;                            //�ϯྐྵ��   
	SPIx_ReadWriteByte(W25X_ReadStatusReg);    //�o�eŪ�����A�H�s���R�O    
	byte=SPIx_ReadWriteByte(0Xff);             //Ū���@�Ӧ줸��  
	Set_SPI_FLASH_CS;                            //��������     
	return byte;   
} 
//�gSPI_FLASH���A�H�s��
//�u��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)�i�H�g!!!
void SPI_FLASH_Write_SR(u8 sr)   
{   
	Clr_SPI_FLASH_CS;                            //�ϯྐྵ��   
	SPIx_ReadWriteByte(W25X_WriteStatusReg);   //�o�e�g�����A�H�s���R�O    
	SPIx_ReadWriteByte(sr);               //�g�J�@�Ӧ줸��  
	Set_SPI_FLASH_CS;                            //��������     	      
}   
//SPI_FLASH�g�ϯ�	
//�NWEL�m��   
void SPI_FLASH_Write_Enable(void)   
{
	Clr_SPI_FLASH_CS;                            //�ϯྐྵ��   
    SPIx_ReadWriteByte(W25X_WriteEnable);      //�o�e�g�ϯ�  
	Set_SPI_FLASH_CS;                            //��������     	      
} 
//SPI_FLASH�g�T��	
//�NWEL�M�s  
void SPI_FLASH_Write_Disable(void)   
{  
	Clr_SPI_FLASH_CS;                            //�ϯྐྵ��   
    SPIx_ReadWriteByte(W25X_WriteDisable);     //�o�e�g�T����O    
	Set_SPI_FLASH_CS;                            //��������     	      
} 			    
//Ū������ID W25X16��ID:0XEF14
u16 SPI_Flash_ReadID(void)
{
	u16 Temp = 0;	  
	Clr_SPI_FLASH_CS;				    
	SPIx_ReadWriteByte(0x90);//�o�eŪ��ID�R�O	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	 			   
	Temp|=SPIx_ReadWriteByte(0xFF)<<8;  
	Temp|=SPIx_ReadWriteByte(0xFF);	 
	Set_SPI_FLASH_CS;				    
	return Temp;
}   		    
//Ū��SPI FLASH  
//�b���w��}�}�lŪ�����w���ת����
//pBuffer:��Ʀs�x��
//ReadAddr:�}�lŪ������}(24bit)
//NumByteToRead:�nŪ�����줸�ռ�(�̤j65535)
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;    												    
	Clr_SPI_FLASH_CS;                            //�ϯྐྵ��   
    SPIx_ReadWriteByte(W25X_ReadData);         //�o�eŪ���R�O   
    SPIx_ReadWriteByte((u8)((ReadAddr)>>16));  //�o�e24bit�a�}    
    SPIx_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPIx_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPIx_ReadWriteByte(0XFF);   //�j��Ū��  
    }
	Set_SPI_FLASH_CS;                            //��������     	      
}  
//SPI�b�@��(0~65535)���g�J�֩�256�Ӧ줸�ժ����	(�@�����@��block)
//�b���w��}�}�l�g�J�̤j256�줸�ժ����
//pBuffer:��Ʀs�x��
//WriteAddr:�}�l�g�J����}(24bit)
//NumByteToWrite:�n�g�J���줸�ռ�(�̤j256),�ӼƤ����ӶW�L�ӭ����Ѿl�줸�ռ�!!!	 
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    SPI_FLASH_Write_Enable();                  //SET WEL 
	Clr_SPI_FLASH_CS;                            //�ϯྐྵ��   
    SPIx_ReadWriteByte(W25X_PageProgram);      //�o�e�g���R�O   
    SPIx_ReadWriteByte((u8)((WriteAddr)>>16)); //�o�e24bit�a�}    
    SPIx_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPIx_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPIx_ReadWriteByte(pBuffer[i]);//�j��g��  
	Set_SPI_FLASH_CS;                            //�������� 
	SPI_Flash_Wait_Busy();					   //���ݼg�J����
} 
//�L����gSPI FLASH 
//�����T�O�Ҽg����}�d�򤺪���ƥ�����0XFF,�_�h�b�D0XFF�B�g�J����ƱN����!
//�㦳�۰ʴ����\�� 
//�b���w��}�}�l�g�J���w���ת����,���O�n�T�O��}���V��!
//pBuffer:��Ʀs�x��
//WriteAddr:�}�l�g�J����}(24bit)
//NumByteToWrite:�n�g�J���줸�ռ�(�̤j65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //�歶�Ѿl���줸�ռ�		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//���j��256�Ӧ줸��
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//�g�J�����F
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��h�w�g�g�J�F���줸�ռ�
			if(NumByteToWrite>256)pageremain=256; //�@���i�H�g�J256�Ӧ줸��
			else pageremain=NumByteToWrite; 	  //����256�Ӧ줸�դF
		}
	};	    
} 
//�gSPI FLASH  
//�b���w��}�}�l�g�J���w���ת����
//�Ө�Ʊa�����ާ@!
//pBuffer:��Ʀs�x��
//WriteAddr:�}�l�g�J����}(24bit)
//NumByteToWrite:�n�g�J���줸�ռ�(�̤j65535)  		   
u8 SPI_FLASH_BUF[4096];
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    

	secpos=WriteAddr/4096;//�ϰϦ�} 0~511 for w25x16 (�ĴX��sector, datasheet pp.09)
	secoff=WriteAddr%4096;//�b�ϰϤ�������
	secremain=4096-secoff;//�ϰϳѾl�Ŷ��j�p   

	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//���j��4096�Ӧ줸��
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//Ū�X��ӺϰϪ����e
		for(i=0;i<secremain;i++)//������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//�ݭn����  	  
		}
		if(i<secremain)//�ݭn����
		{
			SPI_Flash_Erase_Sector(secpos);//�����o�Ӻϰ�
			for(i=0;i<secremain;i++)	   //�ƻs
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//�g�J��Ӻϰ�  

		}else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//�g�w�g�����F��,�����g�J�ϰϳѾl�϶�. 				   
		if(NumByteToWrite==secremain)break;//�g�J�����F
		else//�g�J������
		{
			secpos++;//�ϰϦ�}�W1
			secoff=0;//������m��0 	 

		   	pBuffer+=secremain;  //���w����
			WriteAddr+=secremain;//�g��}����	   
		   	NumByteToWrite-=secremain;				//�줸�ռƻ���
			if(NumByteToWrite>4096)secremain=4096;	//�U�@�Ӻϰ��٬O�g����
			else secremain=NumByteToWrite;			//�U�@�Ӻϰϥi�H�g���F
		}	 
	};	 	 
}
//������Ӵ���
//��������ɶ�:
//W25X16:25s 
//W25X32:40s 
//W25X64:40s 
//���ݮɶ��W��...
void SPI_Flash_Erase_Chip(void)   
{                                             
    SPI_FLASH_Write_Enable();                  //SET WEL 
    SPI_Flash_Wait_Busy();   
  	Clr_SPI_FLASH_CS;                            //�ϯྐྵ��   
    SPIx_ReadWriteByte(W25X_ChipErase);        //�o�e�������R�O  
	Set_SPI_FLASH_CS;                            //��������     	      
	SPI_Flash_Wait_Busy();   				   //���ݴ�����������
}   
//�����@�Ӻϰ�
//Dst_Addr:�ϰϦ�} 0~511 for w25x16
//�����@�Ӯ��Ϫ��̤֮ɶ�:150ms
void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
{   
	Dst_Addr*=4096;
    SPI_FLASH_Write_Enable();                  //SET WEL 	 
    SPI_Flash_Wait_Busy();   
  	Clr_SPI_FLASH_CS;                            //�ϯྐྵ��   
    SPIx_ReadWriteByte(W25X_SectorErase);      //�o�e�ϰ��������O 
    SPIx_ReadWriteByte((u8)((Dst_Addr)>>16));  //�o�e24bit�a�}    
    SPIx_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPIx_ReadWriteByte((u8)Dst_Addr);  
	Set_SPI_FLASH_CS;                            //��������     	      
    SPI_Flash_Wait_Busy();   				   //������������
}  
//���ݪŶ�
void SPI_Flash_Wait_Busy(void)   
{   
	while ((SPI_Flash_ReadSR()&0x01)==0x01);   // ����BUSY��M��
}  
//�i�J���q�Ҧ�
void SPI_Flash_PowerDown(void)   
{ 
  	Clr_SPI_FLASH_CS;                            //�ϯྐྵ��   
    SPIx_ReadWriteByte(W25X_PowerDown);        //�o�e���q�R�O  
	Set_SPI_FLASH_CS;                            //��������     	      
    DELAY_uS(3);                               //����TPD  
}   
//���
void SPI_Flash_WAKEUP(void)   
{  
  	Clr_SPI_FLASH_CS;                            //�ϯྐྵ��   
    SPIx_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	Set_SPI_FLASH_CS;                            //��������     	      
    DELAY_uS(3);                               //����TRES1
}   

