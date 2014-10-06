/********************************************************\
*	文件名：  Do_Handle.h
*	创建时间：2004年10月20日
*	创建人：  
*	版本号：  1.00
*	功能：
*
*	修改历史：（每条详述）

\********************************************************/
#include <msp430x14x.h>
#include "Handle_Protocol.h"
#include "SPI45DB041.h"
#include "TA_Uart.h"
#include "General.h"
#include "D_Buffer.h"
#include "Other_Define.h"
#include "Define_Bit.h"
#include "Uart01.h"
#include "Sub_C.h"
#include "M22_AT.h"
#include "Do_Handle.h"
#include "W_Protocol.h"
#include "Disp_Protocol.h"
#include "Do_SRAM.h"
#include "Msp430_Flash.h"

/*
#include "Check_GSM.h"
#include "Do_Reset.h"
#include "Main_Init.h"
#include "Do_GSM.h"
*/
//#define Debug_GSM_COM1
unsigned char Check_Handle_Command(void);
void Handle_Dailing(unsigned char nPoint_Temp);
void Handle_Answer(void);
void Write_Dail_Record(unsigned char nPhone_Length);
void Read_Dail_Record(unsigned char nItem);
void Write_Talk_Record(unsigned char nPhone_Length);
void Read_Talk_Record(unsigned char nItem);
void Handle_ReadOnePage_Data(unsigned char Page_H,unsigned char Page_L);
//void Handle_EraseOnePage_Data(unsigned char Page_H,unsigned char Page_L);
void Move_Right_Item(unsigned char Start_Item,unsigned char End_Item,unsigned char Move_Count);
//void Handle_Set_NumPhone(void);
//void Handle_Set_TCP_Par(void);
//unsigned char  Check_All_Flash(void);
unsigned int  Check_Allow_Phone(unsigned char Length_Phone);
void Read_Phone_Note_Record(unsigned char nItem);
void Handle_Set_Par(unsigned char nLength);
void Handle_Set_TcpIp(void);
void Handle_Set_Time(void);
void Handle_Ask_Par(void);
void Handle_Set_SMS(void);
void Handle_Set_Phone(unsigned char nLength);
void Handle_Set_Dis(void);
void Handle_Test_Sram(void);
/********************************************************\
*	函数名：Do_Handle_Module
	作用域：外部文件调用
*	功能：	与手柄通讯模块。
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	补充说明：
	收到的每一个手柄数据存储在gGeneral_Buffer[]中。
	由Check_Handle_Command()子程序处理接收在gGeneral_Buffer中的数据
	根据信令的不一样，需要分别处理，需要通过有G20的AT操作的指令则直接
	存储在buffer1中的0x5D----0x7A 区域共30个字节。

	手柄与电话有关的AT指令包括如下几类
	这里只存储AT指令的类型有如下几类：
	1，拨号，2，挂机，3，接听，4，DMTF，5，音量调节，6，CSCA的设置
	根据上面的情形，以30个字节足够存储的

	还有一类的指令则是需要直接处理的，比如说电话记录的查询，密码设置等等

	0x5D----0x7A 区域的分配如下：
	0x5D--  存储后面数据是否处理完的标志，为1则表示后面的数据未处理，为0则表示
			已经处理完毕，可以在填写下个指令。
	0x5E--	存储相关的AT指令的类型。如手柄拨号，挂机等等
	0x5F--	存储后面数据的长度，如需要拨号的电话号码等。如果后面无任何数据，则写0。
\********************************************************/
void Do_Handle_Module(void)
{
	unsigned int i=0,k=0;
	unsigned char nHandle_Piont_Temp=0;
	//指令的重发机制，目前只针对主控发送给手柄的挂机指令和接通指令
	if(   (gSend_Handle_Timer>2)
		&&((gSys_Handle==MAIN_HANG_UP)||(gSys_Handle==MAIN_TALKING))  )
	{
		gSend_Handle_Timer=0;
		Send_COM3_Load(4,gSys_Handle,0);
		gRe_Send_Count++;
		if(gRe_Send_Count>=5)
		{
			gRe_Send_Count=0;
			gSys_Handle=0;
		}
	}
	if(gDTMF_SAVE[0]>0)
	{
		SRAM_Init();
		if(SRAM_Read(PHONE_AT_2)==NULL_2)
		{
			OperateSPIEnd();
			k=gDTMF_SAVE[1];
			gDTMF_SAVE[0]--;
			for(i=0;i<gDTMF_SAVE[0];i++)
			{
				gDTMF_SAVE[i+1]=gDTMF_SAVE[i+2];
			}
			SRAM_Init();
			SRAM_Write(PHONE_AT_2+0,VAILD_2);
			SRAM_Write(PHONE_AT_2+1,H_DTMF);
			SRAM_Write(PHONE_AT_2+2,1);
			SRAM_Write(PHONE_AT_2+3,k);
		}
		OperateSPIEnd();
	}
	if( (gHandle_Receive_Over>40)&&(gHandle_Buffer_Point>0) )
	{
		#ifdef Debug_GSM_COM1
		Send_COM1_Byte(ASCII(gHandle_Buffer_Point/10));
		Send_COM1_Byte(ASCII(gHandle_Buffer_Point%10));
		Send_COM1_Byte('[');
		Send_COM1_String(gHandle_Buffer,gHandle_Buffer_Point);
		Send_COM1_Byte(']');
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		#endif
		//作为判断手柄是否被摘除掉或者手柄出现程序异常的判
		//接收完数据，则开始处理数据
		k=Check_Handle_Command();
		nHandle_Piont_Temp=gHandle_Buffer_Point;
		gHandle_Buffer_Point=0;
		switch(k)
		{
			/********************************************************\
			0,手柄的指令ECHO（手柄命令）
			\********************************************************/
			case	HANDLE_ECHO:
			{
				gHandle_OnLine_Timer=0;
				gStatus1_Flag &=~ HANDLE_ON_0_F_1;
				gStatus1_Flag |= HANDLE_ON_1_F_1;
				//判断返回的是那一个指令的ECHO
				if( (gSys_Handle==MAIN_HANG_UP)&&(gHandle_Buffer[3]==MAIN_HANG_UP) )
				{
					gRe_Send_Count=0;
					gSys_Handle=0;
				}
				else if( (gSys_Handle==MAIN_TALKING)&&(gHandle_Buffer[3]==MAIN_TALKING) )
				{
					gRe_Send_Count=0;
					gSys_Handle=0;
				}
				break;
			}
			/********************************************************\
			1,手柄拨号（手柄命令）
			\********************************************************/
			case	HANDLE_DIAL:
			{
				gCommon_Flag&=~CHECK_RINGPHONE_F_1;
				gHandle_OnLine_Timer=0;
				gStatus1_Flag &=~ HANDLE_ON_0_F_1;
				gStatus1_Flag |= HANDLE_ON_1_F_1;
				k=SRAM_Read(PHONE_AT_2);
				i=SRAM_Read(PHONE_AT_2+1);
				if( (i==H_HUNGUP)&&(k==VAILD_2) )
				{
					Send_COM3_Load(5,MAIN_RETURN_PHONE_ECHO,COMMAND_ERROR);
				}
				else
				{
					Handle_Dailing(nHandle_Piont_Temp);
				}
				break;
			}
			/********************************************************\
			2,手柄接听（手柄命令）
			\********************************************************/
			case 	HANDLE_ANSWER:
			{
				gCommon_Flag&=~CHECK_RINGPHONE_F_1;
				gHandle_OnLine_Timer=0;
				gStatus1_Flag &=~ HANDLE_ON_0_F_1;
				gStatus1_Flag |= HANDLE_ON_1_F_1;
				Send_COM3_Load(5,MAIN_RETURN_PHONE_ECHO,COMMAND_OK);
				Handle_Answer();
				break;
			}
			/********************************************************\
			3,手柄挂机（手柄命令）
			\********************************************************/
			case	HANDLE_HANGUP:
			{
				gCommon_Flag&=~CHECK_RINGPHONE_F_1;
				gHandle_OnLine_Timer=0;
				gStatus1_Flag &=~ HANDLE_ON_0_F_1;
				gStatus1_Flag |= HANDLE_ON_1_F_1;
				Send_COM3_Load(5,MAIN_RETURN_PHONE_ECHO,COMMAND_OK);
				//如果这个时候处于监听状态，则下面不用处理
				if( (gGeneral_Flag&MONITOR_ON_F_1)||(gPhone_Status==ACTIVE_MONITOR))
				{
					gHandle_Buffer_Point=0;
				}
				else
				{
					SRAM_Init();
					SRAM_Write(PHONE_AT_2+0,VAILD_2);
					SRAM_Write(PHONE_AT_2+1,H_HUNGUP);
					SRAM_Write(PHONE_AT_2+2,0);
				}
				break;
			}
			/********************************************************\
			4,手柄调节音量（手柄命令）
			\********************************************************/
			case	HANDLE_VOL:
			{
				gCommon_Flag&=~CHECK_RINGPHONE_F_1;
				gHandle_OnLine_Timer=0;
				gStatus1_Flag &=~ HANDLE_ON_0_F_1;
				gStatus1_Flag |= HANDLE_ON_1_F_1;
				SRAM_Init();
				SRAM_Write(PHONE_AT_2+0,VAILD_2);
				SRAM_Write(PHONE_AT_2+1,H_VOLADJ);
				SRAM_Write(PHONE_AT_2+2,1);
				SRAM_Write(PHONE_AT_2+3,gHandle_Buffer[3]);
				break;
			}
			/********************************************************\
			5,手柄的DTMF（手柄命令）
			\********************************************************/
			case	HANDLE_DTMF:
			{
				gCommon_Flag&=~CHECK_RINGPHONE_F_1;
				gHandle_OnLine_Timer=0;
				gStatus1_Flag &=~ HANDLE_ON_0_F_1;
				gStatus1_Flag |= HANDLE_ON_1_F_1;
				if(gDTMF_SAVE[0]<sizeof(gDTMF_SAVE)-1)
				{
					gDTMF_SAVE[0]++;
					gDTMF_SAVE[gDTMF_SAVE[0]]=gHandle_Buffer[3];
				}
				break;
			}
			/********************************************************\
			6,手柄读最新拨出电话号码（手柄命令）
			\********************************************************/
			case	HANDLE_READ_DIAL:
			{
				gCommon_Flag&=~CHECK_RINGPHONE_F_1;
				gHandle_OnLine_Timer=0;
				gStatus1_Flag &=~ HANDLE_ON_0_F_1;
				gStatus1_Flag |= HANDLE_ON_1_F_1;
				if(gPhone_Status==ACTIVE_MONITOR);
				else if(gGeneral_Flag&TCP_PHONE_F_1)
				{
					gCommon_Flag|=ALLOW_HANGUP_PHONE_F_1;
				}
				//while(gDelay_Flash_Timer<3) {Clear_Exter_WatchDog();}
				Read_Dail_Record(gHandle_Buffer[3]);
				gDelay_Flash_Timer=0;
				break;
			}
			/********************************************************\
			7,手柄读前一个拨出电话号码（手柄命令）
			\********************************************************/
			case	HANDLE_UP_DIAL:
			{
				gCommon_Flag&=~CHECK_RINGPHONE_F_1;
				gHandle_OnLine_Timer=0;
				gStatus1_Flag &=~ HANDLE_ON_0_F_1;
				gStatus1_Flag |= HANDLE_ON_1_F_1;
				/*
				if(gPhone_Status==ACTIVE_MONITOR);
				else if(gGeneral_Flag&TCP_PHONE_F_1)
				{
					gCommon_Flag|=ALLOW_HANGUP_PHONE_F_1;
				}
				&*/
				//while(gDelay_Flash_Timer<3) {Clear_Exter_WatchDog();}
				Read_Dail_Record(gHandle_Buffer[3]);
				gDelay_Flash_Timer=0;
				break;
			}
			/********************************************************\
			8,手柄读后一个拨出电话号码（手柄命令）
			\********************************************************/
			case	HANDLE_DOWN_DIAL:
			{
				gCommon_Flag&=~CHECK_RINGPHONE_F_1;
				gHandle_OnLine_Timer=0;
				gStatus1_Flag &=~ HANDLE_ON_0_F_1;
				gStatus1_Flag |= HANDLE_ON_1_F_1;
				/*
				if(gPhone_Status==ACTIVE_MONITOR);
				else if(gGeneral_Flag&TCP_PHONE_F_1)
				{
					gCommon_Flag|=ALLOW_HANGUP_PHONE_F_1;
				}
				*/
				//while(gDelay_Flash_Timer<3) {Clear_Exter_WatchDog();}
				Read_Dail_Record(gHandle_Buffer[3]);
				gDelay_Flash_Timer=0;
				break;
			}
			/********************************************************\
			9,手柄读最新一次的通话记录（手柄命令）
			\********************************************************/
			/*
			case	HANDLE_READ_TALK:
			{
				gHandle_OnLine_Timer=0;
				if(gPhone_Status==ACTIVE_MONITOR);
				else if(gGeneral_Flag&TCP_PHONE_F_1)
				{
					gCommon_Flag|=ALLOW_HANGUP_PHONE_F_1;
				}
				//while(gDelay_Flash_Timer<3) {Clear_Exter_WatchDog();}
				Read_Talk_Record(gHandle_Buffer[3]);
				gDelay_Flash_Timer=0;

				break;
			}
			*/
			/********************************************************\
			10,手柄读前一个通话记录（手柄命令）
			\********************************************************/
			/*
			case	HANDLE_UP_TALK:
			{
				gHandle_OnLine_Timer=0;
				if(gPhone_Status==ACTIVE_MONITOR);
				else if(gGeneral_Flag&TCP_PHONE_F_1)
				{
					gCommon_Flag|=ALLOW_HANGUP_PHONE_F_1;
				}
				//while(gDelay_Flash_Timer<3) {Clear_Exter_WatchDog();}
				Read_Talk_Record( gHandle_Buffer[3] );
				gDelay_Flash_Timer=0;

				break;
			}
			*/
			/********************************************************\
			11,手柄读后一个通话记录（手柄命令）
			\********************************************************/
			/*
			case	HANDLE_DOWN_TALK:
			{
				gHandle_OnLine_Timer=0;
				if(gPhone_Status==ACTIVE_MONITOR);
				else if(gGeneral_Flag&TCP_PHONE_F_1)
				{
					gCommon_Flag|=ALLOW_HANGUP_PHONE_F_1;
				}
				//while(gDelay_Flash_Timer<3) {Clear_Exter_WatchDog();}
				Read_Talk_Record(gHandle_Buffer[3]);
				gDelay_Flash_Timer=0;

				break;
			}
			*/
			/********************************************************\
			12,手柄送给主控存储的通话记录（手柄命令）
			\********************************************************/
			/*
			case	HANDLE_SEND_TALK:
			{
				gHandle_OnLine_Timer=0;
				if(gPhone_Status==ACTIVE_MONITOR);
				else if(gGeneral_Flag&TCP_PHONE_F_1)
				{
					gCommon_Flag|=ALLOW_HANGUP_PHONE_F_1;
				}
				//while(gDelay_Flash_Timer<3) {Clear_Exter_WatchDog();}
				Init_Buffer2();
				Write_Talk_Record(nHandle_Piont_Temp);
				gDelay_Flash_Timer=0;
				Init_Buffer2();

				break;
			}
			*/
			/********************************************************\
			13,手柄送被盗设防命令（手柄命令）,取消
			\********************************************************/
			/*
			case	HANDLE_SET_DEFENCE:
			{
				gHandle_OnLine_Timer=0;
				break;
			}
			*/
			/********************************************************\
			14,手柄解除设防命令（手柄命令）
			\********************************************************/
			/*
			case	HANDLE_FREE_DEFENCE:
			{
				gHandle_OnLine_Timer=0;
				break;
			}
			*/
			/********************************************************\
			15,手柄提取短消息中心号码（手柄命令,暂时取消）
			\********************************************************/
			/*
			case	HANDLE_ASK_CENTER_NUM:
			{
				gHandle_OnLine_Timer=0;
				break;
			}
			*/
			/********************************************************\
			16,手柄设置短消息中心号码（手柄命令，暂时取消）
			\********************************************************/
			/*
			case	HANDLE_SET_CENTER_NUM:
			{
				break;
			}
			*/
			/********************************************************\
			17,手柄更改设置密码（手柄命令）
			\********************************************************/
			/*
			case	HANDLE_PASSWORD:
			{
				break;
			}
			*/
			/********************************************************\
			18,手柄进行对车载终端进行出厂设置操作（初始化系统）
			\********************************************************/
			case	HANDLE_DEFAULT_SET:
			{
				gHandle_OnLine_Timer=0;
				//1,清除信息区1
				OperateSPIEnd();EraseOnePage(FLASH_INFO1_ONE_2);OperateSPIEnd();
				OperateSPIEnd();EraseOnePage(FLASH_INFO1_TWO_2);OperateSPIEnd();
				Clear_Exter_WatchDog();
				RX2_Ready();RX3_Ready();RX4_Ready();
				//2,清除信息区2
				OperateSPIEnd();EraseOnePage(FLASH_INFO2_ONE_2);OperateSPIEnd();
				OperateSPIEnd();EraseOnePage(FLASH_INFO2_TWO_2);OperateSPIEnd();
				Clear_Exter_WatchDog();
				RX2_Ready();RX3_Ready();RX4_Ready();
				//3,清除电话号码拨出和通话记录区域
				OperateSPIEnd();EraseOnePage(FLASH_DIAL_PHONE);OperateSPIEnd();
				OperateSPIEnd();EraseOnePage(FLASH_RECORD_PHONE_2);OperateSPIEnd();
				Clear_Exter_WatchDog();
				RX2_Ready();RX3_Ready();RX4_Ready();
				//4,清除存储区域数据的区域
				OperateSPIEnd();EraseOnePage(FLASH_AREA_ONE_2);OperateSPIEnd();
				OperateSPIEnd();EraseOnePage(FLASH_AREA_TWO_2);OperateSPIEnd();
				Clear_Exter_WatchDog();
				RX2_Ready();RX3_Ready();RX4_Ready();
				//5,清除存储线路数据的区域1的数据
				OperateSPIEnd();EraseOnePage(FLASH_LINEDATA1_ONE_2);OperateSPIEnd();
				OperateSPIEnd();EraseOnePage(FLASH_LINEDATA1_TWO_2);OperateSPIEnd();
				Clear_Exter_WatchDog();
				RX2_Ready();RX3_Ready();RX4_Ready();
				//6,清除存储线路数据的区域2的数据
				OperateSPIEnd();EraseOnePage(FLASH_LINEDATA2_ONE_2);OperateSPIEnd();
				OperateSPIEnd();EraseOnePage(FLASH_LINEDATA2_TWO_2);OperateSPIEnd();
				Clear_Exter_WatchDog();
				RX2_Ready();RX3_Ready();RX4_Ready();
				//7,清除存储电话号码本的区域数据
				OperateSPIEnd();EraseOnePage(FLASH_PHONE_INFO_ONE_2);OperateSPIEnd();
				OperateSPIEnd();EraseOnePage(FLASH_PHONE_INFO_TWO_2);OperateSPIEnd();
				Clear_Exter_WatchDog();
				RX2_Ready();RX3_Ready();RX4_Ready();
				//
				for(i=0;i<128;i++)
				{
					gGeneral_Buffer[i]=0xFF;
				}
				Write_SegAB(0);
				gTimer=0;
				while(gTimer<3000){Clear_Exter_WatchDog();}
				RX2_Ready();RX3_Ready();RX4_Ready();
				Write_SegAB(1);
				gTimer=0;
				while(gTimer<1500){Clear_Exter_WatchDog();}
			    TACTL|=TASSEL1;         //输入时钟选择MCLK
			    TACTL|=TACLR;           //定时器A清除位CLR清零
			    TACTL|=MC1;             //定时器A选择连续增记数模式
			    TAR=0;                  //定时器A的值清零
			    TACCTL0|=OUT;           //TXD2 Idle as Mark
				TACCTL1|=OUT;           //TXD3 Idle as Mark
				TACCTL2|=OUT;           //TXD4 Idle as Mark
				gON_OFF_Temp0=0xFF;gON_OFF_Temp1=0xFF;gON_OFF_Temp2=0xFF;
				gAlarm_Type=0;gWatch_Type=0;gCycle_Send_Status=NOP_DATA;
				gStatus2_Flag&=~ROB_ALARM_F_1;
				//设防状态清除
				gInternal_Flag&=~VEHICLE_DEFENCE_F_1;
				gStatus1_Flag&=~DEFENCE_STATUS_F_1;
				//行车轨迹上传的间隔
				gCycle_GSM=0;gCycle_TCP=0;
				//TCP的IP地址无效
				gGeneral_Flag&=~NUM1_VAILD_F_1;
				gGeneral_Flag&=~TCP_IP_VALID_1;
				gStatus2_Flag&=~IP_VAILD_F_1;
				gStatus2_Flag&=~TCP_ON_OFF_F_1;
				RX2_Ready();RX3_Ready();RX4_Ready();
				Waiting_Allow_Send();
				Send_COM3_SubProgram(5,MAIN_RETURN_INIT,COMMAND_OK);
				gTimer=0;
				gDelay_Flash_Timer=0;
				while(gTimer<50){}
				break;
			}
			/********************************************************\
			19,手柄返回是否开机的状态（手柄命令）
			\********************************************************/
			case	HANDLE_STATUS:
			{
				//如果接收到此命令，则根据命令字，来判断是否车载电话是否处于开机和关机状态，同时清除计时判断手柄未接的计数变量
				//判断手柄显示终端关机
				gHandle_OnLine_Timer=0;
				if( gHandle_Buffer[3]==0 )
				{
					gStatus1_Flag |= HANDLE_ON_0_F_1;
					gStatus1_Flag &=~ HANDLE_ON_1_F_1;
				}
				//判断手柄显示终端开机
				else if ( gHandle_Buffer[3]==1 )
				{
					gStatus1_Flag &=~ HANDLE_ON_0_F_1;
					gStatus1_Flag |= HANDLE_ON_1_F_1;
				}
				break;
			}
			/********************************************************\
			20,手柄查询指定位置的电话号码本的内容（手柄命令）
			\********************************************************/
			case	HANDLE_PHONE_NOTE:
			{
				gHandle_OnLine_Timer=0;
				gStatus1_Flag &=~ HANDLE_ON_0_F_1;
				gStatus1_Flag |= HANDLE_ON_1_F_1;
				Read_Phone_Note_Record(gHandle_Buffer[3]);
				break;
			}
			/********************************************************\
			A1,手柄对指定页数据的擦除（测试命令）
			\********************************************************/
			/*
			case	HANDLE_ERASE_PAGEDATA:
			{
				break;
			}
			*/
			/********************************************************\
			A2,手柄对指定页数据的读取（测试命令）
			\********************************************************/
			case	HANDLE_READ_PAGEDATA:
			{
				gHandle_OnLine_Timer=0;
				Handle_ReadOnePage_Data(gHandle_Buffer[3],gHandle_Buffer[4]);
				break;
			}
			/********************************************************\
			A3,通过手柄(COM3)设置目标电话号码1（测试命令）
			\********************************************************/
			/*
			case	HANDLE_SET_PHONE_NUM:
			{
				break;
			}
			*/
			/********************************************************\
			A4,通过手柄(COM3)发送检查整个FLASH（测试命令）根据情况，则只检查buffer1和buffer2的情况
			\********************************************************/
			/*
			case	HANDLE_CHECK_FLASH:
			{
				break;
			}
			*/
			/********************************************************\
			A5,通过手柄(COM3)发送检查GPS数据格式有效性（测试命令）
			\********************************************************/
			/*
			case	HANDLE_CHECK_GPS:
			{
				break;
			}
			*/
			/********************************************************\
			A6,通过手柄(COM3)使GSM模块自发自收一个SMS的命令（测试命令）
			\********************************************************/
			/*
			case	HANDLE_CHECK_SMS:
			{
				break;
			}
			*/
			/********************************************************\
			A7,手柄接口调试监听语音电路(测试命令)
			\********************************************************/
			case	HANDLED_ACTIVE_MONITOR:
			{
				gHandle_OnLine_Timer=0;
				SRAM_Init();
				SRAM_Write(PHONE_AT_2+0,VAILD_2);
				SRAM_Write(PHONE_AT_2+1,M_DIAL);
				SRAM_Write(PHONE_AT_2+2,gHandle_Buffer[1]-4);
				for(i=0;i<gHandle_Buffer[1]-4;i++)
				{
					SRAM_Write(PHONE_AT_2+3+i,gHandle_Buffer[i+3]);
				}
				break;
			}
			/********************************************************\
			A8,手柄插除所有的历史轨迹的数据（测试命令）
			\********************************************************/
			/*
			case	HANDLE_ERASE_ALLPASS:
			{
				break;
			}
			*/
			/********************************************************\
			A8-1,测试外部SRAM的数据正确性（测试命令）
			\********************************************************/
			case	HANDLE_TEST_SRAM:
			{
				gHandle_OnLine_Timer=0;
				Handle_Test_Sram();
				break;
			}
			/********************************************************\
			A9,手柄设置相关TCP的参数（测试命令）
				做测试用，设置IP和PORT后，则需要开启进行TCP登陆的操作
				以测试此登陆TCP的功能正确
			\********************************************************/
			/*
			case	HANDLE_SET_TCP:
			{
				break;
			}
			*/
			/********************************************************\
			A10,手柄读取版本号（测试命令）
			\********************************************************/
			case	HNADLE_READ_EDITION:
			{
				gHandle_OnLine_Timer=0;
				if(gHandle_Buffer[3]=(char)(~gHandle_Buffer[2]))
				{
					Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
					Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
					for(i=0;i<35;i++)	Send_COM1_Byte('=');
					Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
					for(i=0;i<sizeof(String_A);i++)
					{
						Send_COM1_Byte(String_A[i]-1);
						Send_COM1_Byte(String_EDIT[i]+1);
					}
					for(i=0;i<35;i++)	Send_COM1_Byte('=');
					Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
					Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
				}
				break;
			}
			/********************************************************\
			A12,手柄接口设置基本参数（参数设置命令）
			\********************************************************/
			case	HANDLE_SET_PAR:
			{
				gHandle_OnLine_Timer=0;
				Handle_Set_Par(nHandle_Piont_Temp);
				break;
			}
			/********************************************************\
			A13,手柄接口设置TCP的相关参数（参数设置命令）
			\********************************************************/
			case	HANDLE_SET_TCPIP:
			{
				Handle_Set_TcpIp();
				break;
			}
			/********************************************************\
			A14,手柄接口设置时间间隔的相关参数（参数设置命令）
			\********************************************************/
			case	HANDLE_SET_TIME:
			{
				gHandle_OnLine_Timer=0;
				Handle_Set_Time();
				break;
			}
			/********************************************************\
			A15,手柄接口设置短消息预置的相关参数（参数设置命令）
			\********************************************************/
			case	HANDLE_SET_SMS:
			{
				gHandle_OnLine_Timer=0;
				Handle_Set_SMS();
				break;
			}
			/********************************************************\
			A16,手柄接口设置电话号码本设置置的相关参数（参数设置命令）
			\********************************************************/
			case	HANDLE_SET_PHONE_NOTE:
			{
				Handle_Set_Phone(nHandle_Piont_Temp);
				break;
			}
			/********************************************************\
			A17,手柄接口设置按距离上传数据的相关参数（参数设置命令）
			\********************************************************/
			case	HANDLE_SET_DISTANCE:
			{
				gHandle_OnLine_Timer=0;
				Handle_Set_Dis();
				break;
			}
			/********************************************************\
			A18,（查询所有参数设置命令的返回的命令）
			\********************************************************/
			case	HANDLE_ASK_PAR:
			{
				gHandle_OnLine_Timer=0;
				Handle_Ask_Par();
				break;
			}
			/********************************************************\
			A19,手柄端口人为设置目标号码在buffer1中错误（调试命令）
			\********************************************************/
			/*
			case	HANDLE_ERROR_NUM:
			{
				break;
			}
			*/
			/********************************************************\
			A20,手柄接口端口人为设置短消息中心号码在buffer1中错误（调试命令）
			\********************************************************/
			/*
			case	HANDLE_ERROR_CENTER:
			{
				break;
			}
			*/
			/********************************************************\
			A21,手柄端口 人为设置430两个信息扇区的错误（调试命令）
			\********************************************************/
			/*
			case	HANDLE_ERROR_IP:
			{

				for(i=0;i<128;i++)
				{
					gGeneral_Buffer[i]=0xFF;
				}
				Write_SegAB(gHandle_Buffer[3]);

				break;
			}
			*/
			/********************************************************\
			A22,手柄端口 人为设置在扇区中的错误（调试命令）
			\********************************************************/
			/*
			case	HANDLE_ERROR_PORT:
			{

				OperateSPIEnd();ReadOnePageToBuffer2((int)gHandle_Buffer[3]);OperateSPIEnd();
				if( (gHandle_Buffer[3]==1)||(gHandle_Buffer[3]==2) )
				{
					if(gHandle_Buffer[4]==0)			k=TARGET_NUMBER1_F_2;
					else if(gHandle_Buffer[4]==1)		k=GPRS_IP_ADDRESS1_F_2;
					WriteOneByteToBuffer2(k+gHandle_Buffer[5],0x47);
					for(i=0;i<gHandle_Buffer[6];i++)
					{
						WriteNextByteToBuffer2(0x47);
					}
					OperateSPIEnd();
					if(gHandle_Buffer[7]==1)
					{
						RFlash_Sector((int)gHandle_Buffer[3]);
					}
					else
					{
						WriteBuffer2ToPage((int)gHandle_Buffer[3]);
						gTimer=0;
						while(gTimer<1000){Clear_Exter_WatchDog();}
					}
				}

				break;
			}
			*/
			/********************************************************\
			A23,手柄端口人为设置在扇区中的错误（调试命令）
			\********************************************************/
			/*
			case	HANDLE_SET_ALARM:
			{
				break;
			}
			*/
			/********************************************************\
			A25,手柄端口 读取数据从外部的SRAM中相应的位置（调试命令）
			\********************************************************/
			/*
			case	HANDLE_READ_SRAM:
			{
				break;
			}
			*/
			/********************************************************\
			A26,手柄端口 设置命令初始化显示终端
			\********************************************************/
			case	HANDLE_CLEAR_DISP:
			{
				gHandle_OnLine_Timer=0;
				if(gDisp_Buffer_Point+6+2<=sizeof(gDisp_Buffer))
				{
					gDisp_Buffer[gDisp_Buffer_Point]=6+2;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=0;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='@';
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='%';
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=6;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=DISP_CLEAR_DOWN;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='$';
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='&';
					gDisp_Buffer_Point++;
				}
				break;
			}

			/********************************************************\
			其他
			\********************************************************/
			default:
				break;
		}
		gHandle_Buffer_Point=0;
		for(i=0;i<sizeof(gHandle_Buffer);i++)	gHandle_Buffer[i]=0;
	}
	//判断是否有需要发送的手柄数据
	else if( 	(gHandle_Receive_Over>20)
			  &&(gDisp_Receive_Over>20)
			  &&( (gCommon_Flag&ALLOW_R232_F_1)==0)
			  &&(gGPS_Receive_Over>20)
			  &&(gReceive1_Over_Count>20)  )
	{
		SRAM_Init();
		i=SRAM_Read(SHANDLE_DATA_VAILD);
		if(i!=NULL_2)
		{
			for(k=0;k<SRAM_Read(SHANDLE_DATA_VAILD+2);k++)
			{
				Send_COM3_Byte( SRAM_Read(SHANDLE_DATA_VAILD+1+k) );
			}
			RX3_Ready();
			SRAM_Init();
			SRAM_Write(SHANDLE_DATA_VAILD+0,NULL_2);
		}
	}
}

