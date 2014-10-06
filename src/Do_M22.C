
/********************************************************\
*	文件名：  Do_GSM.C
*	创建时间：2004年12月7日
*	创建人：  
*	版本号：  1.00
*	功能：	  针对GSM模块进行的一些操作，包括发送数据和接收数据
*	文件属性：公共文件
*	修改历史：（每条详述）
\********************************************************/
#include <msp430x14x.h>
#include "Other_Define.h"
#include "General.h"
#include "SPI45DB041.h"
#include "Define_Bit.h"
#include "D_Buffer.h"
#include "Uart01.h"
#include "M22_AT.h"
#include "Check_GSM.h"
#include "Sub_C.h"
#include "Do_Reset.h"
#include "W_Protocol.h"
#include "TA_Uart.h"
#include "Handle_Protocol.h"
#include "Do_Handle.h"
#include "Do_SRAM.h"
#include "Do_M22.h"
/*
#include "Do_Record.h"
#include "Record_Protocol.h"
#include "Main_Init.h"
#include "Disp_Protocol.h"
*/
#define Debug_GSM_COM1

void Case1_GSM_PHONE(void);
void Case1_PHONE_Out(void);


void Case2_GSM_OTHER(void);
void Case2_OTHER_Out(void);

void Case3_GSM_SMS_IN(void);
void Case3_SMSIN_Out(void);

void Case4_GSM_SMS_OUT(void);
void Case4_SMSOUT_Out(void);
void Case4_OverTime_Out(unsigned char Clear_Flag);

void Case5_LAND_GPRS_OPER(void);
void Case6_GPRS_DATA_OPER(void);

void Send_Type1_GSM_Data_One(unsigned char S_Address);
void Send_Type1_GSM_Data_Two(unsigned char S_Address);

void Send_Circle_GSM_TypeD(void);
void Send_Circle_TCP_TypeD(void);
void Send_Circle_GSM_TypeE(void);
void Send_Circle_TCP_TypeE(void);
void Case_OverTime_Return(void);

