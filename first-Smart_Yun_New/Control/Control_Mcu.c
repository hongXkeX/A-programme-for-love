#include "Smart_Usart.h"
#include "Control_Mcu.h"

/* �ı��豸״̬ */
void Control_Mcu()
{
	switch( usartbuf[9] )
	{
		case 0x01:                          // ��־���һλ����ֵ��Ч
			if( usartbuf[10] == 0x01 )      // ����ֵ��Ч����
			{
				lcd0 = 0;
			}
			else if( usartbuf[10] == 0 )    // ����ֵ��Ч����
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
