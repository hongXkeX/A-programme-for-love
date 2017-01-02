#include "Smart_Usart.h"

unsigned char nowdate,lastdate,nowdate1;

/* ��ʼ������ */
void Usart_Init()
{
	TMOD = 0x21;					//��ʱ��0������ʽ1����ʱ��1������ʽ2
	TH0 = ( 65536 - 10000 ) / 256;
	TL0 = ( 65536 - 10000 ) % 256;
	TH1 = 0xfd;
	TL1 = 0xfd;
	TR0 = 1;                        // ��ʱ��0��ʼ��ʱ
	TR1 = 1;                        // ��ʱ��1��ʼ��ʱ
	REN = 1;
	SM0 = 0;
	SM1 = 1;
	ES = 1;
	
	S2CON = 0x50;				     //��ʽ1����λ���ݣ��ɱ䲨����
	AUXR1 = 0x00;				     //1T������ʽ
 	PCON  = 0x00;					 //������������
 	IP2   = 0x00;					 //���ȼ�Ĭ��
 	BRT   = 0XFD; 					 //���ò�����9600
 	AUXR  = 0x10;					 //���������ʷ�����
 	EA    = 1; 					     //�����ж�
	IE2   = 0x01;					 //������2�ж�
	
	key_config_Ap = 1;
	key_config_Air = 1;	
}

/* mcu�������ݳ��� (mcu --> wifi) */
void Usart_SendArrang(uchar *dat,uchar len)
{
	unsigned char i;
	ES = 0;
	for( i = 0; i < len; i++ )
	{
		SBUF = dat[i];
		/*
		if( i >= 2 && dat[i] == 0xff ) {
			dat[++i] = 0x55;            // ���ڷ��ͷ������⵽����0xff���������ݣ���Ҫ��0xff�����0x55
			--i;
		}
		*/
		while( !TI );
		TI = 0;
	} 
	ES = 1;
}

/* ��ʱ��0�ж� */
void Timer0_Int() interrupt 1
{
	TH0 = ( 65536 - 10000 ) / 256;
	TL0 = ( 65536 - 10000 ) % 256;  
	num_usart++;
	num_usart_se++;
}

/* �����жϺ���  (wifi --> mcu) */
void Usart_Int() interrupt 4
{
	num_usart = 0;
	ET0 = 1;
	TR0 = 1;
	usartbuf[ usrtlen++ ] = SBUF;
	nowdate = SBUF;
	if( usrtlen==2 )
	{
		if( usartbuf[0] != 0xff || usartbuf[1] != 0xff )
		{
			usrtlen = 0;
		}
	}
	// ���ڽ��շ������⵽�ǰ�ͷ���ֳ���0xff,��Ҫ�ѽ�������0x55�Ƴ�
	if( (lastdate == 0xff ) && ( nowdate == 0x55 ) )
	{
		 usrtlen = usrtlen - 1;
	}
	lastdate = nowdate;
	RI = 0;
}

/* ����У����� */ 
unsigned char DateCheck()
{
	unsigned char i,temp;
	unsigned char CheckSum=0;
	for( i = 0; i < ( usartbuf[3] + 1 ); i++ ) //+1�����Ȱ���У��ͣ����ǲ��������ȵ�2���ֽڣ�length=����+2���ֽ�-У���ռ�õ�һ���ֽ�
	{
		CheckSum = CheckSum + usartbuf[i+2];
	}
	temp = usartbuf[3] + 3;					   //��ȡУ��ͣ�����+4-1ΪУ���λ�ã������0��ʼ
	if( CheckSum == usartbuf[temp] )
	   return 1;
	else
	   return 0;
}

void uart2_isr() interrupt 8
{
	usart2buf[ usrt2len++ ] = S2BUF;
	
	if( usrt2len==2 )
	{
		if( usartbuf[0] != 0xff || usartbuf[1] != 0xff )
		{
			usrt2len = 0;
		}
	}
	S2CON &= ~0x01;
}
