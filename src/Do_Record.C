/********************************************************\
*	文件名：  Do_Record.h
*	创建时间：2005年6月21日
*	创建人：  
*	版本号：  1.00
*	功能：
*
*	修改历史：（每条详述）
	补充说明：与外接的行使记录仪相连接，串口1，9600，奇校验，全双工通讯方式
\********************************************************/
#include <msp430x14x.h>
#include <stdlib.h>
#include "General.h"
#include "Do_SRAM.h"
#include "D_Buffer.h"
#include "Other_Define.h"
#include "Uart01.h"
#include "Sub_C.h"
#include "Record_Protocol.h"
#include "Define_Bit.h"
#include "M22_AT.h"
#include "Disp_Protocol.h"
#include "Do_Other.h"
#include "W_Protocol.h"
#include "Record_Protocol.h"
#include "TA_Uart.h"
/*
#include "SPI45DB041.h"
#include "W_Protocol.h"
#include "Handle_Protocol.h"
#include "Do_Record.h"
#include "Define_Bit.h"
#include "Main_Init.h"
#include "Check_GSM.h"
#include "Do_Reset.h"
#include "Do_M22.h"
#include "Do_Handle.h"
#include "Do_GPS.h"
#include "Do_Disp.h"
#include "Do_Other.h"
*/
void Deal_Record_Data(unsigned int Length);
unsigned char Check_Record_Time(void);
/********************************************************\
*	函数名：Do_Record_Module()
	作用域：外部文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）

\********************************************************/
void Do_Record_Module(void)
{
	unsigned int i;
	unsigned int k;
	unsigned char DataTemp=0;
	if( (gReceive1_Over_Count>60)&&(gReceive1_Point>SRAM_RECORD_S)&&((gOther_Flag&RECORD_DATA_F_1)==0) )
	{
		//判断是否正确的数据包
		i=gReceive1_Point;
			//先判断包尾
		if( (SRAM_Read(i-1)==0x0a)&&(SRAM_Read(i-2)==0x0d) )
		{
			//如果包尾正确，则需要判断包长
			k=0;
			k=SRAM_Read(SRAM_RECORD_S+4);
			k<<=8;
			k=k+SRAM_Read(SRAM_RECORD_S+5);
			k=k+10;
			if( (i-SRAM_RECORD_S)==k )
			{
				if((gGeneral_Flag&TCP_PHONE_F_1)==0 )
				{
					#ifdef Debug_GSM_COM1
		            Send_COM1_Byte(ASCII((i-SRAM_RECORD_S)/0x100));
		            Send_COM1_Byte(ASCII(((i-SRAM_RECORD_S)%0x100)/0x10));
		            Send_COM1_Byte(ASCII((i-SRAM_RECORD_S)%0x10));
		            Send_COM1_Byte(':');
		            Send_COM1_Byte(ASCII(k/0x100));
		            Send_COM1_Byte(ASCII((k%0x100)/0x10));
		            Send_COM1_Byte(ASCII(k%0x10));
					Send_COM1_Byte('[');
					for(n=SRAM_RECORD_S;n<i;n++)
					{
						DataTemp=SRAM_Read(n);
						Send_COM1_Byte(ASCII(DataTemp/0x10));
						Send_COM1_Byte(ASCII(DataTemp%0x10));
						Clear_Exter_WatchDog();
					}
					Send_COM1_Byte(']');
					Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
					#endif
				}
				Deal_Record_Data(i-SRAM_RECORD_S);
				gNO_Sram_Data_Timer=0;
			}
		}
		gReceive1_Point=SRAM_RECORD_S;
	}
	//如果超过20秒无记录仪的数据发送，则需要清除外部SRAM.
	if(gNO_Sram_Data_Timer>20)
	{
		gNO_Sram_Data_Timer=0;
		gReceive1_Point=SRAM_RECORD_S;
		gReceive1_Over_Count=0;
		Send_COM1_String((unsigned char *)CLEAR_SRAM,sizeof(CLEAR_SRAM));
	}
}
/********************************************************\
*	函数名：Deal_Record_Command()
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	补充说明：将接收到的完整的数据包已经放在
	外部SRAM的SRAM_DO_S-----SRAM_DO_E区域。
	则这个子程序部分是需要将命令字提取并分析出来
\********************************************************/
void Deal_Record_Data(unsigned int Length)
{
	unsigned char nCmd=0;
	unsigned int iLength=0,i=0;
	unsigned char nStatus;
	unsigned char nTemp=0;
	unsigned char Check_Record=0;
	iLength=Length;
	SRAM_Init();
	nStatus=SRAM_Read(SRAM_RECORD_S);
	switch(nStatus)
	{
		//1，消息类型为0的消息包
		case	NORMAL_TYPE:
		{
			gRecord_Data_Timer=0;
			gStatus2_Flag&=~RECORD_VAILD_F_1;
			gReceive_Record_Info_Timer=0;
			gStatus=NORMAL_TYPE;
			SRAM_Init();
			//读取命令字
			nCmd=SRAM_Read(SRAM_RECORD_S+3);
			/**************************************************************************************\
			1,	返回记录仪发送的记录仪系统数据:信息有，当前速度(米/秒)
				年，月，日，时，分，秒，记录仪状态字节1，记录仪状态字节2
				补充说明：A：速度需要用来做超速和制动方面的处理。同时定期向显示屏显示
						  B：如果判断记录仪的时间和GPS有效的状态下，误差超过2秒则，需要对记录仪的时间进行矫正，
						     同时，如果GPS无效果的状态下，GPS的时间也无效果，则转发记录仪的时间到显示屏。
						  C：记录仪的状态字节1的信息包括如下：	BIT0；间隔距离上发的数据时间到。
						  										BIT1；疲劳驾驶提示标志。
						  										BIT2；疲劳报警标志
						  										BIT3；记录仪打印机的工作状态
						  										BIT4；记录仪USB的工作状态

			00 55 7A 40 00 09 00 00 05 09 02 11 37 09 02 00 45 0D 0A
			\************************************************************************************/
			if(nCmd==K_RECEIVE_RECORDDATA)
			{
				//1，速度的处理
				SRAM_Init();
				gSpeed_Record=SRAM_Read(SRAM_RECORD_S+7);
				gSpeed_Disp=gSpeed_Record;
				gRecord_Null_Count=0;
				gPublic_Flag &=~ RECORD_NULL_F_1;
				//2，年月日时分秒的处理,如果判断记录仪的时间与GPS的时间误差超过，则需要对记录仪进行时间矫正的设置,但只设置一次
				i=Check_Record_Time();
				if(   (i==1)
					&&((gPublic_Flag&CHECK_TIME_F_1)==0)	)
				{
					gPublic_Flag|=SET_RECORD_TIME_F_1;
				}
				//------------------------------
				//3，状态位的处理
				gRecord_Status=SRAM_Read(SRAM_RECORD_S+14);
				//3-1，判断BIT0，属于需要发送当前定位点的数据
				if(   (gRecord_Status&J_DISTANCE_DATA_F_1)
					&&(gCycle_Send_Status==TRACK_DATA_DISTANCE)
					&&((gPublic_Flag&ALLOW_SEND_DISTANCE_F_1)==0)	 )
				{
//					Send_COM1_String( (unsigned char *)DISTANCE_DATA,sizeof(DISTANCE_DATA) );
					gPublic_Flag|=ALLOW_SEND_DISTANCE_F_1;
				}
				else
				{
					if(gPublic_Flag&ALLOW_SEND_DISTANCE_F_1)
					{
						if( (gInternal_Flag&SEND_CIRCLE_F_1)==0 )
						{
							Circle_Data_TCP(VEHICLE_RUNDATA_UP,0);
							gInternal_Flag|=SEND_CIRCLE_F_1;
							gPublic_Flag&=~ALLOW_SEND_DISTANCE_F_1;
							//处于GSM的工作方式下
							if(gM22_Status==GSM_WORK)
							{
								//属于常连在线的方式
								if((gON_OFF_Temp1&DISTANCE_TCP_ON_1)==0)
								{
									gInternal_Flag&=~SEND_CIRCLE_F_1;
									gInternal_Flag&=~ALLOW_DATA_F_1;
									gCircle_Buffer_Point=0;
								}
								//属于不常在线的方式，则需要登陆
								else
								{
									//在登陆部分有处理和判断
								}
							}
						}
					}
				}
				//3-2,判断BIT1,属于疲劳驾驶提示的标志
				if(gRecord_Status&J_TIRE_NOTICE_F_1)
				{
					gFore_Tire_Count++;
					if(gFore_Tire_Count>20)
					{
						gDisp_Status|=S_TIRE_NOTICE_F_1;
						gFore_Tire_Count=0;
					}
					else
					{
						gDisp_Status&=~S_TIRE_NOTICE_F_1;
					}
				}
				else
				{
					gFore_Tire_Count=0;
					gDisp_Status&=~S_TIRE_NOTICE_F_1;
				}
				//3-3,判断BIT2，属于疲劳驾驶报警的标志
				if(gRecord_Status&J_TIRE_ALARM_F_1)
				{
					gOver_Tire_Count++;
					gStop_Tire_Count=0;
				}
				else
				{
					gOver_Tire_Count=0;
					gStop_Tire_Count++;
				}
				//3-4,判断BIT3，属于打印机的工作状态
				if(gRecord_Status&J_PRINTER_STATUS_F_1)	gDisp_Status|=S_PRINTER_STATUS_F_1;
				else									gDisp_Status&=~S_PRINTER_STATUS_F_1;

				//3-5,判断BIT4，属于USB的工作状态
				if(gRecord_Status&J_USB_STATUS_F_1)		gDisp_Status|=S_USB_STATUS_F_1;
				else									gDisp_Status&=~S_USB_STATUS_F_1;

				//4,接收到记录仪的数据，则需要返回一个状态包，包括经度，纬度.
				//------------------------------
		    	Send_COM1_Byte(gStatus);
		    	Check_Record = gStatus;
		    	Send_COM1_Byte(0xAA);
		    	Check_Record ^= 0xAA;
		    	Send_COM1_Byte(0x75);
		    	Check_Record ^= 0x75;
		    	Send_COM1_Byte(K_SEND_GPSDATA);
		    	Check_Record ^= K_SEND_GPSDATA;
		    	Send_COM1_Byte(0);
		    	Check_Record ^= 0;
		    	Send_COM1_Byte(10);
		    	Check_Record ^= 10;
		    	Send_COM1_Byte(0x00);
		    	Check_Record ^= 0x00;
		    	if(gGeneral_Flag&GPS_VALID_F_1)
		    	{
		    		Send_COM1_Byte(0x01);
		    		Check_Record ^= 0x01;
		    	}
				else
				{
					Send_COM1_Byte(0x00);
					Check_Record ^= 0x00;
				}
		    	Send_COM1_Byte(gLongitude/0x1000000);
		    	Check_Record ^= gLongitude/0x1000000;
		    	Send_COM1_Byte(gLongitude%0x1000000/0x10000);
		    	Check_Record ^= gLongitude%0x1000000/0x10000;
		    	Send_COM1_Byte(gLongitude%0x10000/0x100);
		    	Check_Record ^= gLongitude%0x10000/0x100;
		    	Send_COM1_Byte(gLongitude%0x100);
		    	Check_Record ^= gLongitude%0x100;
		    	Send_COM1_Byte(gLatitude/0x1000000);
		    	Check_Record ^= gLatitude/0x1000000;
		    	Send_COM1_Byte(gLatitude%0x1000000/0x10000);
		    	Check_Record ^= gLatitude%0x1000000/0x10000;
		    	Send_COM1_Byte(gLatitude%0x10000/0x100);
		    	Check_Record ^= gLatitude%0x10000/0x100;
		    	Send_COM1_Byte(gLatitude%0x100);
		    	Check_Record ^= gLatitude%0x100;
		    	Send_COM1_Byte(gRecord_Status);
		    	Check_Record ^= gRecord_Status;
		    	Send_COM1_Byte(0);
		    	Check_Record ^= 0;
		    	Send_COM1_Byte(Check_Record);					//校验核
		    	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);		//包尾
			}
			/**************************************************************************************\
			2,主控接收记录仪的驾驶员信息的响应（不管是主控主动查询的还是记录仪主动发送的，均为同一个命令字）
				因为需要显示屏实时的显示当前记录仪IC卡中的驾驶员信息。所以有一些特殊的处理方式
				A，主控系统重新上电或者是系统程序重新启动的时候，则需要先进行一次进行采集记录仪当前驾驶员信息
					的命令。
				B，主控已经开机，正常运行的过程中，如果此时记录仪的驾驶员信息更新，则需要记录仪主动发送一个
					当前驾驶员信息的命令给主控。

				不管是通过什么方式，主控系统只要得到一次当前记录仪的驾驶员信息，则需要将此驾驶员的信息
				存储在FLASH中。并设置需要发送驾驶员信息给显示屏的标志。

				在系统程序运行过程中，如果判断显示中断处于开机的状态，并有发送驾驶员标志给显示屏。
			\**************************************************************************************/
			else if(nCmd==K_GET_DRIVEDATA_E_ECHO)
			{
				gReceive_Record_Info_Timer=0;
				SRAM_Init();
				SRAM_Write(SRAM_DRIVER_INFO_VAILD,VAILD_2);
				for(i=0;i<29;i++)
				{
					nTemp=SRAM_Read(SRAM_RECORD_S+7+i);
					SRAM_Write(SRAM_DRIVER_INFO_CODE+i,nTemp);
				}
				///////////////////////////////
				gDisp_Buffer_Point=0;
				if(gDisp_Buffer_Point+37<=sizeof(gDisp_Buffer))
				{
					gDisp_Buffer[gDisp_Buffer_Point]=37;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=0;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='@';
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='%';
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=35;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=DISP_MAIN_IC_INFO_DOWN;
					gDisp_Buffer_Point++;
					SRAM_Init();
					for(i=0;i<29;i++)
					{
		 				gDisp_Buffer[gDisp_Buffer_Point]=SRAM_Read(SRAM_DRIVER_INFO_CODE+i);
						gDisp_Buffer_Point++;
					}
					gDisp_Buffer[gDisp_Buffer_Point]='$';
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='&';
					gDisp_Buffer_Point++;
					gGet_Driver_Info_Count=0;
					//gDriver_Info=WAIT_SEND_DRIVER_INFO;
				}
				/////////////////////////////////
			}
			/**************************************************************************************\
			3,收到主控向记录仪设置间隔记录的记录仪的响应
			\**************************************************************************************/
			else if(nCmd==K_SET_INTERDISTANCE_ECHO)
			{
				gPublic_Flag|=SET_DISTANCE_OK_F_1;
				gSet_Distance_Info_Count=0;
			}
			/**************************************************************************************\
			4,收到主控向记录仪设置时间的记录仪的响应
			\**************************************************************************************/
			else if(nCmd==G_SET_TIME_EHCO)
			{
				gPublic_Flag&=~SET_RECORD_TIME_F_1;
				gPublic_Flag|=CHECK_TIME_F_1;
			}
			break;
		}
		/*
		========================================================================
		二，消息类型为1的消息包(转移给中心的数据)
			如果记录仪有数据发送给中心，则首先需要进行TCP登陆操作
			如果登陆操作失败，则可以判断是否允许SMS的形式发送
		========================================================================
		*/
		case	CENTER_TYPE:
		{
			gRecord_Data_Timer=0;
			gStatus2_Flag&=~RECORD_VAILD_F_1;
			gReceive_Record_Info_Timer=0;
			gStatus=CENTER_TYPE;
			gOther_Flag|=RECORD_DATA_F_1;
			gOther_Flag|=ALLOW_SEND_RECORD_F_1;
			SRAM_Init();
			SRAM_Write(SRAM_SEND_VAILD,VAILD_2);
			SRAM_Write(SRAM_SEND_LENGTH_H,(iLength-3)/0x100);
			SRAM_Write(SRAM_SEND_LENGTH_L,(iLength-3)%0x100);
			for(i=0;i<iLength-3;i++)
			{
				nTemp=SRAM_Read(SRAM_RECORD_S+1+i);
				SRAM_Write(SRAM_SEND_S+i,nTemp);
			}
			Judge_SMS_Way();
			break;
		}
		/*
		========================================================================
		三，消息类型为2的消息包（转移给显示屏的数据）
		========================================================================
		*/
		case	DISP_TYPE:
		{
			gRecord_Data_Timer=0;
			gStatus2_Flag&=~RECORD_VAILD_F_1;
			gStatus=DISP_TYPE;
			if(gDisp_Buffer_Point+iLength+8<=sizeof(gDisp_Buffer))
			{
				gDisp_Buffer[gDisp_Buffer_Point]=iLength+8;
				gDisp_Buffer_Point++;
				gDisp_Buffer[gDisp_Buffer_Point]=0;
				gDisp_Buffer_Point++;
				gDisp_Buffer[gDisp_Buffer_Point]='@';
				gDisp_Buffer_Point++;
				gDisp_Buffer[gDisp_Buffer_Point]='%';
				gDisp_Buffer_Point++;
				gDisp_Buffer[gDisp_Buffer_Point]=iLength+6;
				gDisp_Buffer_Point++;
				gDisp_Buffer[gDisp_Buffer_Point]=DISP_ASK_RECORD_INFO_ECHO_DOWM;
				gDisp_Buffer_Point++;
				for(i=0;i<iLength;i++)
				{
					gDisp_Buffer[gDisp_Buffer_Point] = SRAM_Read(SRAM_RECORD_S+i) ;
					gDisp_Buffer_Point++;
				}
				gDisp_Buffer[gDisp_Buffer_Point]='$';
				gDisp_Buffer_Point++;
				gDisp_Buffer[gDisp_Buffer_Point]='&';
				gDisp_Buffer_Point++;
			}
			break;
		}
		default:
			break;
	}
}
/********************************************************\
*	函数名：Deal_Record_Command()
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned char Check_Record_Time(void)
{
	SRAM_Init();
	if(   (gGeneral_Flag&GPS_VALID_F_1)
		&&((gPublic_Flag&SET_RECORD_TIME_F_1)==0)
		&&(gSecond>=55) )
	{
		if(gYear!=SRAM_Read(SRAM_RECORD_S+8) )				return(1);
		else if(gMonth!=SRAM_Read(SRAM_RECORD_S+9))			return(1);
		else if(gDate !=SRAM_Read(SRAM_RECORD_S+10))		return(1);
		else if(gHour !=SRAM_Read(SRAM_RECORD_S+11))		return(1);
		else if(gMinute!=SRAM_Read(SRAM_RECORD_S+12))		return(1);
		else												return(0);
	}
	else
	{
		return(0);
	}
}