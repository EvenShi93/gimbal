#include "sd.h"			   
#include "spi.h"

u8  SD_Type=0;//SD�d���� 

//�̷�SD Card���쪺SPI��Ū�g�ʰ�
//data:�n�g�J���ƾ�
//return:Ū�쪺�ƾ�
u8 SD_SPI_ReadWriteByte(u8 data)
{
	return SPIx_ReadWriteByte(data);
}
//SD��l�ƪ��ɭԻݭn�ϥΧC�t�]�w
void SD_SPI_SpeedLow(void)
{
 	SPIx_SetSpeed(SPI_BaudRatePrescaler_256);//�]�m��C�t�Ҧ�	
}
//SD���`�B�@�ɨϥΰ��t
void SD_SPI_SpeedHigh(void)
{
 	SPIx_SetSpeed(SPI_BaudRatePrescaler_8);//�]�m�D���t	
}
//SPI�~�]��l��
void SD_SPI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	SD_CS_CLK,ENABLE);	 

    GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;//Push-pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SD_CS_PORT, &GPIO_InitStructure);
	Clr_SD_CS;
	SPIx_Init();
	Set_SD_CS;
}
///////////////////////////////////////////////////////////////////////////////////
//����SD Card���
void SD_DisSelect(void)
{
	Set_SD_CS;
	SD_SPI_ReadWriteByte(0xff);//�žާ@
}
//���SD Card�A���ݥd�ǳƧ����i��U�@�Ӱʧ@
//Retrun:0,���\;1,����;
u8 SD_Select(void)
{
	Clr_SD_CS;
	if(SD_WaitReady()==0)return 0;//���ݦ��\
	//
	SD_DisSelect();
	return 1;//���ݥ���
}
//����SD Card�ǳƦn
//return:0,�ǳƦn;��L��,����
u8 SD_WaitReady(void)
{
	u32 t=0;
	do
	{
		if(SD_SPI_ReadWriteByte(0XFF)==0XFF)return 0;//OK
		t++;		  	
	}while(t<0XFFFFFF);//���� 
	return 1;
}
//����SD�d�^��
//Response:�n�o�쪺�^����
//return:0,���\�o��F�Ӧ^����
//     ��L,�o��^���ȥ���
u8 SD_GetResponse(u8 Response)
{
	u16 Count=0xFFF;//���ݦ���	   						  
	while ((SD_SPI_ReadWriteByte(0XFF)!=Response)&&Count)Count--;//���ݱo��ǽT���^��  	  
	if (Count==0)return MSD_RESPONSE_FAILURE;//�o��^������   
	else return MSD_RESPONSE_NO_ERROR;//���T�^��
}
//�qSD�dŪ���@�Ӽƾڥ]�����e��
//buf:�ƾڽw�İ���
//len:�nŪ�����ƾڪ���
//Retrun:0,���\;1,����;	
u8 SD_RecvData(u8*buf,u16 len)
{			  	  
	if(SD_GetResponse(0xFE))return 1;
    while(len--)
    {
        *buf=SPIx_ReadWriteByte(0xFF);
        buf++;
    }
 
    SD_SPI_ReadWriteByte(0xFF);
    SD_SPI_ReadWriteByte(0xFF);									  					    
    return 0;
}
//�VSD�d�¤J�@�Ӽƾڥ]�����e,512 Bytes
//buf:�ƾڼȦs����
//cmd:���O
//Retrun:0,���\;1,����;		
u8 SD_SendBlock(u8*buf,u8 cmd)
{	
	u16 t;		  	  
	if(SD_WaitReady())return 1;//�ȴ�׼��ʧЧ
	SD_SPI_ReadWriteByte(cmd);
	if(cmd!=0XFD)//���ǽ���ָ��
	{
		for(t=0;t<512;t++)SPIx_ReadWriteByte(buf[t]);//����ٶ�,���ٺ�������ʱ��
	    SD_SPI_ReadWriteByte(0xFF);//����crc
	    SD_SPI_ReadWriteByte(0xFF);
		t=SD_SPI_ReadWriteByte(0xFF);//������Ӧ
		if((t&0x1F)!=0x05)return 2;//��Ӧ����									  					    
	}						 									  					    
    return 0;//д��ɹ�
}

