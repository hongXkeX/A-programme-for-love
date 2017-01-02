#include <reg52.h>
#include <intrins.h> 

typedef unsigned char  u8;      
typedef signed   char  s8;       
typedef unsigned int   u16;     

#define uchar unsigned char
#define uint unsigned int
#define Data_0_time 4

//----------------------------------------------//
//----------------IO�ڶ�����--------------------//
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
//----------------������--------------------//
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
* ��ͷ�̶�0xffff
* ��ַ 0xaa
* ���ܺ� 0x01
* ���� len = 02
* ֮��len������
* ���һλУ��
*/
u8 mcu_send[8]={0xff,0xff,0xAA,0x01,0x02,0x00,0x00,0x00};

/* ��ʼ������ */
void Usart_Init()
{
	TMOD = 0x20;
	TH0 = ( 65536 - 46080 ) / 256;
	TL0 = ( 65536 - 46080 ) % 256;
	TH1 = 0xfd;
	TL1 = 0xfd;
	TR1 = 1;                        // ��ʱ��1��ʼ��ʱ
	ET0 = 1;
	REN = 0;                        // ֻ������
	SM0 = 0;
	SM1 = 1;
	EA = 1;
	ES = 1;
}

/* mcu�������ݳ��� */
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

void ds_reset()		// ��ʼ��DS18B20
{
	ds = 1;	        // ����׼��
	delayus(5);		// ��������ǰ������ʱ
	ds = 0;			// �������� 480us~960us
	delayus(60);	// ���ͱ���Լ600us ����(TX)һ����λ����
	ds = 1;		    // �ͷ����ߺ�������״̬��RX��
	delayus(5);	    // 18b20 ̽�⵽�ϲ�I/O���ŵ������غ� �ȴ�15~60us
	if(ds == 0)		// 18b20 �ȴ�15~60us�� ����һ��60~240us�͵�ƽ�źŹ��ɵĴ�������
		flag = 1;    
	else
		flag = 0;
	delayus(20);	// �������������60~240us  �˴�Լ200us  
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
		j = ds_read_bit();      // �ӵ�λ��ʼ��
		k = (k>>1) | (j<<7);    // ��λ��������
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
	ds_write_byte(0xcc);	 // ֻ��һֻ�ӻ���������ʱ ����ROM
	ds_write_byte(0xbe);     // ����ROMָ���ֻ�ܽӶ��ݴ���ָ��
	a=ds_read_byte();		 // ����8λ
	b=ds_read_byte();		 // ����8λ
	temp=b;
	temp=temp<<8;
	temp=temp|a;			 // ��ϳ�uint���¶�����
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
	   	//��ʱ������forѭ��		  
	   	if(U8FLAG==1)break;
	   	//�ж�����λ��0����1	 
		// ����ߵ�ƽ�߹�Ԥ��0�ߵ�ƽֵ������λΪ 1
		U8comdata<<=1;
	   	U8comdata|=U8temp;        // 0
	}//rof   
}

	//--------------------------------
	//-----ʪ�ȶ�ȡ�ӳ��� ------------
	//--------------------------------
	//----���±�����Ϊȫ�ֱ���--------
	//----�¶ȸ�8λ== U8T_data_H------
	//----�¶ȵ�8λ== U8T_data_L------
	//----ʪ�ȸ�8λ== U8RH_data_H-----
	//----ʪ�ȵ�8λ== U8RH_data_L-----
	//----У�� 8λ == U8checkdata-----
	//----��������ӳ�������----------
	//---- Delay();, Delay_10us();,COM(); 
	//--------------------------------

void RH(void)
{
	//��������18ms 
    P2_0=0;
	Delay(180);
	P2_0=1;
	//������������������ ������ʱ20us
	Delay_10us();
	Delay_10us();
	Delay_10us();
	Delay_10us();
	//������Ϊ���� �жϴӻ���Ӧ�ź� 
	P2_0=1;
	//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	  
	if(!P2_0)		 //T !	  
	{
		U8FLAG=2;
	    //�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����	 
		while((!P2_0)&&U8FLAG++);
	    U8FLAG=2;
	    //�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
	    while((P2_0)&&U8FLAG++);
	    //���ݽ���״̬		 
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
	    //����У�� 
	    U8temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
	    if(U8temp==U8checkdata_temp)
	    {
			mcu_send[6]=U8RH_data_H_temp;
	   	    U8RH_data_L=U8RH_data_L_temp;
		    //mcu_send[5]=U8T_data_H_temp;  // �¶���ǰ
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
//main()��������:  AT89C51  11.0592MHz 	���ڷ� 
//����ʪ������,������ 9600 
//----------------------------------------------
void main()
{
	init();
	Usart_Init();
	Delay(1);         // ��ʱ100US��12M����)
	
	while(1)
	{   
	    ds_reset();                  // ��ʼ��DS18B20
	    ds_write_byte(0xcc);	     // ����ROM
	    ds_write_byte(0x44);		 // �¶�ת��ָ��
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

