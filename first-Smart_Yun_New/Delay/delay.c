#include "delay.h"

/* 延时函数 */
void Delay_ms(unsigned char z)
{
	unsigned char x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}

/* 延时函数 */
void delayms(unsigned char z)
{
	unsigned char x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}