/********************************************************\
*	函数名：Check_Handle_Command
	作用域：本地文件调用
*	功能：	校验在手柄通讯完后，判断收到数据包的数据是否属于正确数据包
			并返回信令字
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned char Check_Handle_Command(void)
{
	if(gHandle_Buffer_Point<4)	return(0);							//最小的包长为4个字节，如果小于4说明数据包未收全
	if(gHandle_Buffer_Point!=gHandle_Buffer[1]) return(0);			//接收到的数据包与数据包里的字长不符合，也说明数据未收全
	if(gHandle_Buffer[0]!=0x24) return(0);							//收到的包头不正确
	if(gHandle_Buffer[gHandle_Buffer_Point-1]!=0x0d) return(0);		//收到的包尾不正确
	return(gHandle_Buffer[2]);
}
/********************************************************\
*	函数名：Handle_Dailing
	作用域：本地文件调用
*	功能：

*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Handle_Dailing(unsigned char nPoint_Temp)
{
	unsigned int i=0;
	unsigned char nTemp=0;
	unsigned char nFlag=0;
	nTemp=nPoint_Temp;

	//1,如果主控状态处于电话禁止呼出状态
	if(  (gON_OFF_Temp0&PHONE_OUT_ON)==0 )
	{
		//如果存在中心服务电话，则允许此号码拨出
		for(i=0;i<nTemp-4;i++)
		{
			gGeneral_Buffer[i]=gHandle_Buffer[3+i];
		}
		i=1;
		i=Check_Allow_Phone(nTemp-4);
		//判断书否属于112，110，119，13800138000，1860，1861，1001，等一类的服务号码
		if(Compare_String(gGeneral_Buffer,(unsigned char *)PHONE1,nTemp-4))				i=0;
		else if(Compare_String(gGeneral_Buffer,(unsigned char *)PHONE2,nTemp-4))		i=0;
		else if(Compare_String(gGeneral_Buffer,(unsigned char *)PHONE3,nTemp-4))		i=0;
		else if(Compare_String(gGeneral_Buffer,(unsigned char *)PHONE4,nTemp-4))		i=0;
		else if(Compare_String(gGeneral_Buffer,(unsigned char *)PHONE5,nTemp-4))		i=0;
		else if(Compare_String(gGeneral_Buffer,(unsigned char *)PHONE6,nTemp-4))		i=0;
		else if(Compare_String(gGeneral_Buffer,(unsigned char *)PHONE7,nTemp-4))		i=0;
		//拨出号码与比较号码均不匹配，则不允许拨出此号码
		if(i==1)
		{
			nFlag=1;
		}
		else if( (gGeneral_Flag&MONITOR_ON_F_1)||(gPhone_Status==ACTIVE_MONITOR) )
		{
			nFlag=2;
		}
		else if( (gGeneral_Flag&DAILING_UP_F_1)||(gPhone_Status==ACTIVE_HANDLE)||(gGeneral_Flag&RING_F_1) )
		{
			nFlag=3;
		}
		else
		{
			nFlag=0;
		}
	}
	//如果这个时候处于监听状态，或者处于正在监听的拨号状态下，则返回手柄一个busy信令
	else if( (gGeneral_Flag&MONITOR_ON_F_1)||(gPhone_Status==ACTIVE_MONITOR) )
	{
		nFlag=2;
	}
	//如果这个时候处于手柄拨号，或者正在通话的状态，或者正在来电的状态下，则返回一个busy信令
	else if( (gGeneral_Flag&DAILING_UP_F_1)||(gPhone_Status==ACTIVE_HANDLE)||(gGeneral_Flag&RING_F_1) )
	{
		nFlag=3;
	}
	//返回一个电话禁止呼出
	if(nFlag==1)
	{
		Send_COM3_Load(4,MAIN_RETURN_NO_CALLOUT,0);
	}
	//返回给手柄一个Busy信令
	else if(nFlag==2)
	{
		Send_COM3_Load(4,MAIN_BUSY,0);
	}
	//返回挂机命令
	else if(nFlag==3)
	{
		SRAM_Init();
		SRAM_Write(PHONE_AT_2+0,VAILD_2);
		SRAM_Write(PHONE_AT_2+1,H_HUNGUP);
		SRAM_Write(PHONE_AT_2+2,0);
		//Send_COM3_Load(5,MAIN_RETURN_PHONE_ECHO,COMMAND_ERROR);
	}
	//返回一个正常的拨出号码
	else
	{
		Send_COM3_Load(5,MAIN_RETURN_PHONE_ECHO,COMMAND_OK);
		SRAM_Init();
		SRAM_Write(PHONE_AT_2+0,VAILD_2);
		SRAM_Write(PHONE_AT_2+1,H_DIAL);
		SRAM_Write(PHONE_AT_2+2,nTemp-4);
		for(i=3;i<nTemp-1;i++)
		{
			SRAM_Write(PHONE_AT_2+i,gHandle_Buffer[i]);
		}
		//将拨出电话记录存储在相应的Flash中
		//while(gDelay_Flash_Timer<3) {Clear_Exter_WatchDog();}
		//Init_Buffer2();
		Write_Dail_Record(nTemp);
		gDelay_Flash_Timer=0;
		gHandle_Buffer_Point=0;
		gTimer=0;Clear_Exter_WatchDog();
		while(gTimer<500){}
		Clear_Exter_WatchDog();
		OperateSPIEnd();
		//Init_Buffer2();
	}
}

/********************************************************\
*	函数名：Handle_Answer
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Handle_Answer(void)
{
	unsigned char nFlag=0;
	//1，如果设备处于呼入禁止状态下，判断是否有允许此号码呼入的标志
	if(  (gON_OFF_Temp0&PHONE_IN_ON)==0 )
	{
		if(gPublic_Flag&ALLOW_PHONE_IN_F_1)		nFlag=1;
		else									nFlag=0;
	}
	//2，设备没有处于呼入禁止的状态
	else										nFlag=1;

	if(nFlag==1)
	{
		SRAM_Init();
		SRAM_Write(PHONE_AT_2+0,VAILD_2);
		SRAM_Write(PHONE_AT_2+1,H_ANSWER);
		SRAM_Write(PHONE_AT_2+2,0);
	}
	else
	{
		Send_COM3_Load(4,MAIN_RETURN_NO_CALLIN,0);
		SRAM_Init();
		SRAM_Write(PHONE_AT_2+0,VAILD_2);
		SRAM_Write(PHONE_AT_2+1,H_HUNGUP);
		SRAM_Write(PHONE_AT_2+2,0);
	}
}
/********************************************************\
*	函数名：Move_Right_Item()
	作用域：本地文件调用
*	功能：	针对gGeneral_Buffer[]中存储的号码有重复的地方，则如何消除重复号码那一条
*	参数：	Start_Item：
			End_Item：
			Move_Count：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Move_Right_Item(unsigned char Start_Item,unsigned char End_Item,unsigned char Move_Count)
{
	unsigned int i=0,j=0;
	if(Move_Count==1)
	{
		for( i=End_Item;i>Start_Item;i=i-1 )
		{
			for(j=0;j<16;j++)
			{
				gGeneral_Buffer[i*16+j]=gGeneral_Buffer[(i-1)*16+j];
			}
		}
	}
	else if(Move_Count==2)
	{
		for( i=7;i>0;i-- )
		{
			for(j=0;j<32;j++)
			{
				gGeneral_Buffer[i*32+j]=gGeneral_Buffer[(i-1)*32+j];
			}
		}
	}
}

/********************************************************\
*	函数名：Write_Dail_Record
	作用域：本地文件调用
*	功能：	用来在每次拨出电话号码后，存储拨出电话号码的记录
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	补充说明：这个子程序是将拨出的电话号码存储在缓冲区中。
			  先判断已经存储的缓冲区中是否有相同的拨好记录
			  如果有，则先将此记录删除，然后重新排列，将
			  整个缓冲向左移动一个存储记录，则将第一个存储
			  记录的空间留下来，则将最新的拨出电话记录存储在
			  位置上。
\********************************************************/
void Write_Dail_Record(unsigned char nPhone_Length)
{
	unsigned char nItem=0xff;
	unsigned int i,j,k;
	i=nPhone_Length;
	Clear_Exter_WatchDog();
	OperateSPIEnd();
	ReadOnePageToBuffer2(FLASH_DIAL_PHONE);	//读第3个扇区的数据到buffer2中
	OperateSPIEnd();
	//将buffer2中的数据转移到gGeneral_Buffer中
	gGeneral_Buffer[0]=ReadByteFromBuffer2(0);
	for(i=1;i<264;i++)	gGeneral_Buffer[i]=ReadNextFromBuffer2();
	OperateSPIEnd();
	//判断gHandle_Buffer[]中的号码是否与gGeneral_Buffer[]中有重复的号码
	i=0;
	k=0;
	while(i<16)
	{
		j=0;
		while(j<nPhone_Length-4)
		{
			if( gGeneral_Buffer[i*16+j]!=gHandle_Buffer[3+j] )
			{
				i++;j=0;k=0;break;
			}
			j++;
			if(j>=nPhone_Length-4)
			{
				k=1;break;
			}
		}
		if(k==1)	break;
	}

	//说明有重复的号码,则将这一条的重复号码给覆盖住
	if(k==1)
	{
		nItem=i;
		if(nItem>0)	Move_Right_Item(0,nItem,1);
	}
	//没有重复号码，则将整个号码向后移动一条。
	else
	{
		Move_Right_Item(0,15,1);
	}
	//将最新号码写在第一条号码的位置
	for(i=0;i<16;i++)
	{
		gGeneral_Buffer[i]=0x20;
	}
	for(i=0;i<nPhone_Length-4;i++)
	{
		gGeneral_Buffer[i]=gHandle_Buffer[3+i];
	}
	//将gGeneral_Buffer[]中的数写入buffer2中
	OperateSPIEnd();
	WriteOneByteToBuffer2(0,gGeneral_Buffer[0]);
	for(i=1;i<264;i++)
	{
		WriteNextByteToBuffer2( gGeneral_Buffer[i] );
	}
	OperateSPIEnd();
	//4,将已经写入了新的拨号记录的buffer2写到flash 相应的区域中
	Clear_Exter_WatchDog();
	P6OUT|=SPI45DB041_WP;
	WriteBuffer2ToPage(FLASH_DIAL_PHONE);
	P6OUT&=~SPI45DB041_WP;
	gTimer=0;Clear_Exter_WatchDog();
	OperateSPIEnd();
}
/********************************************************\
*	函数名：Read_Dail_Record
	作用域：本地文件调用
*	功能：  读取记录拨出电话记录的FLASH中的相应位置的数据

*	参数：	nItem：表示当前所指向的记录号，其值有如下0-----15

*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Read_Dail_Record(unsigned char nItem)
{
	unsigned char nTemp,Record_Flag;
	unsigned int i;
	Clear_Exter_WatchDog();
	OperateSPIEnd();
	//读第3个扇区的数据到buffer2中
	ReadOnePageToBuffer2(FLASH_DIAL_PHONE);
	OperateSPIEnd();
	nTemp = ReadByteFromBuffer2(nItem*0x10);
	OperateSPIEnd();
	Record_Flag=0;
	//判断当前电话记录是否为空
	if( (nTemp>'9')||(nTemp<'0') )	Record_Flag=1;
	else							Record_Flag=0;
	//将所指向的拨出电话号码记录发送出去
	SRAM_Init();
	SRAM_Write(SHANDLE_DATA_VAILD+0,VAILD_2);
	SRAM_Write(SHANDLE_DATA_VAILD+1,0x24);
	SRAM_Write(SHANDLE_DATA_VAILD+2,0x15);
	SRAM_Write(SHANDLE_DATA_VAILD+3,MAIN_DIALRECORD);
	SRAM_Write(SHANDLE_DATA_VAILD+4,nItem);
	if(Record_Flag==0)
	{
		OperateSPIEnd();
		nTemp=ReadByteFromBuffer2(nItem*0x10);
		SRAM_Write(SHANDLE_DATA_VAILD+5,nTemp);
		for(i=1;i<16;i++)
		{
			SRAM_Write(SHANDLE_DATA_VAILD+5+i,ReadNextFromBuffer2());
		}
		OperateSPIEnd();
	}
	else
	{
		for(i=0;i<16;i++)
		{
			SRAM_Write(SHANDLE_DATA_VAILD+5+i,0x20);
		}
	}
	SRAM_Write(SHANDLE_DATA_VAILD+5+16,0x0d);
}

/********************************************************\
*	函数名：Read_Phone_Note_Record
	作用域：本地文件调用
*	功能：  读取电话号码

*	参数：	nItem：表示当前所指向的记录号，其值有如下1-----20

*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Read_Phone_Note_Record(unsigned char nItem)
{
	unsigned int i=0,j=0,k=0;
	unsigned char Phone_Page=0;
	unsigned char Phone_Item=0;
	unsigned int  Phone_Address=0;
	unsigned char Phone_Num=0;
	unsigned char nTemp=0;

	if( nItem<10 )									Phone_Page=1;
	else if( (nItem>=10)&&(nItem<20) )				Phone_Page=2;
	else											{Phone_Page=0;return;}

	OperateSPIEnd();
	if(Phone_Page==1)
	{
		ReadOnePageToBuffer2(FLASH_PHONE_INFO_ONE_2);
		Phone_Item=nItem;
	}
	else if(Phone_Page==2)
	{
		ReadOnePageToBuffer2(FLASH_PHONE_INFO_TWO_2);
		Phone_Item=nItem-10;
	}
	OperateSPIEnd();
	Phone_Num=ReadByteFromBuffer2(PHONE_NUM_2);
	OperateSPIEnd();
	if( (Phone_Num>0)&&(Phone_Num<=10)&&(Phone_Item<=Phone_Num) )		j=1;
	else																j=0;
	Phone_Address=Phone_Item*21+ONE_1_FRAME_2;
	OperateSPIEnd();
	nTemp=ReadByteFromBuffer2(Phone_Address);
	OperateSPIEnd();
	if(nTemp==0x20)	j=0;

	SRAM_Init();
	SRAM_Write(SHANDLE_DATA_VAILD+0,VAILD_2);
	SRAM_Write(SHANDLE_DATA_VAILD+1,0x24);
	SRAM_Write(SHANDLE_DATA_VAILD+2,0x1A);
	SRAM_Write(SHANDLE_DATA_VAILD+3,MAIN_RETURN_PHONE_NOTE);
	SRAM_Write(SHANDLE_DATA_VAILD+4,nItem);
	if(j==1)
	{
		SRAM_Write(SHANDLE_DATA_VAILD+5,ReadByteFromBuffer2(Phone_Address));
		for(i=1;i<8+13;i++)
		{
			SRAM_Write(SHANDLE_DATA_VAILD+5+i,ReadNextFromBuffer2());
		}
	}
	else
	{
		for(i=0;i<8+13;i++)
		{
			SRAM_Write(SHANDLE_DATA_VAILD+5+i,0x20);
		}
	}
	SRAM_Write(SHANDLE_DATA_VAILD+5+8+13,0x0d);
}
/********************************************************\
*	函数名：Write_Talk_Record
	作用域：本地文件调用
*	功能：	用来每次通话结束后，存储通话记录(此记录均是用手柄发送过来的数据包)
	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
void Write_Talk_Record(unsigned char nPhone_Length)
{
    unsigned int i;
    OperateSPIEnd();
    Clear_Exter_WatchDog();
    //1,读第4个扇区的数据到buffer2中
	ReadOnePageToBuffer2(FLASH_RECORD_PHONE_2);
	OperateSPIEnd();
	//将buffer2中的数据转移到gGeneral_Buffer中
	gGeneral_Buffer[0]=ReadByteFromBuffer2(0);
	for(i=1;i<264;i++)	gGeneral_Buffer[i]=ReadNextFromBuffer2();
	OperateSPIEnd();
	//2,平移动buffer2中的通话记录
	Move_Right_Item(0,14,2);

	for(i=0;i<32;i++)
	{
		gGeneral_Buffer[i]=0x20;
	}
	//3,将新的通话记录的数据写入第一个记录区
	for(i=0;i<nPhone_Length-4;i++)
	{
		gGeneral_Buffer[i]=gHandle_Buffer[3+i];
	}
	//将gGeneral_Buffer[]中的数写入buffer2中
	OperateSPIEnd();
	WriteOneByteToBuffer2(0,gGeneral_Buffer[0]);
	for(i=1;i<264;i++)
	{
		WriteNextByteToBuffer2( gGeneral_Buffer[i] );
	}
	OperateSPIEnd();
	//4,将更新完毕的buffer2中的数据写入第4扇区
	Clear_Exter_WatchDog();
	P6OUT|=SPI45DB041_WP;
	WriteBuffer2ToPage(FLASH_RECORD_PHONE_2);
	P6OUT&=~SPI45DB041_WP;
	gTimer=0;Clear_Exter_WatchDog();
	OperateSPIEnd();
}
*/
/********************************************************\
*	函数名：Read_Talk_Record
	作用域：本地文件调用
*	功能：
*	参数：  nItem：表示当前所指向的记录，其值有如下0-----7

*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Read_Talk_Record(unsigned char nItem)
{
	unsigned char nTemp,Record_Flag;
	unsigned int i;
	Record_Flag=0;
	if( nItem>7 )		return;
	//读第4个扇区的数据到buffer2中
	ReadOnePageToBuffer2(FLASH_RECORD_PHONE_2);
	OperateSPIEnd();
	nTemp = ReadByteFromBuffer2( nItem*0x10 );
	OperateSPIEnd();
	ReadOnePageToBuffer2(FLASH_RECORD_PHONE_2);
	nTemp = ReadByteFromBuffer2( (2*nItem)*0x10 );
	OperateSPIEnd();
	if( (nTemp>'9')||(nTemp<'0') )		Record_Flag=1;
	else								Record_Flag=0;
	SRAM_Init();
	SRAM_Write(SHANDLE_DATA_VAILD+0,VAILD_2);
	SRAM_Write(SHANDLE_DATA_VAILD+1,0x24);
	SRAM_Write(SHANDLE_DATA_VAILD+2,0x1e);
	SRAM_Write(SHANDLE_DATA_VAILD+3,MAIN_TALKRECORD);
	SRAM_Write(SHANDLE_DATA_VAILD+4,nItem);
	if(Record_Flag==0)
	{
		OperateSPIEnd();
		ReadOnePageToBuffer2(FLASH_RECORD_PHONE_2);
		OperateSPIEnd();
		nTemp=ReadByteFromBuffer2( (2*nItem)*0x10 );

		SRAM_Write(SHANDLE_DATA_VAILD+5,nTemp);
		for(i=1;i<25;i++)
		{
			SRAM_Write(SHANDLE_DATA_VAILD+5+i,ReadNextFromBuffer2());
		}
		OperateSPIEnd();
	}
	else
	{
		for(i=0;i<25;i++)
		{
			SRAM_Write(SHANDLE_DATA_VAILD+5+i,0x20);
		}
	}
	SRAM_Write(SHANDLE_DATA_VAILD+5+25,0x0d);
}
/********************************************************\
*	函数名：Handle_ReadOnePage_Data
	作用域：本地文件调用
*	功能：
*	参数：
*	参数：
*	返回值：
*	创建人：
*	修改历史：（每条详述）
\********************************************************/
void Handle_ReadOnePage_Data(unsigned char Page_H,unsigned char Page_L)
{
	unsigned int Page_Address=0,i=0;
	unsigned char Length=0;
	Page_Address = Page_H;
	Page_Address <<=8;
	Page_Address += Page_L;

	if(Page_Address<2048)
	{
		OperateSPIEnd();
		ReadOnePageToBuffer2(Page_Address);
		OperateSPIEnd();
		gGeneral_Buffer[0]=ReadByteFromBuffer2(0);
		for(i=1;i<256;i++)	gGeneral_Buffer[i]=ReadNextFromBuffer2();
		OperateSPIEnd();
		Length=4+256;
		Waiting_Allow_Send();
		Send_COM3_Byte(0x24);Send_COM3_Byte(Length);
		Send_COM3_Byte(MAIN_READPAGE_ECHO);
		for(i=0;i<256;i++)
		{
			Send_COM3_Byte(gGeneral_Buffer[i]);
			Clear_Exter_WatchDog();
		}
		Send_COM3_Byte(0x0d);
		RX3_Ready();
	}
}
/********************************************************\
*	函数名：Handle_EraseOnePage_Data
	作用域：本地文件调用
*	功能：
*	参数：
*	参数：
*	返回值：
*	创建人：
*	修改历史：（每条详述）
\********************************************************/
/*
void Handle_EraseOnePage_Data(unsigned char Page_H,unsigned char Page_L)
{
	unsigned int Page_Address=0;
	Page_Address = Page_H;
	Page_Address <<=8;
	Page_Address += Page_L;

	if(Page_Address<2048)
	{
		OperateSPIEnd();EraseOnePage(Page_Address);OperateSPIEnd();
	}
	Send_COM3_Load(5,MAIN_ERASEPAGE_ECHO,COMMAND_OK);
}
*/
/********************************************************\
*	函数名：Send_COM3_SubProgram
	作用域：本地文件调用
*	功能：
*	参数：
*	参数：
*	返回值：
*	创建人：
*	修改历史：（每条详述）
\********************************************************/
void Send_COM3_SubProgram(unsigned char DataLength,unsigned char nCommand,unsigned char DataTemp)
{
	unsigned char Length=0;
	unsigned char Command=0;
	unsigned char Data=0;
	Length=DataLength;
	Command=nCommand;
	Data=DataTemp;
	Send_COM3_Byte(0x24);Send_COM3_Byte(Length);
	Send_COM3_Byte(Command);
	if(Length==4)
	{
	}
	else if(Length==5)
	{
		Send_COM3_Byte(Data);
	}
	Send_COM3_Byte(0x0d);
	RX3_Ready();
}
/********************************************************\
*	函数名：Send_COM3_SubProgram
	作用域：本地文件调用
*	功能：
*	参数：
*	参数：
*	返回值：
*	创建人：
*	修改历史：（每条详述）
\********************************************************/
void Send_COM3_Load(unsigned char DataLength,unsigned char nCommand,unsigned char DataTemp)
{
	unsigned char Length=0;
	unsigned char Command=0;
	unsigned char Data=0;
	Length=DataLength;
	Command=nCommand;
	Data=DataTemp;
	SRAM_Init();
	SRAM_Write(SHANDLE_DATA_VAILD+0,VAILD_2);
	SRAM_Write(SHANDLE_DATA_VAILD+1,0x24);
	SRAM_Write(SHANDLE_DATA_VAILD+2,Length);
	SRAM_Write(SHANDLE_DATA_VAILD+3,Command);
	if(Length==4)
	{
		SRAM_Write(SHANDLE_DATA_VAILD+4,0x0d);
	}
	else if(Length==5)
	{
		SRAM_Write(SHANDLE_DATA_VAILD+4,Data);
		SRAM_Write(SHANDLE_DATA_VAILD+5,0x0d);
	}
}
/********************************************************\
*	函数名：Handle_Set_Par
	作用域：本地文件调用
*	功能:测试外部设备的功能
*	参数：
*	参数：
*	返回值：
*	创建人：
*	修改历史：（每条详述）
\********************************************************/
void Handle_Set_Par(unsigned char nLength)
{
	unsigned int i=0;
	for(i=0;i<sizeof(gGeneral_Buffer);i++) 	 	gGeneral_Buffer[i]=0;
	for(i=0;i<nLength-3;i++)
	{
		gGeneral_Buffer[i+1+9]=gHandle_Buffer[i+3];
	}
	Do_Par();
	Waiting_Allow_Send();
	Send_COM3_SubProgram(5,HANDLE_SET_PAR_ECHO,COMMAND_OK);
	gTimer=0;
	while(gTimer<50){}
}

