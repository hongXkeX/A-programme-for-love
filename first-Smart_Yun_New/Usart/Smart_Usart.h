#ifndef __SMART_USART__
#define __SMART_USART__

#include "stc12c5a60s2.h"
	
typedef unsigned char uchar; 
typedef unsigned int uint; 

sbit key_config_Ap = P3^4;  // s2 °´¼ü 
sbit key_config_Air = P3^5;   
sbit key_rt = P3^6;

sbit lcd0=P1^0;
sbit lcd1=P1^1;
sbit lcd2=P1^2;
sbit lcd3=P1^3;
sbit lcd4=P1^4;
sbit lcd5=P1^5;
sbit lcd6=P1^6;
sbit lcd7=P1^7;

sbit p00 = P0^0; 
sbit p01 = P0^1;
sbit p02 = P0^2;
sbit p03 = P0^3;
sbit p04 = P0^4;
sbit p05 = P0^5;
sbit p06 = P0^6;
sbit p07 = P0^7;

sbit p20 = P2^0;
sbit p21 = P2^1;
sbit p22 = P2^2;   
sbit p23 = P2^3;
sbit p24 = P2^4;
sbit p25 = P2^5;
sbit p26 = P2^6;
sbit p27 = P2^7;

extern uchar usartbuf[50];
extern uchar usart2buf[8];

extern uchar sn,checksum,usrtlen,usrt2len,usarrtflag,num_usart;
extern uchar send_3_config,send_7_error,send_8_dev,send_9_dev;
extern uchar data temper,humper;
extern uchar num_usart_se;

void Usart_Init();
void Usart_SendArrang(uchar *dat,uchar len);

#endif
