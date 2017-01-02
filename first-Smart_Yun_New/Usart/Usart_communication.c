#include "Usart_communication.h"
#include "delay.h"

unsigned char q1 = 5;
unsigned char q2 = 5;

/* �����ڴ�����wifi���� */
void Usart_Communication()
{
	uchar i;
	if( num_usart >= 10 )               // 100ms
	{
		temper = usart2buf[5];
		humper = usart2buf[6];
		ET0 = 0;                        // switch off time0
		usrt2len = 0;
		num_usart = 0;      
		// usarrtflag = 1;
		usarrtflag = DateCheck();
		REN = 0;                        // switch off usart
	}
	if( usarrtflag )
	{
		sn = usartbuf[5];               // ���� �ɷ��ͷ������� ��Ϣ���(sn)
		checksum = 0; 
		switch( usartbuf[4] )           // �ж� wifi ������ cmd ����
		{
			case 0x01:                  // 1.WiFiģ�������豸��Ϣ WiFiģ�鷢�� cmd
				mcu_send_1[5] = sn;     // �� MCU�ظ� �����е� ��Ϣ���(sn) �����շ���Ӧ����ʱ�����Ϣ��ŷ��ظ����ͷ���   
				checksum = 0;
				for( i = 2; i < 74; i++ )
				{
					checksum = checksum + mcu_send_1[i];
				}
				checksum = checksum % 256;             // ��������(checksum)
				mcu_send_1[74] = checksum;             // �� MCU�ظ� �����е� �����(checksum)
 				Usart_SendArrang( mcu_send_1, 75 );    // 1.WiFiģ�������豸��Ϣ���豸MCU�ظ�  
				break;

			case 0x07:                  // 2.WiFiģ�����豸MCU������  WiFiģ�鷢��cmd              
				mcu_send_2[5] = sn;     // �� MCU�ظ� �����е� ��Ϣ���(sn)
				checksum = 0;
				for( i = 2; i < 8; i++ )
				{
					checksum = checksum + mcu_send_2[i];
				}
				checksum = checksum % 256;
				mcu_send_2[8] = checksum;
				Usart_SendArrang( mcu_send_2, 9 );     // 2.WiFiģ�����豸MCU������  �豸MCU�ظ�
				break;	

			case 0x0a:                  // 3.�豸MCU֪ͨWiFiģ���������ģʽ WiFiģ��ظ�cmd
				lcd4 = 0;
				break;

			case 0x0c:                  // 4.�豸MCU����WiFiģ��  WiFiģ��ظ�cmd
				lcd4 = 1;
				break;

			case 0x0d:                  // 5.WiFiģ�����豸MCU֪ͨWiFiģ�鹤��״̬�ı仯 WiFiģ�鷢��
				lcd4 = 1;
				lcd5 = ~lcd5;
				// �ж�wifiģ���Ƿ�ɹ�����·����
				if( usartbuf[9] & 0x10 ) { 
					lcd6 = 0;                             // ����·��������������
					q1 = 0;
				} else if( ( (usartbuf[9] & 0x10) == 0 )  && q1 ) {
					lcd6 = 1; 
					delayms(400); 
					lcd6 = 0; 
					delayms(400);
				}
				// �ж�wifiģ���Ƿ�ɹ������ƶ�            
				if( usartbuf[9] & 0x20 ) {
					lcd7 = 0;   
					q2 = 0;
				} else if ( ( (usartbuf[9] & 0x20) == 0 )  && q2 )  {
					lcd7 = 1; 
					delayms(400); 
					lcd7 = 0; 
					delayms(400);
				}
				
				mcu_send_5[5] = sn;
			
				for( i = 2; i < 8; i++ )
				{
					checksum = checksum + mcu_send_5[i];
				}
				checksum = checksum % 256;
				mcu_send_5[8] = checksum;
				Usart_SendArrang( mcu_send_5, 9 ); // 5. �豸MCU�ظ�
				
				checksum = 0;
				mcu_send_9[5] = sn;
				send_9_dev = ~P1;  // ( ((~P1) & 0x01) | ( (P1) & 0x02) )
				
				mcu_send_9[9] = send_9_dev;
				mcu_send_9[10] = temper;
				mcu_send_9[11] = humper;
				
				for( i = 2; i < 12; i++ )
				{
					checksum = checksum + mcu_send_9[i];
				}
				checksum = checksum % 256;
				mcu_send_9[12] = checksum;
				Usart_SendArrang( mcu_send_9, 13 );
					
				break;

			case 0x0f:                   // 6. WiFiģ����������MCU WiFiģ�鷢��
				mcu_send_6[5] = sn;
				checksum = 0;
				for( i = 2; i < 8; i++ )
				{
					checksum = checksum + mcu_send_6[i];
				}
				checksum = checksum % 256;
				mcu_send_6[8] = checksum;
				Usart_SendArrang( mcu_send_6, 9 );  // 6.�豸MCU�ظ�
				break;	

			case 0x11:                 // 7.�Ƿ���Ϣ֪ͨ WiFiģ���ӦMCU��Ӧ����ŵ����ݰ��Ƿ�
				mcu_send_7[5] = sn;
				checksum = 0;
				send_7_error = usartbuf[8];   // �� error_code
				mcu_send_7[8] = send_7_error;
				for( i = 2; i < 9; i++ )
				{
					checksum = checksum + mcu_send_7[i];
				}
				checksum = checksum % 256;
				mcu_send_7[9] = checksum;
				Usart_SendArrang( mcu_send_7, 10 );       // MCU��ӦWiFiģ���Ӧ����ŵ����ݰ��Ƿ�
				break;

			case 0x03:
				if( usartbuf[3] = 0x06 && usartbuf[8] == 0x02 )  // 8. WiFiģ���ȡ�豸�ĵ�ǰ״̬ WiFiģ�鷢��
				{
					mcu_send_8[5] = sn;
					checksum = 0;
					send_8_dev = ~P1;  // ( ((~P1) & 0x01) | ( (~P0) & 0x02) )
			
					mcu_send_8[9] = send_8_dev;
					mcu_send_8[10] = temper;
					mcu_send_8[11] = humper;    // dev_status(1B)

					for( i = 2; i < 12; i++ )
					{
						checksum = checksum + mcu_send_8[i];
					}
					checksum = checksum % 256;
					mcu_send_8[12] = checksum;
					Usart_SendArrang( mcu_send_8, 13 );   // 8.�豸MCU�ظ�
					break;
				}
				if( usartbuf[3] = 0x08 && usartbuf[8] == 0x01 ) // 10.WiFiģ������豸��WiFiģ�鷢��
				{
					Control_Mcu();								// �յ����ݺ�mcu�����豸����
					
					mcu_send_10[5] = sn;
					for( i = 2; i < 8; i++ )
					{
						checksum = checksum + mcu_send_10[i];
					}
					checksum = checksum % 256;
					mcu_send_10[8] = checksum;
					Usart_SendArrang( mcu_send_10, 9 );	     // 10.WiFiģ������豸���豸MCU�ظ�

					mcu_send_9[5] = sn;
					send_9_dev = ~P1 ; // ( ((~P1) & 0x01) | ( (~P0) & 0x02) )
					checksum = 0;
					
					mcu_send_9[9] = send_9_dev;
					mcu_send_9[10] = temper;
					mcu_send_9[11] = humper;
					
					for( i = 2; i < 12; i++ )
					{
						checksum = checksum + mcu_send_9[i];
					}
					checksum = checksum % 256;
					mcu_send_9[12] = checksum;
					Usart_SendArrang( mcu_send_9, 13 );	    // 9. mcu�����ϱ��豸״̬					
					break;	
				}

			case 0x06:  // 9.�豸MCU��WiFiģ�������ϱ���ǰ״̬ WiFiģ��ظ�
				break;

			case 0x16:  // 12.MCU֪ͨWiFiģ�����ɰ�ģʽ WiFiģ��ظ�
				break;	
	
		}
		usrtlen = 0;
		usarrtflag = 0;
		REN = 1;
	}	
}

