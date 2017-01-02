#include "Smart_Usart.h"
#include "Control_Mcu.h"

/* 改变设备状态 */
void Control_Mcu()
{
	switch( usartbuf[9] )
	{
		case 0x01:                          // 标志最后一位数据值有效
			if( usartbuf[10] == 0x01 )      // 数据值有效灯亮
			{
				lcd0 = 0;
			}
			else if( usartbuf[10] == 0 )    // 数据值无效灯灭
			{
				lcd0 = 1; 
			}
		break;
			
		case 0x02:                         
			if( usartbuf[10] == 0x02 )    
			{
				lcd1 = 0;
			}
			else if( usartbuf[10] == 0 ) 
			{
				lcd1 = 1;
			}
		break;
			
			
	}
}
