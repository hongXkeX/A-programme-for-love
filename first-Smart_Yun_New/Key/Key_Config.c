#include "Smart_Usart.h"
#include "delay.h"
#include "Key_Config.h"
#include "Usart_communication.h"

sbit beep = P2^3;

void di()
{
	beep = 0;
	delayms(300);
	beep = 1;
}

/* 按键配置设备入网 */
void Key_Usart_Config()
{
	if( key_config_Ap == 0 )				// SoftAp配置模式
	{
		Delay_ms(20);
		if( key_config_Ap == 0 ) 
		{	
			di();
			REN = 0;
			Usart_SendArrang( mcu_send_3_1, 10 );
			while( key_config_Ap == 0 );
			REN=1;
		}	
	}

	if( key_config_Air == 0 )		      // AirLink配置模式
	{
		Delay_ms(20);
		if( key_config_Air == 0 )
		{	
			di();
			REN = 0;
			Usart_SendArrang( mcu_send_3_2, 10 );
			while( key_config_Air == 0 );
			REN=1;	
		}	
	}	
	
	if( key_rt == 0 )		     
	{
		Delay_ms(20);
		if( key_rt == 0 )
		{	
			di();
			REN = 0;
			Usart_SendArrang( mcu_send_4, 9 );
			while( key_rt == 0 );
			REN=1;	
		}	
	}
}
