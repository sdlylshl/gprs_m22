
/********************************************************\
*	�ļ�����  Uart01.C
*	����ʱ�䣺2004��12��7��
*	�����ˣ�  
*	�汾�ţ�  1.00
*	���ܣ�	  ����MSP430���ص㣬�ⲿ��ʵ��MSP430����Ӳ���ڵĹ���
			  ��������ģ�⴮�ڵķ��ͺͽ���
*	�ļ����ԣ������ļ�
*	�޸���ʷ����ÿ��������
\********************************************************/
#include <msp430x14x.h>
#include "Uart01.h"
#include "General.h"
#include "Define_Bit.h"
#include "Do_SRAM.h"
#include "D_Buffer.h"
/********************************************************\
*	��������Send_COM0_Byte
	�������ⲿ�ļ�����
*	���ܣ�  ͨ������0����һ���ֽ�
*	������  Input
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Send_COM0_Byte(unsigned char Input)
{
	IFG1&=~UTXIFG0;
	TXBUF0=Input;
	while((IFG1&UTXIFG0)==0){}
}
/********************************************************\
*	��������Send_COM1_Byte
	�������ⲿ�ļ�����
*	���ܣ�  ͨ������1����һ���ֽ�
*	������  Input
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Send_COM1_Byte(unsigned char Input)
{
	IFG2&=~UTXIFG1;
	TXBUF1=Input;
	while((IFG2&UTXIFG1)==0){}
}
/********************************************************\
*	��������Send_COM0_String
	�������ⲿ�ļ�����
*	���ܣ�  ͨ������0����һ���ַ���
*	������  *Point��Count
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Send_COM0_String(unsigned char *Point,unsigned int Count)
{
	while(Count>0)
	{
		IFG1&=~UTXIFG0;
		TXBUF0=*Point;
		while((IFG1&UTXIFG0)==0){}
		Point++;
		Count--;
	}
}
/********************************************************\
*	��������Send_COM1_String
	�������ⲿ�ļ�����
*	���ܣ�  ͨ������1����һ���ַ���
*	������  *Point��Count
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Send_COM1_String(unsigned char *Point,unsigned int Count)
{
	while(Count>0)
	{
		IFG2&=~UTXIFG1;
		TXBUF1=*Point;
		while((IFG2&UTXIFG1)==0){}
		Point++;
		Count--;
	}
}
/********************************************************\
*	��������Receive_COM0(����0�Ľ����жϴ�������)
	�����򣺱����ļ�ʹ��
*	���ܣ�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
interrupt [UART0RX_VECTOR] void Receive_COM0(void)
{
	IFG1&=~URXIFG0;
	if( gReceive0_GSM_Buffer_Point != gReceive0_GSM_Buffer_End )
	{
		gReceive0_GSM_Buffer[gReceive0_GSM_Buffer_Point] = RXBUF0;
		gReceive0_GSM_Buffer_Point++;
		if( gReceive0_GSM_Buffer_Point >= sizeof(gReceive0_GSM_Buffer) ) gReceive0_GSM_Buffer_Point=0;
		gM22_GSM_ECHO_Timer=0;
		gM22_TCP_ECHO_Timer=0;
	}
}
/********************************************************\
*	��������Send_COM0(����0�ķ����жϴ�������)
	�����򣺱���Ŀ��ʱ��ʹ��
*	���ܣ�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
/*
interrupt [UART0TX_VECTOR] void Send_COM0(void)
{
}
*/
/********************************************************\
*	��������Receive_COM1(����1�Ľ����жϴ�������)
	�����򣺱���Ŀ��ʱ��ʹ��
*	���ܣ�	�������մ���������ʾ�ն˵Ľ�������
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
interrupt [UART1RX_VECTOR] void Receive_COM1(void)
{
	unsigned char Data_Temp=0;
	IFG2&=~URXIFG1;

	Data_Temp=RXBUF1;
	SRAM_Init();
	SRAM_Write(gReceive1_Point,Data_Temp);
	gReceive1_Point++;
	if(gReceive1_Point>SRAM_RECORD_E)	gReceive1_Point=SRAM_RECORD_S;
	gReceive1_Over_Count=0;

}
/********************************************************\
*	��������Send_COM1(����1�ķ����жϴ�������)
	�����򣺱���Ŀ��ʱ��ʹ��
*	���ܣ�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
/*
interrupt [UART1TX_VECTOR] void Send_COM1(void)
{
}
*/