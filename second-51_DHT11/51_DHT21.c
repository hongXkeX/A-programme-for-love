#include <reg52.h>
#include <intrins.h> 

typedef unsigned char  u8;      
typedef signed   char  s8;       
typedef unsigned int   u16;     

#define uchar unsigned char
#define uint unsigned int
#define Data_0_time 4

//----------------------------------------------//
//----------------IO口定义区--------------------//
//----------------------------------------------//
sbit P2_0 = P2^0;
sbit ds = P2^2;
sbit dula = P2^6;
sbit wela = P2^7;
sbit lcden = P3^4;
sbit lcdrs = P3^5;
sbit lcdrw = P3^6;

uchar t0_num = 0;
bit flag = 0;
uint temp = 0;

uchar num;
uchar table[] = "Temperature:";
uchar table1[] = "Humidity:";
						 
//----------------------------------------------//
//----------------定义区--------------------//
//----------------------------------------------//
u8  U8FLAG;
u8  U8count,U8temp;
u8  U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
u8  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
u8  U8comdata;  
u8  indata[5];
u8  count, count_r=0;
u8  str[5]={ 0 };

u8 usartbuf[7]={ 0 };
u8 usrtlen = 0;
u8 checksum;
u8 num_usart;
u8 usarrtflag;

/*
* 包头固定0xffff
* 地址 0xaa
* 功能号 0x01
* 长度 len = 02
* 之后len个数据
* 最后一位校验
*/
u8 mcu_send[8]={0xff,0xff,0xAA,0x01,0x02,0x00,0x00,0x00};

/* 初始化函数 */
void Usart_Init()
{
	TMOD = 0x20;
	TH0 = ( 65536 - 46080 ) / 256;
	TL0 = ( 65536 - 46080 ) % 256;
	TH1 = 0xfd;
	TL1 = 0xfd;
	TR1 = 1;                        // 定时器1开始计时
	ET0 = 1;
	REN = 0;                        // 只发不收
	SM0 = 0;
	SM1 = 1;
	EA = 1;
	ES = 1;
}

/* mcu发送数据程序 */
void Usart_SendArrang( u8 *dat, u8 len)
{
	unsigned char i;
	ES = 0;
	for( i = 0; i < len; i++ )
	{
		SBUF = dat[i];
		while( !TI );
		TI = 0;
	} 
	ES = 1;
}

void Delay(u16 j)
{      
	u8 i;
	for(;j>0;j--) { 	
		for(i=0;i<27;i++);
	}
}
     
void Delay_10us(void)
{
	u8 i;
    i--;
    i--;
    i--;
    i--;
    i--;
    i--;
}

void delayus(uint t)
{
	while(t--);
}

void delayms(uint x)
{
	uint i,j;
	for(i=x; i>0; i--)
		for(j=111; j>0; j--);
}  

void ds_reset()		// 初始化DS18B20
{
	ds = 1;	        // 拉低准备
	delayus(5);		// 拉低总线前短暂延时
	ds = 0;			// 拉低总线 480us~960us
	delayus(60);	// 拉低保持约600us 发出(TX)一个复位脉冲
	ds = 1;		    // 释放总线后进入接收状态（RX）
	delayus(5);	    // 18b20 探测到上步I/O引脚的上升沿后 等待15~60us
	if(ds == 0)		// 18b20 等待15~60us后 发出一个60~240us低电平信号构成的存在脉冲
		flag = 1;    
	else
		flag = 0;
	delayus(20);	// 存在脉冲需持续60~240us  此处约200us  
}

bit ds_read_bit()	 
{
	bit dat;
	ds = 0;
	_nop_();
	_nop_();
	ds = 1;
	_nop_();
	dat = ds;
	delayus(10);
	return dat;
}

uchar ds_read_byte()
{
	uchar i,j,k;
	for(i=0; i<8; i++)
	{
		j = ds_read_bit();      // 从低位开始读
		k = (k>>1) | (j<<7);    // 移位整理数据
	}
	return k;
}

void ds_write_byte(uchar dat)
{
	uchar i;
	for(i=0; i<8; i++)
	{
		ds = 0;
		_nop_();
		ds = dat & 0x01;
		delayus(6);
		ds = 1;
		dat = dat >> 1;
	}
	delayus(6);
}

void read_temperature()
{
	uchar a,b;
	ds_reset();
	ds_write_byte(0xcc);	 // 只有一只从机在总线上时 忽略ROM
	ds_write_byte(0xbe);     // 忽略ROM指令后只能接读暂存器指令
	a=ds_read_byte();		 // 读低8位
	b=ds_read_byte();		 // 读高8位
	temp=b;
	temp=temp<<8;
	temp=temp|a;			 // 组合成uint型温度数据
	temp=temp*0.0625*10+0.5;
	mcu_send[5] = temp / 10;
}

