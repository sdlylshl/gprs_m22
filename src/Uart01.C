
/********************************************************\
*	文件名：  Uart01.C
*	创建时间：2004年12月7日
*	创建人：  
*	版本号：  1.00
*	功能：	  根据MSP430的特点，这部分实现MSP430两个硬串口的功能
			  包括三个模拟串口的发送和接收
*	文件属性：公共文件
*	修改历史：（每条详述）
\********************************************************/
#include <msp430x14x.h>
#include "Uart01.h"
#include "General.h"
#include "Define_Bit.h"
#include "Do_SRAM.h"
#include "D_Buffer.h"
/********************************************************\
*	函数名：Send_COM0_Byte
	作用域：外部文件调用
*	功能：  通过串口0发送一个字节
*	参数：  Input
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Send_COM0_Byte(unsigned char Input)
{
	IFG1&=~UTXIFG0;
	TXBUF0=Input;
	while((IFG1&UTXIFG0)==0){}
}
/********************************************************\
*	函数名：Send_COM1_Byte
	作用域：外部文件调用
*	功能：  通过串口1发送一个字节
*	参数：  Input
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Send_COM1_Byte(unsigned char Input)
{
	IFG2&=~UTXIFG1;
	TXBUF1=Input;
	while((IFG2&UTXIFG1)==0){}
}
/********************************************************\
*	函数名：Send_COM0_String
	作用域：外部文件调用
*	功能：  通过串口0发送一个字符串
*	参数：  *Point，Count
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
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
*	函数名：Send_COM1_String
	作用域：外部文件调用
*	功能：  通过串口1发送一个字符串
*	参数：  *Point，Count
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
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
*	函数名：Receive_COM0(串口0的接收中断处理程序)
	作用域：本地文件使用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
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
*	函数名：Send_COM0(串口0的发送中断处理程序)
	作用域：本项目暂时不使用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
interrupt [UART0TX_VECTOR] void Send_COM0(void)
{
}
*/
/********************************************************\
*	函数名：Receive_COM1(串口1的接收中断处理程序)
	作用域：本项目暂时不使用
*	功能：	用来接收处理来自显示终端的接收数据
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
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
*	函数名：Send_COM1(串口1的发送中断处理程序)
	作用域：本项目暂时不使用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
interrupt [UART1TX_VECTOR] void Send_COM1(void)
{
}
*/