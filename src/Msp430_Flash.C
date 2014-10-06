/********************************************************\
*	文件名：  Msp430_Flash.C
*	创建时间：2005年10月16日
*	创建人：  
*	版本号：  1.00
*	功能：
*	文件属性：特殊文件
*	修改历史：（每条详述）
\********************************************************/
#include <msp430x14x.h>
#include "Msp430_Flash.h"
#include "General.h"
/*
#include "TA_Uart.h"
#include "SPI45DB041.h"
#include "W_Protocol.h"
#include "Handle_Protocol.h"
#include "Uart01.h"
#include "M22_AT.h"
#include "Define_Bit.h"
#include "D_Buffer.h"
#include "Other_Define.h"

#include "Main_Init.h"
#include "Sub_C.h"
#include "Check_GSM.h"
#include "Do_Reset.h"
#include "Do_M22.h"
#include "Do_Handle.h"
#include "Do_GPS.h"
#include "Do_Disp.h"
#include "Do_Other.h"
#include "Do_SRAM.h"
#include "Do_Record.h"
#include "Record_Protocol.h"
*/
/********************************************************\
*	函数名：Init_OperFlash()
	作用域：外部文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
\********************************************************/
void Init_OperFlash(void)
{
	WDTCTL=WDTPW+WDTHOLD;   //关闭看门狗WDT  ,WDTPW看门狗的口令
	FCTL2 =	FWKEY + FSSEL1 + FN0;
}
/********************************************************\
*	函数名：Write_SegAB()
	作用域：外部文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
\********************************************************/
void Write_SegAB(unsigned char Address_Flag)
{
	char *Flash_ptr;
	unsigned int i;
	if(Address_Flag==0)
	{
		Flash_ptr = (char *)0x1080;	//初始化指针
	}
	else if(Address_Flag==1)
	{
		Flash_ptr = (char *)0x1000;	//初始化指针
	}
	else
	{
		return;
	}
	_DINT();							//关闭其他所有中断
	FCTL1 = FWKEY + ERASE;				//使能擦除操作
	FCTL3 = FWKEY;						//FLASH解锁
	*Flash_ptr = 0;						//空写，启动擦除
	FCTL1 = FWKEY + WRT;				//允许写操作
	for(i=0;i<128;i++)
	{
		*Flash_ptr++=gGeneral_Buffer[i];
	}
	FCTL1 = FWKEY;
	FCTL3 = FWKEY+LOCK;					//写完了，锁定
	_EINT();							//开启其他所有中断
}

/********************************************************\
*	函数名：Erase_SegAB()
	作用域：外部文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
\********************************************************/
void Read_SegAB(unsigned char Address_Flag)
{
	unsigned char *Flash_ptr;
	unsigned int i;
	if(Address_Flag==0)
	{
		Flash_ptr = (unsigned char *)0x1080;	//初始化指针
	}
	else if(Address_Flag==1)
	{
		Flash_ptr = (unsigned char *)0x1000;	//初始化指针
	}
	else
	{
		return;
	}
	for(i=0;i<128;i++)
	{
		gGeneral_Buffer[i]=*Flash_ptr++;
	}
}