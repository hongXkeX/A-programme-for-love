#include "delay.h"

/* ��ʱ���� */
void Delay_ms(unsigned char z)
{
	unsigned char x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}

/* ��ʱ���� */
void delayms(unsigned char z)
{
	unsigned char x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}
