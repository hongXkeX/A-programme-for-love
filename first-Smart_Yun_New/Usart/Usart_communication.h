#ifndef __USART_COMMUNICATION_H__
#define __USART_COMMUNICATION_H__

#include "Smart_Usart.h"
#include "Control_Mcu.h"

extern uchar usartbuf[50];

extern uchar mcu_send_1[75];
extern uchar mcu_send_2[9];
extern uchar mcu_send_3[10];
extern uchar mcu_send_4[9];
extern uchar mcu_send_5[9];
extern uchar mcu_send_6[9];
extern uchar mcu_send_7[10];
extern uchar mcu_send_8[13];
extern uchar mcu_send_9[13];
extern uchar mcu_send_10[9];
extern uchar mcu_send_12[9];

extern unsigned char DateCheck();
void Usart_Communication();

#endif