//作为音量四个档位(起作用的也就是后面的四个数，前一个数的存在是为了使数组的下标和音量值想对应上)
/********************************************************\
*	函数名：Do_G20_Out
	作用域：外部文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	A,表示GSM工作方式
	B,表示TCP挂起的工作方式
	C,表示TCP在线的工作方式

	补充说明：因为根据M22模块的特性：几个CASE的功能
	1，处理与电话相关的业务(AB)，
	2，处理与CSQ方面的查询(AB)
	3，SMS的接收(中断方式接收处理)(AB)
	4，SMS的发送操作(A)
	5，处理需要TCP登陆，TCP退出,TCP在线。
	6，TCP在线状态下进行TCP的数据业务

	根据M22的相关特性：则可以分为如下几种操作方式

	正常的工作流程：
	设备启动后：如果根据条件需要进行TCP登陆的话
	1-2-3-4-5(登陆)--6(发送登陆信息)---等待登陆响应----(A)接收到响应(A01)6(循环处理)(A02)CSQ查询时间到(A03)+++挂起(A03)2---(A04)6(循环处理)
												   							---
												                            ---(A011)判断有电话相关的业务(拨出号码和电话打入)--(A012)+++挂起---1()

												   ----(B)未接收到响应(B01)6(发送登陆信息)--等待登陆响应
	具体的的工作方式：
	1，设备上电，启动M22，进入GSM的工作方式：
	2，如果判断设备判断有报警信息(报警信息默认为30秒的周期发送)，
		同时IP地址有效，TCP处于开启状态，则需要在启动M22工作后，
		进行TCP登陆操作，此时需要TCP一直在线，进行发送报警信息
	3，如果判断设备有需要发送行车轨迹数据，同时IP地址有效，TCP处于开启状态，
		则也需要在启动M22工作后，进行TCP登陆操作，此时又分两重情况
		如果发送轨迹数据的周期如果小于3分钟，则属于一直在线状态)，而如何
		发送行车轨迹数据的周期大于等于3分钟，则在需要发送轨迹数据的时刻
		才进行登陆操作，然后进行发送数据。发送完数据后，则立即退出TCP在线。
	4，如果属于TCP一直在线的状态，则每间隔一定时间将TCP挂起，进行CSQ的查询

登陆TCP的工作流程：如果登陆上TCP，并且通过TCP发送登陆信息后，等待返回登陆响应
如果超过一定时间未返回登陆响应，则继续发送登陆信息。如果连续5次均为发送登陆信息后
仍无登陆响应，则退出TCPZ状态，重新进行TCP登陆
\********************************************************/
void Do_G20_Out(void)
{
	switch(gGSM_Oper_Type)
	{
		/*
		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		1，GSM_PHONE:主要是针对电话语音方面的处理，拨号，接听，挂机，DTMF，音量调节。根据M22的特殊性，如果TCP
			在线的状态下，必须要先退出TCP在线，然后才能进行相关电话业务的处理
			只有在GSM的工作方式下执行这个指令
		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		*/
		case GSM_PHONE:
		{
			Case1_GSM_PHONE();
			break;
		}
		/*
		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		2，GSM_OTHER：没电话的时候CSQ的查询,根据M22的方式，则可以不通过查询CLCC来确定电话业务的状态
		   在GSM和TCP挂起的状态下执行
		+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		*/
		case GSM_OTHER:
		{
			Case2_GSM_OTHER();
			break;
		}
		/*
		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		3，GSM_SMS_IN：因为M22的接收SMS的方式采用的是中断方式，则直接在中断处理中处理，则可以不处理这个模块
		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		*/
		case GSM_SMS_IN:
		{
			Case3_GSM_SMS_IN();
			break;
		}
		/*
		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		4，GSM_SMS_OUT：主要处理M22通过GSM方式发送数据包，只有在GSM工作方式下
		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		*/
		case GSM_SMS_OUT:
		{
			Case4_GSM_SMS_OUT();
			break;
		}
		/*
		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		5，CHECK_GPRS_LINK：专门用于进行退出TCP和登陆TCP的操作流程，或者重新执行进入TCP的操作
							在GSM工作方式和TCP挂起的方式下
		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		*/
		case LAND_TCP_OPER:
		{
			Case5_LAND_GPRS_OPER();
			break;
		}
		/*
		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		6，GPRS_DATA_OPER：M22的TCP方式发送和接收数据(),如果需要退出的时候进行AT指令的其他操作的时候，则需要
			先将TCP挂起，可以进行接收SMS，或者发送SMS。如果是电话业务，则需要挂起后，然后退出TCP登陆，才可以进行
			电话方面的相关业务
		++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		*/
		case GPRS_DATA_OPER:
		{
			Case6_GPRS_DATA_OPER();
			break;
		}
		default:
			break;
	}

}
/********************************************************\
*	函数名：Case1_GSM_PHONE
	作用域：本地文件调用
*	功能：Do_G20_Out模块中的一个case执行模块，主要是针对电话语音方面的处理，拨号，接听，挂机，DTMF，音量调节方面的处理。
*
	参数：	只有在满足能进入这个case的条件时候，才能进入执行这个模块，
*	返回值：不同的情形是需要对不同的全局变量进行重新附值
*	创建人：
*
*	修改历史：（每条详述）
	补充说明：	手柄相关的AT指令存储在buffer1中的0x5D----0x7A区域。第一个字节0x5D存储0或者1，为0则表示
				后面的数据已经处理，为1则表示后面的数据未处理。0x5E存储相关的AT指令的类型，如挂机，拨号等
				后面如果还有数据（电话号码等）最后数据结束以0X00作为结束标志处理。

	改为M22模块后，因为其特性，在这个CASE中需要增加一些处理：
	1,如果有手柄或者监听对外拨号的信令，同时如果M22处于TCP在线且被挂起的状态，则需要先执行ATH指令，退出TCP
	状态，然后才能进行拨号处理
\********************************************************/
void Case1_GSM_PHONE(void)
{
	unsigned int i=0,j=0,k=0,n=0;
	unsigned char Phone_s=0;
	unsigned char Phone_e=0;
	OperateSPIEnd();
	//判断监听完后，是否需要恢复语音通道的处理
	if( ( (gGeneral_Flag&TCP_PHONE_F_1)==0)&&(gGeneral_Flag&MONITOR_RESUME_F_1) )
	{
		gTimer=0;
		while(gTimer<200){Check_GSM();}
		Send_COM0_String((unsigned char *)AT_AUGAIN,sizeof(AT_AUGAIN));
		Send_COM0_Byte('1');Send_COM0_Byte(',');
		Send_COM0_Byte('5');Send_COM0_Byte(0x0d);
		gInternal_Flag&=~M22_RETURN_OK_1;
		Wait_OK();
		if( (gInternal_Flag&M22_RETURN_OK_1)==0 );
		else
		{
			//2,固定调节SPEKER的接收增益
			gTimer=0;
			while(gTimer<200){Check_GSM();}
			Send_COM0_String((unsigned char *)AT_AUGAIN,sizeof(AT_AUGAIN));
			Send_COM0_Byte('2');Send_COM0_Byte(',');Send_COM0_Byte(Handle_VOL[TWO]);Send_COM0_Byte(0x0d);
			gInternal_Flag&=~M22_RETURN_OK_1;
			Wait_OK();
			if( (gInternal_Flag&M22_RETURN_OK_1)==0 );
			else
			{
				//3,调节AT$AUVOL的接收增益
				gTimer=0;
				while(gTimer<200){Check_GSM();}
				Send_COM0_String((unsigned char *)AT_AUVOL,sizeof(AT_AUVOL));
				Send_COM0_Byte('5');Send_COM0_Byte(0x0d);
				gInternal_Flag&=~M22_RETURN_OK_1;
				Wait_OK();
				if( (gInternal_Flag&M22_RETURN_OK_1)==0 );
				else
				{
					gGeneral_Flag&=~MONITOR_RESUME_F_1;
					P6OUT&=~SEL_MIC;
	   	 			Send_COM3_Load(4,MAIN_HANG_UP,0);
	   	 			gSys_Handle=MAIN_HANG_UP;
				}
			}
		}
	}
	OperateSPIEnd();

	//返回退出电话相关业务的CASE结构的满足条件:唯一正常的跳出此
	if( (SRAM_Read(PHONE_AT_2)!=VAILD_2)
		 &&( (gPhone_Status==ACTIVE_HANDLE)||(gPhone_Status==ACTIVE_MONITOR)||(gPhone_Status==READY) )   )
	{
		if( (gGeneral_Flag&RING_F_1)==0 )
		{
			gGSM_Oper_Type=GSM_OTHER;
			gGSM_OTHER_Step=0;gPhone_Step=0;
			gTimer_GSM_AT=0;
			Check_GSM();
			gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
			OperateSPIEnd();
			return;
		}
	}
	//如果buffer1中有相应的处理数据
	if( (gInternal_Flag&GSM_ONEAT_SUCCESS_1)==0 )
	{
		if( gTimer_GSM_AT>600)
		{
			gTimer_GSM_AT=0;
			SRAM_Init();
			j=SRAM_Read(PHONE_AT_2);
			if(j==VAILD_2)
			{
				i=SRAM_Read(PHONE_AT_2+1);
				if	   (i==H_DIAL)		   	gPhone_Step=1;		//手柄拨号
				else if(i==M_DIAL)			gPhone_Step=5;		//监听拨号
				else if(i==H_ANSWER)		gPhone_Step=10;		//手柄接听
				else if(i==H_DTMF)			gPhone_Step=11;		//手柄DTMF拨号
				else if(i==H_VOLADJ)		gPhone_Step=12;		//手柄调节音量
				else if(i==H_HUNGUP)		gPhone_Step=13;		//手柄挂机
				else
				{
					Check_GSM();
					SRAM_Init();
					SRAM_Write(PHONE_AT_2,NULL_2);
					SRAM_Write(PHONE_AT_2+1,0);
				}
			}
		}
		else if(gGeneral_Flag&RING_F_1)
		{
			//判断是否有来电显示
			//+CLIP: "13670155704",129,,,,0
			if( Check_GSM()!=0 )
			{
				if(Compare_String(gGeneral_Buffer,(unsigned char *)String_CLIP,sizeof(String_CLIP))!=0)
				{
					gStatus1_Flag|=PHONE_ONLINE_F_1;
					j=SRAM_Read(PHONE_AT_2+0);
					i=SRAM_Read(PHONE_AT_2+1);
					if((i==H_HUNGUP)&&(j==VAILD_2));
					else
					{
						i=5;
						gPublic_Flag&=~ALLOW_PHONE_IN_F_1;
						while(1)
						{
							if(gGeneral_Buffer[i]=='"')	break;
							i++;if(i>15) return;
						}
						i++;
						Phone_s=i;				//第一个号码的位置序号
						SRAM_Init();
						SRAM_Write(SHANDLE_DATA_VAILD+0,VAILD_2);
						SRAM_Write(SHANDLE_DATA_VAILD+1,0x24);
						SRAM_Write(SHANDLE_DATA_VAILD+2,0x14);
						SRAM_Write(SHANDLE_DATA_VAILD+3,MAIN_PHONE_IN);
						j=i+16;
						k=0;
						while(1)
						{
							if(gGeneral_Buffer[i]!='"')
							{
								SRAM_Write(SHANDLE_DATA_VAILD+4+k,gGeneral_Buffer[i]);
								i++;k++;
								if(i>j)
								{
									n=0;
									break;
								}
							}
							else
							{
								n=k;
								break;
							}
						}
						Phone_e=i-1;					//号码最后一个位的序号
						for(k=i;k<j;k++)
						{
							SRAM_Write(SHANDLE_DATA_VAILD+4+n+(k-i),0x20);
						}
						SRAM_Write(SHANDLE_DATA_VAILD+4+n+j-i,0x0d);
						gPhone_Status=INCOMING;
						//判断比较是否与Flash中存储的中心服务号码相同
						if(   ((gON_OFF_Temp0&PHONE_IN_ON)==0)
							&&((gCommon_Flag&CHECK_RINGPHONE_F_1)==0 )	)
						{
							for(i=0;i<Phone_e-Phone_s;i++)
							{
								gGeneral_Buffer[i]=gGeneral_Buffer[Phone_s+i];
							}
							i=1;
							i=Check_Allow_Phone(Phone_e-Phone_s);
							//与匹配的号码不一样
							if(i==1)			gPublic_Flag&=~ALLOW_PHONE_IN_F_1;
							//与匹配的号码一样
							else				gPublic_Flag|=ALLOW_PHONE_IN_F_1;
							gCommon_Flag|=CHECK_RINGPHONE_F_1;
						}
					}
					gPhone_Status=INCOMING;
				}
			}
		}
	}
	/********************************************************\
	\********************************************************/
	//1，手柄拨号处理
	if(gPhone_Step==1)
	{
		gPhone_Step=4;gTimer_GSM_AT=0;
	}
	else if(gPhone_Step==4)
	{
		if( gTimer_GSM_AT>400 )
		{
			if(gPhone_Status==READY)
			{
				Send_COM0_String((unsigned char *)ATD,sizeof(ATD));
				//电话号码的长度
				k=SRAM_Read(PHONE_AT_2+2);
				for(j=0;j<k;j++)
				{
					Send_COM0_Byte(SRAM_Read(PHONE_AT_2+3+j));
				}
				OperateSPIEnd();
				Send_COM0_Byte(';');
				Send_COM0_Byte(0x0d);
				gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
				gPhone_Step=14;
				gTimer_GSM_AT=0;
				gGeneral_Flag|=DAILING_UP_F_1;
				gGeneral_Flag|=TCP_PHONE_F_1;
				gHangUp_Timer=0;
				gPhone_Status=DAILING;
				SRAM_Init();
				SRAM_Write(PHONE_AT_2+0,NULL_2);
				SRAM_Write(PHONE_AT_2+1,0);
				for(i=PHONE_AT_2+1;i<ECHO_SMS_2;i++)
				{
					SRAM_Write(i,0);
				}

				gStatus1_Flag|=PHONE_ONLINE_F_1;
			}
			else	Case1_PHONE_Out();
		}
		else Check_GSM();
	}
	/********************************************************\
	2，监听拨号处理()如果属于是监听处理，则先处理将麦克的增益调节最大
		将,然后将SPEAKER的接收增益调节为关闭状态，然后控制模拟开关的通道转换
	\********************************************************/
	else if(gPhone_Step==5)
	{
		if( gTimer_GSM_AT>400 )
		{
			if(gPhone_Status==READY)
			{
				gTimer=0;
				while(gTimer<200){Check_GSM();}
				Send_COM0_String((unsigned char *)AT_AUGAIN,sizeof(AT_AUGAIN));
				Send_COM0_Byte('1');Send_COM0_Byte(',');
				Send_COM0_Byte('9');Send_COM0_Byte(0x0d);
				gInternal_Flag&=~M22_RETURN_OK_1;
				Wait_OK();
				if( (gInternal_Flag&M22_RETURN_OK_1)==0 )
				{
					gPhone_Step=5;gTimer_GSM_AT=0;
				}
				else
				{
					//2,固定调节SPEKER的接收增益为1
					gTimer=0;
					while(gTimer<200){Check_GSM();}
					Send_COM0_String((unsigned char *)AT_AUGAIN,sizeof(AT_AUGAIN));
					Send_COM0_Byte('2');Send_COM0_Byte(',');Send_COM0_Byte('1');Send_COM0_Byte(0x0d);
					gInternal_Flag&=~M22_RETURN_OK_1;
					Wait_OK();
					if( (gInternal_Flag&M22_RETURN_OK_1)==0 )
					{
						gPhone_Step=5;gTimer_GSM_AT=0;
					}
					else
					{
						//3,固定调节AUVOL的接收增益为1
						gTimer=0;
						while(gTimer<200){Check_GSM();}
						Send_COM0_String((unsigned char *)AT_AUVOL,sizeof(AT_AUVOL));
						Send_COM0_Byte('0');Send_COM0_Byte(0x0d);
						gInternal_Flag&=~M22_RETURN_OK_1;
						Wait_OK();
						if( (gInternal_Flag&M22_RETURN_OK_1)==0 )
						{
							gPhone_Step=5;gTimer_GSM_AT=0;
						}
						else
						{
							gPhone_Step=9;gTimer_GSM_AT=0;
						}
					}
				}
			}
			else
			{
				Case1_PHONE_Out();
			}
		}
		else 	Check_GSM();
	}
	else if(gPhone_Step==9)
	{
		if( gTimer_GSM_AT>400 )
		{
			if(gPhone_Status==READY)
			{
				Send_COM0_String((unsigned char *)ATD,sizeof(ATD));
				//电话号码的长度
				k=SRAM_Read(PHONE_AT_2+2);
				for(j=0;j<k;j++)
				{
					Send_COM0_Byte(SRAM_Read(PHONE_AT_2+3+j));
				}
				Send_COM0_Byte(';');
				Send_COM0_Byte(0x0d);
				gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
				gPhone_Step=14;
				gGeneral_Flag|=MONITOR_ON_F_1;
				gGeneral_Flag|=TCP_PHONE_F_1;
				gStatus1_Flag|=PHONE_ONLINE_F_1;
				gHangUp_Timer=0;
				gPhone_Status=DAILING;
				gTimer_GSM_AT=0;
				P6OUT|=SEL_MIC;
			}
			else
			{
				Case1_PHONE_Out();
			}
		}
		else Check_GSM();
	}
	/********************************************************\
	3，手柄接听处理
	\********************************************************/
	else if(gPhone_Step==10)
	{
		if( gTimer_GSM_AT>400  )
		{
			Send_COM0_String((unsigned char *)ATA,sizeof(ATA));
			gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
			gPhone_Step=15;
			gTimer_GSM_AT=0;
			OperateSPIEnd();
		}
		else Check_GSM();
	}
	/********************************************************\
	4，手柄DTMF处理
	\********************************************************/
	else if(gPhone_Step==11)
	{
		OperateSPIEnd();
		if( gTimer_GSM_AT>400  )
		{
			if(gPhone_Status==ACTIVE_HANDLE)
			{
				//检测DTMF中的数据
				#ifdef Debug_GSM_COM1
				Send_COM1_Byte('[');
				Send_COM1_Byte(ASCII(gDTMF_SAVE[0]/10));
				Send_COM1_Byte(ASCII(gDTMF_SAVE[0]%10));
				Send_COM1_Byte(']');
				Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
				#endif
				////////////////////////////////////////////
				Send_COM0_String((unsigned char *)AT_VTS,sizeof(AT_VTS));
				//需要发送的音频数据
				k=SRAM_Read(PHONE_AT_2+3);
				//直接存储的是数字ASCII形式
				Send_COM0_Byte(k);
				Send_COM0_Byte(0x0d);
				gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
				gPhone_Step=15;
				gTimer_GSM_AT=0;
				OperateSPIEnd();
			}
			else	Case1_PHONE_Out();
		}
		else Check_GSM();
	}
	/********************************************************\
	5，手柄音量调节处理
	\********************************************************/
	else if(gPhone_Step==12)
	{
		OperateSPIEnd();
		if( gTimer_GSM_AT>400  )
		{
			if( gM22_Status!=TCP_ONLINE_WORK )
			{
				Send_COM0_String((unsigned char *)AT_AUGAIN,sizeof(AT_AUGAIN));
				k=SRAM_Read(PHONE_AT_2+3);
				//直接存储的是数字ASCII形式
				if( (k<1)||(k>4) )	k=2;
				Send_COM0_Byte('2');Send_COM0_Byte(',');
				Send_COM0_Byte(Handle_VOL[k]);
				Send_COM0_Byte(0x0d);
				gVolume=k;
				gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
				gPhone_Step=15;
				gTimer_GSM_AT=0;
				OperateSPIEnd();
			}
			else	Case1_PHONE_Out();
		}
		else Check_GSM();
	}
	/********************************************************\
	6，手柄挂机处理
	\********************************************************/
	else if(gPhone_Step==13)
	{
		if( gTimer_GSM_AT>400)
		{
			if(	gPhone_Status==ACTIVE_MONITOR)
			{
				Case1_PHONE_Out();
			}
			else
			{
				Send_COM0_String((unsigned char *)ATH,sizeof(ATH));
				gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
				gPhone_Step=16;
				gTimer_GSM_AT=0;
				OperateSPIEnd();
			}
		}
		else Check_GSM();
	}
	/********************************************************\
	手柄拨号，监听拨号的返回状态处理
	\********************************************************/
	else if(gPhone_Step==14)
	{
		if( Check_GSM()!=0 )
		{
			gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;gTimer_GSM_AT=0;
			//接通
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_COLP,sizeof(String_COLP)))
			{
				if(gGeneral_Flag&DAILING_UP_F_1)
				{
					gGeneral_Flag&=~DAILING_UP_F_1;
					gPhone_Status=ACTIVE_HANDLE;
				    Send_COM3_Load(4,MAIN_TALKING,0);
				    gSys_Handle=MAIN_TALKING;
					//返回手柄一个接通命令
				}
				else if(gGeneral_Flag&MONITOR_ON_F_1)
				{
					gGeneral_Flag&=~MONITOR_ON_F_1;
					gPhone_Status=ACTIVE_MONITOR;
				}
				Case1_PHONE_Out();
			}
			//未接通，则自己主动挂断
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK)))
			{

				if(gM22_Status==TCP_HANG_WORK)	gGeneral_Flag|=TCP_STATUS_F_1;
				if(gGeneral_Flag&DAILING_UP_F_1)
				{
					gGeneral_Flag&=~DAILING_UP_F_1;
					gPhone_Status=READY;
					gGeneral_Flag&=~TCP_PHONE_F_1;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
				}
				else if(gGeneral_Flag&MONITOR_ON_F_1)
				{
					gGeneral_Flag&=~MONITOR_ON_F_1;
					gPhone_Status=READY;
					P6OUT&=~SEL_MIC;
				}
				Case1_PHONE_Out();
			}
			//未接通，返回手柄挂机
			else if( Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER)) )
			{
				if(gM22_Status==TCP_HANG_WORK)	gGeneral_Flag|=TCP_STATUS_F_1;
				if(gGeneral_Flag&DAILING_UP_F_1)
				{
					gGeneral_Flag&=~DAILING_UP_F_1;
					gPhone_Status=READY;
					gGeneral_Flag&=~TCP_PHONE_F_1;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
				    Send_COM3_Load(4,MAIN_HANG_UP,0);
				    gSys_Handle=MAIN_HANG_UP;
					//返回手柄挂机命令
				}
				else if(gGeneral_Flag&MONITOR_ON_F_1)
				{
					gGeneral_Flag&=~MONITOR_ON_F_1;
					gPhone_Status=READY;
					gGeneral_Flag&=~TCP_PHONE_F_1;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
					P6OUT&=~SEL_MIC;
				}
				Case1_PHONE_Out();
			}
			//未接通，对方主动挂机，返回手柄挂机
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_BUSY,sizeof(String_BUSY)))
			{
				if(gM22_Status==TCP_HANG_WORK)	gGeneral_Flag|=TCP_STATUS_F_1;
				if(gGeneral_Flag&DAILING_UP_F_1)
				{
					gGeneral_Flag&=~DAILING_UP_F_1;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
					gPhone_Status=READY;
					gGeneral_Flag&=~TCP_PHONE_F_1;
				    Send_COM3_Load(4,MAIN_HANG_UP,0);
				    gSys_Handle=MAIN_HANG_UP;
					//返回主动挂机命令
				}
				else if(gGeneral_Flag&MONITOR_ON_F_1)
				{
					gGeneral_Flag&=~MONITOR_ON_F_1;
					gPhone_Status=READY;
					gGeneral_Flag&=~TCP_PHONE_F_1;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
					P6OUT&=~SEL_MIC;
				}
				Case1_PHONE_Out();
			}
			//未接通，对方无应答
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_NOANSWER,sizeof(String_NOANSWER)))
			{
				if(gM22_Status==TCP_HANG_WORK)	gGeneral_Flag|=TCP_STATUS_F_1;
				if(gGeneral_Flag&DAILING_UP_F_1)
				{
					gGeneral_Flag&=~DAILING_UP_F_1;
					gPhone_Status=READY;
					gGeneral_Flag&=~TCP_PHONE_F_1;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
				    Send_COM3_Load(4,MAIN_HANG_UP,0);
				    gSys_Handle=MAIN_HANG_UP;
					//返回手柄挂机命令
				}
				else if(gGeneral_Flag&MONITOR_ON_F_1)
				{
					gGeneral_Flag&=~MONITOR_ON_F_1;
					gPhone_Status=READY;
					gGeneral_Flag&=~TCP_PHONE_F_1;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
					P6OUT&=~SEL_MIC;
				}
				Case1_PHONE_Out();
			}
			//未接通，对方占线
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_NOCARRIER,sizeof(String_NOCARRIER)) )
			{
				if(gM22_Status==TCP_HANG_WORK)	gGeneral_Flag|=TCP_STATUS_F_1;
				if(gGeneral_Flag&DAILING_UP_F_1)
				{
					gGeneral_Flag&=~DAILING_UP_F_1;
					gPhone_Status=READY;
					gGeneral_Flag&=~TCP_PHONE_F_1;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
				    Send_COM3_Load(4,MAIN_BUSY,0);
					//返回手柄挂机命令
				}
				else if(gGeneral_Flag&MONITOR_ON_F_1)
				{
					gGeneral_Flag&=~MONITOR_ON_F_1;
					gPhone_Status=READY;
					gGeneral_Flag&=~TCP_PHONE_F_1;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
					P6OUT&=~SEL_MIC;
				}
				Case1_PHONE_Out();
			}
		}
		else
		{
			if(gGeneral_Flag&DAILING_UP_F_1)
			{
				//手柄未接通主动挂机命令的操作
				SRAM_Init();
				i=SRAM_Read(PHONE_AT_2+1);
				if( i==H_HUNGUP )
				{
					Send_COM0_String((unsigned char *)ATH,sizeof(ATH));
					gGeneral_Flag&=~MONITOR_ON_F_1;
					gPhone_Status=READY;
					gGeneral_Flag&=~TCP_PHONE_F_1;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
				}
			}
			if(gTimer_GSM_AT>65000)
			{
				gGeneral_Flag&=~MONITOR_ON_F_1;
				gPhone_Status=READY;
				gGeneral_Flag&=~TCP_PHONE_F_1;
				gStatus1_Flag&=~PHONE_ONLINE_F_1;
				Case1_PHONE_Out();
				SRAM_Init();
				SRAM_Write(PHONE_AT_2+0,VAILD_2);
				SRAM_Write(PHONE_AT_2+1,H_HUNGUP);
				SRAM_Write(PHONE_AT_2+2,0);
			}
		}
	}
	/********************************************************\
	手柄接听，DTMF拨号，音量调节的返回状态处理
	\********************************************************/
	else if(gPhone_Step==15)
	{
		if( Check_GSM()!=0 )
		{
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK)))
			{
				if(gPhone_Status==INCOMING)
				{
					gPhone_Status=ACTIVE_HANDLE;
					gGeneral_Flag&=~RING_F_1;
					gPhone_Step=0;
				    Send_COM3_Load(4,MAIN_TALKING,0);
				    gSys_Handle=MAIN_TALKING;
					Case1_PHONE_Out();
				}
				else
				{
					Case1_PHONE_Out();
				}
			}
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER)))
			{
				Case1_PHONE_Out();
			}
		}
		else
		{
			if(gTimer_GSM_AT>10000)		Case1_PHONE_Out();
		}
	}
	/********************************************************\
	手柄挂机的返回状态处理
	\********************************************************/
	else if(gPhone_Step==16)
	{
		if( Check_GSM()!=0 )
		{
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK)))
			{
				gGeneral_Flag&=~DAILING_UP_F_1;
				gGeneral_Flag&=~MONITOR_ON_F_1;
				gPhone_Status=READY;
				gGeneral_Flag&=~TCP_PHONE_F_1;
				gStatus1_Flag&=~PHONE_ONLINE_F_1;
				gGeneral_Flag&=~RING_F_1;
				if(gM22_Status==TCP_HANG_WORK)	gGeneral_Flag|=TCP_STATUS_F_1;
				Case1_PHONE_Out();
			}
		}
		else
		{
			if(gTimer_GSM_AT>6000)		Case_OverTime_Return();
		}
	}
	OperateSPIEnd();
}
/********************************************************\
*	函数名：Case1_PHONE_Out
	作用域：本地文件调用
*	功能：	将CASE1中处理的出口,但这这个出口在下一个程序循环中，仍然会进入这个CASE中

*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Case1_PHONE_Out(void)
{
	unsigned int i=0;
	gPhone_Step=0;
	gTimer_GSM_AT=0;
	gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
	SRAM_Init();
	SRAM_Write(PHONE_AT_2+0,NULL_2);
	for(i=PHONE_AT_2+1;i<ECHO_SMS_2;i++)
	{
		SRAM_Write(i,0);
	}
}

/********************************************************\
*	函数名：Case2_GSM_OTHER
	作用域：本地文件调用
*	功能：	主要是处理有电话方面的时候CLCC查询，没电话的时候CSQ的查询,还有如果收到参数设置的话，则
			在需要进行短消息中心号码的设置，GPRS的IP改变，端口改变后的GPRS的重新登陆设置等
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	补充说明：因为在这个case中有需要对短消息中心号码的设置，根据通过SMS接收到新的参数设置，需要将
				短消息中心号码存储先存储在buffer1的0x4B----0x5C区域。第一个字节开始就存储短消息中心
				号码的第一个字节，以0x00作为结束符号。在这个case中需要将存储在buffer1中的短消息中心
				号码通过AT+CSCA指令进行SIM设置。
\********************************************************/
void Case2_GSM_OTHER(void)
{
	unsigned int i=0,j=0,k=0;
	static unsigned char CSQ_Single_Error=0;

	if(gGSM_OTHER_Step==0)		{gGSM_OTHER_Step=1;gTimer_GSM_AT=0;Check_GSM();}

	/********************************************************\
	1,gGSM_OTHER_Step=1的时候，判断是否在这个case中进行处理
		A：如果无电话方面的业务，满足一定时间间隔查询信号，则进行AT+CSQ的指令操作
		B：判断是否有需要进行短消息中心号码的设置，如果判断有gInternal_Flag&SET_CSCA_F_1置位，则进行
			AT+CSCA的指令操作
		C：如果满足需要恢复语音方面的设置(监听后的的模块语音方面的参数设置)
	\********************************************************/
	else if(gGSM_OTHER_Step==1)
	{
		if( 	(gTimer_CSQ>5)
			  &&((gPhone_Status==ACTIVE_HANDLE)||(gPhone_Status==ACTIVE_MONITOR)) )
		{
			if( gTimer_GSM_AT>200 )
			{
				Send_COM0_String((unsigned char *)AT_CLCC,sizeof(AT_CLCC));
				gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
				gTimer_GSM_AT=0;
				gTimer_CSQ=0;
				gGSM_OTHER_Step=5;
			}
			else Check_GSM();
		}
		/********************************************************\
		A,1,无电话业务，2，处于GSM工作方式下。3。GSM信号查询时间到
		\********************************************************/
	    else if((gPhone_Status==READY)&&(gM22_Status==GSM_WORK)&&(gTimer_CSQ>GSM_CHECKCSQ_TIME) )
		{
			if( gTimer_GSM_AT>200 )
			{
				Send_COM0_String((unsigned char *)AT_CSQ,sizeof(AT_CSQ));
				gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
				gTimer_GSM_AT=0;
				gTimer_CSQ=0;
				gGSM_OTHER_Step=2;
			}
			else Check_GSM();
		}
		/********************************************************\
		B,1，无电话业务，2，处于TCP挂起的工作方式下。4，GSM信号查询时间到
		\********************************************************/
		else if((gPhone_Status==READY)&&(gM22_Status==TCP_HANG_WORK)&&(gTimer_CSQ>TCP_CHECKCSQ_TIME))
		{
			if( gTimer_GSM_AT>200 )
			{
				Send_COM0_String((unsigned char *)AT_CSQ,sizeof(AT_CSQ));
				gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
				gTimer_GSM_AT=0;
				gTimer_CSQ=0;
				gGSM_OTHER_Step=2;
			}
			else Check_GSM();
		}
		else	Case2_OTHER_Out();
	}
	/********************************************************\
	AB-1：查询信号的模块返回
	\********************************************************/
	else if(gGSM_OTHER_Step==2)
	{
		if(Check_GSM()!=0)
		{
			gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_CSQ,sizeof(String_CSQ)))
			{
				if(gGeneral_Buffer[9]==',')
				{
					gSignal=(gGeneral_Buffer[7]-0x30)*10+(gGeneral_Buffer[8]-0x30);
				}
				else if(gGeneral_Buffer[7]==',')
				{
					gSignal=gGeneral_Buffer[6]-0x30;
				}
				else if(gGeneral_Buffer[8]==',')
				{
					gSignal=(gGeneral_Buffer[6]-0x30)*10+(gGeneral_Buffer[7]-0x30);
				}
				if(gSignal>31)
				{
					gSignal=0;
					CSQ_Single_Error++;
					if(CSQ_Single_Error>20)
					{
						CSQ_Single_Error=0;
						gGeneral_Flag|=OFF_M22_F_1;
						gOFF_Power_S=1;
						gPhone_Status=READY;
						gM22_ReturnER_SIM_Count=0;
						gStatus1_Flag&=~PHONE_ONLINE_F_1;
						return;
					}
				}
				else
				{
					CSQ_Single_Error=0;
				}
				gGSM_OTHER_Step=3;
				gTimer_GSM_AT=0;
				gTimer_CSQ=0;
			}
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER)))
			{
				gGSM_OTHER_Step=3;
				gTimer_GSM_AT=0;
				gTimer_CSQ=0;
			}
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK)))
			{
				gGSM_OTHER_Step=3;
				gTimer_GSM_AT=0;
				gTimer_CSQ=0;
			}
		}
		else
		{
			if(gTimer_GSM_AT>10000)
			{
				Case_OverTime_Return();
			}
		}
	}
	//查询SIM卡的状态是否正常
	else if(gGSM_OTHER_Step==3)
	{
		if(gTimer_GSM_AT>200)
		{
			Send_COM0_String((unsigned char *)AT_CSCA_C,sizeof(AT_CSCA_C));
			gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
			gTimer_GSM_AT=0;
			gGSM_OTHER_Step=4;
		}
		else	Check_GSM();
	}
	else if(gGSM_OTHER_Step==4)
	{
		if(Check_GSM()!=0)
		{
			gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;gTimer_GSM_AT=0;gTimer_CSQ=0;
		    if(Compare_String(gGeneral_Buffer,(unsigned char *)String_CSCA,sizeof(String_CSCA))!=0)
		    {

	    		if(	 ((gInternal_Flag&CENTER_NUM_VAILD_F_1)==0)
					&&(gInternal_Flag&SET_CSCA_F_1)	)
	    		{
			    	if(gGeneral_Buffer[7]=='"')
			    	{
			    		i=8;
						while(1)
						{
							if(gGeneral_Buffer[i]=='"') break;
							i++;if(i>8+18) {gReset_M22_On_Setp=1;return;}		//非法长度，则退出
						}
						if(i>8+1)
						{
							Load_SMS_CenterNum(i);
							Send_COM1_String( (unsigned char *)CENTER_RELOAD,sizeof(CENTER_RELOAD) );
							gInternal_Flag|=CENTER_NUM_VAILD_F_1;
							gInternal_Flag&=~SET_CSCA_F_1;
						}
						else
						{
							gInternal_Flag&=~CENTER_NUM_VAILD_F_1;
							gInternal_Flag|=SET_CSCA_F_1;
						}
					}
	    		}
	    		//表示已经找到SIM卡的网络
	    		gM22_ReturnER_SIM_Count=0;
				if(gM22_Status==TCP_HANG_WORK)
				{
					//进入TCP的在线状态
					gGSM_OTHER_Step=0;
					gGSM_Oper_Type=LAND_TCP_OPER;
					gLand_GPRS_Step=0;
					gGeneral_Flag|=TCP_STATUS_F_1;
				}
				else
				{
					Case2_OTHER_Out();
				}
				SRAM_Init();
				SRAM_Write(SHANDLE_DATA_VAILD+0,VAILD_2);
				SRAM_Write(SHANDLE_DATA_VAILD+1,0x24);
				SRAM_Write(SHANDLE_DATA_VAILD+2,0x06);
				SRAM_Write(SHANDLE_DATA_VAILD+3,MAIN_SEND_STATUS);
				SRAM_Write(SHANDLE_DATA_VAILD+4,gStatus1_Flag);
				SRAM_Write(SHANDLE_DATA_VAILD+5,gStatus2_Flag);
				SRAM_Write(SHANDLE_DATA_VAILD+6,0x0d);
			}
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER))!=0)
			{
				//表示还没有找到SIM卡的网络
    			gM22_ReturnER_SIM_Count++;
    			if(gM22_ReturnER_SIM_Count>20)
    			{
					gGeneral_Flag|=OFF_M22_F_1;
					gOFF_Power_S=1;
					gPhone_Status=READY;
					gM22_ReturnER_SIM_Count=0;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
					return;
    			}
    			else
    			{
					if(gM22_Status==TCP_HANG_WORK)
					{
						//进入TCP的在线状态
						gGSM_OTHER_Step=0;
						gGSM_Oper_Type=LAND_TCP_OPER;
						gLand_GPRS_Step=0;
						gGeneral_Flag|=TCP_STATUS_F_1;
					}
					else
					{
						Case2_OTHER_Out();
					}
					gTimer_CSQ=0;
    			}
			}
		}
		else
		{
			if(gTimer_GSM_AT>10000)
			{
				Case_OverTime_Return();
			}
		}
	}
	/********************************************************\
	CLCC查询返回
	\********************************************************/
	else if(gGSM_OTHER_Step==5)
	{
		if(Check_GSM()!=0)
		{
			gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;gTimer_GSM_AT=0;gTimer_CSQ=0;
		    if(Compare_String(gGeneral_Buffer,(unsigned char *)String_CLCC,sizeof(String_CLCC))!=0)
		    {
		    	Case2_OTHER_Out();
		    }
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK)))
			{
				SRAM_Init();
				SRAM_Write(PHONE_AT_2+0,VAILD_2);
				SRAM_Write(PHONE_AT_2+1,H_HUNGUP);
				SRAM_Write(PHONE_AT_2+2,0);
				Send_COM3_Load(4,MAIN_HANG_UP,0);
				gSys_Handle=MAIN_HANG_UP;
				Case2_OTHER_Out();
			}
		    else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER))!=0)
		    {
		    	Case2_OTHER_Out();
		    }
		}
		else
		{
			if(gTimer_GSM_AT>10000)
			{
				Case2_OTHER_Out();
			}
		}
	}
}
/********************************************************\
*	函数名：Case2_OTHER_Out
	作用域：本地文件调用
*	功能：	将CASE2中处理的出口
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Case2_OTHER_Out(void)
{
	gGSM_Oper_Type=GSM_SMS_IN;
	gGSM_OTHER_Step=0;gPhone_Step=0;
	gTimer_GSM_AT=0;
	Check_GSM();
	gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
	OperateSPIEnd();
}
/********************************************************\
*	函数名：Case3_GSM_SMS_IN
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	补充说明：

\********************************************************/
void Case3_GSM_SMS_IN(void)
{
	Check_GSM();
	Case3_SMSIN_Out();
}
/********************************************************\
*	函数名：Case3_SMSIN_Out
	作用域：本地文件调用
*	功能：	将CASE3中处理的出口
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Case3_SMSIN_Out(void)
{
	gGSM_Oper_Type=GSM_SMS_OUT;
	gSMS_OUT_Step=0;
	gTimer_GSM_AT=0;
	Check_GSM();
	gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
	OperateSPIEnd();
}
/********************************************************\
*	函数名：Case4_GSM_SMS_OUT
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	2004年12月15日
	补充说明：
\********************************************************/
void Case4_GSM_SMS_OUT(void)
{
	unsigned int i=0,j=0,k=0,x=0,y=0;
	unsigned char nTemp_Length=0;
	unsigned int  iLength=0;
	//1，手柄正在拨号，2，监听正在拨号，3有电话打入
	if( (gGeneral_Flag&RING_F_1)||(gGeneral_Flag&DAILING_UP_F_1)||(gGeneral_Flag&MONITOR_ON_F_1) )
	{
		Case4_SMSOUT_Out();
		return;
	}
	//2,如果目标号码无效，则不能进行SMS的发送操作
	if( (gGeneral_Flag&NUM1_VAILD_F_1)==0 )
	{
		Case4_SMSOUT_Out();
		return;
	}
	//3,如果短消息中心号码无效，则不能 SMS的发送操作
	if( (gInternal_Flag&CENTER_NUM_VAILD_F_1)==0)
	{
		Case4_SMSOUT_Out();
		return;
	}
	//4,否则的可以进行下面的操作
	if(gSMS_OUT_Step==0)	 gSMS_OUT_Step=1;

	else if(gSMS_OUT_Step==1)
	{
		gSend_Type1=0;
		SRAM_Init();
		i=SRAM_Read(ECHO_SMS_2);
		j=SRAM_Read(OTHER_SMS_2);
		//第一类的数据发送
		if( (i!=NULL_2)&&(gInternal_Flag&ALLOW_DATA_F_1) )
		{
			gSMS_OUT_Step=7;gTimer_GSM_AT=0;
			#ifdef Debug_GSM_COM1
          	Send_COM1_Byte('2');
			Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
			#endif
			gSend_Type1=1;
		}
		else if( (j!=NULL_2)&&(gInternal_Flag&ALLOW_DATA_F_1) )
		{
			gSMS_OUT_Step=7;gTimer_GSM_AT=0;
			#ifdef Debug_GSM_COM1
          	Send_COM1_Byte('3');
			Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
			#endif
			gSend_Type1=2;
		}
		//第二类数据的发送
		else if( (gInternal_Flag&SEND_CIRCLE_F_1)&&(gInternal_Flag&ALLOW_DATA_F_1) )
		{
			gInternal_Flag&=~SEND_CIRCLE_F_1;
			gInternal_Flag&=~ALLOW_DATA_F_1;
			gGSM_Work_Count=0;gCheck_Data=0;
			gSMS_OUT_Step=17;
			gTimer_GSM_AT=0;
			#ifdef Debug_GSM_COM1
          	Send_COM1_Byte('4');
			Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
			#endif
			if(gCycle_Send_Status==WATCH_DATA)
			{
				if(gWatch_Count>0)	gWatch_Count--;
			}

		}
		/**************************************************************************************\
		第三类数据的TCP发送（外接部件的大量数据-存储在外部SRAM中）
		\**************************************************************************************/
		else if( (gOther_Flag&ALLOW_SEND_RECORD_F_1)&&(gInternal_Flag&ALLOW_DATA_F_1) )
		{
			SRAM_Init();
			if( SRAM_Read(SRAM_SEND_VAILD)==VAILD_2 )
			{
				//判断长度
				j = SRAM_Read(SRAM_SEND_LENGTH_H);
				j <<= 8;
				j += SRAM_Read(SRAM_SEND_LENGTH_L);
				if(j>120)
				{
					gOther_Flag&=~RECORD_DATA_F_1;
					gOther_Flag&=~ALLOW_SEND_RECORD_F_1;
					SRAM_Init();
					SRAM_Write(SRAM_SEND_VAILD,NULL_2);
					SRAM_Write(SRAM_SEND_LENGTH_H,0);
					SRAM_Write(SRAM_SEND_LENGTH_L,0);
				}
				else
				{
					gSMS_OUT_Step=22;
					gTimer_GSM_AT=0;
					#ifdef Debug_GSM_COM1
					Send_COM1_Byte('F');Send_COM1_String((unsigned char *)TYPE,sizeof(TYPE));
					#endif
				}
			}
			else
			{
				gOther_Flag&=~RECORD_DATA_F_1;
				gOther_Flag&=~ALLOW_SEND_RECORD_F_1;
				SRAM_Init();
				SRAM_Write(SRAM_SEND_VAILD,NULL_2);
				SRAM_Write(SRAM_SEND_LENGTH_H,0);
				SRAM_Write(SRAM_SEND_LENGTH_L,0);
				Case4_SMSOUT_Out();
			}
		}
		else
		{
			Case4_SMSOUT_Out();
		}
	}
	/********************************************************\
	第一类GSM数据的发送：
	1-1:
		0x7B	存储表示这个区域的SMS是否处理，为1则表示后面还有未处理的SMS，为0则表示已经处理完了SMS
		0x7C	存储数据包的命令字在这里则为nCommand
		0x7D	存储数据包的ACK的值ACK
		0x7E	存储后面数据包的长度（但注意没有存储CHK(MSB),CHK(LSB),EOT,这三个字节）
		0x7F  	从这开始存储数据包

	1-2:
		0xB1	存储表示这个区域的SMS是否处理，为1则表示后面还有未处理的SMS，为0则表示已经处理完了SMS
		0xB2	存储数据包的命令字在这里则为nCommand
		0xB3	存储数据包的ACK的值ACK
		0xB4	存储后面数据包的长度（但注意没有存储CHK(MSB),CHK(LSB),EOT,这三个字节）
		0xB5  	从这开始存储数据包
	\********************************************************/
	else if(gSMS_OUT_Step==7)
	{
		if(gTimer_GSM_AT>300)
		{
			if(gSend_Type1==1)			Send_Type1_GSM_Data_One(ECHO_SMS_2);
			else if(gSend_Type1==2)		Send_Type1_GSM_Data_One(OTHER_SMS_2);
			gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
			gSMS_OUT_Step=8;
			gTimer_GSM_AT=0;
		}
		else Check_GSM();
	}
	else if(gSMS_OUT_Step==8)
	{
		if(Check_GSM()!=0)
		{
			gTimer_GSM_AT=0;
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_Input_Label,sizeof(String_Input_Label)))
			{
				gSMS_OUT_Step=9;
			}
		    else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER))!=0)
		    {
		    	Case4_OverTime_Out(gSend_Type1);
				Case4_SMSOUT_Out();
		    }
		}
		else
		{
			if(gTimer_GSM_AT>10000)	Case_OverTime_Return();
		}
	}
	else if(gSMS_OUT_Step==9)
	{
		if(gTimer_GSM_AT>200)
		{
			if(gSend_Type1==1)			Send_Type1_GSM_Data_Two(ECHO_SMS_2);
			else if(gSend_Type1==2)		Send_Type1_GSM_Data_Two(OTHER_SMS_2);
			gSMS_OUT_Step=10;
			gTimer_GSM_AT=0;
		}
		else Check_GSM();
	}
	else if(gSMS_OUT_Step==10)
	{
		if(Check_GSM()!=0)
		{
			gTimer_GSM_AT=0;
	    	Case4_OverTime_Out(gSend_Type1);
			Case4_SMSOUT_Out();
		}
		else
		{
			if(gTimer_GSM_AT>60000)
			{
				Case4_OverTime_Out(gSend_Type1);
				Case4_SMSOUT_Out();
			}
		}
	}
 	/********************************************************\
	二，第二类GSM数据的发送
		存储在区域的数据Circle_Buffer[]
		1，行车轨迹数据	；3，报警数据；4，监控数据，5，历史轨迹数据
		这类周期发送的数据因为不能同时存在发送的情形，则可以均用Circle_Buffer[]作为
		存储周期数据发送。
		gCycle_Send_Status=0		表示无任何周期性的数据发送
		gCycle_Send_Status=1		表示属于行车轨迹的周期数据正在发送
		gCycle_Send_Status=2		表示报警的周期数据正在发送
		gCycle_Send_Status=3		表示监控数据正在发送（另外还有标志位表示属于那一类型的监控数据）
		gCycle_Send_Status=4		表示历史轨迹数据正在发送（另外还有标志位表示属于那一类型的历史轨迹数据）
		gCycle_Send_Status=5		表示是盲区点补偿发送的数据

	AT+CMGS=27
	>
	0891683108705505F011000D91683167108230F70004000CFF4FB008000101000102090D
	27的长度为11000D91683167108230F70004000CFF4FB008000101000102090D

	\********************************************************/
	else if(gSMS_OUT_Step==17)
	{
		if( gTimer_GSM_AT>200)
		{
			nTemp_Length=SRAM_Read(TARGET_NUMBER1_2);			//目标电话号码的长度
			nTemp_Length=(nTemp_Length+1)/2+7+1+gCircle_Buffer_Point;	//要发送的数据长度
			Ask_SMS_Send(nTemp_Length);									//发送AT指令AT+CMGS=(SMS长度)
			gInternal_Flag|=GSM_ONEAT_SUCCESS_1;						//发送AT指令的标志置1
			gSMS_OUT_Step=18;
			gTimer_GSM_AT=0;
		}
		else Check_GSM();
	}
	else if(gSMS_OUT_Step==18)
	{
		if(Check_GSM()!=0)
		{
			gTimer_GSM_AT=0;
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_Input_Label,sizeof(String_Input_Label)))
			{
				gSMS_OUT_Step=19;
			}
		    else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER))!=0)
		    {
				OperateSPIEnd();
				for(i=0;i<sizeof(gCircle_Buffer);i++) gCircle_Buffer[i]=0;
				gCircle_Buffer_Point=0;
				Case4_SMSOUT_Out();
		    }
		}
		else
		{
			if(gTimer_GSM_AT>10000)	Case_OverTime_Return();
		}
	}
	else if(gSMS_OUT_Step==19)
	{
		if(gTimer_GSM_AT>200)
		{
			//这个操作发送完000400
			Send_SMS_Head();
			OperateSPIEnd();
			//发送gCircle_Buffer[]周期数据中的数据
			Send_Circle_GSM_TypeD();
			Send_COM0_Byte(0x1A);
			gSMS_OUT_Step=20;
			gTimer_GSM_AT=0;
		}
	}
	else if(gSMS_OUT_Step==20)
	{
		if(Check_GSM()!=0)
		{
			gTimer_GSM_AT=0;
			OperateSPIEnd();
			for(i=0;i<sizeof(gCircle_Buffer);i++) gCircle_Buffer[i]=0;
			gCircle_Buffer_Point=0;
			Case4_SMSOUT_Out();
		}
		else
		{
			if(gTimer_GSM_AT>60000)
			{
				gTimer_GSM_AT=0;
				for(i=0;i<sizeof(gCircle_Buffer);i++) gCircle_Buffer[i]=0;
				gCircle_Buffer_Point=0;
				Case4_SMSOUT_Out();
			}
		}
	}
	/********************************************************\
	四，第四类GSM数据的发送，外部记录仪的数据发送。注意只能发送
	外部数据的长度有限制
	\********************************************************/
	else if( gSMS_OUT_Step==22 )
	{
		if( gTimer_GSM_AT>200)
		{
			SRAM_Init();
			iLength = SRAM_Read(SRAM_SEND_LENGTH_H);
			iLength <<= 8;
			iLength += SRAM_Read(SRAM_SEND_LENGTH_L);
			nTemp_Length=SRAM_Read(TARGET_NUMBER1_2);			//目标电话号码的长度
			nTemp_Length=(nTemp_Length+1)/2+7+1+(char)iLength+13;		//要发送的数据长度
			Ask_SMS_Send(nTemp_Length);									//发送AT指令AT+CMGS=(SMS长度)
			gInternal_Flag|=GSM_ONEAT_SUCCESS_1;						//发送AT指令的标志置1
			gSMS_OUT_Step=23;
			gTimer_GSM_AT=0;
		}
		else Check_GSM();
	}
	else if( gSMS_OUT_Step==23 )
	{
		if(Check_GSM()!=0)
		{
			gTimer_GSM_AT=0;
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_Input_Label,sizeof(String_Input_Label)))
			{
				gSMS_OUT_Step=24;
			}
		    else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER))!=0)
		    {
				gOther_Flag&=~RECORD_DATA_F_1;
				gOther_Flag&=~ALLOW_SEND_RECORD_F_1;
				SRAM_Init();
				SRAM_Write(SRAM_SEND_VAILD,NULL_2);
				SRAM_Write(SRAM_SEND_LENGTH_H,0);
				SRAM_Write(SRAM_SEND_LENGTH_L,0);
				Case4_SMSOUT_Out();
		    }
		}
		else
		{
			if(gTimer_GSM_AT>10000)	Case_OverTime_Return();
		}
	}
	else if( gSMS_OUT_Step==24 )
	{
		if(gTimer_GSM_AT>200)
		{
			//这个操作发送完000400
			Send_SMS_Head();
			OperateSPIEnd();
			Send_Circle_GSM_TypeE();
			Send_COM0_Byte(0x1A);
			gSMS_OUT_Step=25;
			gTimer_GSM_AT=0;
		}
	}
	else if(gSMS_OUT_Step==25)
	{
		if(Check_GSM()!=0)
		{
			gOther_Flag&=~RECORD_DATA_F_1;
			gOther_Flag&=~ALLOW_SEND_RECORD_F_1;
			SRAM_Init();
			SRAM_Write(SRAM_SEND_VAILD,NULL_2);
			SRAM_Write(SRAM_SEND_LENGTH_H,0);
			SRAM_Write(SRAM_SEND_LENGTH_L,0);
			Case4_SMSOUT_Out();
		}
		else
		{
			if(gTimer_GSM_AT>60000)
			{
				gTimer_GSM_AT=0;
				gOther_Flag&=~RECORD_DATA_F_1;
				gOther_Flag&=~ALLOW_SEND_RECORD_F_1;
				SRAM_Init();
				SRAM_Write(SRAM_SEND_VAILD,NULL_2);
				SRAM_Write(SRAM_SEND_LENGTH_H,0);
				SRAM_Write(SRAM_SEND_LENGTH_L,0);
				Case4_SMSOUT_Out();
			}
		}
	}
}
/********************************************************\
*	函数名：Case4_SMSOUT_Out
	作用域：本地文件调用
*	功能：	将CASE3中处理的出口
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Case4_SMSOUT_Out(void)
{
	gGSM_Oper_Type=LAND_TCP_OPER;
	gLand_GPRS_Step=0;
	gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
	gSMS_OUT_Step=0;gTimer_GSM_AT=0;
	OperateSPIEnd();
}
/********************************************************\
*	函数名：Case4_OverTime_Out
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Case4_OverTime_Out(unsigned char Clear_Flag)
{
	unsigned int i=0;
	OperateSPIEnd();
	SRAM_Init();
	if(Clear_Flag==1)
	{
		SRAM_Write(ECHO_SMS_2+0,NULL_2);
		for(i=ECHO_SMS_2+1;i<OTHER_SMS_2;i++)
		{
			SRAM_Write(i,0);
		}
	}
	else if(Clear_Flag==2)
	{
		SRAM_Write(OTHER_SMS_2+0,NULL_2);
		for(i=OTHER_SMS_2+1;i<END_SRAM_2;i++)
		{
			SRAM_Write(i,0);
		}
	}
	gSend_Type1=0;
}
/********************************************************\
*	函数名：Case5_CHECK_GPRS_LINK
	作用域：本地文件调用
*	功能：1，进行TCP登陆 2，退出TCP
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Case5_LAND_GPRS_OPER(void)
{
	unsigned int i=0,j=0,k=0;

	if(gLand_GPRS_Step==0)	{gLand_GPRS_Step=1;gTimer_GSM_AT=0;Check_GSM();}

	else if(gLand_GPRS_Step==1)
	{
		/********************************************************\
		A 1,如果处于GSM的工作方式下，2，有需要进行TCP登陆的标志
			首先执行一个ATH指令
		\********************************************************/
		if( (gM22_Status==GSM_WORK)&&(gGeneral_Flag&LAND_TCP_F_1)
			&&((gGeneral_Flag&LAND_TCP_ERROR_F_1)==0)
			&&((gGeneral_Flag&TCP_PHONE_F_1)==0) )
		{
			if( gTimer_GSM_AT>500 )
			{
				Send_COM0_String((unsigned char *)ATH,sizeof(ATH));
				gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
				gTimer_GSM_AT=0;
				gLand_GPRS_Step=2;
			}
			else	Check_GSM();
		}
		/********************************************************\
		B 1,如果处于TCP在线挂起状态下，2，有需要进行TCP准备好退出登陆的标志||
		\********************************************************/
		else if( (gM22_Status==TCP_HANG_WORK)&&(gOther_Flag&QUIT_TCP_F_1) )
		{
			if( gTimer_GSM_AT>500 )
			{
				Send_COM0_String((unsigned char *)ATH,sizeof(ATH));
				gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
				gTimer_GSM_AT=0;
				gLand_GPRS_Step=12;
			}
			else	Check_GSM();
		}
		/********************************************************\
		C 1,TCP在线挂起状态 ，2，判断有需要需要发送ATO操作的标志
		\********************************************************/
		else if( (gM22_Status==TCP_HANG_WORK)
				&&(gGeneral_Flag&TCP_STATUS_F_1)
				&&((gGeneral_Flag&TCP_PHONE_F_1)==0)  )
		{
			if( gTimer_GSM_AT>500 )
			{
				Send_COM0_String((unsigned char *)ATO,sizeof(ATO));
				gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
				gTimer_GSM_AT=0;
				gLand_GPRS_Step=14;
			}
			else	Check_GSM();
		}
		/********************************************************\
		如果上面条件均不满足，则退出这个CASE
		\********************************************************/
		else
		{
			if(gM22_Status==GSM_WORK) gOther_Flag&=~QUIT_TCP_F_1;
			/********************************************************\
			判断在GSM的状态下是否需要进行TCP登陆的操作
			1，IP地址有效，2，TCP开关量开启状态，3，GSM的工作方式，4，允许进行TCP登陆的操作
			\********************************************************/
			if(   (gGeneral_Flag&TCP_IP_VALID_1)
				&&((gON_OFF_Temp0&TCP_LAND_ON_OFF_1)==0)
				&&(gM22_Status==GSM_WORK)
				&&((gGeneral_Flag&LAND_TCP_ERROR_F_1)==0) )
			{
				//1,如果有报警信息
				if(   (gCycle_Send_Status==ALARM_DATA)
					&&(gCycle_Alarm>0) )
				{
					gGeneral_Flag|=LAND_TCP_F_1;
					gGSM_Oper_Type=LAND_TCP_OPER;
					gLand_GPRS_Step=0;
					gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
				}
				//2,如果有行车轨迹，且行车轨迹的周期小于20分钟，则允许长期在线，则进行登陆TCP的操作
				else if(   (gCycle_Send_Status==TRACK_DATA_TIME)
						 &&(gCycle_TCP<TCP_TRACK_MINTIME)
						 &&(gCycle_TCP>0) )
				{
					gGeneral_Flag|=LAND_TCP_F_1;
					gGSM_Oper_Type=LAND_TCP_OPER;
					gLand_GPRS_Step=0;
					gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
				}
				//3,如果是行车轨迹周期大于20分钟，且已经到有需要发送的数据包，则需要进行登陆
				else if(  (gCycle_Send_Status==TRACK_DATA_TIME)
						&&(gCycle_TCP>=TCP_TRACK_MINTIME)
						&&(gInternal_Flag&SEND_CIRCLE_F_1) )
				{
					gGeneral_Flag|=LAND_TCP_F_1;
					gGSM_Oper_Type=LAND_TCP_OPER;
					gLand_GPRS_Step=0;
					gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
				}
				//4,如果属于历史轨迹查询或者重点监控状态
				else if(   (gCycle_Send_Status==PASS_DATA)
						 ||(gCycle_Send_Status==WATCH_DATA)  )
				{
					gGeneral_Flag|=LAND_TCP_F_1;
					gGSM_Oper_Type=LAND_TCP_OPER;
					gLand_GPRS_Step=0;
					gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
				}
				//5,如果属于间隔距离传输数据点，并且TCP属于常开状态,则需要登陆TCP
				else if(   (gCycle_Send_Status==TRACK_DATA_DISTANCE)
						 &&((gON_OFF_Temp1&DISTANCE_TCP_ON_1)==0) )
				{
					gGeneral_Flag|=LAND_TCP_F_1;
					gGSM_Oper_Type=LAND_TCP_OPER;
					gLand_GPRS_Step=0;
					gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
				}
				//6,如果属于间隔距离传输数据点，并且TCP属于不常在线状态，但有数据点需要发送，则需要进行登陆
				else if(   (gCycle_Send_Status==TRACK_DATA_DISTANCE)
						 &&(gON_OFF_Temp1&DISTANCE_TCP_ON_1)
						 &&(gInternal_Flag&SEND_CIRCLE_F_1) )
				{
					gGeneral_Flag|=LAND_TCP_F_1;
					gGSM_Oper_Type=LAND_TCP_OPER;
					gLand_GPRS_Step=0;
					gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
				}
				//7, 如果有记录仪的数据需要上行给数据中心的，则先进行登陆TCP的操作
				else if( gOther_Flag&RECORD_DATA_F_1)
				{
					gGeneral_Flag|=LAND_TCP_F_1;
					gGSM_Oper_Type=LAND_TCP_OPER;
					gLand_GPRS_Step=0;
					gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
				}
				//8,如果其他要发送的数据，则需要先登陆TCP
				else if( gOther_Flag&NEED_LADN_TCP_F_1 )
				{
					gOther_Flag&=~NEED_LADN_TCP_F_1;
					gGeneral_Flag|=LAND_TCP_F_1;
					gGSM_Oper_Type=LAND_TCP_OPER;
					gLand_GPRS_Step=0;
					gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
				}
				//如果其他
				else
				{
					gGSM_Oper_Type=GSM_PHONE;gPhone_Step=0;
					gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
					gLand_GPRS_Step=0;
				}
			}
			else
			{
				gGSM_Oper_Type=GSM_PHONE;gPhone_Step=0;
				gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
				gLand_GPRS_Step=0;
			}
			gTimer_GSM_AT=0;
			OperateSPIEnd();
		}
	}
	else if(gLand_GPRS_Step==2)
	{
		if( Check_GSM()!=0 )
		{
			gLand_GPRS_Step=3;
		}
		else
		{
			if(gTimer_GSM_AT>10000)	Case_OverTime_Return();
		}
	}
	else if(gLand_GPRS_Step==3)
	{
		if( gTimer_GSM_AT>500 )
		{
			Send_COM0_String((unsigned char *)AT_CGDCONT,sizeof(AT_CGDCONT));
			j=SRAM_Read(GPRS_APN_ADDRESS2_2);
			for(i=0;i<j;i++)
			{
				Send_COM0_Byte(SRAM_Read(GPRS_APN_ADDRESS2_2+1+i));
			}
			Send_COM0_Byte(0x0d);
			gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
			gTimer_GSM_AT=0;
			gLand_GPRS_Step=4;
		}
		else	Check_GSM();
	}
	//A-1
	else if(gLand_GPRS_Step==4)
	{
		if( Check_GSM()!=0 )
		{
			gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;gTimer_GSM_AT=0;
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK)))
			{
				gLand_GPRS_Step=5;
			}
		}
		else
		{
			if(gTimer_GSM_AT>10000)
			{
				TCP_Land_Error_Return();
			}
		}
	}
	//A-2
	else if(gLand_GPRS_Step==5)
	{
		if( gTimer_GSM_AT>500 )
		{
			Send_COM0_String((unsigned char *)AT_CGPCO,sizeof(AT_CGPCO));
			gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
			gTimer_GSM_AT=0;
			gLand_GPRS_Step=6;
		}
		else	Check_GSM();
	}
	//A-3
	else if(gLand_GPRS_Step==6)
	{
		if( Check_GSM()!=0 )
		{
			gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;gTimer_GSM_AT=0;
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK)))
			{
				gLand_GPRS_Step=7;
			}
		}
		else
		{
			if(gTimer_GSM_AT>10000)
			{
				//表示登陆TCP失败的处理
				TCP_Land_Error_Return();
			}
		}
	}
	//A-4
	else if(gLand_GPRS_Step==7)
	{
		if( gTimer_GSM_AT>500 )
		{
			Send_COM0_String((unsigned char *)AT_DESTINFO,sizeof(AT_DESTINFO));
			//1,从buffer1中取出IP地址
			Send_COM0_Byte('"');
			j=SRAM_Read(GPRS_IP_ADDRESS1_2);
			for(i=0;i<j;i++)
			{
				Send_COM0_Byte(SRAM_Read(GPRS_IP_ADDRESS1_2+1+i));
			}
			Send_COM0_Byte('"');
			Send_COM0_Byte(',');
			Send_COM0_Byte('1');
			Send_COM0_Byte(',');
			//2，从buffer中取出端口号，在buffer1中端口号是以5位ASCII形式存储的，如果实际有效数据不够5位，则以前面填0
			//例如：端口号为9988，在buffer1中相应的区域存储为30 39 39 38 38
			gGeneral_Buffer[0]=SRAM_Read(GPRS_SOCKET1_2);
			gGeneral_Buffer[1]=SRAM_Read(GPRS_SOCKET1_2+1);
			gGeneral_Buffer[2]=SRAM_Read(GPRS_SOCKET1_2+2);
			gGeneral_Buffer[3]=SRAM_Read(GPRS_SOCKET1_2+3);
			gGeneral_Buffer[4]=SRAM_Read(GPRS_SOCKET1_2+4);
			OperateSPIEnd();
			i=0;
			while(1)
			{
				//从最高位开始判断，如果不为0，则后面的均为有效数据
				if(gGeneral_Buffer[i]!=0x30) break;
				i++;
				if(i==5) break;
			}
			if(i<5)
			{
				for(j=i;j<5;j++)
				{
					Send_COM0_Byte(gGeneral_Buffer[j]);
				}
			}
			else
			{
				gLand_GPRS_Step=0;
				return;
			}
			Send_COM0_Byte(0x0d);
			gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
			gLand_GPRS_Step=8;
			gTimer_GSM_AT=0;
		}
		else 		Check_GSM();
	}
	//A-5
	else if(gLand_GPRS_Step==8)
	{
		if( Check_GSM()!=0 )
		{
			gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;gTimer_GSM_AT=0;
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_DESTINFO,sizeof(String_DESTINFO)))
			{
				gLand_GPRS_Step=9;
			}
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER)))
			{
				TCP_Land_Error_Return();
			}
		}
		else
		{
			if(gTimer_GSM_AT>10000)
			{
				//表示登陆TCP失败的处理
				TCP_Land_Error_Return();
			}
		}
	}
	//A-6
	else if(gLand_GPRS_Step==9)
	{
		if(gTimer_GSM_AT>1500)
		{
			Send_COM0_String((unsigned char *)ATD_97,sizeof(ATD_97));
			gInternal_Flag|=GSM_ONEAT_SUCCESS_1;
			gLand_GPRS_Step=10;
			gTimer_GSM_AT=0;
		}
		else	Check_GSM();
	}
	//A-7
	else if(gLand_GPRS_Step==10)
	{
		if( Check_GSM()!=0 )
		{
			gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;gTimer_GSM_AT=0;
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_L_TCP,sizeof(String_L_TCP)))
			{
				gLand_GPRS_Step=11;
			}
			//返回ERROR，说明SIM卡不支持TCP的工作方式或者说任何场强信号很弱的时候
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER)))
			{
				TCP_Land_Error_Return();
			}
		}
		else
		{
			if(gTimer_GSM_AT>65000)
			{
				TCP_Land_Error_Return();
			}
		}
	}
	//A-8
	else if(gLand_GPRS_Step==11)
	{
		if( Check_GSM()!=0 )
		{
			gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;gTimer_GSM_AT=0;
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK)))
			{
				//说明登陆成功,此时不能进行AT指令的任何操作，直接进入CASE6中
				gGeneral_Flag&=~LAND_TCP_F_1;
				gLand_TCP_ERROR_Count=0;
				gGSM_Oper_Type=GPRS_DATA_OPER;
				gM22_Status=TCP_ONLINE_WORK;
				gLand_GPRS_Step=0;
				gTimer_CSQ=0;
				for(i=0;i<sizeof(gReceive0_GSM_Buffer);i++)	gReceive0_GSM_Buffer[i]=0;
				gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
				gReceive0_GSM_Buffer_Point=0;
				gReceive0_GSM_Buffer_Move=0;
				gGSM_Work_Count=0;gCheck_Data=0;
				gInternal_Flag&=~ALLOW_DATA_F_1;
				//发送登陆TCP的数据
				gOther_Flag|=ALLOW_SENDLAND_F_1;
				gGeneral_Flag&=~LAND_TCP_SUCCESS_F_1;
			}
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER)))
			{
				gLand_GPRS_Step=111;
			}
		}
		else
		{
			if(gTimer_GSM_AT>65000)
			{
				TCP_Land_Error_Return();
			}
		}
	}
	else if(gLand_GPRS_Step==111)
	{
		if( Check_GSM()!=0 )
		{
			TCP_Land_Error_Return();
		}
		else
		{
			if(gTimer_GSM_AT>40000)
			{
				TCP_Land_Error_Return();
			}
		}
	}
	//B-1
	else if(gLand_GPRS_Step==12)
	{
		if( Check_GSM()!=0 )
		{
			gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;gTimer_GSM_AT=0;
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK)))
			{
				gLand_GPRS_Step=13;
				gOther_Flag&=~QUIT_TCP_F_1;
			}
		}
		else
		{
			if(gTimer_GSM_AT>20000)
			{
				Case_OverTime_Return();
			}
		}
	}
	//B-2
	else if(gLand_GPRS_Step==13)
	{
		if( Check_GSM()!=0 )
		{
			gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;gTimer_GSM_AT=0;
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_NOCARRIER,sizeof(String_NOCARRIER)))
			{
				gM22_Status=GSM_WORK;
				gGeneral_Flag&=~LAND_TCP_SUCCESS_F_1;
				gGSM_Oper_Type=GSM_PHONE;
				gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
				gPhone_Step=0;gTimer_GSM_AT=0;
				gOther_Flag&=~QUIT_TCP_F_1;
			}
		}
		else
		{
			if(gTimer_GSM_AT>20000)
			{
				Case_OverTime_Return();
			}
		}
	}
	//C-1
	else if(gLand_GPRS_Step==14)
	{
		if( Check_GSM()!=0 )
		{
			gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;gTimer_GSM_AT=0;
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_CONNECT,sizeof(String_CONNECT)))
			{
				//进入TCP在线状态
				gM22_Status=TCP_ONLINE_WORK;
				gLand_GPRS_Step=0;
				gGeneral_Flag&=~TCP_STATUS_F_1;
				gReceive0_GSM_Buffer_Move=gReceive0_GSM_Buffer_Point;
				if(gReceive0_GSM_Buffer_Point==0) 	gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
				else     							gReceive0_GSM_Buffer_End=gReceive0_GSM_Buffer_Point-1;
				gGSM_Oper_Type=GPRS_DATA_OPER;
				gGPRS_Data_Step=0;
				gTimer_CSQ=0;
				//登陆成功，如果有报警，
				if(gCycle_Send_Status==ALARM_DATA)
				{
					gCycle_Alarm=ALARM_CIRCLE_TIME;
					gAlarm_Count=0;
				}
			}
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER)))
			{
				//进入GSM的工作方式下
				gReset_M22_On_Setp=0;
				gGeneral_Flag&=~RESET_GSM_ON_F_1;
				gM22_Status=GSM_WORK;
				gGeneral_Flag&=~LAND_TCP_SUCCESS_F_1;
				gGSM_Oper_Type=GSM_PHONE;
				gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
				gPhone_Step=0;gTimer_GSM_AT=0;
			}
		}
		else
		{
			if(gTimer_GSM_AT>65000)	Case_OverTime_Return();
		}
	}
}
/********************************************************\
*	函数名：TCP_Land_Error_Return
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	现在如果TCP登陆失败，则重新初始化M22模块，不采用掉电的方式，,如果连续登陆失败5次，则需要进行
	模块掉电处理。
\********************************************************/
void TCP_Land_Error_Return(void)
{
	Send_COM1_String((unsigned char *)TCP_LAND_ERROR,sizeof(TCP_LAND_ERROR));
	gGeneral_Flag|=RESET_GSM_ON_F_1;
	gReset_M22_On_Setp=1;
	gM22_Status=GSM_WORK;
	gM22_GSM_ECHO_Timer=0;
	gLand_Interval_Timer=0;
	gTCP_FF0D_Send_Timer=0;
	gGPS_No_Output_Timer=0;
	gTimer_GSM_AT=0;
	gTimer_CSQ=0;
	gGPS_Receive_Over=0;

    gPhone_Status=READY;
	gStatus1_Flag&=~PHONE_ONLINE_F_1;
	gGeneral_Flag&=~DAILING_UP_F_1;
	gGeneral_Flag&=~MONITOR_ON_F_1;
	gGeneral_Flag&=~TCP_PHONE_F_1;
	gGeneral_Flag&=~RING_F_1;
	gGSM_Oper_Type=0;
	gSMS_IN_Step=0;
	gSMS_OUT_Step=0;
	gLand_GPRS_Step=0;
	gGPRS_Data_Step=0;
	gGSM_OTHER_Step=0;

	gGeneral_Flag|=LAND_TCP_ERROR_F_1;
	gLand_TCP_ERROR_Count++;
	if(gLand_TCP_ERROR_Count>5)
	{
		gLand_TCP_ERROR_Count=0;
		gGeneral_Flag|=OFF_M22_F_1;
		gOFF_Power_S=1;
		gPhone_Status=READY;
		gStatus1_Flag&=~PHONE_ONLINE_F_1;
	}

	//如果属于周期数据的发送
	if(gInternal_Flag&SEND_CIRCLE_F_1)
	{
		//1，按时间间隔上报数据，且间隔大于分界值
		if( (gCycle_Send_Status==TRACK_DATA_TIME)&&(gCycle_TCP>TCP_TRACK_MINTIME) )
		{
			gInternal_Flag|=ALLOW_DATA_F_1;
		}
		else
		{
			gInternal_Flag&=~ALLOW_DATA_F_1;
			gInternal_Flag&=~SEND_CIRCLE_F_1;
		}
	}
	//如果属于外部行使记录仪数据的发送，
	else if(gOther_Flag&RECORD_DATA_F_1)
	{
		gOther_Flag&=~RECORD_DATA_F_1;
		gOther_Flag&=~ALLOW_SEND_RECORD_F_1;
		SRAM_Init();
		SRAM_Write(SRAM_SEND_VAILD,NULL_2);
		SRAM_Write(SRAM_SEND_LENGTH_H,0);
		SRAM_Write(SRAM_SEND_LENGTH_L,0);
		gInternal_Flag&=~ALLOW_DATA_F_1;
	}
	else
	{
		gInternal_Flag|=ALLOW_DATA_F_1;
	}
}
/********************************************************\
*	函数名：Case6_GPRS_DATA_OPER
	作用域：本地文件调用
*	功能：	进行与GPRS相关的数据操作，以发送为主，接收数据则在Check_GSM()中直接处理
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	2004年12月15日
	补充说明：
	1，如果在这个CASE外判断有数据要发送，同时TCP处于被挂起状态，则首先执行ATO指令后，然后进入这个
	CASE中进行通过TCP方式发送数据：一旦数据发送完成，则执行+++，将TCP的状态挂起，但如果发送的数据
	属于握手信号FF 0D，则需要发送完数据后，执行一个等待的时间，判断是否接收到下行的握手数据
	不管是否接收到下行的握手数据，在等待时间过后，则执行+++，将TCP挂起！但会将这次握手失败的记数增加一
\********************************************************/
void Case6_GPRS_DATA_OPER(void)
{
 	unsigned int i=0,j=0,k=0,x=0,y=0;
	unsigned char nTemp_Length=0;
	static unsigned char Hang_Error=0;

	if(gM22_TCP_ECHO_Timer>200)				gInternal_Flag|=TCPSEND_ALLOW_F_1;
	else if(gM22_TCP_ECHO_Timer==0)			gInternal_Flag&=~TCPSEND_ALLOW_F_1;

    if(gM22_Status!=TCP_ONLINE_WORK)
    {
		gGSM_Oper_Type=GSM_PHONE;
		gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
		gPhone_Step=0;gTimer_GSM_AT=0;
    	return;
    }
	if(gGPRS_Data_Step==0)	 			gGPRS_Data_Step=1;
	else if(gGPRS_Data_Step==1)
	{
		Check_TCP();
		gSend_Type1=0;
		SRAM_Init();
		i=SRAM_Read(ECHO_SMS_2);		//存储是否有第二类发送数据的标志
		j=SRAM_Read(OTHER_SMS_2);		//存储是否有第三类发送数据的标志
		//发送登陆信息数据
		if(gOther_Flag&ALLOW_SENDLAND_F_1)
		{
			gOther_Flag&=~ALLOW_SENDLAND_F_1;
			gGPRS_Data_Step=12;gTimer_GSM_AT=0;
			Send_COM1_String((unsigned char *)CONNCET_TCP,sizeof(CONNCET_TCP));
		}
		//第一类数据的TCP发送（存储在buffer1中的数据）
		else if( (i!=NULL_2)&&(gGeneral_Flag&LAND_TCP_SUCCESS_F_1) )
		{
			gGPRS_Data_Step=2;gTimer_GSM_AT=0;
			gSend_Type1=1;
			#ifdef Debug_GSM_COM1
			Send_COM1_Byte('B');Send_COM1_String((unsigned char *)TYPE,sizeof(TYPE));
			#endif
		}
		else if( (j!=NULL_2)&&(gGeneral_Flag&LAND_TCP_SUCCESS_F_1)  )
		{
			gGPRS_Data_Step=2;gTimer_GSM_AT=0;
			gSend_Type1=2;
			#ifdef Debug_GSM_COM1
			Send_COM1_Byte('C');Send_COM1_String((unsigned char *)TYPE,sizeof(TYPE));
			#endif
		}
		//第二类数据的TCP发送（周期数据）
		else if( (gInternal_Flag&SEND_CIRCLE_F_1)&&(gGeneral_Flag&LAND_TCP_SUCCESS_F_1) )
		{
			gInternal_Flag&=~SEND_CIRCLE_F_1;
			gGPRS_Data_Step=3;gTimer_GSM_AT=0;
			if(gCycle_Send_Status==TRACK_DATA_TIME)				Send_COM1_String((unsigned char *)TRACK,sizeof(TRACK));
			else if(gCycle_Send_Status==ALARM_DATA)				Send_COM1_String((unsigned char *)ALARM,sizeof(ALARM));
			else if(gCycle_Send_Status==TRACK_DATA_DISTANCE)	Send_COM1_String((unsigned char *)DISTANCE,sizeof(DISTANCE));
			else if(gCycle_Send_Status==WATCH_DATA)
			{
				Send_COM1_String((unsigned char *)WATCH,sizeof(WATCH));
				if(gWatch_Count>0)	gWatch_Count--;
			}
			Send_COM1_String((unsigned char *)TYPE,sizeof(TYPE));
		}
		//第三类数据的TCP发送（FF 0D ）
		else if ( (gTCP_FF0D_Send_Timer>TCP_TEST_TIME) )
		{
			gTCP_FF0D_Send_Timer=0;
			gGPRS_Data_Step=4;gTimer_GSM_AT=0;
			#ifdef Debug_GSM_COM1
			Send_COM1_Byte('E');Send_COM1_String((unsigned char *)TYPE,sizeof(TYPE));
			#endif
			gTimer_CSQ=0;
		}
		//第四类数据的发送（外部记录仪数据）
		else if ( (gOther_Flag&ALLOW_SEND_RECORD_F_1)&&(gGeneral_Flag&LAND_TCP_SUCCESS_F_1) )
		{
			SRAM_Init();
			if( SRAM_Read(SRAM_SEND_VAILD)==VAILD_2 )
			{
				gGPRS_Data_Step=5;gTimer_GSM_AT=0;
				#ifdef Debug_GSM_COM1
				Send_COM1_Byte('F');Send_COM1_String((unsigned char *)TYPE,sizeof(TYPE));
				#endif
				//显示可以发送的数据的长度
				i=0;
				i=SRAM_Read(SRAM_SEND_LENGTH_H);
				i<<=8;
				i=i+SRAM_Read(SRAM_SEND_LENGTH_L);
				Send_COM1_Byte('[');
	            Send_COM1_Byte(ASCII(i/0x100));
	            Send_COM1_Byte(ASCII((i%0x100)/0x10));
	            Send_COM1_Byte(ASCII(i%0x10));
	            Send_COM1_Byte(']');
	            gTCP_FF0D_Send_Timer=0;
	            //////////////////////////
	            /*
	            #ifdef Debug_GSM_COM1
				for(j=0;j<i;j++)
				{
					k=SRAM_Read(SRAM_SEND_S+j);
					Send_COM1_Byte(ASCII(k/0x10));
					Send_COM1_Byte(ASCII(k%0x10));
					Clear_Exter_WatchDog();
				}
	            //////////////////////////
	            Send_COM1_Byte(0x0d);Send_COM1_Byte(0xa);
	            #endif
	            */
			}
			else
			{
				gOther_Flag&=~RECORD_DATA_F_1;
				gOther_Flag&=~ALLOW_SEND_RECORD_F_1;
				SRAM_Init();
				SRAM_Write(SRAM_SEND_VAILD,NULL_2);
				SRAM_Write(SRAM_SEND_LENGTH_H,0);
				SRAM_Write(SRAM_SEND_LENGTH_L,0);
				gGPRS_Data_Step=0;
			}
		}
		//8,如果没有判断到有发送的数据，如果到了需要查询CSQ的时间到，则发送挂起操作
		else if(   (gTimer_CSQ>TCP_CHECKCSQ_TIME)
				 &&(gGeneral_Flag&LAND_TCP_SUCCESS_F_1)
				 &&((gOther_Flag&RECORD_DATA_F_1)==0) )
		{
			gGPRS_Data_Step=9;gTimer_GSM_AT=0;
		}
		//9,判断是否需要退出TCP在线
		else if(gOther_Flag&QUIT_TCP_F_1)
		{
			gOther_Flag&=~NEED_LADN_TCP_F_1;
			gGPRS_Data_Step=9;gTimer_GSM_AT=0;
		}
		//10,如果有电话相关的业务，则需要先挂起TCP
		else if(gGeneral_Flag&TCP_PHONE_F_1)
		{
			gGPRS_Data_Step=9;gTimer_GSM_AT=0;
		}
		//11,如果没有上面的数据，则执行接收TCP数据的处理
		else
		{
			Check_TCP();
			gGPRS_Data_Step=0;gTimer_GSM_AT=0;
			i=SRAM_Read(ECHO_SMS_2);		//存储是否有第二类发送数据的标志
			j=SRAM_Read(OTHER_SMS_2);		//存储是否有第三类发送数据的标志
			/********************************************************\
			判断在TCP在线的状态下是否需要进行TCP退出的操作
			\********************************************************/
			//1,判断如果是属于行车轨迹状态，同时周期数据大于10分钟,并且无周期数据发送
			if(  	  (gCycle_Send_Status==TRACK_DATA_TIME)
					&&(gCycle_TCP>=TCP_TRACK_MINTIME)
					&&(gGeneral_Flag&LAND_TCP_SUCCESS_F_1)
					&&((gInternal_Flag&SEND_CIRCLE_F_1)==0)
					&&((gOther_Flag&RECORD_DATA_F_1)==0)  )
			{
				gOther_Flag|=QUIT_TCP_F_1;
			}
			//2,    判断如果属于报警状态，同时发送的周期数据大于10分钟秒，并且无周期数据发送
			else if(  (gCycle_Send_Status==ALARM_DATA)
					&&(gCycle_Alarm>=TCP_TRACK_MINTIME)
					&&(gGeneral_Flag&LAND_TCP_SUCCESS_F_1)
					&&((gInternal_Flag&SEND_CIRCLE_F_1)==0)
					&&((gOther_Flag&RECORD_DATA_F_1)==0)  )
			{
				gOther_Flag|=QUIT_TCP_F_1;
			}
			//3,如果属于按间隔发送，且不属于常在线方式，并且无周期数据发送
			else if(  (gCycle_Send_Status==TRACK_DATA_DISTANCE)
					&&(gON_OFF_Temp1&DISTANCE_TCP_ON_1)
					&&(gGeneral_Flag&LAND_TCP_SUCCESS_F_1)
					&&((gInternal_Flag&SEND_CIRCLE_F_1)==0)
					&&((gOther_Flag&RECORD_DATA_F_1)==0)  )
			{
				gOther_Flag|=QUIT_TCP_F_1;
			}
			//3,无任何需要发送的数据，则需要退出TCP
			else if(  (gCycle_Send_Status==NOP_DATA)
					&&(gGeneral_Flag&LAND_TCP_SUCCESS_F_1)
					&&(i==NULL_2)
					&&(j==NULL_2)
					&&( (gOther_Flag&RECORD_DATA_F_1)==0)  )
			{
				gOther_Flag|=QUIT_TCP_F_1;
			}
			else
			{
				j=SRAM_Read(PHONE_AT_2);
				if(j==VAILD_2)
				{
					i=SRAM_Read(PHONE_AT_2+1);
					if(i==H_DIAL)			//手柄主动拨号
					{
						gGeneral_Flag|=TCP_PHONE_F_1;
						gStatus1_Flag|=PHONE_ONLINE_F_1;
						gGeneral_Flag|=DAILING_UP_F_1;
					}
					else if(i==M_DIAL)		//监听主动拨号
					{
						gGeneral_Flag|=MONITOR_ON_F_1;
						gGeneral_Flag|=TCP_PHONE_F_1;
						gStatus1_Flag|=PHONE_ONLINE_F_1;
					}
					else if(i==H_HUNGUP)
					{
						SRAM_Init();
						SRAM_Write(PHONE_AT_2,NULL_2);
					}
				}

			}
		}
	}
	/********************************************************\
	一，第一类数据的发送
	1-1，
		0x7B	存储表示这个区域的SMS是否处理，为1则表示后面还有未处理的SMS，为0则表示已经处理完了SMS
		0x7C	存储数据包的命令字在这里则为nCommand
		0x7D	存储数据包的ACK的值ACK
		0x7E	存储后面数据包的长度（但注意没有存储CHK(MSB),CHK(LSB),EOT,这三个字节）
		0x7F  	从这开始存储数据包
	1-2
		0xB1	存储表示这个区域的SMS是否处理，为1则表示后面还有未处理的SMS，为0则表示已经处理完了SMS
		0xB2	存储数据包的命令字在这里则为nCommand
		0xB3	存储数据包的ACK的值ACK
		0xB4	存储后面数据包的长度（但注意没有存储CHK(MSB),CHK(LSB),EOT,这三个字节）
		0xB5  	从这开始存储数据包
	\********************************************************/
	else if(gGPRS_Data_Step==2)
	{
		if( (gTimer_GSM_AT>200)&&(gInternal_Flag&TCPSEND_ALLOW_F_1) )
		{
			OperateSPIEnd();
			if(gSend_Type1==1)
			{
				i=SRAM_Read(ECHO_SMS_2+1);
				j=SRAM_Read(ECHO_SMS_2+2);				//ACK
				k=SRAM_Read(ECHO_SMS_2+3);				//长度
			}
			else if(gSend_Type1==2)
			{
				i=SRAM_Read(OTHER_SMS_2+1);
				j=SRAM_Read(OTHER_SMS_2+2);				//ACK
				k=SRAM_Read(OTHER_SMS_2+3);				//长度
			}
			OperateSPIEnd();
			//一直发送到VID
			x=Send_Echo_TCP_Head( (char)(k+12),(char)i,(char)j,(char)(k+12-4) );
			//发送数据区ECHO_SMS_2为buffer1的起始地址，k为数据长度
			if(gSend_Type1==1)			y=Send_Buffer1_TCP_Data(x,ECHO_SMS_2+4,k);
			else if(gSend_Type1==2)		y=Send_Buffer1_TCP_Data(x,OTHER_SMS_2+4,k);
			//发送校验核和结束符
			Send_Echo_TCP_End(y);
			Case4_OverTime_Out(gSend_Type1);
			gGPRS_Data_Step=10;
		}
		else	Check_TCP();
	}
 	/********************************************************\
	二，
		存储在区域的数据Circle_Buffer[]
		1，行车轨迹数据	；2，报警数据
		这5类周期发送的数据因为不能同时存在发送的情形，则可以均用Circle_Buffer[]作为
		存储周期数据发送。
		gCycle_Send_Status=1		表示属于行车轨迹的周期数据正在发送
		gCycle_Send_Status=2		表示报警的周期数据正在发送
		gCycle_Send_Status=3		表示监控数据正在发送（另外还有标志位表示属于那一类型的监控数据）
	\********************************************************/
	else if(gGPRS_Data_Step==3)
	{
		if( (gTimer_GSM_AT>200)&&(gInternal_Flag&TCPSEND_ALLOW_F_1) )
		{
			Send_Circle_TCP_TypeD();
			for(i=0;i<sizeof(gCircle_Buffer);i++) gCircle_Buffer[i]=0;
			gCircle_Buffer_Point=0;
			gGPRS_Data_Step=10;
		}
		else	Check_TCP();
	}
 	/********************************************************\
	三，发送TCP数据FF 0D
	\********************************************************/
	else if(gGPRS_Data_Step==4)
	{
		if( (gTimer_GSM_AT>500)&&(gInternal_Flag&TCPSEND_ALLOW_F_1) )
		{
			Send_COM1_String((unsigned char *)String_TestGPRS,sizeof(String_TestGPRS));
			Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
			Send_COM0_Byte(0xFF);
			Send_COM0_Byte(0x0D);
			gTimer_GSM_AT=0;
			gGPRS_Data_Step=10;
		}
		else	Check_TCP();
	}
	/********************************************************\
	四，外接部件的TCP数据发送
	\********************************************************/
	else if(gGPRS_Data_Step==5)
	{
		if( (gTimer_GSM_AT>500)&&(gInternal_Flag&TCPSEND_ALLOW_F_1) )
		{
			Send_Circle_TCP_TypeE();
			gTimer_GSM_AT=0;
			gGPRS_Data_Step=10;
		}
		else	Check_TCP();
	}
	/********************************************************\
	五，TCP挂起的操作
	\********************************************************/
	else  if(gGPRS_Data_Step==9)
	{
		if( (gTimer_GSM_AT>500)&&(gInternal_Flag&TCPSEND_ALLOW_F_1) )
		{
			gReceive0_GSM_Buffer_Move=gReceive0_GSM_Buffer_Point;
			if(gReceive0_GSM_Buffer_Point==0) 	gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
			else     							gReceive0_GSM_Buffer_End=gReceive0_GSM_Buffer_Point-1;

			Send_COM1_String((unsigned char *)HANG_TCP,sizeof(HANG_TCP));
			Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
			Send_COM0_String((unsigned char *)HANG_TCP,sizeof(HANG_TCP));
			gTimer_GSM_AT=0;
			gGPRS_Data_Step=11;
		}
		else	Check_TCP();
	}
	/********************************************************\
	六，数据已经送给M22，则等待延迟时间后发送
	\********************************************************/
	else if(gGPRS_Data_Step==10)
	{
		if(gTimer_GSM_AT>1500)
		{
			gTimer_GSM_AT=0;
			gGPRS_Data_Step=0;
		}
		else	Check_TCP();
	}
	/********************************************************\
	十，发送+++后，等待模块响应的部分
	\********************************************************/
	else if(gGPRS_Data_Step==11)
	{
		if(Check_GSM()!=0)
		{
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK)))
			{
				//表示TCP已经挂起,则可以进行AT指令的操作
				Hang_Error=0;
				gGPRS_Data_Step=0;
				gM22_Status=TCP_HANG_WORK;
				gGeneral_Flag&=~HANG_M22_TCP_F_1;
				gGSM_Oper_Type=GSM_PHONE;
				gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
				gPhone_Step=0;gTimer_GSM_AT=0;
			}
		}
		else
		{
			if(gTimer_GSM_AT>10000)
			{
				gTimer_GSM_AT=0;
				Hang_Error++;
				if(Hang_Error>4)
				{
					//直接掉电处理
					Hang_Error=0;
					gGeneral_Flag|=OFF_M22_F_1;
					gOFF_Power_S=1;
					gPhone_Status=READY;
					gGSM_Oper_Type=GSM_PHONE;
					gGPRS_Data_Step=0;
					gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
					gPhone_Step=0;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
					return;
				}
				else
				{
					gGPRS_Data_Step=9;
				}
			}
		}
	}
	/********************************************************\
	十一，发送登陆信息后，等待登陆响应
	\********************************************************/
	else if(gGPRS_Data_Step==12)
	{
		if( (gTimer_GSM_AT>500)&&(gInternal_Flag&TCPSEND_ALLOW_F_1) )
		{
			i=Send_Echo_TCP_Head(12,(char)VEHICLE_ENTRY_UP,0,12-4);
			Send_Echo_TCP_End(i);
			gGPRS_Data_Step=10;
		}
		else	Check_TCP();
	}
}
/********************************************************\
*	函数名：Case_OverTime_Return
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Case_OverTime_Return(void)
{
	Reset_Variable();
}
/********************************************************\
*	函数名：Send_Circle_GSM_TypeD
	作用域：本地文件调用
*	功能：  在GSM方式下D类发送信息数据部分的打包（定时发送的行驶数据包）
			具体是发送存储在gCircle_Buffer[]缓冲中的数据
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Send_Circle_GSM_TypeD(void)
{
	unsigned int i=0;
	Send_COM0_Byte(ASCII( gCircle_Buffer_Point/0x10) );
	Send_COM0_Byte(ASCII( gCircle_Buffer_Point%0x10) );
	for(i=0;i<gCircle_Buffer_Point;i++)
	{
		Send_COM0_Byte(ASCII( (gCircle_Buffer[i])/0x10) );
		Send_COM0_Byte(ASCII( (gCircle_Buffer[i])%0x10) );
	}
}
/********************************************************\
*	函数名：Send_Circle_TCP_TypeD
	作用域：本地文件调用
*	功能：  在TCP方式下D类发送信息数据部分的打包（定时发送的行驶数据包）
			具体是发送存储在gCircle_Buffer[]缓冲中的数据
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/

void Send_Circle_TCP_TypeD(void)
{
	unsigned int i=0;
	for(i=0;i<gCircle_Buffer_Point;i++)
	{
		Send_COM0_Byte(gCircle_Buffer[i]);
	}
}
/********************************************************\
*	函数名：Send_Circle_GSM_TypeE
	作用域：本地文件调用
*	功能：  在GSM方式下E类发送信息数据部分的打包(发送接收到的外接部件的数据)
			具体是发送存储在gCom4_Buffer[]缓冲中的数据
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Send_Circle_GSM_TypeE(void)
{
	unsigned int i=0;
	unsigned int iLength=0;
	unsigned char nData=0;
	SRAM_Init();
	iLength = SRAM_Read(SRAM_SEND_LENGTH_H);
	iLength <<= 8;
	iLength += SRAM_Read(SRAM_SEND_LENGTH_L);
	Send_COM0_Byte(ASCII( (iLength+13)/0x10) );
	Send_COM0_Byte(ASCII( (iLength+13)%0x10) );

	Send_COM0_Byte(ASCII( SOH/0x10) );
	Send_COM0_Byte(ASCII( SOH%0x10) );

	Send_COM0_Byte(ASCII( EXTER_EQUIPMENT_UP/0x10) );
	Send_COM0_Byte(ASCII( EXTER_EQUIPMENT_UP%0x10) );

	Send_COM0_Byte(ASCII( (char)(~EXTER_EQUIPMENT_UP)/0x10) );
	Send_COM0_Byte(ASCII( (char)(~EXTER_EQUIPMENT_UP)%0x10) );

	Send_COM0_Byte(ASCII( (iLength+13-4)/0x10) );
	Send_COM0_Byte(ASCII( (iLength+13-4)%0x10) );

	Send_COM0_Byte(ASCII( gSMS_ACK[RECORD_EQUIPMENT_ACK]/0x10) );
	Send_COM0_Byte(ASCII( gSMS_ACK[RECORD_EQUIPMENT_ACK]%0x10) );

	Send_COM0_Byte(ASCII( gKind/0x10) );
	Send_COM0_Byte(ASCII( gKind%0x10) );

	Send_COM0_Byte(ASCII( gGroup/0x10) );
	Send_COM0_Byte(ASCII( gGroup%0x10) );

	Send_COM0_Byte(ASCII( (gVID/0x100)/0x10) );
	Send_COM0_Byte(ASCII( (gVID/0x100)%0x10) );

	Send_COM0_Byte(ASCII( (gVID%0x100)/0x10) );
	Send_COM0_Byte(ASCII( (gVID%0x100)%0x10) );

	//================================
	//外接部件的类型
	Send_COM0_Byte(ASCII( 1/0x10) );
	Send_COM0_Byte(ASCII( 1%0x10) );
	//外接部件的数据内容
	if( (iLength>0)&&(iLength<144) )
	{
		for(i=0;i<iLength;i++)
		{
			nData=SRAM_Read(SRAM_SEND_S+i);
			Send_COM0_Byte(ASCII( nData/0x10 ) );
			Send_COM0_Byte(ASCII( nData%0x10 ) );
		}
	}
	Send_COM0_Byte(ASCII( 0/0x10) );
	Send_COM0_Byte(ASCII( 0%0x10) );	//校验核H

	Send_COM0_Byte(ASCII( 0/0x10) );
	Send_COM0_Byte(ASCII( 0%0x10) );	//校验核L

	Send_COM0_Byte(ASCII( EOT/0x10) );
	Send_COM0_Byte(ASCII( EOT%0x10) );
}

/********************************************************\
*	函数名：Send_Circle_TCP_TypeE
	作用域：本地文件调用
*	功能：  在TCP方式下E类发送信息数据部分的打包(发送接收到的外接部件的数据)
			具体是发送存储在gCom4_Buffer[]缓冲中的数据
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/

void Send_Circle_TCP_TypeE(void)
{
  	unsigned int i=0,j=0;
  	unsigned int iLength=0;
  	unsigned int Length_Temp=0;
  	unsigned char nData_Temp=0;
	j=0;
	//读数据区的长度
	SRAM_Init();
	iLength = SRAM_Read(SRAM_SEND_LENGTH_H);
	iLength <<= 8;
	iLength += SRAM_Read(SRAM_SEND_LENGTH_L);
	//如果发送数据的长度超过M22一次TCP发送的长度，则需要多次组包发送
	if(iLength>=MAX_TCP_DATA_LENGHT)
	{
		Length_Temp=iLength-MAX_TCP_DATA_LENGHT;
		iLength=MAX_TCP_DATA_LENGHT;
		if(Length_Temp>0)	j=1;
		//j=1，说明存储的数据长度超过M22能一次发送的数据长度
	}
	Send_COM0_Byte( SOH );
	Send_COM0_Byte( EXTER_EQUIPMENT_UP );
	Send_COM0_Byte( (char)(~EXTER_EQUIPMENT_UP) );
	Send_COM0_Byte( iLength+9 );
	Send_COM0_Byte( gSMS_ACK[RECORD_EQUIPMENT_ACK] );

	Send_COM0_Byte( gKind );
	Send_COM0_Byte( gGroup );
	Send_COM0_Byte( gVID/0x100 );
	Send_COM0_Byte( gVID%0x100 );
	Send_COM0_Byte( 1 );
	//================================
	for( i=0;i<iLength;i++ )
	{
		Send_COM0_Byte( SRAM_Read(SRAM_SEND_S+i) );
	}
	Send_COM0_Byte( 0 );
	Send_COM0_Byte( 0 );
	Send_COM0_Byte( EOT );
	if(j==1)
	{
		SRAM_Init();
		SRAM_Write(SRAM_SEND_VAILD,VAILD_2);
		SRAM_Write(SRAM_SEND_LENGTH_H,Length_Temp/0x100);
		SRAM_Write(SRAM_SEND_LENGTH_L,Length_Temp%0x100);
		for(i=0;i<Length_Temp;i++)
		{
			nData_Temp=SRAM_Read(SRAM_SEND_S+iLength+i);
			SRAM_Write((SRAM_SEND_S+i),nData_Temp);
		}
		gSend_RecordData_Timer=0;
		gOther_Flag|=RECORD_DATA_F_1;
		gOther_Flag&=~ALLOW_SEND_RECORD_F_1;
	}
	else
	{
		gOther_Flag&=~RECORD_DATA_F_1;
		gOther_Flag&=~ALLOW_SEND_RECORD_F_1;
		SRAM_Init();
		SRAM_Write(SRAM_SEND_VAILD,NULL_2);
		SRAM_Write(SRAM_SEND_LENGTH_H,0);
		SRAM_Write(SRAM_SEND_LENGTH_L,0);
	}
}
/********************************************************\
*	函数名：Send_Type1_GSM_Data_One
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Send_Type1_GSM_Data_One(unsigned char S_Address)
{
	unsigned char nLength=0;
	SRAM_Init();
	nLength=SRAM_Read(TARGET_NUMBER1_2);				//目标电话号码的长度
	nLength=(nLength+1)/2+8+12+SRAM_Read(S_Address+3);	//整个数据区的长度
	Ask_SMS_Send(nLength);											//发送AT指令AT+CMGS=(SMS长度)
}
/********************************************************\
*	函数名：Send_Type1_GSM_Data_Two
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Send_Type1_GSM_Data_Two(unsigned char S_Address)
{
	unsigned int i,j,k,x,y;
	SRAM_Init();
	Send_SMS_Head();												//这个操作发送完000400
	i=SRAM_Read(S_Address+1);										//nCommand，命令字
	j=SRAM_Read(S_Address+2);										//ACK
	k=SRAM_Read(S_Address+3);										//长度
	OperateSPIEnd();
	x=Send_Echo_GSM_Head( (char)(k+12),(char)i,(char)j,(char)(k+12-4) );//同时计算校验核
	y=Send_Buffer1_GSM_Data(x,S_Address+4,k);
	Send_Echo_GSM_End(y);											//这个操作完成校验核和结束符号的发送
	Send_COM0_Byte(0x1A);
	OperateSPIEnd();
}
