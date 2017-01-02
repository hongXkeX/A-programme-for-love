#include "Usart_communication.h"
#include "delay.h"

unsigned char q1 = 5;
unsigned char q2 = 5;

/* 处理串口传来的wifi数据 */
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
		sn = usartbuf[5];               // 接收 由发送方给出的 消息序号(sn)
		checksum = 0; 
		switch( usartbuf[4] )           // 判断 wifi 传来的 cmd 类型
		{
			case 0x01:                  // 1.WiFi模组请求设备信息 WiFi模组发送 cmd
				mcu_send_1[5] = sn;     // 改 MCU回复 数据中的 消息序号(sn) （接收方响应命令时需把消息序号返回给发送方）   
				checksum = 0;
				for( i = 2; i < 74; i++ )
				{
					checksum = checksum + mcu_send_1[i];
				}
				checksum = checksum % 256;             // 计算检验和(checksum)
				mcu_send_1[74] = checksum;             // 改 MCU回复 数据中的 检验和(checksum)
 				Usart_SendArrang( mcu_send_1, 75 );    // 1.WiFi模组请求设备信息，设备MCU回复  
				break;

			case 0x07:                  // 2.WiFi模组与设备MCU的心跳  WiFi模组发送cmd              
				mcu_send_2[5] = sn;     // 改 MCU回复 数据中的 消息序号(sn)
				checksum = 0;
				for( i = 2; i < 8; i++ )
				{
					checksum = checksum + mcu_send_2[i];
				}
				checksum = checksum % 256;
				mcu_send_2[8] = checksum;
				Usart_SendArrang( mcu_send_2, 9 );     // 2.WiFi模组与设备MCU的心跳  设备MCU回复
				break;	

			case 0x0a:                  // 3.设备MCU通知WiFi模组进入配置模式 WiFi模组回复cmd
				lcd4 = 0;
				break;

			case 0x0c:                  // 4.设备MCU重置WiFi模组  WiFi模组回复cmd
				lcd4 = 1;
				break;

			case 0x0d:                  // 5.WiFi模组向设备MCU通知WiFi模组工作状态的变化 WiFi模组发送
				lcd4 = 1;
				lcd5 = ~lcd5;
				// 判断wifi模组是否成功连上路由器
				if( usartbuf[9] & 0x10 ) { 
					lcd6 = 0;                             // 连上路由器亮第六个灯
					q1 = 0;
				} else if( ( (usartbuf[9] & 0x10) == 0 )  && q1 ) {
					lcd6 = 1; 
					delayms(400); 
					lcd6 = 0; 
					delayms(400);
				}
				// 判断wifi模组是否成功连上云端            
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
				Usart_SendArrang( mcu_send_5, 9 ); // 5. 设备MCU回复
				
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

			case 0x0f:                   // 6. WiFi模组请求重启MCU WiFi模组发送
				mcu_send_6[5] = sn;
				checksum = 0;
				for( i = 2; i < 8; i++ )
				{
					checksum = checksum + mcu_send_6[i];
				}
				checksum = checksum % 256;
				mcu_send_6[8] = checksum;
				Usart_SendArrang( mcu_send_6, 9 );  // 6.设备MCU回复
				break;	

			case 0x11:                 // 7.非法消息通知 WiFi模组回应MCU对应包序号的数据包非法
				mcu_send_7[5] = sn;
				checksum = 0;
				send_7_error = usartbuf[8];   // 存 error_code
				mcu_send_7[8] = send_7_error;
				for( i = 2; i < 9; i++ )
				{
					checksum = checksum + mcu_send_7[i];
				}
				checksum = checksum % 256;
				mcu_send_7[9] = checksum;
				Usart_SendArrang( mcu_send_7, 10 );       // MCU回应WiFi模组对应包序号的数据包非法
				break;

			case 0x03:
				if( usartbuf[3] = 0x06 && usartbuf[8] == 0x02 )  // 8. WiFi模组读取设备的当前状态 WiFi模组发送
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
					Usart_SendArrang( mcu_send_8, 13 );   // 8.设备MCU回复
					break;
				}
				if( usartbuf[3] = 0x08 && usartbuf[8] == 0x01 ) // 10.WiFi模组控制设备，WiFi模组发送
				{
					Control_Mcu();								// 收到数据后，mcu控制设备函数
					
					mcu_send_10[5] = sn;
					for( i = 2; i < 8; i++ )
					{
						checksum = checksum + mcu_send_10[i];
					}
					checksum = checksum % 256;
					mcu_send_10[8] = checksum;
					Usart_SendArrang( mcu_send_10, 9 );	     // 10.WiFi模组控制设备，设备MCU回复

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
					Usart_SendArrang( mcu_send_9, 13 );	    // 9. mcu主动上报设备状态					
					break;	
				}

			case 0x06:  // 9.设备MCU向WiFi模组主动上报当前状态 WiFi模组回复
				break;

			case 0x16:  // 12.MCU通知WiFi模组进入可绑定模式 WiFi模组回复
				break;	
	
		}
		usrtlen = 0;
		usarrtflag = 0;
		REN = 1;
	}	
}

