/********************************************************\
*	文件名：  Do_SRAM.C
*	创建时间：2005年5月16日
*	创建人：  
*	版本号：  1.00
*	功能：	  针对GPS方面数据的处理
*	文件属性：公共文件
*	修改历史：（每条详述）

\********************************************************/
#include <msp430x14x.h>
#include "Do_SRAM.h"
#include "Uart01.h"
#define NOP() { _NOP(); _NOP(); _NOP(); _NOP(); _NOP();  }
/********************************************************\
*	函数名：SRAM_Init
	作用域：外部文件调用
*	功能：  SRAM的初始化
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
补充说明：该程序初始化SRAM的接口逻辑，必须在在任何SRAM_Read或者SRAM_Write操作之前
		  被调用一次，可作为复位才操作的一部分
\********************************************************/
void SRAM_Init(void)
{
	P5DIR=0xFF;			//允许端口5作为输出方式
	P5OUT=0xFF;
	P4DIR=0x00;			//允许端口4作为输入方式
	P3DIR|=LE+WR+RD;	//允许三个控制管脚作为输出方式
	P3OUT|=WR+RD;		//设置WR，RD为高电平
	P3OUT&=~LE;			//设置ALE为低电平
}

/********************************************************\
*	函数名：SRAM_Write
	作用域：外部文件调用
*	功能：  SRAM对指定的地址写入一个数据
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
补充说明：读程序外部SRAM
\********************************************************/
void SRAM_Write(unsigned int ByteAddress,unsigned char WriteOneByte)
{
	unsigned char DPH,DPL;
	IE2&=~URXIE1;            //USART1接收中断禁止
	DPH=ByteAddress/0x100;
	DPL=ByteAddress%0x100;
	P5OUT=DPH;			//输出外部高8位地址A15---A8
	P4DIR=0xFF;			//允许A7----A0为输出方式
	P4OUT=DPL;			//输出外部低8位地址A7----A0
	P3OUT|=LE;			//设置ALE为高电平
	NOP();				//延迟一定的时间间隔
	P3OUT&=~LE;			//设置ALE为低电平
	P4OUT=WriteOneByte;	//将数据送到数据总线
	P3OUT&=~WR;			//启动写操作;
	P3OUT|=WR;			//写选通无效;
	P4DIR=0x00;			//允许A7----A0为输入方式
	IE2|=URXIE1;            //USART1接收中断允许
}

/********************************************************\
*	函数名：SRAM_Read
	作用域：外部文件调用
*	功能：  SRAM对指定的地址读出一个数据
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned char SRAM_Read(unsigned int ByteAddress)
{
	unsigned char DPH,DPL;
	unsigned char Data_Temp;
	IE2&=~URXIE1;            //USART1接收中断禁止
	DPH=ByteAddress/0x100;
	DPL=ByteAddress%0x100;
	P5OUT=DPH;		//输出外部高8位地址A15---A8
	P4DIR=0xFF;		//允许A7----A0为输出方式
	P4OUT=DPL;		//输出外部低8位地址A7----A0
	P3OUT|=LE;		//设置ALE为高电平
	NOP();			//延迟一定的时间间隔
	NOP();			//延迟一定的时间间隔
	P3OUT&=~LE;		//设置ALE为低电平
	P4DIR=0x00;		//允许A7----A0为输入方式
	P4OUT=0xFF;
	P3OUT&=~RD;		//发出读选通;
	NOP();			//延迟一定的时间间隔
	Data_Temp=P4IN; //读数据
	NOP();			//延迟一定的时间间隔
	P3OUT|=RD;		//使读选通无效；
	IE2|=URXIE1;            //USART1接收中断允许
	return(Data_Temp);
}




