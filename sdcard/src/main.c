/*************************************************************
************************A R M S*******************************
**************************************************************
Name	: SD card Driver Example
Author	: Alex Huang
Version	: V1.0
Description: example of how to creat a .txt file in the SD Card
			 via FatFs.
**************************************************************/
#include "pervia.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "spi.h"
#include "flash.h"
#include "sd.h"	
#include "ff.h"
#include "string.h"

/*�����ܼ�*/
//FatFs
FATFS fs; 		// Work area (file system object) for logical drive
FIL fsrc, fdst; // file objects
FRESULT res; 	// FatFs function common result code
UINT br, bw; 	// File R/W count
DIR dir;
FILINFO fileInfo;
//txt
unsigned char buffer[512];
//�ڥؿ�
const char filedir[]="0:/";
//file path
char root_filedir[50][50]; //�ڥؿ��U��󧨸��|,�w�]�̦h50�Ӥ��. ���䴩���ɦW�A��󧨦W�ٳ̦h8�Ӧr
u8 nbof_rootDoc = 0;//�����ڥؿ��U���X�Ӹ�Ƨ�

int main(void)
{	
	float SD_Size = 0;//SD�d�e�q
	u32 a,br ;//Ū����Ƭy�p��

	Initial_STM32();//Initial STM32
	LED_Configuration();//Initial LED Pins
	USART_Configuration();//Initial USART
	SPI_Flash_Init();//Initial Flash
	while(SD_Initialize())//��l��SD�d
	{
		printf("Please insert SD Card.\r\n");
		DELAY_mS(1000);	
	}
		SD_Size = SD_GetSectorCount();	
		SD_Size = (SD_Size*512.0f)/1024.0f/1024.0f/1024.0f ;		
		//Show SD Card Size
		printf("SD Card Size:%f GB\r\n",SD_Size);
	/*Ū�������*/
	f_mount(0 , &fs);			  //���U�u�@  ����
	res = f_opendir(&dir,filedir);//���}���w�ؿ��U���
	if(res==FR_OK)
	{	
		
		res = f_open(&fsrc, "DCIM/109___01/IMG_5542.JPG", FA_READ);
		if(!res)
		{
			printf("open news.txt : %d\r\n",res);
			br=1;
			a=0;
			printf("File Content:\r\n");
			for (;;)
			{
				for(a=0; a<512; a++) buffer[a]=0;
				res = f_read(&fsrc, buffer, sizeof(buffer), &br);
				printf("%s\r\n",buffer);
				if (res || br < sizeof(buffer)) break; // error or eof
			}
		}
		f_close(&fsrc); //������Ƭy		
	}
	f_mount(0,NULL);//�����x�s���U��
	while(1)
	{	
	 	  
	  if(GPIO_ReadOutputDataBit(LED1_Port,LED1_Pin))
	  {
     	LED1_ON();
		LED2_OFF();
		DELAY_mS(1000);
	  }
	  else
	  {
	  	LED1_OFF();
		LED2_ON();
		DELAY_mS(1000);	  	
	  }
	}
}