/********************************************************\
*	函数名：Handle_Set_TcpIp
	作用域：本地文件调用
*	功能:测试外部设备的功能
*	参数：
*	参数：
*	返回值：
*	创建人：
*	修改历史：（每条详述）
\********************************************************/
void Handle_Set_TcpIp(void)
{
	unsigned int i,j,k;
	unsigned char APN_Length=0;
	unsigned char Check_Data=0;
	//在gHandle_Buffer[i]中表示IP地址的第一个数
	i=3;
	while(1)
	{
		if(gHandle_Buffer[i]==0x00) break;
		i++;
		if(i>16+3) return;
	}
	//一，+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//1-1,存储IP的数据值到相应的BUFFER1中,存储IP地址的长度，不包括长度本身
	Check_Data=0;
	SRAM_Init();
	SRAM_Write(GPRS_IP_ADDRESS1_2+0,i-3);
	for(j=3;j<i;j++)
	{
		Check_Data += gHandle_Buffer[j];
		SRAM_Write(GPRS_IP_ADDRESS1_2+j-2,gHandle_Buffer[j]);
	}
	SRAM_Write(GPRS_IP_ADDRESS1_CHECK_2,Check_Data);

	//1-2,将IP地址的SOCKET数据存储在buffer1相应的缓冲区中，因为上面gHandle_Buffer[i]=0x00
	i++;
	gIP_Port_Num = gHandle_Buffer[i];
	gIP_Port_Num<<=8;
	gIP_Port_Num += gHandle_Buffer[i+1];
	gIP_Port_Num<<=8;
	gIP_Port_Num += gHandle_Buffer[i+2];
	gIP_Port_Num<<=8;
	gIP_Port_Num += gHandle_Buffer[i+3];
	SRAM_Init();
	Check_Data=ASCII(gIP_Port_Num/10000);
	SRAM_Write(GPRS_SOCKET1_2+0,ASCII(gIP_Port_Num/10000));
	Check_Data += ASCII((gIP_Port_Num%10000)/1000);
	SRAM_Write(GPRS_SOCKET1_2+1,ASCII((gIP_Port_Num%10000)/1000));
	Check_Data += ASCII((gIP_Port_Num%1000)/100);
	SRAM_Write(GPRS_SOCKET1_2+2,ASCII((gIP_Port_Num%1000)/100));
	Check_Data += ASCII((gIP_Port_Num%100)/10);
	SRAM_Write(GPRS_SOCKET1_2+3,ASCII((gIP_Port_Num%100)/10));
	Check_Data += ASCII(gIP_Port_Num%10);
	SRAM_Write(GPRS_SOCKET1_2+4,ASCII(gIP_Port_Num%10));
	SRAM_Write(GPRS_SOCKET1_2+5,Check_Data);
	OperateSPIEnd();
	//1-3,TCP开关量的判断处理
	gON_OFF_Temp0 &=~ TCP_LAND_ON_OFF_1;
	gStatus2_Flag |= TCP_ON_OFF_F_1;

	//1-4，APN方面的判断处理
	k=i+5;
	while(1)
	{
		if(gHandle_Buffer[k]==0x00) break;
		k++;
		if(k>gHandle_Buffer[1]) return;
	}
	APN_Length=k-i-5;
	if( (APN_Length>=4)&&(APN_Length<=25) )
	{
		SRAM_Init();
		SRAM_Write(GPRS_APN_ADDRESS2_2+0,APN_Length);
		Check_Data=0;
		for(j=i+5;j<k;j++)
		{
			SRAM_Write(GPRS_APN_ADDRESS2_2+(j-i-5)+1,APN_Length);
			Check_Data += gHandle_Buffer[j];
		}
		SRAM_Write(GPRS_APN_END_2,Check_Data);
	}
	else
	{
		SRAM_Write(GPRS_APN_ADDRESS2_2,sizeof(APN_DEFAULT));
		//5，存储APN的数据值到相应的BUFFER1中
		Check_Data=0;
		for(i=0;i<sizeof(APN_DEFAULT);i++)
		{
			SRAM_Write(GPRS_APN_ADDRESS2_2+i+1,APN_DEFAULT[i]);
			Check_Data += APN_DEFAULT[i];
		}
		SRAM_Write(GPRS_APN_END_2,Check_Data);
	}
	//将TCP的开关量的信息存储在相应的FLASH中
	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO2_ONE_2);OperateSPIEnd();
	WriteOneByteToBuffer2(ON_OFF_V1_2,gON_OFF_Temp0);OperateSPIEnd();
	RFlash_Sector(FLASH_INFO2_ONE_2);
	Re430_Flash(1);
	RX2_Ready();RX3_Ready();RX4_Ready();
	//二，++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//注意，这里在程序刚上电的最开始已经检查过了FLASH的第1，第2扇区的数据
	//在这里则不需要在继续检查
	//读第一个扇区的数据到buffer2中
	gTimer=0;
	while(gTimer<500){}
	Clear_Exter_WatchDog();
	//2-1，将IP地址数据已经写入相应的buffer2中
	i=3;
	while(1)
	{
		if(gHandle_Buffer[i]==0x00) break;
		i++;
		if(i>16+3) return;
	}
	//这个时候的i指向0x00
	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO1_ONE_2);OperateSPIEnd();
	WriteOneByteToBuffer2(GPRS_IP_ADDRESS1_F_2,i-3);
	for(j=3;j<i;j++)
	{
		WriteNextByteToBuffer2(gHandle_Buffer[j]);
	}
	OperateSPIEnd();
	gGeneral_Flag|=TCP_IP_VALID_1;
	gStatus2_Flag|=IP_VAILD_F_1;
	//2-2，将SOCKET的数据写入相应的buffer2中
	OperateSPIEnd();
	WriteOneByteToBuffer2(GPRS_SOCKET1_F_2,ASCII(gIP_Port_Num/10000));
	WriteNextByteToBuffer2(ASCII((gIP_Port_Num%10000)/1000));
	WriteNextByteToBuffer2(ASCII((gIP_Port_Num%1000)/100));
	WriteNextByteToBuffer2(ASCII((gIP_Port_Num%100)/10));
	WriteNextByteToBuffer2(ASCII(gIP_Port_Num%10));
	OperateSPIEnd();
	//2-3,将APN的数据写入到相应的buffer2中
	//i指向APN的第一个字节
	k=i+6;
	while(1)
	{
		if(gHandle_Buffer[k]==0x00) break;
		k++;
		if(k>gHandle_Buffer[1]) return;
	}
	APN_Length=k-i-6;
	WriteOneByteToBuffer2(GPRS_APN_DATA_F_2,APN_Length);
	for(j=i+6;j<k;j++)
	{
		WriteNextByteToBuffer2(gHandle_Buffer[j]);
	}
	OperateSPIEnd();

	gTimer=0;
	while(gTimer<50){}
	Clear_Exter_WatchDog();
	RFlash_Sector(FLASH_INFO1_ONE_2);
	Re430_Flash(0);
	Waiting_Allow_Send();
	Send_COM3_SubProgram(5,HANDLE_SET_TCPIP_ECHO,COMMAND_OK);
	gCheck_Par_Timer=CHECK_SYS_PAR_TIME+1;
	Init_Buffer2();
	RX2_Ready();
	RX3_Ready();
	RX4_Ready();


}
/********************************************************\
*	函数名：Handle_Set_Time
	作用域：本地文件调用
*	功能:测试外部设备的功能
*	参数：
*	参数：
*	返回值：
*	创建人：
*	修改历史：（每条详述）
\********************************************************/
void Handle_Set_Time(void)
{
	unsigned int i=0,j=0;
	gCycle_TCP = gHandle_Buffer[3];
	gCycle_TCP <<= 8;
	gCycle_TCP += gHandle_Buffer[4];
	gCycle_GSM = gHandle_Buffer[5];
	gCycle_GSM <<= 8;
	gCycle_GSM += gHandle_Buffer[6];

	if( (gCycle_TCP>0)&&(gCycle_TCP<5) )	gCycle_TCP=5;
	if( (gCycle_GSM>0)&&(gCycle_GSM<5) )	gCycle_GSM=5;
	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO2_ONE_2);OperateSPIEnd();
	WriteOneByteToBuffer2(TCP_TIMER_2,gHandle_Buffer[3]);
	WriteNextByteToBuffer2(gHandle_Buffer[4]);
	WriteNextByteToBuffer2(gHandle_Buffer[5]);
	WriteNextByteToBuffer2(gHandle_Buffer[6]);
	OperateSPIEnd();
	RFlash_Sector(FLASH_INFO2_ONE_2);
	Re430_Flash(1);
	//1，IP地址有效果，并且2，TCP的开关量是否开启
	if( (gGeneral_Flag&TCP_IP_VALID_1)&&((gON_OFF_Temp0&TCP_LAND_ON_OFF_1)==0) )
	{
		if( (gCycle_TCP>0)&&(gCycle_Send_Status!=ALARM_DATA)&&(gCycle_Send_Status!=PASS_DATA) )
		{
			gCycle_Send_Status=TRACK_DATA_TIME;
		}
		else if( (gCycle_TCP==0)&&(gCycle_Send_Status==TRACK_DATA_TIME))
		{
			gCycle_Send_Status=NOP_DATA;
		}
	}
	//1，IP地址无效，或者TCP的开关量关闭
	else
	{
		if( (gCycle_GSM>0)&&(gCycle_Send_Status!=ALARM_DATA)&&(gCycle_Send_Status!=PASS_DATA) )
		{
			gCycle_Send_Status=TRACK_DATA_TIME;
		}
		else if( (gCycle_GSM==0)&&(gCycle_Send_Status==TRACK_DATA_TIME))
		{
			gCycle_Send_Status=NOP_DATA;
		}
	}
	Waiting_Allow_Send();
	Send_COM3_SubProgram(5,HANDLE_SET_TIME_ECHO,COMMAND_OK);
	gTimer=0;
	while(gTimer<50){}
}

