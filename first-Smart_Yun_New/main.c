#include "stc12c5a60s2.h"
#include "Smart_Usart.h"
#include "Usart_communication.h"
#include "Key_Config.h"
#include "delay.h"



void main()
{
	Usart_Init();             // ��ʼ������
	while(1)
	{
		Key_Usart_Config();   // ��ⰴ�������豸����
		Usart_Communication();// ����������
		
	}
}
