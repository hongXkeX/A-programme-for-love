#include "Smart_Usart.h"

unsigned char nowdate,lastdate,nowdate1;

/* 初始化函数 */
void Usart_Init()
{
	TMOD = 0x21;					//定时器0工作方式1，定时器1工作方式2
	TH0 = ( 65536 - 10000 ) / 256;
	TL0 = ( 65536 - 10000 ) % 256;
	TH1 = 0xfd;
	TL1 = 0xfd;
	TR0 = 1;                        // 定时器0开始计时
	TR1 = 1;                        // 定时器1开始计时
	REN = 1;
	SM0 = 0;
	SM1 = 1;
	ES = 1;
	
	S2CON = 0x50;				     //方式1，八位数据，可变波特率
	AUXR1 = 0x00;				     //1T工作方式
 	PCON  = 0x00;					 //不倍增波特率
 	IP2   = 0x00;					 //优先级默认
 	BRT   = 0XFD; 					 //设置波特率9600
 	AUXR  = 0x10;					 //启动波特率发生器
 	EA    = 1; 					     //开总中断
	IE2   = 0x01;					 //开串口2中断
	
	key_config_Ap = 1;
	key_config_Air = 1;	
}

/* mcu发送数据程序 (mcu --> wifi) */
void Usart_SendArrang(uchar *dat,uchar len)
{
	unsigned char i;
	ES = 0;
	for( i = 0; i < len; i++ )
	{
		SBUF = dat[i];
		/*
		if( i >= 2 && dat[i] == 0xff ) {
			dat[++i] = 0x55;            // 对于发送方，如检测到出现0xff的数据内容，需要在0xff后添加0x55
			--i;
		}
		*/
		while( !TI );
		TI = 0;
	} 
	ES = 1;
}

/* 定时器0中断 */
void Timer0_Int() interrupt 1
{
	TH0 = ( 65536 - 10000 ) / 256;
	TL0 = ( 65536 - 10000 ) % 256;  
	num_usart++;
	num_usart_se++;
}

/* 串口中断函数  (wifi --> mcu) */
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
	// 对于接收方，如检测到非包头部分出现0xff,需要把紧跟其后的0x55移除
	if( (lastdate == 0xff ) && ( nowdate == 0x55 ) )
	{
		 usrtlen = usrtlen - 1;
	}
	lastdate = nowdate;
	RI = 0;
}

/* 数据校验程序 */ 
unsigned char DateCheck()
{
	unsigned char i,temp;
	unsigned char CheckSum=0;
	for( i = 0; i < ( usartbuf[3] + 1 ); i++ ) //+1，长度包含校验和，但是不包括长度的2个字节，length=长度+2个字节-校验和占用的一个字节
	{
		CheckSum = CheckSum + usartbuf[i+2];
	}
	temp = usartbuf[3] + 3;					   //提取校验和，长度+4-1为校验和位置，数组从0开始
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