/********************************************************\
*	函数名：Handle_Ask_Par
	作用域：本地文件调用
*	功能:测试外部设备的功能
*	参数：
*	参数：
*	返回值：
*	创建人：
*	修改历史：（每条详述）
\********************************************************/
void Handle_Ask_Par(void)
{
	unsigned int i=0;
	Ask_Par();
	SRAM_Init();
	SRAM_Write(SHANDLE_DATA_VAILD+0,VAILD_2);
	SRAM_Write(SHANDLE_DATA_VAILD+1,0x24);
	SRAM_Write(SHANDLE_DATA_VAILD+2,gGeneral_Buffer[0]+1+3);
	SRAM_Write(SHANDLE_DATA_VAILD+3,HANDLE_ASK_PAR_ECHO);
	for(i=1;i<=gGeneral_Buffer[0];i++)
	{
		SRAM_Write(SHANDLE_DATA_VAILD+3+i,gGeneral_Buffer[i]);
	}
	SRAM_Write(SHANDLE_DATA_VAILD+4+gGeneral_Buffer[0],0x0d);
}

/********************************************************\
*	函数名：Handle_Set_SMS
	作用域：本地文件调用
*	功能:
*	参数：
*	参数：
*	返回值：
*	创建人：
*	修改历史：（每条详述）
\********************************************************/
void Handle_Set_SMS(void)
{
	if(gDisp_Buffer_Point+gHandle_Buffer[1]-4+2+6<=sizeof(gDisp_Buffer))
	{
	    gDisp_Buffer[gDisp_Buffer_Point]=gHandle_Buffer[1]-4+2+6;
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]=0;
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]='@';
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]='%';
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]=gHandle_Buffer[1]-4+2+4;
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]=DISP_MAIN_WORD_DOWN;
	    gDisp_Buffer_Point++;
	    //从0xFF开始存储数据(转移整个中心的数据包)
	    Copy_String(gHandle_Buffer+3,gDisp_Buffer+gDisp_Buffer_Point,gHandle_Buffer[1]-4);
	    gDisp_Buffer_Point=gDisp_Buffer_Point+gHandle_Buffer[1]-4;
	    gDisp_Buffer[gDisp_Buffer_Point]='$';
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]='&';
	    gDisp_Buffer_Point++;
		Waiting_Allow_Send();
		Send_COM3_SubProgram(5,HANDLE_SET_SMS_ECHO,COMMAND_OK);
	}
}
/********************************************************\
*	函数名：Handle_Set_Phone
	作用域：本地文件调用
*	功能:
*	参数：
*	参数：
*	返回值：
*	创建人：
*	修改历史：（每条详述）
\********************************************************/
void Handle_Set_Phone(unsigned char nLength)
{
	unsigned int i=0;
	for(i=0;i<sizeof(gGeneral_Buffer);i++) 	 	gGeneral_Buffer[i]=0;
	for(i=0;i<nLength-3;i++)
	{
		gGeneral_Buffer[i+1+9]=gHandle_Buffer[i+3];
	}
	Do_Phone_Note();
	Waiting_Allow_Send();
	Send_COM3_SubProgram(5,HANDLE_SET_PHONE_NOTE_ECHO,COMMAND_OK);
}
/********************************************************\
*	函数名：Handle_Set_Dis
	作用域：本地文件调用
*	功能:
*	参数：
*	参数：
*	返回值：
*	创建人：
*	修改历史：（每条详述）
\********************************************************/
void Handle_Set_Dis(void)
{
	gCycle_Distance = gHandle_Buffer[3];
	gCycle_Distance <<= 8;
	gCycle_Distance += gHandle_Buffer[4];

	if(gHandle_Buffer[5]==1)
	{
		gON_OFF_Temp1 &=~ DISTANCE_TCP_ON_1;
		gStatus3_Flag |= DISTANCE_ON_F_1;
	}
	else if(gGeneral_Buffer[5]==0)
	{
		gON_OFF_Temp1 |= DISTANCE_TCP_ON_1;
		gStatus3_Flag &=~ DISTANCE_ON_F_1;
	}
	gCycle_TCP=0;
	gCycle_GSM=0;
	gCycle_Send_Status = NOP_DATA;
	//根据状态的改变，则需要重新更新扇区0中的数据，并重新计算校验核，写入扇区
	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO2_ONE_2);OperateSPIEnd();
	WriteOneByteToBuffer2(ON_OFF_V2_2,gON_OFF_Temp1);
	OperateSPIEnd();
	WriteOneByteToBuffer2(TCP_TIMER_2,0);
	WriteNextByteToBuffer2(0);
	WriteNextByteToBuffer2(0);
	WriteNextByteToBuffer2(0);
	WriteNextByteToBuffer2(gHandle_Buffer[3]);
	WriteNextByteToBuffer2(gHandle_Buffer[4]);
	OperateSPIEnd();
	RFlash_Sector(FLASH_INFO2_ONE_2);
	Re430_Flash(1);
	gPublic_Flag &=~ SET_DISTANCE_OK_F_1;
	gSet_Distance_Info_Count=WAIT_DISPINFO_TIME-2;

	//返回设置成功
	Waiting_Allow_Send();
	Send_COM3_SubProgram(5,HANDLE_SET_DISTANCE_ECHO,COMMAND_OK);
}
/********************************************************\
*	函数名：Handle_Test_Sram
	作用域：本地文件调用
*	功能:
*	参数：
*	返回值：
*	创建人：
*	修改历史：（每条详述）
\********************************************************/
void Handle_Test_Sram(void)
{
	unsigned int i=0,j=0,k=0;
	unsigned char Data_Temp=0;
	unsigned char Data_Read=0;
	//关闭中断1;停止中断对外部SRAM的操作
	IE2&=~URXIE1;            //USART1接收中断禁止
	j=0;
	for(k=0;k<=0x100;k++)
	{
		for(i=0;i<=0xFF;i++)
		{
			gGeneral_Buffer[i]=(char)(i+k);
		}
		Clear_Exter_WatchDog();
		SRAM_Init();
		for(i=0;i<=0xFF;i++)
		{
			SRAM_Write( (int)(k*0xFF+i),gGeneral_Buffer[i]);
		}
		SRAM_Init();
		for(i=0;i<=0xFF;i++)
		{
			Data_Temp=SRAM_Read((int)(k*0xFF+i));
			if(Data_Temp!=gGeneral_Buffer[i])
			{
				j=1;break;
			}
		}
		if(j==1)	break;
	}
	if(j==1)
	{
		Send_COM3_SubProgram(5,HANDLE_TEST_SRAM,COMMAND_ERROR);
	}
	else
	{
		Send_COM3_SubProgram(5,HANDLE_TEST_SRAM,COMMAND_OK);
	}
	IE2|=URXIE1;            //USART1接收中断允许
	RX2_Ready();
	RX3_Ready();
	RX4_Ready();
	gTimer=0;
	while(gTimer<50){}
}