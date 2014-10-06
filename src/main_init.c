/********************************************************\
*	文件名：  Main_Init.C
*	创建时间：2004年12月7日
*	创建人：  
*	版本号：  1.00
*	功能：	  针对程序刚上电阶段，对系统的一些初始化的操作
*	文件属性：公共文件
*	修改历史：（每条详述）
	2004年12月15日
\********************************************************/
#include <msp430x14x.h>
#include "SPI45DB041.h"
#include "Define_Bit.h"
#include "D_Buffer.h"
#include "Other_Define.h"
#include "General.h"
#include "Uart01.h"
#include "Main_Init.h"
#include "Sub_C.h"
#include "M22_AT.h"
#include "Do_SRAM.h"
#include "Msp430_Flash.h"
/*
#include "SPI45DB041.h"
#include "W_Protocol.h"
#include "Handle_Protocol.h"
#include "Uart01.h"
#include "Define_Bit.h"
#include "D_Buffer.h"
#include "Other_Define.h"
#include "General.h"
#include "Main_Init.h"
#include "Sub_C.h"
#include "Check_GSM.h"
#include "Do_Reset.h"
#include "Do_GSM.h"
#include "Do_Handle.h"
#include "Do_GPS.h"
#include "Do_Disp.h"
#include "Do_Other.h"

*/
//#define Debug_GSM_COM1
/********************************************************\
*	函数名：Init_MAIN_Module
	作用域：外部文件调用
*	功能：  这个模块是在系统上电或者系统两个模块的控制电源有重新上电的时候，需要运行
			针对FLASH进行操作的初始化模块。具体细节见FLASH的详细描述
*	参数：  无

*	返回值：针对部分相关的全局变量进行附值操作。

*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Init_MAIN_Module(void)
{
    unsigned int i;
	Clear_Exter_WatchDog();
	gCycle_Send_Status=NOP_DATA;
	//显示MSP430两个信息扇区的数据内容
	Read_SegAB(0);
	Send_COM1_Byte('A');
	Send_COM1_Byte('[');
	for(i=0;i<128;i++)
	{
		Send_COM1_Byte(ASCII(gGeneral_Buffer[i]/0x10));
		Send_COM1_Byte(ASCII(gGeneral_Buffer[i]%0x10));
	}
	Send_COM1_Byte(']');Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	gTimer=0;
	while(gTimer<1500){Clear_Exter_WatchDog();}
	Read_SegAB(1);
	Send_COM1_Byte('B');
	Send_COM1_Byte('[');
	for(i=0;i<128;i++)
	{
		Send_COM1_Byte(ASCII(gGeneral_Buffer[i]/0x10));
		Send_COM1_Byte(ASCII(gGeneral_Buffer[i]%0x10));
	}
	Send_COM1_Byte(']');Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	/********************************************************\
	一，第1个扇区和第2个扇区的数据的处理(这里指的是1，2扇区)
	\********************************************************/
	gTimer=0;
	while(gTimer<1500){Clear_Exter_WatchDog();}
	Load_Info_One();
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	/********************************************************\
	二，
		1，存储拨出电话记录的页
		2，存储通话记录的页
	\********************************************************/
	/********************************************************\
	三，存储系统信息2的两个扇区(这里指的是5，6扇区)
	\********************************************************/
	gTimer=0;
	while(gTimer<1500){Clear_Exter_WatchDog();}
	Load_Info_Two();
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	/********************************************************\
	四，处理区域报警的参数
	\********************************************************/
	gTimer=0;
	while(gTimer<1500){Clear_Exter_WatchDog();}
	Load_Info_Three();
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	/********************************************************\
	五，处理线路偏离报警的参数
	\********************************************************/
	gTimer=0;
	while(gTimer<1500){Clear_Exter_WatchDog();}
	Load_Info_Four();
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	//增加线路偏离的数据
	/********************************************************\
	六，其他变量和缓冲的初始化
	\********************************************************/
	gTimer=0;
	while(gTimer<1500){Clear_Exter_WatchDog();}
	Reset_Variable();
	/********************************************************\
	七，其他方面的初始化
	\********************************************************/
	gOFF_Power_S=0;
    gGeneral_Flag&=~OFF_M22_F_1;				//取消掉电G20的标志位
    gGeneral_Flag&=~OFF_GPS_F_1;				//取消掉电GPS的标志位
    gPhone_Status=READY;gStatus1_Flag&=~PHONE_ONLINE_F_1;
	gRload_Flash_Par_Timer=0;
	gSet_Distance_Info_Count=0;

	SRAM_Init();
	SRAM_Write(PHONE_AT_2+0,NULL_2);
	for(i=PHONE_AT_2+1;i<END_SRAM_2;i++)
	{
		SRAM_Write(i,0);
	}
	//gStatus1_Flag|=ACC_ON_F_1;
	gON_OFF_Temp0 &=~ TCP_LAND_ON_OFF_1;
	gStatus2_Flag |= TCP_ON_OFF_F_1;
	//默认手柄开启
	gStatus1_Flag |= HANDLE_ON_0_F_1;
	gStatus1_Flag &=~ HANDLE_ON_1_F_1;
	//默认显示终端开启
	gStatus1_Flag|=DISPLAY_ON_F_1;
}