//�VSD�d�o�e�@�өR�O
//��J��: u8 cmd   �R�O 
//      u32 arg  �R�O�Ѽ�
//      u8 crc   CRC�ˬd��	   
//��^��:SD�d�^�Ǫ���														  
u8 SD_SendCmd(u8 cmd, u32 arg, u8 crc)
{
    u8 r1;	
	u8 Retry=0; 
	SD_DisSelect();//��������
	if(SD_Select())return 0XFF;//���異�� 
	//����
    SD_SPI_ReadWriteByte(cmd | 0x40);//���O�g�J�R�O
    SD_SPI_ReadWriteByte(arg >> 24);
    SD_SPI_ReadWriteByte(arg >> 16);
    SD_SPI_ReadWriteByte(arg >> 8);
    SD_SPI_ReadWriteByte(arg);	  
    SD_SPI_ReadWriteByte(crc); 
	if(cmd==CMD12)SD_SPI_ReadWriteByte(0xff);//Skip a stuff byte when stop reading
    //����SD�d�^���A�άO�W�ɰh�X
	Retry=0X1F;
	do
	{
		r1=SD_SPI_ReadWriteByte(0xFF);
	}while((r1&0X80) && Retry--);	 
	//��^���A��
    return r1;
}		    																			  
//Ū��SD�d��CID�H���A�]�A��y�ӫH��
//��J: u8 *cid_data(�s��CID�����s�A�ܤ�16Byte�^	  
//��^��:0�GNO_ERR
//		 1:���~														   
u8 SD_GetCID(u8 *cid_data)
{
    u8 r1;	   
    //�o�eCMD10�R�O,ŪCID
    r1=SD_SendCmd(CMD10,0,0x01);
    if(r1==0x00)
	{
		r1=SD_RecvData(cid_data,16);//����16��Byte�ƾ�	 
    }
	SD_DisSelect();
	if(r1)return 1;
	else return 0;
}																				  
//Ū��SD�d��CSD�H���A�]�t�e�q�P�t�׫H��
//��J: u8 *cid_data(�s��CID�����s�A�ܤ�16Byte�^	  
//��^��:0�GNO_ERR
//		 1:���~														   
u8 SD_GetCSD(u8 *csd_data)
{
    u8 r1;	 
    r1=SD_SendCmd(CMD9,0,0x01);//�o�eCMD9�R�O�AŪCSD
    if(r1==0)
	{
    	r1=SD_RecvData(csd_data, 16);//����16��Byte�ƾ�
    }
	SD_DisSelect();
	if(r1)return 1;
	else return 0;
}  
//���SD�d�`���ϼƶq   
//��^��:0,Ū���e�q�X��
//       ��L,SD�d�e�q(���ϼ�/512Byte)
//�C�Ӯ��Ϫ�Byte�ƥ��w��512�A�p�G���O512�h��l����q�L														  
u32 SD_GetSectorCount(void)
{
    u8 csd[16];
    u32 Capacity;  
    u8 n;
	u16 csize;  					    
	//Ū��CSD�H���A�p�G�����X���h��^0
    if(SD_GetCSD(csd)!=0) return 0;	    
    //�p�G��SDHC�d�A���ӤU���覡�p��
    if((csd[0]&0xC0)==0x40)	 //V2.00���d
    {	
		csize = csd[9] + ((u16)csd[8] << 8) + 1;
		Capacity = (u32)csize << 10;//���ϼ�	 		   
    }else//V1.XX���d
    {	
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
		Capacity= (u32)csize << (n - 9);//�o�쮰�ϼ�   
    }
    return Capacity;
}
u8 SD_Idle_Sta(void)
{
	u16 i;
	u8 retry;	   	  
    for(i=0;i<0xf00;i++);	 
    for(i=0;i<10;i++)SPIx_ReadWriteByte(0xFF); 
    retry = 0;
    do
    {	   
        i = SD_SendCmd(CMD0, 0, 0x95);
        retry++;
    }while((i!=0x01)&&(retry<200));
    if(retry==200)return 1; 
	return 0;				  
}
//��l��SD�d
u8 SD_Initialize(void)
{
    u8 r1;      // �s��SD�d��^��
    u16 retry;  // �ΨӶi��W�ɭp��
    u8 buf[4];  
	u16 i;

	SD_SPI_Init();		//��l�ƾާ@IO
 	SD_SPI_SpeedLow();	//�]�m��C�t�Ҧ� 	  
	for(i=0;i<10;i++)SD_SPI_ReadWriteByte(0XFF);//�ֵ̤o�e74�ӯ߽�
	retry=20;
	do
	{
		r1=SD_SendCmd(CMD0,0,0x95);//�i�JIDLE���A
	}while((r1!=0X01) && retry--);
 	SD_Type=0;//�w�]�S�����JSD�d
	if(r1==0X01)
	{
		if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
		{
			for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(0XFF);	//Get trailing return value of R7 resp
			if(buf[2]==0X01&&buf[3]==0XAA)//SD�d�O�_�䤸2.7~3.6V
			{
				retry=0XFFFE;
				do
				{
					SD_SendCmd(CMD55,0,0X01);	//�o�eCMD55
					r1=SD_SendCmd(CMD41,0x40000000,0X01);//�o�eCMD41
				}while(r1&&retry--);
				if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//Ų�OSD2.0�d
				{
					for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(0XFF);//�o�쪺OCR��
					if(buf[0]&0x40)SD_Type=SD_TYPE_V2HC;    //�˯�CCS
					else SD_Type=SD_TYPE_V2;   
				}
			}
		}else//SD V1.x/ MMC	V3
		{
			SD_SendCmd(CMD55,0,0X01);	//�o�eCMD55
			r1=SD_SendCmd(CMD41,0,0X01);//�o�eCMD41
			if(r1<=1)
			{		
				SD_Type=SD_TYPE_V1;
				retry=0XFFFE;
				do //�ȴ��˳�IDLEģʽ
				{
					SD_SendCmd(CMD55,0,0X01);	//�o�eCMD55
					r1=SD_SendCmd(CMD41,0,0X01);//�o�eCMD41
				}while(r1&&retry--);
			}else
			{
				SD_Type=SD_TYPE_MMC;//MMC V3
				retry=0XFFFE;
				do //�ȴ��˳�IDLEģʽ
				{											    
					r1=SD_SendCmd(CMD1,0,0X01);//�o�eCMD1
				}while(r1&&retry--);  
			}
			if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)SD_Type=SD_TYPE_ERR;//���~���d
		}
	}
	SD_DisSelect();//��������
	SD_SPI_SpeedHigh();//���׼Ҧ�
	if(SD_Type)return 0;
	else if(r1)return r1; 	   
	return 0xaa;//��L�Ҧ�
}
 
