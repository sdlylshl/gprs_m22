/********************************************************\
*	文件名：  Do_GSM.C
*	创建时间：2004年12月7日
*	创建人：  
*	版本号：  1.00
*	功能：	  针对GPS方面数据的处理
*	文件属性：公共文件
*	修改历史：（每条详述）
\********************************************************/
#include <msp430x14x.h>
#include "TA_Uart.h"
#include "General.h"
#include "M22_AT.h"
#include "Sub_C.h"
#include "Define_Bit.h"
#include "Uart01.h"
#include "Handle_Protocol.h"
#include "SPI45DB041.h"
#include "D_Buffer.h"
#include "Disp_Protocol.h"
#include "Other_Define.h"
#include "W_Protocol.h"
#include "Do_Other.h"
#include "Other_Define.h"
#include "Record_Protocol.h"
#include "Msp430_Flash.h"
#include "Do_SRAM.h"
/*
#include "Main_Init.h"
#include "Check_GSM.h"
#include "Do_Reset.h"
#include "Do_GSM.h"
#include "Do_Handle.h"
#include "Do_GPS.h"
#include "Do_Disp.h"

*/
//#define Debug_GPS_COM1

unsigned char GPS_Buffer[80];		//接收存储GPS数据，只能接收一帧数据d
unsigned char GPS_Buffer_Point=0;	//接收指针
unsigned char GPS_Send_Count=0;
const unsigned char Tab[4][12]={ { 31,29,31,30,31,30,31,31,30,31,30,31 },
                                 { 31,28,31,30,31,30,31,31,30,31,30,31 },
                                 { 31,28,31,30,31,30,31,31,30,31,30,31 },
                                 { 31,28,31,30,31,30,31,31,30,31,30,31 } };

