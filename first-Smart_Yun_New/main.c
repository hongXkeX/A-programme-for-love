#include "stc12c5a60s2.h"
#include "Smart_Usart.h"
#include "Usart_communication.h"
#include "Key_Config.h"
#include "delay.h"



void main()
{
	Usart_Init();             // 初始化函数
	while(1)
	{
		Key_Usart_Config();   // 检测按键配置设备入网
		Usart_Communication();// 处理串口数据
		
	}
}