void COM(void)
{
	u8 i;
    for(i=0;i<8;i++)	   
	{
		U8FLAG=2;	
	   	while((!P2_0)&&U8FLAG++);
		Delay_10us();
		Delay_10us();
		Delay_10us();
	    U8temp=0;
	    if(P2_0) U8temp=1;
		U8FLAG=2;
		while((P2_0)&&U8FLAG++);
	   	//超时则跳出for循环		  
	   	if(U8FLAG==1)break;
	   	//判断数据位是0还是1	 
		// 如果高电平高过预定0高电平值则数据位为 1
		U8comdata<<=1;
	   	U8comdata|=U8temp;        // 0
	}//rof   
}

	//--------------------------------
	//-----湿度读取子程序 ------------
	//--------------------------------
	//----以下变量均为全局变量--------
	//----温度高8位== U8T_data_H------
	//----温度低8位== U8T_data_L------
	//----湿度高8位== U8RH_data_H-----
	//----湿度低8位== U8RH_data_L-----
	//----校验 8位 == U8checkdata-----
	//----调用相关子程序如下----------
	//---- Delay();, Delay_10us();,COM(); 
	//--------------------------------

void RH(void)
{
	//主机拉低18ms 
    P2_0=0;
	Delay(180);
	P2_0=1;
	//总线由上拉电阻拉高 主机延时20us
	Delay_10us();
	Delay_10us();
	Delay_10us();
	Delay_10us();
	//主机设为输入 判断从机响应信号 
	P2_0=1;
	//判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  
	if(!P2_0)		 //T !	  
	{
		U8FLAG=2;
	    //判断从机是否发出 80us 的低电平响应信号是否结束	 
		while((!P2_0)&&U8FLAG++);
	    U8FLAG=2;
	    //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
	    while((P2_0)&&U8FLAG++);
	    //数据接收状态		 
	    COM();
	    U8RH_data_H_temp=U8comdata;
	    COM();
	    U8RH_data_L_temp=U8comdata;
	    COM();
	    U8T_data_H_temp=U8comdata;
	    COM();
	    U8T_data_L_temp=U8comdata;
	    COM();
	    U8checkdata_temp=U8comdata;
	    P2_0=1;
	    //数据校验 
	    U8temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
	    if(U8temp==U8checkdata_temp)
	    {
			mcu_send[6]=U8RH_data_H_temp;
	   	    U8RH_data_L=U8RH_data_L_temp;
		    //mcu_send[5]=U8T_data_H_temp;  // 温度在前
	   	    U8T_data_L=U8T_data_L_temp;
	   	    U8checkdata=U8checkdata_temp;
	    }
	}
}
	
void Usart_Communication()
{
	u8 i;
	checksum = 0;              
	for( i = 2; i < 7; i++ ) {
		checksum += mcu_send[i];
	} 
	checksum = checksum % 256;
	mcu_send[7] = checksum;
	Usart_SendArrang( mcu_send, 8 );
}

void write_com(uchar com)
{
	lcdrs=0;
	P0=com;
	delayms(5);
	lcden=1;
	delayms(5);
	lcden=0;	
}

void write_dat(uchar dat)
{
	lcdrs=1;
	P0=dat;
	delayms(5);
	lcden=1;
	delayms(5);
	lcden=0;
}

void init()
{
	dula=0;
	wela=0;
	lcden=0;
	lcdrw=0;
	write_com(0x38);
	write_com(0x0c);
	write_com(0x06);
	write_com(0x01);
	
	write_com(0x80);
	for(num=0;num<12;num++){
		write_dat(table[num]);
		delayms(5);
	}
	write_com(0x80+0x40);
	for(num=0;num<9;num++){
		write_dat(table1[num]);
		delayms(5);
	}
}


//----------------------------------------------
//main()功能描述:  AT89C51  11.0592MHz 	串口发 
//送温湿度数据,波特率 9600 
//----------------------------------------------
void main()
{
	init();
	Usart_Init();
	Delay(1);         // 延时100US（12M晶振)
	
	while(1)
	{   
	    ds_reset();                  // 初始化DS18B20
	    ds_write_byte(0xcc);	     // 忽略ROM
	    ds_write_byte(0x44);		 // 温度转换指令
		read_temperature();          
	    RH();
		
		write_com(0x80 + 12);
		delayms(3);
		write_dat(temp / 10 / 10 + '0');
		delayms(3);
		write_dat(temp / 10 % 10 + '0');
		delayms(3);
		write_dat(0xdf);
		delayms(3);
		write_dat(0x43);
		
		
		write_com(0x80 + 0x40 + 9);
		delayms(3);
		write_dat(mcu_send[6]/10 + '0');
		delayms(3);
		write_dat(mcu_send[6]%10 + '0');
		
		Usart_Communication();
		delayms(2000);
	}
}