/********************************************************\
*	函数名：Do_GPS_Module
	作用域：外部文件调用
*	功能：	与手柄通讯模块。
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	补充说明：
			关于GPS数据的接收处理，则采用中断接收处理的方式，一旦中断接收完
			数据后，在Do_GPS_Module里处理接收到的GPS数据，GPS_Buffer[]缓冲
			负责接收GPS数据，但只能存储一个GPS数据包。全局变量gGPS_Receive_over
			作为判断是否接收数据全部完成的记数，一旦中断接收到一个字节的数据
			则gGPS_Receive_over=0,如果数据接收完，则不会在进入中断了，则
			gGPS_Receive_over会在定时中断中++，超过一定时间，仍然未在进入接收
			中断接收到数据，则表示一个完整的GPS数据包接收完，则进入处理
			GPS数据处理部分。
\********************************************************/
void Do_GPS_Module(void)
{
	unsigned int i=0,j=0,k=0;
	unsigned char CYear=0x05,CMonth=0x01,CDate=0x01,CHour=0x12,CMinute=0x00,CSecond=0x00;
//	unsigned char nFlag=0;
	unsigned char Check_Data=0;
	unsigned char Check_Record=0;
//	nFlag=gAlarm_Type;
	/********************************************************\
	A，判断是否需要进行GPS的复位操作
		判断是否满足复位GPS的标志位置位
		具体此GPS置位的标志位的条件：每一次的无效点则gGPS_Invaild_Timer++
		如果连续GPS_INVALID_COUNT的时间均为无效点，则需要置复位GPS的标志位GPS_RESET_F_1
		但一旦有有效点，则需要将无效点的计数清为0。

		如果满足复位GPS的条件，则需要将GPS的复位管脚设置为低电平，持续GPS_LOW_TIME时间后在置高
	\********************************************************/
	/********************************************************\
	A，如果连续100秒，GPS无输出或者输出格式无效，则进行一次GPS模块掉电处理
		如果连续5次进行了这样的操作，则以后不在进行GPS模块的掉电处理，则回直接设置
		GPS模块故障的标志位。
	\********************************************************/
	if(gGPS_No_Output_Timer>NO_OUTPUT_TIME)
	{
		gGPS_No_Output_Timer=0;
		gNO_GPS_Reset_Count=0;
		gCommon_Flag|=GPSNO_RESET_F_1;
		gStatus3_Flag|=GPS_OUTPUT_WAY_F_1;
		//如果GPS无输出，同时判断到显示终端和手柄无任何时候进行的通讯，则需要关闭看门狗，使得程序重新启动
		if(  ((gStatus1_Flag&DISPLAY_ON_F_1)==0)
		   &&((gStatus1_Flag&HANDLE_ON_0_F_1)==0)
		   &&((gStatus1_Flag&HANDLE_ON_1_F_1)==0)	)
		{
			gOther_Flag|=RESET_PROGRAME_F_1;
		}
	}
	/********************************************************\
	B 如果GPS超过5秒无输出，则如果以前是有效标志，则需要将其设置为
		无效标志
	\********************************************************/
	else if((gGPS_No_Output_Timer>20)&&(gGeneral_Flag&GPS_VALID_F_1) )
	{
		gGeneral_Flag&=~GPS_VALID_F_1;
	}
	/********************************************************\
	D，判断是否接受完数据:
		在整个GPS的数据处理中。
		1，无效点的累计和复位GPS标志的判断
		2，处于设防状态下，速度超过3.6*4的有效点的累计
		（初步以判断连续有30个有效点满足条件均可，产生被盗报警信息）
	\********************************************************/
	else if( (gGPS_Receive_Over>90)&&(GPS_Buffer_Point>1) )
	{
		//判断接收到正确的包头，包尾
        if((Compare_String((unsigned char *)String_GPRMC,GPS_Buffer,sizeof(String_GPRMC))==1)&&(GPS_Buffer[GPS_Buffer_Point-1]==0x0a))
		{
			//关GPS接收中断
			//比较器A的中断关闭
			//TAUARTCTL&=~TAUARTIE;
			if((gGeneral_Flag&TCP_PHONE_F_1)==0)
			{
				GPS_Send_Count++;
				if(GPS_Send_Count>10)
				{
					GPS_Send_Count=0;
					Send_COM1_String(GPS_Buffer,6);
				}
			}
			gNO_GPS_Reset_Count=0;
			gGPS_No_Output_Timer=0;
			gGeneral_Flag&=~GPS_VALID_F_1;
			gCommon_Flag&=~GPSNO_RESET_F_1;
			gStatus3_Flag&=~GPS_OUTPUT_WAY_F_1;
			i=0;
			while(1)
			{
				//处理时，分，秒信息
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				else
				{
					CHour  =ASCIITOHEX( GPS_Buffer[i+1],GPS_Buffer[i+2] );
					CMinute=ASCIITOHEX( GPS_Buffer[i+3],GPS_Buffer[i+4] );
					CSecond=ASCIITOHEX( GPS_Buffer[i+5],GPS_Buffer[i+6] );
					i++;
				}
				//处理GPS数据的有效性
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')		break;
				if(GPS_Buffer[i+1]!='A')
				{
					gGeneral_Flag &=~GPS_VALID_F_1;
				}
				else
				{
					gGeneral_Flag |= GPS_VALID_F_1;
					gGPS_Invaild_Timer=0;
				}
				i++;
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				else
				{	//纬度,只有在有效的状态下，才更新纬度值
					if(gGeneral_Flag&GPS_VALID_F_1)
					{
						gLatitude=ASCIITOHEX( 0,GPS_Buffer[i+1] )*10+ASCIITOHEX( 0,GPS_Buffer[i+2] );
						gLatitude=gLatitude*60;
						gLatitude=gLatitude+ASCIITOHEX( 0,GPS_Buffer[i+3] )*10+ASCIITOHEX( 0,GPS_Buffer[i+4] );
						gLatitude=gLatitude*10+ASCIITOHEX( 0,GPS_Buffer[i+6] );
						gLatitude=gLatitude*10+ASCIITOHEX( 0,GPS_Buffer[i+7] );
						gLatitude=gLatitude*10+ASCIITOHEX( 0,GPS_Buffer[i+8] );
						gLatitude=gLatitude*10+ASCIITOHEX( 0,GPS_Buffer[i+9] );
						gLatitude=gLatitude*6;
					}
					i++;
				}
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				i++;
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				else
				{	//经度,只有在有效的状态下才更新经度值
					if(gGeneral_Flag&GPS_VALID_F_1)
					{
						gLongitude=ASCIITOHEX( 0,GPS_Buffer[i+1] );
						gLongitude=gLongitude*10+ASCIITOHEX( 0,GPS_Buffer[i+2] );
						gLongitude=gLongitude*10+ASCIITOHEX( 0,GPS_Buffer[i+3] );
						gLongitude=gLongitude*60;
						gLongitude=gLongitude+ASCIITOHEX( 0,GPS_Buffer[i+4] )*10+ASCIITOHEX( 0,GPS_Buffer[i+5] );
						gLongitude=gLongitude*10+ASCIITOHEX( 0,GPS_Buffer[i+7] );
						gLongitude=gLongitude*10+ASCIITOHEX( 0,GPS_Buffer[i+8] );
						gLongitude=gLongitude*10+ASCIITOHEX( 0,GPS_Buffer[i+9] );
						gLongitude=gLongitude*10+ASCIITOHEX( 0,GPS_Buffer[i+10] );
						gLongitude=gLongitude*6;
					}
					i++;
				}
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				i++;
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				else
				{
					j=0;
					gSpeed_Gps=0;
					while(1)
					{
					    if(GPS_Buffer[i+j+1]=='.') break;
					    if(GPS_Buffer[i+j+1]==',') break;
					    gSpeed_Gps=gSpeed_Gps*10+ASCIITOHEX(0,GPS_Buffer[i+j+1]);
					    j++;
					    if( j>=GPS_Buffer_Point ) break;
					}
					gSpeed_Gps=gSpeed_Gps/2;
					i++;
				}
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				else
				{
					j=0;
					k=0;
					while(1)
					{
					    if( GPS_Buffer[i+j+1]=='.' )break;
					    if( GPS_Buffer[i+j+1]==',' )break;
					    k=k*10+ASCIITOHEX( 0,GPS_Buffer[i+j+1] );
					    j++;
					    if( j>=GPS_Buffer_Point ) break;
					}
					gAzimuth=k/15;
					i++;
				}
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				else
				{
					CDate = ASCIITOHEX( GPS_Buffer[i+1],GPS_Buffer[i+2] );
					CMonth =ASCIITOHEX( GPS_Buffer[i+3],GPS_Buffer[i+4] );
					CYear = ASCIITOHEX( GPS_Buffer[i+5],GPS_Buffer[i+6] );
					i++;
				}
				break;
			}
			/********************************************************\
			1;	上面是处理接收完GPS数据
				下面是需要进行接收的GPS数据进行跟进一步的处理
				进一步的处理时间，年，月，日
			\********************************************************/
			while(1)
			{
				i=CHour/16*10+CHour%16;
				i=i+8;
				if( i<24 ){ CHour=i/10*16+i%10;break; }
				i=i-24;
				CHour=i/10*16+i%10;
				i=CDate/16*10+CDate%16;
				j=CYear/16*10+CYear%16;
				k=CMonth/16*10+CMonth%16;
				i++;
				if( i<=Tab[j/4][k-1] ){ CDate=i/10*16+i%10;break; }
				CDate=1;
				k++;
				if( k<=12 ){ CMonth=k/10*16+k%10;break; }
				CMonth=1;
				j++;
				CYear=j/10*16+j%10;
				break;
			}
			/********************************************************\
			2,如果CSecond=0，则需要向手柄和显示屏发送一个时间矫正
			\********************************************************/
			if(CSecond==0)
			{
				SRAM_Init();
				SRAM_Write(SHANDLE_DATA_VAILD+0,VAILD_2);
				SRAM_Write(SHANDLE_DATA_VAILD+1,0x24);
				SRAM_Write(SHANDLE_DATA_VAILD+2,0x09);
				SRAM_Write(SHANDLE_DATA_VAILD+3,MAIN_TIME);
				SRAM_Write(SHANDLE_DATA_VAILD+4,CYear);
				SRAM_Write(SHANDLE_DATA_VAILD+5,CMonth);
				SRAM_Write(SHANDLE_DATA_VAILD+6,CDate);
				SRAM_Write(SHANDLE_DATA_VAILD+7,CHour);
				SRAM_Write(SHANDLE_DATA_VAILD+8,CMinute);
				SRAM_Write(SHANDLE_DATA_VAILD+9,0x0d);

				if(   (gDisp_Buffer_Point+14<=sizeof(gDisp_Buffer))
				    &&((gCommon_Flag&ALLOW_OUT_DISP_F_1)==0)   )
				{
					gDisp_Buffer[gDisp_Buffer_Point]=14;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=0;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='@';
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='%';
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=12;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=DISP_MAIN_STATUS_DOWN;
					gDisp_Buffer_Point++;
					if(gGeneral_Flag&GPS_VALID_F_1)
					{
						gDisp_Buffer[gDisp_Buffer_Point]=CYear;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=CMonth;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=CDate;
					}
					else
					{
						gDisp_Buffer[gDisp_Buffer_Point]=0x05;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=0x01;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=0x01;
					}
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=CHour;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=CMinute;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=CSecond;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='$';
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='&';
					gDisp_Buffer_Point++;
				}
			}
			/********************************************************\
			3，如果GPS数据有效,则进行的有关处理
			\********************************************************/
		    if(gGeneral_Flag&GPS_VALID_F_1)
		    {
				/*
		    	//显示当前GPS的经，纬度值
				Send_COM1_Byte('O');Send_COM1_Byte('[');
				Send_COM1_Byte(ASCII((gLongitude/0x1000000)/0x10));
				Send_COM1_Byte(ASCII((gLongitude/0x1000000)%0x10));
				Send_COM1_Byte(ASCII((gLongitude%0x1000000/0x10000)/0x10));
				Send_COM1_Byte(ASCII((gLongitude%0x1000000/0x10000)%0x10));
				Send_COM1_Byte(ASCII((gLongitude%0x10000/0x100)/0x10));
				Send_COM1_Byte(ASCII((gLongitude%0x10000/0x100)%0x10));
				Send_COM1_Byte(ASCII((gLongitude%0x100)/0x10));
				Send_COM1_Byte(ASCII((gLongitude%0x100)%0x10));
				Send_COM1_Byte(']');
				Send_COM1_Byte('A');Send_COM1_Byte('[');
				Send_COM1_Byte(ASCII((gLatitude/0x1000000)/0x10));
				Send_COM1_Byte(ASCII((gLatitude/0x1000000)%0x10));
				Send_COM1_Byte(ASCII((gLatitude%0x1000000/0x10000)/0x10));
				Send_COM1_Byte(ASCII((gLatitude%0x1000000/0x10000)%0x10));
				Send_COM1_Byte(ASCII((gLatitude%0x10000/0x100)/0x10));
				Send_COM1_Byte(ASCII((gLatitude%0x10000/0x100)%0x10));
				Send_COM1_Byte(ASCII((gLatitude%0x100)/0x10));
				Send_COM1_Byte(ASCII((gLatitude%0x100)%0x10));
				Send_COM1_Byte(']');
				Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
				*/
				/////////////////////////////////////////
                gYear=CYear;
                gMonth=CMonth;
                gDate=CDate;
                gHour=CHour;
                gMinute = CMinute;
                gSecond=CSecond;
                gGPS_Invaild_Timer=0;
				gGps_Null_Count=0;
				gPublic_Flag &=~ GPS_NULL_F_1;
				//判断是否满足条件，进行记录仪的时间矫正设置的命令
				if(   (gPublic_Flag&SET_RECORD_TIME_F_1)
					&&(gSecond==0)
					&&(gStatus==NORMAL_TYPE)
					&&((gPublic_Flag&CHECK_TIME_F_1)==0)  )
				{
					Send_COM1_String( (unsigned char *)CHECK_TIME,sizeof(CHECK_TIME) );
			    	Send_COM1_Byte(gStatus);
			    	Check_Record = gStatus;
			    	Send_COM1_Byte(0xAA);
			    	Check_Record ^= 0xAA;
			    	Send_COM1_Byte(0x75);
			    	Check_Record ^= 0x75;
			    	Send_COM1_Byte(G_SET_TIME);		//命令字
			    	Check_Record ^= G_SET_TIME;
			    	Send_COM1_Byte(0);
			    	Check_Record ^= 0;
			    	Send_COM1_Byte(6);		//长度6
			    	Check_Record ^= 6;
			    	Send_COM1_Byte(0x00);
			    	Check_Record ^= 0x00;	//保留字节
			    	Send_COM1_Byte(gYear);
			    	Check_Record ^= gYear;
			    	Send_COM1_Byte(gMonth);
			    	Check_Record ^= gMonth;
			    	Send_COM1_Byte(gDate);
			    	Check_Record ^= gDate;
			    	Send_COM1_Byte(gHour);
			    	Check_Record ^= gHour;
			    	Send_COM1_Byte(gMinute);
			    	Check_Record ^= gMinute;
			    	Send_COM1_Byte(gSecond);
			    	Check_Record ^= gSecond;
			    	Send_COM1_Byte(Check_Record);					//校验核
			    	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);		//包尾

				}
				/*
                //D:存储一分钟一个点的历史轨迹
                if( (CMinute!=CMinute_Temp)&&(gCycle_Send_Status!=PASS_DATA) )
                {
                	gPassTrack_Store_Item++;
                	//将指向下一个历史点的存储位置，如果判断已经到了页尾，则需要从新开始下一个页的存储
                    if( ( gPassTrack_Store_Item>=16 )||( gPassTrack_Store_Item<=0 ) )
                    {
                    	gPassTrack_Store_Item=1;
                    	//1,判断是否到了规定存储历史轨迹区域的最后一个扇区
						if((gPassTrack_Store_Sector>=FLASH_PASTDATA_E_2)||(gPassTrack_Store_Sector<FLASH_PASTDATA_S_2) )
						{
							gPassTrack_Store_Sector=FLASH_PASTDATA_S_2;
						}
						else 	gPassTrack_Store_Sector++;
						//2,将下一个扇区的内容写成记忆页
						OperateSPIEnd();
						WriteOneByteToBuffer2(0xFF,0xaa);
						OperateSPIEnd();
						WriteBuffer2ToPage(gPassTrack_Store_Sector+1);
						gTimer=0;Clear_Exter_WatchDog();
						while(gTimer<500){}
						Clear_Exter_WatchDog();
						OperateSPIEnd();
                    }
					OperateSPIEnd();
					//读对应gPassTrack_Store_Sector扇区中的数据到buffer2.
					ReadOnePageToBuffer2(gPassTrack_Store_Sector);
					//计算在这个扇区需要存到的下一个位置
					i=(gPassTrack_Store_Item-1)*17;
					OperateSPIEnd();
					//long int gLatitude  gLongitude 四个字节
					WriteOneByteToBuffer2( (i+0),gLongitude/0x1000000);
					WriteNextByteToBuffer2( gLongitude%0x1000000/0x10000 );
					WriteNextByteToBuffer2( gLongitude%0x10000/0x100 );
					WriteNextByteToBuffer2( gLongitude%0x100 );

					WriteNextByteToBuffer2( gLatitude/0x1000000 );
					WriteNextByteToBuffer2( gLatitude%0x1000000/0x10000 );
					WriteNextByteToBuffer2( gLatitude%0x10000/0x100 );
					WriteNextByteToBuffer2( gLatitude%0x100 );

					WriteNextByteToBuffer2( gSpeed );
					WriteNextByteToBuffer2( gAzimuth );
					WriteNextByteToBuffer2( nFlag|FLAG_GPSVAILD_1 );
					WriteNextByteToBuffer2( gYear );
					WriteNextByteToBuffer2( gMonth );
					WriteNextByteToBuffer2( gDate );
					WriteNextByteToBuffer2( gHour );
					WriteNextByteToBuffer2( gMinute );
					WriteNextByteToBuffer2( gSecond );
					OperateSPIEnd();
					//写已经存储的条数
					WriteOneByteToBuffer2( 0xFF,gPassTrack_Store_Item );

					//后面存储的是里程统计数据
					WriteNextByteToBuffer2( gALL_Distance/0x1000000 );
					WriteNextByteToBuffer2( gALL_Distance%0x1000000/0x10000 );
					WriteNextByteToBuffer2( gALL_Distance%0x10000/0x100 );
					WriteNextByteToBuffer2( gALL_Distance%0x100 );
					OperateSPIEnd();
					//将buffer2中的数据写入相应的Flash的区域中
					if( (gPassTrack_Store_Sector>=FLASH_PASTDATA_S_2)&&(gPassTrack_Store_Sector<=FLASH_PASTDATA_E_2) )
					{
						WriteBuffer2ToPage(gPassTrack_Store_Sector);
						gTimer=0;Clear_Exter_WatchDog();
						while(gTimer<500){}
						Clear_Exter_WatchDog();
					}
					OperateSPIEnd();
					CMinute_Temp=CMinute;
                }
           		*/
		    }
			/********************************************************\
			4，如果GPS数据无效，则进行相关的处理
			\********************************************************/
		    else
		    {
				gGPS_Invaild_Timer++;
				//如果超过1800秒持续无效点，则重新掉电处理GPS模块
				if(gGPS_Invaild_Timer>GPS_INVALID_COUNT)
				{
					gGPS_Invaild_Timer=0;
					gGeneral_Flag|=OFF_GPS_F_1;
					gOFF_Power_S=1;
				}
		    }
		}
		GPS_Buffer_Point=0;
		gGPS_Receive_Over=0;
		for(i=0;i<sizeof(GPS_Buffer);i++) GPS_Buffer[i]=0;
	}
	TAUARTCTL|=TAUARTIE;       //比较器A的中断允许
}
/********************************************************\
*	函数名：Receive_COM2(中断子程序)
	作用域：本地文件调用
*	功能：	利用比较器A的中断向量，进行模拟串口2的接收处理,处理接收GPS的数据
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
interrupt [TAUART_VECTOR] void Receive_COM2(void)
{
	TAUARTCTL&=~TAUARTIFG;	//清除比较器A的中断标志位
	if(RTI2&RECEIVE_FLAG)	//表示接收到一个字节完成
	{
		GPS_Buffer[GPS_Buffer_Point]=SBUFIN2;
		GPS_Buffer_Point++;
		if(GPS_Buffer_Point>=sizeof(GPS_Buffer))	GPS_Buffer_Point=0;
		gGPS_Receive_Over=0;
		RX2_Ready();
	}
}


/********************************************************\
*	函数名：Disp_Ask_Info()
	作用域：本地文件调用
*	功能：	接收到来自显示终端的按键请求信息，需要将这些信息上行给中心
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述
   Dim Instring As String
   Dim Buffer(10) As Byte
   Dim IObuffer(14) As Byte
   Dim InBuffer(10) As Byte
   Dim Savebuffer(5) As Byte

   Dim I

   Buffer(0) = &H10
   Buffer(1) = &H7A
   Buffer(2) = &H0
   Buffer(3) = &H1
   Buffer(4) = &H0
   Buffer(5) = &H0
   Buffer(6) = &H1  ' RMC
   Buffer(7) = &H0
   'buffer(7)=&H1C 在设置GSA，GSV，VTG，时，buffer(7)=&H1C
   Buffer(8) = &H10
   Buffer(9) = &H3


   IObuffer(0) = &H10
   IObuffer(1) = &HBC
   IObuffer(2) = &H0
   IObuffer(3) = &H7
   IObuffer(4) = &H7
   IObuffer(5) = &H3
   IObuffer(6) = &H0
   IObuffer(7) = &H0
   IObuffer(8) = &H0
   IObuffer(9) = &H2
   IObuffer(10) = &H4
   IObuffer(11) = &H0
   IObuffer(12) = &H10
   IObuffer(13) = &H3

   Savebuffer(0) = &H10
   Savebuffer(1) = &H8E
   Savebuffer(2) = &H26
   Savebuffer(3) = &H10
   Savebuffer(4) = &H3


   ' 使用 COM1。
   MSComm1.CommPort = 1
   ' 9600 波特，无奇偶校验，8 位数据，一个停止位。
   MSComm1.Settings = "9600,O,8,1"
   ' 当输入占用时，
   ' 告诉控件读入整个缓冲区。
   MSComm1.InputLen = 0
   ' 打开端口。
   MSComm1.PortOpen = True
   ' 将 attention 命令送到调制解调器。
   MSComm1.Output = Buffer ' "ATV1Q0" & Chr$(13) ' 确保

   MSComm1.Output = IObuffer

   'MSComm1.Output = Savebuffer

   MSComm1.PortOpen = False

   ''''''''''''''
    MSComm1.CommPort = 1
   ' 9600 波特，无奇偶校验，8 位数据，一个停止位。
   MSComm1.Settings = "9600,N,8,1"
   ' 当输入占用时，
   ' 告诉控件读入整个缓冲区。
   MSComm1.InputLen = 0
   ' 打开端口。
   MSComm1.PortOpen = True
   ' 将 attention 命令送到调制解调器。

   MSComm1.Output = Savebuffer

   MSComm1.PortOpen = False

\********************************************************/