//ŪSD�d
//buf:�ƾڼȦs��
//sector:������
//cnt:���ϼ�
//return:0,ok;��L,����.
u8 SD_ReadDisk(u8 *buf,u32 sector,u8 cnt)
{
	u8 r1;
	if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD17,sector,0X01);
		if(r1==0)
		{
			r1=SD_RecvData(buf,512);  
		}
	}else
	{
		r1=SD_SendCmd(CMD18,sector,0X01);//�s��R�O
		do
		{
			r1=SD_RecvData(buf,512);//����512Bytes	 
			buf+=512;  
		}while(--cnt && r1==0); 	
		SD_SendCmd(CMD12,0,0X01);	//�o�e����R�O
	}   
	SD_DisSelect();//��������
	return r1;//
}

//�gSD�d
//buf:�ƾڼȦs����
//sector:�_�l������
//cnt:���ϼ�
//return:0,ok;��L,����.

u8 SD_WriteDisk(u8 *buf,u32 sector,u8 cnt)
{
	u8 r1;
	if(SD_Type!=SD_TYPE_V2HC)sector *= 512;
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD24,sector,0X01);
		if(r1==0)
		{	
			r1=SD_SendBlock(buf,0xFE);	   
		}
	}else
	{
		if(SD_Type!=SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0X01);	
			SD_SendCmd(CMD23,cnt,0X01);	
		}
 		r1=SD_SendCmd(CMD25,sector,0X01);
		if(r1==0)
		{
			do
			{
				r1=SD_SendBlock(buf,0xFC);
				buf+=512;  
			}while(--cnt && r1==0);
			r1=SD_SendBlock(0,0xFD);
		}
	}   
	SD_DisSelect();
	return r1;
}