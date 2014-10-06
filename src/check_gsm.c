
/********************************************************\
*	文件名：  Check_GSM.C
*	创建时间：2004年12月7日
*	创建人：  
*	版本号：  1.00
*	功能：
*	文件属性：特殊文件
*	修改历史：（每条详述）
\********************************************************/
#include <msp430x14x.h>
#include "Uart01.h"
#include "General.h"
#include "Define_Bit.h"
#include "Sub_C.h"
#include "M22_AT.h"
#include "Check_GSM.h"
#include "W_Protocol.h"
#include "SPI45DB041.h"
#include "D_Buffer.h"
#include "Other_Define.h"
#include "TA_Uart.h"
#include "Handle_Protocol.h"
#include "Disp_Protocol.h"
#include "Do_Handle.h"
#include "Do_SRAM.h"
/*
#include "Main_Init.h"
#include "Do_Reset.h"
#include "Do_GSM.h"
#include "Msp430_Flash.h"
*/
#define Debug_GSM_COM1
void Do_SMS_Receive(unsigned int iLength,unsigned char iRLength);
void Do_GPRS_Receive(void);
void Deal_Command(void);
unsigned char Check_Command(void);
void IP_SOCKET_Switch(void);
void Do_Parameter_Set(void);
void Position_Echo(void);
void Pass_Data_Echo(void);
void Set_Cycle_Timer(void);
void Disp_Dispaly_Info(void);
void Set_TCP_OnOrOff(void);
void Query_SYS_Ver(void);
void Deal_Alarm_Echo(void);
void Deal_Vehicle_Monitor(void);

void Set_Phone_Limit(void);

void Set_Area_Limit(void);
void Send_RecordData(void);
void Set_Distance_Par(void);
void Set_Phone_Num(void);
void Watch_Way(void);
void Line_Data_Deal(void);
//unsigned char Do_Deal_Area(unsigned char Add_Flash,unsigned char Add_Buffer );

/********************************************************\
*	函数名：Check_GSM
	作用域：外部文件调用
*	功能：	判断处理GSM接收缓冲gReceive0_GSM_Buffer中的数据，如果判断有0x0d,0x0a表示有
			一包GSM数据返回。将这一包数据转移到处理缓冲gGeneral_Buffer中进行处理

*	参数：  全局变量：GSM的接收缓冲gReceive0_GSM_Buffer[]，指针gReceive0_GSM_Buffer_Move
					  gReceive0_GSM_Buffer_Point。
					  数据处理缓冲gGeneral_Buffer[]

*	返回值：0：表示对GSM数据接收缓冲未做任何处理
			1：表示转移一包数据，并处理完成
			其中在返回的过程中，有写特殊的出口处理，需要对一些全局变量进行操作。具体见具体详细描叙

*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned int Check_GSM(void)
{
	unsigned int i,j,k;
	for(k=0;k<sizeof(gGeneral_Buffer);k++)	gGeneral_Buffer[k]=0;
	i = 0;
	j = gReceive0_GSM_Buffer_Move;
	if( gReceive0_GSM_Buffer_Point == gReceive0_GSM_Buffer_Move ) return(0);
	gGeneral_Buffer[i] = gReceive0_GSM_Buffer[j];
	i++;j++;
	if( j >= sizeof(gReceive0_GSM_Buffer) ) j=0;
	while(j!=gReceive0_GSM_Buffer_Point)
	{
		gGeneral_Buffer[i] = gReceive0_GSM_Buffer[j];
		i++;j++;
		if( i >= sizeof(gGeneral_Buffer) )
		{
//			Send_COM1_String((unsigned char *)R_OVERTOP,sizeof(R_OVERTOP));
			for(i=0;i<sizeof(gGeneral_Buffer);i++) 	 	gGeneral_Buffer[i]=0;
			for(i=0;i<sizeof(gReceive0_GSM_Buffer);i++)	gReceive0_GSM_Buffer[i]=0;
			gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
			gReceive0_GSM_Buffer_Point=0;
			gReceive0_GSM_Buffer_Move=0;
			return(0);
		}
		if( j >= sizeof(gReceive0_GSM_Buffer) )  	j=0;

		/********************************************************\
		\********************************************************/
		//判断有发送SMS的符号+++++++++++++++++++++++++++++++++++
		if( (gGeneral_Buffer[i-1]==' ')&&(gGeneral_Buffer[i-2]=='>') )
		{
			#ifdef Debug_GSM_COM1
			Send_COM1_String(gGeneral_Buffer,i);
			#endif
			gReceive0_GSM_Buffer_Move=j;				//表示一包有效帧数据处理完
			if(j==0) gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
			else     gReceive0_GSM_Buffer_End=j-1;
			return(1);
		}
		//判断有一包有效帧数据++++++++++++++++++++++++++++++++++++
		else if( (gGeneral_Buffer[i-1]==0x0a)&&(gGeneral_Buffer[i-2]==0x0d) )
		{
			#ifdef Debug_GSM_COM1
			Send_COM1_String(gGeneral_Buffer,i);
			#endif
			gReceive0_GSM_Buffer_Move=j;				//表示一包有效帧数据处理完
			if(j==0) gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
			else     gReceive0_GSM_Buffer_End=j-1;

			//*下面则是一个返回数据包在缓冲General_Buffer[]中,数据包的长度为i
			/********************************************************\
			1,判断是否有电话打入方面的处理：
			RING

			+CLIP: "13670155704",129,,,,0

			+CLIP: "13670155704",129,,,,0
			\********************************************************/
			//***************************************************************************
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_RING,sizeof(String_RING))!=0)
			{
				return(0);
			}
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_CLIP,sizeof(String_CLIP))!=0)
			{
				if(   ((gGeneral_Flag&RESET_GSM_ON_F_1)==0 )
					&&((gGeneral_Flag&LAND_TCP_F_1)==0)
					&&((gInternal_Flag&GSM_ONEAT_SUCCESS_1)==0)   )
				{
					if((gGeneral_Flag&RING_F_1)==0)			gHangUp_Timer=0;
					gGeneral_Flag|=TCP_PHONE_F_1;
					gStatus1_Flag|=PHONE_ONLINE_F_1;
					gGeneral_Flag|=RING_F_1;
					return(1);
				}
				else return(0);
			}
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_BUSY,sizeof(String_BUSY))!=0)
			{
				return(1);
			}
			/********************************************************\
			2,判断是否有AT打头的属于AT指令的返回方面的处理或者空行
			  比方说AT指令的回显字符，或者0x0d,0x0a
			\********************************************************/
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_AT,sizeof(String_AT))!=0)
			{
				return(0);
			}
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_END,sizeof(String_END))!=0)
			{
				return(0);
			}
			/********************************************************\
			3,判断是否有NO CARRIER 返回的处理
			\********************************************************/
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_NOCARRIER,sizeof(String_NOCARRIER))!=0)
			{
				if( (gM22_Status==TCP_HANG_WORK) )
				{
					if( (gGeneral_Flag&TCP_PHONE_F_1)==0 )
					{
						gM22_Status=GSM_WORK;
						gGSM_Oper_Type=GSM_PHONE;
						gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
						gPhone_Step=0;gTimer_GSM_AT=0;
					}
					else
					{
						if(gPhone_Status==ACTIVE_MONITOR)
						{
							//监听结束
							gPhone_Status=READY;
							gGeneral_Flag&=~TCP_PHONE_F_1;
							gStatus1_Flag&=~PHONE_ONLINE_F_1;
							gGeneral_Flag|=MONITOR_RESUME_F_1;
							P6OUT&=~SEL_MIC;
							gGeneral_Flag|=TCP_STATUS_F_1;
						}
						else if( (gPhone_Status==ACTIVE_HANDLE)||(gGeneral_Flag&RING_F_1) )
 						{
							//手柄通话结束
							gPhone_Status=READY;
							gGeneral_Flag&=~RING_F_1;
							gGeneral_Flag&=~TCP_PHONE_F_1;
							gStatus1_Flag&=~PHONE_ONLINE_F_1;
							//发送一个挂机信号给手柄
						    Send_COM3_Load(4,MAIN_HANG_UP,0);
							gSys_Handle=MAIN_HANG_UP;
							gGeneral_Flag|=TCP_STATUS_F_1;
						}
						gGeneral_Flag&=~DAILING_UP_F_1;
						gGeneral_Flag&=~MONITOR_ON_F_1;
						gPhone_Status=READY;
						gGeneral_Flag&=~TCP_PHONE_F_1;
						gStatus1_Flag&=~PHONE_ONLINE_F_1;
						gGeneral_Flag&=~RING_F_1;
						gGeneral_Flag|=TCP_STATUS_F_1;
						gCommon_Flag&=~CHECK_RINGPHONE_F_1;
					}
				}
				else if(gM22_Status==GSM_WORK)
				{
					if(gPhone_Status==ACTIVE_MONITOR)
					{
						//监听结束
						gPhone_Status=READY;
						gGeneral_Flag&=~TCP_PHONE_F_1;
						gStatus1_Flag&=~PHONE_ONLINE_F_1;
					}
					else if( (gPhone_Status==ACTIVE_HANDLE)||(gGeneral_Flag&RING_F_1) )
					{
						//手柄通话结束
						gPhone_Status=READY;
						gGeneral_Flag&=~RING_F_1;
						gGeneral_Flag&=~TCP_PHONE_F_1;
						gStatus1_Flag&=~PHONE_ONLINE_F_1;
				        Send_COM3_Load(4,MAIN_HANG_UP,0);
				        gSys_Handle=MAIN_HANG_UP;

						//发送一个挂机信号给手柄
					}
					gGeneral_Flag&=~DAILING_UP_F_1;
					gGeneral_Flag&=~MONITOR_ON_F_1;
					gPhone_Status=READY;
					gGeneral_Flag&=~TCP_PHONE_F_1;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
					gGeneral_Flag&=~RING_F_1;
					gGeneral_Flag|=MONITOR_RESUME_F_1;
					gCommon_Flag&=~CHECK_RINGPHONE_F_1;
				}
				return(1);
			}
			/********************************************************\
			4,判断是否有+CMT: 返回的处理(属于中断接收SMS的信息处理)
			+CMT: ,032
			0891683108705505F0040D91683128782840F90004503011613495000CFF000000000000000000001B
			\********************************************************/
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_CMT,sizeof(String_CMT))!=0)
			{
		        gSMS_In_Lenth=0;
		        j=4;k=0;
		        while(1)
		        {
		        	if(gGeneral_Buffer[j]==',')	{k=1;break;}
		        	j++;
		        	if(j>i) break;
		        }
		        if(k==1)
		        {
		        	for(k=0;k<3;k++)
		        	{
						gSMS_In_Lenth*=10;
						gSMS_In_Lenth=gSMS_In_Lenth+(gGeneral_Buffer[k+j+1]-0x30);
		        	}
					gInternal_Flag|=RECEIVE_SMS_F_1;
		        }
				return(0);
			}
			else if(gInternal_Flag&RECEIVE_SMS_F_1)
			{
				gInternal_Flag&=~RECEIVE_SMS_F_1;
				Do_SMS_Receive(i,gSMS_In_Lenth);
				return(0);
			}
			/********************************************************\
			5,其他
			\********************************************************/
			return(1);
		}//end else if( (gGeneral_Buffer[i-1]==0x0a)&&(gGeneral_Buffer[i-2]==0x0d) )
	}//end while
	return(0);
}
/********************************************************\
*	函数名：Check_TCP
	作用域：外部文件调用
*	功能：

*	参数：  全局变量：GSM的接收缓冲gReceive0_GSM_Buffer[]，指针gReceive0_GSM_Buffer_Move
					  gReceive0_GSM_Buffer_Point。
					  数据处理缓冲gGeneral_Buffer[]

*	返回值：0：表示对GSM数据接收缓冲未做任何处理
			1：表示转移一包数据，并处理完成
			其中在返回的过程中，有写特殊的出口处理，需要对一些全局变量进行操作。具体见具体详细描叙

*	创建人：
*
*	修改历史：（每条详述）
	补充说明：
\********************************************************/
unsigned int Check_TCP(void)
{
	unsigned int i,j,k,x;
	unsigned char nLength;
	unsigned char nFlag;
	nLength=0;
	//已经100ms，未接收到M22的数据，则可以判断接收缓冲区中的数据
	if(gM22_TCP_ECHO_Timer>60)
	{
		nFlag=0;
		for(k=0;k<sizeof(gGeneral_Buffer);k++)	gGeneral_Buffer[k]=0;
		i = 0;
		j = gReceive0_GSM_Buffer_Move;
		if( gReceive0_GSM_Buffer_Point == gReceive0_GSM_Buffer_Move ) return(0);
		gGeneral_Buffer[i] = gReceive0_GSM_Buffer[j];
		i++;j++;
		if( j >= sizeof(gReceive0_GSM_Buffer) ) j=0;
		while(j!=gReceive0_GSM_Buffer_Point)
		{
			gGeneral_Buffer[i] = gReceive0_GSM_Buffer[j];
			i++;j++;
			if( i >= sizeof(gGeneral_Buffer) )
			{
//				Send_COM1_String((unsigned char *)R_OVERTOP,sizeof(R_OVERTOP));
				for(i=0;i<sizeof(gGeneral_Buffer);i++) 	 	gGeneral_Buffer[i]=0;
				for(i=0;i<sizeof(gReceive0_GSM_Buffer);i++)	gReceive0_GSM_Buffer[i]=0;
				gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
				gReceive0_GSM_Buffer_Point=0;
				gReceive0_GSM_Buffer_Move=0;
				return(0);
			}
			if( j >= sizeof(gReceive0_GSM_Buffer) )  	j=0;
			//如果能执行到这里，则已经将接收缓冲的数据转移到gGeneral_Buffer缓冲中。
			for(k=0;k<i;k++)
			{
				//判断是否为下行的命令数据包
				//例：FF 4F 0B 08 00 01 01 00 08 02 10 0D
				if( (gGeneral_Buffer[k]==SOH)									//判断包头
					&&(gGeneral_Buffer[gGeneral_Buffer[k+3]+3]==EOT)			//判断包尾
					&&(gGeneral_Buffer[k+1]==(char)(~gGeneral_Buffer[k+2])) )	//判断命令字是否取反
				{
//					Send_COM1_String((unsigned char *)TCP_COMMAAND,sizeof(TCP_COMMAAND));
					nLength=gGeneral_Buffer[k+3]+3+1;	//分析有效数据包长度
					j = gReceive0_GSM_Buffer_Move;
					for(x=0;x<nLength+k;x++)
					{
						j++;
						if( j >= sizeof(gReceive0_GSM_Buffer) ) j=0;
					}
					gReceive0_GSM_Buffer_Move=j;		//移动处理处理缓冲区指针
					if(j==0) gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
					else     gReceive0_GSM_Buffer_End=j-1;
					//处理适合Do_GPRS_Receive()处理的gGeneral_Buffer缓冲数据安排
					if(k==0)
					{
						for(x=nLength;x>=1;x--)
						{
							gGeneral_Buffer[x]=gGeneral_Buffer[x-1];
						}
						gGeneral_Buffer[0]=nLength;
					}
					else if(k>0)
					{
						gGeneral_Buffer[k-1]=nLength;
						for(x=0;x<=nLength;x++)
						{
							gGeneral_Buffer[x]=gGeneral_Buffer[k-1+x];
						}
					}
					gGeneral_Flag|=LAND_TCP_SUCCESS_F_1;
					Do_GPRS_Receive();
					nFlag=1;
					break;
				}
				//判断是否为FF 0D的握手数据
				else if( (gGeneral_Buffer[0]==0xFF)&&(gGeneral_Buffer[0+1]==0x0D) )
				{
					gGeneral_Flag|=LAND_TCP_SUCCESS_F_1;
					gLand_Interval_Timer=0;
					if(gFF0D_Receive_Timer<=2)
					{
						for(i=0;i<sizeof(gGeneral_Buffer);i++) 	 	gGeneral_Buffer[i]=0;
						for(i=0;i<sizeof(gReceive0_GSM_Buffer);i++)	gReceive0_GSM_Buffer[i]=0;
						gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
						gReceive0_GSM_Buffer_Point=0;
						gReceive0_GSM_Buffer_Move=0;
						return(0);
					}
					else
					{
						Send_COM1_String((unsigned char *)TCP_TEST,sizeof(TCP_TEST));
						j = gReceive0_GSM_Buffer_Move;
						for(x=0;x<2;x++)
						{
							j++;
							if( j >= sizeof(gReceive0_GSM_Buffer) ) j=0;
						}
						gReceive0_GSM_Buffer_Move=j;		//移动处理处理缓冲区指针
						if(j==0) gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
						else     gReceive0_GSM_Buffer_End=j-1;
						gFF0D_Receive_Timer=0;
						nFlag=1;
					}
					break;
				}
				//判断是否出现TCP掉线信息
				//0D 0A 4F 4B 0D 0A 0D 0A 43 4F 4E 4E 45 43 54 0D 0A 0D 0A 4F
				//4B 0D 0A 0D 0A 4F 4B 0D 0A 0D 0A 4E 4F 20 43 41 52 52 49 45 52 0D 0A
				else if( (gGeneral_Buffer[k]==0x4E)
						 &&(gGeneral_Buffer[k+1]==0x4F)
						 &&(gGeneral_Buffer[k+2]==0x20)
						 &&(gGeneral_Buffer[k+3]==0x43)
						 &&(gGeneral_Buffer[k+4]==0x41)
						 &&(gGeneral_Buffer[k+5]==0x52)
						 &&(gGeneral_Buffer[k+6]==0x52)
						 &&(gGeneral_Buffer[k+7]==0x49)
						 &&(gGeneral_Buffer[k+8]==0x45)
						 &&(gGeneral_Buffer[k+9]==0x52)
						 &&(gGeneral_Buffer[k+10]==0x0D)
						 &&(gGeneral_Buffer[k+11]==0x0A) )
			 	{
			 		//如果满足条件，说明已经TCP断线
			 		Send_COM1_String((unsigned char *)TCP_QUIT,sizeof(TCP_QUIT));
					gGeneral_Flag&=~LAND_TCP_SUCCESS_F_1;
					gGSM_Oper_Type=GSM_PHONE;
					gM22_Status=GSM_WORK;
					gReceive0_GSM_Buffer_Move=gReceive0_GSM_Buffer_Point;
					if(gReceive0_GSM_Buffer_Point==0) 	gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
					else     							gReceive0_GSM_Buffer_End=gReceive0_GSM_Buffer_Point-1;
					gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
					gPhone_Step=0;gTimer_GSM_AT=0;
			 		break;
			 	}
			}//for(k=0;k<i;k++)
		}//end while(j!=gReceive0_GSM_Buffer_Point)
	}//end	gM22_GSM_ECHO_Timer>100
	return(0);
}
/********************************************************\
*	函数名：Do_SMS_Receive
	作用域：本文件调用
*	功能：  在G20中用AT+CMGR指令读取出SMS的内容，接收并处理

*	参数：  iLength	 作为接收整个数据的长度，如下的例子此数据为64
        	iRLength 作为AT+CMGR读出返回SMS的长度为32
        	全局变量缓冲gGeneral_Buffer

*	返回值：这个模块是将返回数据区的值变换后存储在全局变量gGeneral_Buffer中，并调用
			Deal_Command()模块处理数据
*	创建人：
*
*	修改历史：（每条详述）
	//////////////////////////////////////////////////////////

	AT+CMGR=1
	+CMGR: 0,,24
	08                   0D
	0891683108705505F004 0D91683176105507F4 0008 40 0111900093000462 C94E01 (0D 0A)
	----------20-------- ---------20--------- ------20------------ -----8-------
	0891683108705505F0 04 0D91683128782840F9 0004 40 01 03 41 34 5400 0C FF4FB00800302E303002C40D

	i=HEX(GSM_Echo_Buffer[0],GSM_Echo_Buffer[1]);	读出值为08
	j=(i+2)*2;  20个  (0D的第一个地址)
	i=HEX(GSM_Echo_Buffer[j],GSM_Echo_Buffer[j+1]);	读出值为0D
	j=j+(i+1)/2*2+22; (02的第一个地址)
	i=HEX(GSM_Echo_Buffer[j],GSM_Echo_Buffer[j+1]); 读出值为0C,后面的为数据包的内容
	j++;
	j++;



	+CMT: ,032
	0891683108705505F0040D91683128782840F90004503011613495000CFF000000000000000000001B
\********************************************************/
void Do_SMS_Receive(unsigned int iLength,unsigned char iRLength)
{
	unsigned int i,j,k;
	unsigned int m,n;
	unsigned char y;
	//判断SMS的长度与接收的数据是否符合,整个数据包是否收全。iLength为收到的数据包长
	//如上例中iRLength=24,iLength=68
	if(ASCIITOHEX(gGeneral_Buffer[0],gGeneral_Buffer[1])+iRLength+2==iLength/2)
	{
		//将内容提取出来仍然存储在General_Buffer[]缓冲中
	    i=ASCIITOHEX(gGeneral_Buffer[0],gGeneral_Buffer[1]);
	    j=(i+2)*2;
	    i=ASCIITOHEX(gGeneral_Buffer[j],gGeneral_Buffer[j+1]);

	    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	    for(m=0;m<sizeof(gNum_Phone_Temp);m++)	gNum_Phone_Temp[m]=0;
	    m=j;					//20
	    n=m+(i+1)/2*2+3;		//37
	    y=0;
		gNum_Phone_Temp[y]=0;
		y++;
		if(gGeneral_Buffer[2]=='9')
		{
			gNum_Phone_Temp[y]='+';
			y++;
		}
		m=j+4;
		while(1)
		{
			if(m<n)
			{
				gNum_Phone_Temp[y]=gGeneral_Buffer[m+1];
				y++;
				gNum_Phone_Temp[y]=gGeneral_Buffer[m];
				y++;
				m++;m++;
			}
			else if(m==n)
			{
				gNum_Phone_Temp[y]=gGeneral_Buffer[m];
				y++;
				m++;
			}
			else break;
		}
		for(m=2;m<y;m++)
		{
			if( (gNum_Phone_Temp[m]>'9')||(gNum_Phone_Temp[m]<'0') )
			break;
		}
		gNum_Phone_Temp[0]=m-1;
		if(gNum_Phone_Temp[0]>14)	gNum_Phone_Temp[0]=0;
		/*
		//显示出当前的号码存储的是否正确
	    Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		Send_COM1_Byte(ASCII(gNum_Phone_Temp[0]/10));
		Send_COM1_Byte(ASCII(gNum_Phone_Temp[0]%10));
		Send_COM1_Byte('<');
		for(m=1;m<=gNum_Phone_Temp[0];m++)
		{
			Send_COM1_Byte(gNum_Phone_Temp[m]);
		}
		Send_COM1_Byte('>');
		Send_COM1_Byte(0x0d); Send_COM1_Byte(0x0a);
		*/
		//现在在缓冲gNum_Phone_Temp[]中存储的是这个的信息的来源号码
	    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	    j=j+(i+1)/2*2+22;
	    i=ASCIITOHEX(gGeneral_Buffer[j],gGeneral_Buffer[j+1]);//表示数据区数据的长度
	    //这个时候，j所指向的是数据区的第一个字节，开始转移
	    //存储整个数据区的长度，不包括记录长度的第一个字节
	    gGeneral_Buffer[0]=i;
		for(k=1;k<=i;k++)
		{
			gGeneral_Buffer[k]=ASCIITOHEX(gGeneral_Buffer[j+2*k],gGeneral_Buffer[j+2*k+1]);
		}
		Deal_Command();
	}
}
/********************************************************\
*	函数名：Do_GPRS_Receive
	作用域：本文件调用
*	功能：
*	参数：  全局变量缓冲gGeneral_Buffer

*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Do_GPRS_Receive(void)
{
	Deal_Command();
}
/********************************************************\
*	函数名：Deal_Command
	作用域：本文件调用
*	功能：	通过掉用Check_Command模块，进行下行协议的分析，根据不同的协议命令，
			在这个模块中进行相应的处理和操作，具体见每个不同命令的详细描述
*	参数：  全局变量缓冲gGeneral_Buffer[]

*	返回值：根据不同，需要对一些全局变量重新进行附值操作（详细见不同命令的处理方式）
*	创建人：
*
*	修改历史：（每条详述）
    详细说明：
	Deal_Command()；模块程序说明:已经通过程序知道了命令字，则需要进行针对不
						同的命令字进行相应的操作
	说明：不管是从GSM还是GPRS接收来的数据最后在运行这个子程序的时候，General_Buffer
		缓冲中存储数据包的格式是一样的，举例：
	0891683108705505F004 0D91683176105507F400 0840907271356100 02 00 41 (0x0d 0x0a)
	08                   0D                                    02
	这是从GSM接收到的数据，最后处理完成的结果General_Buffer缓冲中存储的结果为
	gGeneral_Buffer[0]=2   （长度）
	gGeneral_Buffer[1]=0
	gGeneral_Buffer[2]=41
	同样道理GPRS接收的数据
	+MIPRTCP: 1,0,0041
	gGeneral_Buffer[0]=2   （长度）
	gGeneral_Buffer[1]=0
	gGeneral_Buffer[2]=41
\********************************************************/
void Deal_Command(void)
{
	unsigned char nCmd;
	unsigned char nSpeed=0;
//	Send_COM1_Byte('S');
	#ifdef Debug_GSM_COM1
	Send_COM1_Byte(ASCII(gGeneral_Buffer[0]/16));
	Send_COM1_Byte(ASCII(gGeneral_Buffer[0]%16));
	Send_COM1_Byte('[');
	for(nCmd=1;nCmd<=gGeneral_Buffer[0];nCmd++)
	{
		if(nCmd==0)	break;
		Send_COM1_Byte(ASCII(gGeneral_Buffer[nCmd]/16));
		Send_COM1_Byte(ASCII(gGeneral_Buffer[nCmd]%16));
		Clear_Exter_WatchDog();
	}
	Send_COM1_Byte(']');
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	#endif

	nCmd=Check_Command();
	//补充说明：根据以前的兼容，则如果在上行的信息中，如果带有经度，纬度的上行数据，则在后面增加4个状态量的字节
	switch(nCmd)
	{//内容由开发者自己加入
		//1，车辆登陆响应(不加状态位)
		case	VEHICLE_ENTRY_ECHO_DOWN:
		{
			break;
		}
		//2，车辆参数设置(不加状态位)
		case	PARAMETER_SET_DOWN:
		{
			break;
		}
		//3，设置上传位置时间间隔 (不加状态位，从新写)
		case	TIME_INTERVAL_DOWN:
		{
			break;
		}
		//4，设置服务器地址，端口 (不加状态位)
		case	SET_ADDRESS_PORT_DOWN:
		{
		        break;
		}
		//5，车辆定位查询 ( 加状态位 )
		case 	POSITION_ASK_DOWN:
		{
			break;
		}
		//6，报警应答(不加状态位)
		case	ALARM_ECHO_DOWN:
		{
			break;
		}
		//7，车辆监听(不加状态位)
		case	VEHICLE_MONITOR_DOWN:
		{
			break;
		}
		//8，外接部件的相关命令(不加状态位)
		case	EXTER_EQUIPMENT_DOWN:
		{
			break;
		}
		//9-0,和显示终端相关的相关的命令(文字信息的处理)(不加状态位)
		case	WORD_INFORMATION_DOWN:
		{
			break;
		}
		//9-1，下载动态菜单
		case	SET_MENU_DOWN:
		{
			break;
		}
		//9-2,下载设定短消息的下行命令
		case	SET_SMS_COMMAND_DOWN:
		{
			break;
		}
		//10,出城登记响应(不加状态位)
		case	OUTCITY_ECHO_DOWN:
		{
			break;
		}
		//11,历史轨迹查询的下行命令(不加状态位),
		case	TRACK_ASK_DOWN:
		{
			break;
		}
		//12,接收下行的设置终端进入/退出状态的命令	(不加状态位)从新写
		case	SET_TCP_ONOROFF_DOWN:
		{
			break;
		}
		//13,车载终端的版本查询 (不加状态位)
		case	QUERY_VERSION_DOWN:
		{
			break;
		}
		//14，车辆允许退出TCP登陆的响应(不加状态位)
		case	ALLOW_TCP_QUIT_ECHO_DOWN:
		{
			break;
		}
		//15,中心提取解除设防密码的下行命令(不加状态位)
		case	PASSWORD_ASK_DOWN:
		{
			break;
		}
		//16，中心强制设防或者解除设防状态的下行命令(不加状态位)
		case	FORCE_FORTIFY_DOWN:
		{
			break;
		}
		//17，中心强制制动的下行命令(不加状态位)
		case	SET_STOP_COMMAND_DOWN:
		{
			break;
		}
		//18，中心强制解除制动的下行命令(不加状态位)
		case	FREE_STOP_COMMAND_DOWN:
		{
			break;
		}
		//19,收到设置最大时速的下行命令
		case	SET_MAXSPEED_DOWN:
		{
			break;
		}
		//20,设置电话的权限的下行命令
		case	SET_PHONE_LIMIT_DOWN:
		{
			break;
		}
		//21,收到重新设置里程初始值的下行命令
		case	SET_DISTANCE_INIT_DOWN:
		{
			break;
		}
		//22,设置中心服务号码
		case	SET_CENTER_NUM_DOWN:
		{
			break;
		}
		//23,设置区域参数的设置
		case	SET_ALLOW_AREA_DOWM:
		{
			break;
		}
		//24,设置间隔距离的参数
		case	SET_DISTANCE_DATA_DOWN:
		{
			break;
		}
		//25,接收到下行的电话号码本的参数
		case	CARRY_PHONE_NUM_DOWN:
		{
			break;
		}
		//26，接收到重点监控的命令
		case	VEHICLE_WATCH_DOWN:
		{
			break;
		}
		//27，接收到线路偏离数据的命令
		case	LINE_DATA_DOWN:
		{
			break;
		}
		//28,接收到线路偏离数据的距离限制值的设置
		case	LINE_LIMIT_DISTANCE_DOWN:
		{
			break;
		}
		//29,收到测试信息的测试SMS
		case	TEST_SMS_COMMAND_DOWN:
		{
			break;
		}
		default:
		{
			break;
		}
	}
}
/********************************************************\
*	函数名：Check_Command
	作用域：本文件调用
*	功能：  针对全局变量缓冲gGeneral_Buffer[]中的数据进行数据有效果判断，如果有效则返回
			有效数据包中的命令字

*	参数：  需要对全局变量缓冲gGeneral_Buffer[]进行操作

*	返回值：0：无效的数据包
			gGeneral_Buffer[2]：数据包有效，返回其中命令字

*	创建人：
*
*	修改历史：（每条详述）

	补充说明：	校验数据包，提取命令字，并判断返回命令字，主要是针对gGeneral_Buffer[]
				gGeneral_Buffer[]的具体分布如下：
				gGeneral_Buffer[0]表示整个数据包的长度（但不包括这个长度本身这个字节）
				gGeneral_Buffer[1]消息头0XFF
				gGeneral_Buffer[2]命令字 gGeneral_Buffer[2]命令字的取反
				............

\********************************************************/
unsigned char Check_Command(void)
{
	unsigned int i,j,k;
	if(gGeneral_Buffer[0]==0)	return(0);
	if(gGeneral_Buffer[1]!=0xFF)	return(0);
	if(gGeneral_Buffer[2]!=(char)(~gGeneral_Buffer[3]) ) return(0);
	//计算校验核
	i=gGeneral_Buffer[4]+1;							//数据包长度
    k=0;
    for( j=1;j<=i;j++ ) k=k+gGeneral_Buffer[j];		//计算校验核
    if( k/256!=gGeneral_Buffer[gGeneral_Buffer[4]+2] ) return(0);
    if( k%256!=gGeneral_Buffer[gGeneral_Buffer[4]+3] ) return(0);
    return( gGeneral_Buffer[2] );
}
/********************************************************\
*	函数名：IP_SOCKET_Switch
	作用域：本文件调用
*	功能： 	这里是将从接收到的参数设置中的IP地址和端口号（long int）型的数据。
			IP地址直接是N个ASCII字符，则只需要直接存储在FLASH和buffer1中
			SOCKET因为是long int的数值，则需要转换成ASCLL字符的形式表示。这里做个限制
			最多5位数字的ASCII。并存储在Flash 和buffer1中
*	参数：

*	返回值：
*	创建人：
*
*	修改历史：（每条详述）

	补充说明1:
	long int变量为IP_Port_Num,将这个变量所表示的数值转换为最多5位的ASCII数字形式
	处理结果是在Flash和buffer1中存储断口数据
	调用这个子程序，是在收到下行的设置服务器地址和端口号以后，处理好数据后，所得到的结果
	先处理IP地址的数据，
	还包括一个长度字节（General_Buffer[0]）

	补充说明2：	在运行这个子程序的时候，判断在gGeneral_Buffer[]中的命令字是属于设置服务器地址，端口
				gGeneral_Buffer[]的数据结构分布如下：
				gGeneral_Buffer[0]表示整个数据包的长度（但不包括这个长度字节本身）
				gGeneral_Buffer[1]----gGeneral_Buffer[9] 				前面固定的（包括消息头，消息号等等）
				gGeneral_Buffer[10]----gGeneral_Buffer[10+N]			IP地址的字符串，以0x00作为结束符号（ASCII字符串）
				gGeneral_Buffer[10+N+1]----gGeneral_Buffer[10+N+1+4]	为服务器通讯端口号（4个字节，为一个long int型数据）

	//处理第二类型GPRS数据的详细步骤

	//0x7B----0xB6用来存储针对下行信息的反馈信息（长度不固定）根据协议，此存储空间以60个字节存储
	//#define  ECHO_SMS_2	0x7B
	//0x7B	存储表示这个区域的SMS是否处理，为1(VAILD_2)则表示后面还有未处理的SMS，为0(NULL_2)则表示已经处理完了SMS
	//0x7C	存储数据包的命令字在这里则为nCommand
	//0x7D	存储数据包的ACK的值ACK
	//0x7E	存储后面数据包的长度（但注意没有存储CHK(MSB),CHK(LSB),EOT,这三个字节）
	//0x7F  从这开始存储数据包
	在这个命令中的数据包为从在gGeneral_Buffer[10]开始
\********************************************************/
void IP_SOCKET_Switch(void)
{
	unsigned int i,j,k;
	unsigned char APN_Length=0;
	unsigned char Check_Data=0;
	//在gGeneral_Buffer[i]中表示IP地址的第一个数
	i=10;
	while(1)
	{
		if(gGeneral_Buffer[i]==0x00) break;
		i++;
		if(i>gGeneral_Buffer[0]) 	return;
	}
	//存储IP地址的长度，不包括长度本身
	SRAM_Init();
	SRAM_Write(GPRS_IP_ADDRESS1_2,i-10);
	//一，+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//1,IP的服务器地址的处理
	Check_Data=0;
	for(j=10;j<i;j++)
	{
		SRAM_Write(GPRS_IP_ADDRESS1_2+j-9,gGeneral_Buffer[j]);
		Check_Data += gGeneral_Buffer[j];
	}
	SRAM_Write(GPRS_IP_ADDRESS1_CHECK_2,Check_Data);

	//2，将IP地址的SOCKET数据存储在buffer1相应的缓冲区中，因为上面gGeneral_Buffer[i]=0x00
	gIP_Port_Num = gGeneral_Buffer[i+1];
	gIP_Port_Num<<=8;
	gIP_Port_Num += gGeneral_Buffer[i+2];
	gIP_Port_Num<<=8;
	gIP_Port_Num += gGeneral_Buffer[i+3];
	gIP_Port_Num<<=8;
	gIP_Port_Num += gGeneral_Buffer[i+4];
	//3，将GPRS的APN的内容存储在buffer1中
	k=i+5;
	while(1)
	{
		if(gGeneral_Buffer[k]==0x00) break;
		k++;
		if(k>gGeneral_Buffer[0]) return;
	}
	//4，存储APN地址的长度，不包括长度本身这个字节的内容
	APN_Length=k-i-5;
	if( (APN_Length>=4)&&(APN_Length<=25) )
	{
		SRAM_Write(GPRS_APN_ADDRESS2_2,APN_Length);
		//5，存储APN的数据值到相应的BUFFER1中
		Check_Data=0;
		for(j=i+5;j<k;j++)
		{
			SRAM_Write(GPRS_APN_ADDRESS2_2+j-i-4,gGeneral_Buffer[j]);
			Check_Data += gGeneral_Buffer[j];
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
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//存储作为第2类信息发送的响应数据的存储
	//服务器端口设置成功后的响应
	SRAM_Write(ECHO_SMS_2,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,ADDRESS_PORT_ECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(ECHO_SMS_2+3,k-10);
	for(j=10;j<k;j++)
	{
		SRAM_Write(ECHO_SMS_2+4+j-10,gGeneral_Buffer[j]);
	}
	//因为端口号为一个long int 型的数据，则需要转换为ASCII字符的形式做实际应用
	//这里最多能表示5位的端口号，
	Check_Data=0;
	SRAM_Write(GPRS_SOCKET1_2+0,ASCII(gIP_Port_Num/10000));
	Check_Data += ASCII(gIP_Port_Num/10000);
	SRAM_Write(GPRS_SOCKET1_2+1,ASCII((gIP_Port_Num%10000)/1000));
	Check_Data += ASCII((gIP_Port_Num%10000)/1000);
	SRAM_Write(GPRS_SOCKET1_2+2,ASCII((gIP_Port_Num%1000)/100));
	Check_Data += ASCII((gIP_Port_Num%1000)/100);
	SRAM_Write(GPRS_SOCKET1_2+3,ASCII((gIP_Port_Num%100)/10));
	Check_Data +=ASCII((gIP_Port_Num%100)/10);
	SRAM_Write(GPRS_SOCKET1_2+4,ASCII(gIP_Port_Num%10));
	Check_Data += ASCII(gIP_Port_Num%10);
	SRAM_Write(GPRS_SOCKET1_END_2,Check_Data);
	gTimer=0;Clear_Exter_WatchDog();
	while(gTimer<100){}
	Clear_Exter_WatchDog();
	//二，++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//注意，这里在程序刚上电的最开始已经检查过了FLASH的第1，第2扇区的数据
	//在这里则不需要在继续检查
	//读第一个扇区的数据到buffer2中
//	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO1_ONE_2);OperateSPIEnd();
	Deal_Sector(FLASH_INFO1_ONE_2);
	//1，将IP地址数据已经写入相应的buffer2中
	i=10;
	while(1)
	{
		if(gGeneral_Buffer[i]==0x00) break;
		i++;
		if(i>26) return;
	}
	//1-1,写IP地址数据长度
	OperateSPIEnd();
	WriteOneByteToBuffer2(GPRS_IP_ADDRESS1_F_2,i-10);
	//1-2，写IP地址数据包
	for(j=10;j<i;j++)
	{
		WriteNextByteToBuffer2(gGeneral_Buffer[j]);
	}
	OperateSPIEnd();
	gGeneral_Flag|=TCP_IP_VALID_1;
	gStatus2_Flag|=IP_VAILD_F_1;
	//2，将SOCKET的数据写入相应的buffer2中
	i=i+4;//此时的gGeneral_Buffer[i]=0x00
	i++;
	OperateSPIEnd();
	WriteOneByteToBuffer2(GPRS_SOCKET1_F_2,ASCII(gIP_Port_Num/10000));
	WriteNextByteToBuffer2(ASCII((gIP_Port_Num%10000)/1000));
	WriteNextByteToBuffer2(ASCII((gIP_Port_Num%1000)/100));
	WriteNextByteToBuffer2(ASCII((gIP_Port_Num%100)/10));
	WriteNextByteToBuffer2(ASCII(gIP_Port_Num%10));
	OperateSPIEnd();
	//3,将APN的内容写入相应的buffer2中
	OperateSPIEnd();
	//3-1,写入APN数据的长度
	WriteOneByteToBuffer2(GPRS_APN_DATA_F_2,APN_Length);
	//3-2，写APN数据包
	for(j=i;j<i+APN_Length;j++)
	{
		WriteNextByteToBuffer2(gGeneral_Buffer[j]);
	}
	OperateSPIEnd();
	RFlash_Sector(FLASH_INFO1_ONE_2);
	Re430_Flash(0);
	if(gM22_Status!=GSM_WORK)	gOther_Flag|=QUIT_TCP_F_1;
	gCheck_Par_Timer=CHECK_SYS_PAR_TIME+1;
	Judge_SMS_Way();
}

/********************************************************\
*	函数名：Do_Parameter_Set
	作用域：本文件调用
*	功能：  如果在Deal_Command()模块中判断属于处理下行的数据命令字为
			参数设置，则调用这个函数进行处理，收到的参数有
			车型，类别，组别，车号，目标号码1(以0x00结束)，目标号码2(备用)(以0x00结束)
			对于上面信息的处理，一是存储在Flash相应的区域，一是存储在buffer1相应的区域
			同时设置参数设置响应的标志（发送第一类的SMS），存储此ACK到gSMS_ACK[]相应的区域
*	参数：	全局变量gGeneral_Buffer[]中的数据

*	返回值：进行和参数相关的设置

*	创建人：
*
*	修改历史：（每条详述）

	补充说明：	在运行这个子程序的时候，判断在gGeneral_Buffer[]中的数据是参数设置，此时
				gGeneral_Buffer[]的数据结构分布如下：
				gGeneral_Buffer[0]表示整个数据包的长度（但不包括这个长度字节本身）
				gGeneral_Buffer[1]----gGeneral_Buffer[9] 			前面固定的（包括消息头，消息号等等）
				gGeneral_Buffer[10]----gGeneral_Buffer[14]  		需要设置的车型，类别，组别，车号
				gGeneral_Buffer[15]----gGeneral_Buffer[15+N]		中心的短消息接收号码（以0x00作为结束符）
				gGeneral_Buffer[15+N+1]-----gGeneral_Buffer[15+N+M]	中心的短消息接收备用号码 （以0x00作为结束符）

	如果收到参数设置的下行命令，则首先的需要恢复出厂时的设置，将第0个扇区，第1个扇区，第2个扇区里的数据ERASE
	执行条件：只有处于GSM工作方式下才处理
\********************************************************/
void Do_Parameter_Set(void)
{
	//进行参数设置响应（上行数据发送）
	//表示第一类SMS的数据发送:参数设置响应
	//设置发送参数设置响应的标志
	Load_Buffer1_Echo(PARAMETER_SETECHO_UP,gGeneral_Buffer[5]);
	Do_Par();
}

/********************************************************\
*	函数名：Position_Echo
	作用域：本文件调用
*	功能：  收到下行的需要定位查询的命令，则做相关的处理
			将此时此刻的效果的经度，纬度，时间信息打成数据包
			存储在第二类SMS数据的存储区（buffer1的0x7B----0xB6区域）
			0x7B设置为1（VAILD_2）,并将数据包依次存储在后面的字节缓冲中。
			后面数据包的形式为

*	参数：	全局变量gGeneral_Buffer[]中的数据

*	返回值：进行和参数相关的设置

*	创建人：
*
*	修改历史：（每条详述）

	补充说明：

	补充说明：具体在buffer1中存储第2类SMS的数据形式如下：0x7B----0xB6（整个分配的区域大小）
			  0x7B	存储表示这个区域的SMS是否处理，为1则表示后面还有未处理的SMS，为0则表示已经处理完了SMS
			  0x7C	存储数据包的命令字在这里则为nCommand
			  0x7D	存储数据包的ACK的值ACK
			  0x7E	存储后面数据包的长度（但注意没有存储CHK(MSB),CHK(LSB),EOT,这三个字节）
			  0x7F  从这开始存储数据包
数据区的内容：
LONGTITUDE	4 bytes，无符号整型	经度
LATITUDE	4 bytes，无符号整型	纬度
SPEED	1 byte， 无符号整型	速度，单位：米／秒
AZIMUTH	1 byte， 无符号整型	方位角／15，单位：度
FLAG	1 byte， 无符号整型	标志
TIME	6 bytes，字符型	时间(YYMMDDHHMISS)

\********************************************************/
void Position_Echo(void)
{
	unsigned char nCheck=0;
	unsigned int i=0;
	//1，如果中心号码无效果，则进行暂时的返回发送
	if(   ((gGeneral_Flag&NUM1_VAILD_F_1)==0)
		&&(gNum_Phone_Temp[0]<=14) )
	{
		//1,存储目标号码
		SRAM_Init();
		SRAM_Write(TARGET_NUMBER1_2+0,gNum_Phone_Temp[0]);
		for(i=1;i<=gNum_Phone_Temp[0];i++)
		{
			SRAM_Write(TARGET_NUMBER1_2+i,gNum_Phone_Temp[i]);
			nCheck += gNum_Phone_Temp[i];
		}
		SRAM_Write(TARGET_NUMBER1_CHECK_2,nCheck);
		//2,存储车组，车类，车型，车号
		gKind=gGeneral_Buffer[6];
		gGroup=gGeneral_Buffer[7];
		gVID = gGeneral_Buffer[8];
		gVID<<=8;
		gVID += gGeneral_Buffer[9];
		gGeneral_Flag|=NUM1_VAILD_F_1;
	}
	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,POSITION_ASKECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(ECHO_SMS_2+3,17+4);
	Write_PartData_Sram(ECHO_SMS_2+3);	//17
	SRAM_Write(ECHO_SMS_2+3+18,0);
	SRAM_Write(ECHO_SMS_2+3+19,0);
	SRAM_Write(ECHO_SMS_2+3+20,gStatus2_Flag);
	SRAM_Write(ECHO_SMS_2+3+21,gStatus1_Flag);
	Judge_SMS_Way();
}

/********************************************************\
*	函数名：Set_Cycle_Timer
	作用域：本地文件调用
*	功能：	收到设置上传位置时间间隔的下行命令后，做一些相关的处理
			一是附给变量gCycle_GPRS和gCycle_GSM，二是存储在相应的Flash区中
			gCycle_GPRS变量存储的是在GPRS在线的状态下发送行驶状态数据上行信息的时间间隔
			gCycle_GSM变量存储的是在GSM在线的但GPRS不在线的状态下发送行驶状态数据上行信息的时间间隔

*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Set_Cycle_Timer(void)
{
	unsigned int i=0,j=0;
	unsigned char nACK=0;
	nACK=gGeneral_Buffer[5];
	gCycle_TCP = gGeneral_Buffer[1+9];
	gCycle_TCP <<= 8;
	gCycle_TCP += gGeneral_Buffer[1+10];

	gCycle_GSM = gGeneral_Buffer[1+11];
	gCycle_GSM <<= 8;
	gCycle_GSM += gGeneral_Buffer[1+12];

	if( (gCycle_TCP>0)&&(gCycle_TCP<5) )	gCycle_TCP=5;
	if( (gCycle_GSM>0)&&(gCycle_GSM<5) )	gCycle_GSM=5;

	//如果间隔距离符合条件,则将间隔距离设置为0
	if( gCycle_TCP>=5 )
	{
		gCycle_Distance=0;
	}
	gPublic_Flag &=~ SET_DISTANCE_OK_F_1;
	gSet_Distance_Info_Count=WAIT_DISPINFO_TIME-2;
//	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO2_ONE_2);OperateSPIEnd();
	Deal_Sector(FLASH_INFO2_ONE_2);
	WriteOneByteToBuffer2(TCP_TIMER_2,gGeneral_Buffer[1+9]);
	WriteNextByteToBuffer2(gGeneral_Buffer[1+10]);
	WriteNextByteToBuffer2(gGeneral_Buffer[1+11]);
	WriteNextByteToBuffer2(gGeneral_Buffer[1+12]);
	WriteNextByteToBuffer2(0);
	WriteNextByteToBuffer2(0);
	OperateSPIEnd();
	RFlash_Sector(FLASH_INFO2_ONE_2);
	Re430_Flash(1);
	OperateSPIEnd();
	Clear_Exter_WatchDog();

	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,TIME_INTERVAL_ECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,nACK);
	SRAM_Write(ECHO_SMS_2+3,21);
	Write_PartData_Sram(ECHO_SMS_2+3);	//17
	SRAM_Write(ECHO_SMS_2+3+18,0);
	SRAM_Write(ECHO_SMS_2+3+19,0);
	SRAM_Write(ECHO_SMS_2+3+20,gStatus2_Flag);
	SRAM_Write(ECHO_SMS_2+3+21,gStatus1_Flag);
	Judge_SMS_Way();

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
}
/********************************************************\
*	函数名：Pass_Data_Echo()
	作用域：本地文件调用
*	功能：	接收到下行的查询历史轨迹的命令
			将需要查询的历史轨迹的起始时间附给变量Check_Track_Time_Start
			将需要查询的历史轨迹的结束时间附给变量Check_Track_Time_end
			然后设置有需要进行历史轨迹操作的标志位 gInternal_Flag|=PASS_DATA_F_1

*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
	补充说明：这个时候需要处理的gGeneral_Buffer[]缓冲中的数据
				SMS_ACK[TRACK_ASKECHO_ACK]=gGeneral_Buffer[5]   存储ACK


\********************************************************/
void Pass_Data_Echo(void)
{
	
	unsigned int i=0;
	//存储发送历史轨迹的开始的ACK
	gSMS_ACK[TRACK_ASKECHO_ACK]=gGeneral_Buffer[5];
	//存储计算历史轨迹的开始时间
	gCheck_Track_Time_Start=0;
	gCheck_Track_Time_Start= ( (gGeneral_Buffer[10]%100)/10*16+ (gGeneral_Buffer[10]%10)  );
	gCheck_Track_Time_Start <<= 8;
	gCheck_Track_Time_Start += ( (gGeneral_Buffer[11]%100)/10*16+ (gGeneral_Buffer[11]%10) );
	gCheck_Track_Time_Start <<= 8;
	gCheck_Track_Time_Start += ( (gGeneral_Buffer[12]%100)/10*16+ (gGeneral_Buffer[12]%10) );
	gCheck_Track_Time_Start <<= 8;
	gCheck_Track_Time_Start += ( (gGeneral_Buffer[13]%100)/10*16+ (gGeneral_Buffer[13]%10) );

    //存储计算历史轨迹的结束时间
    gCheck_Track_Time_end=0;
	gCheck_Track_Time_end= ( (gGeneral_Buffer[15]%100)/10*16+ (gGeneral_Buffer[15]%10) );
	gCheck_Track_Time_end <<= 8;
	gCheck_Track_Time_end += ( (gGeneral_Buffer[16]%100)/10*16+ (gGeneral_Buffer[16]%10) );
	gCheck_Track_Time_end <<= 8;
	gCheck_Track_Time_end += ( (gGeneral_Buffer[17]%100)/10*16+ (gGeneral_Buffer[17]%10) );
	gCheck_Track_Time_end <<= 8;
	gCheck_Track_Time_end+= ( (gGeneral_Buffer[18]%100)/10*16+ (gGeneral_Buffer[18]%10) );;


	//如果接收到的历史轨迹开始查询时间和结束时间相同，则表示接收到的是结束历史轨迹查询的命令
	if(gCheck_Track_Time_Start==gCheck_Track_Time_end)
	{
		gCycle_Send_Status=NOP_DATA;
		gCircle_Buffer_Point=0;
		for(i=0;i<sizeof(gCircle_Buffer);i++)	gCircle_Buffer[i]=0;
		gOther_Flag&=~FIND_PASSPAGE_F_1;gOther_Flag&=~FIRST_PASSPAGE_F_1;gOther_Flag&=~SEND_PASSPAGE_F_1;
		gOther_Flag&=~PASS_DATA_LASE_F_1;
		gOther_Flag&=~SEND_PASSPAGE_F_1;
		//设置发送历史轨迹结束的标志
		Load_Buffer1_Echo(TRACK_ENDECHO_UP,gGeneral_Buffer[5]);
		Send_COM1_String( (unsigned char *)PASS_END,sizeof(PASS_END) );
		return;
	}
	if (gCycle_Send_Status!=ALARM_DATA )
	{

		gCycle_Send_Status=PASS_DATA;gOther_Flag|=FIND_PASSPAGE_F_1;
		gOther_Flag&=~FIRST_PASSPAGE_F_1;gOther_Flag&=~SEND_PASSPAGE_F_1;
		if(gPassTrack_Store_Sector==FLASH_PASTDATA_E_2-1)
		{
			gPassPage_First=FLASH_PASTDATA_S_2;
			gPassPage_End=FLASH_PASTDATA_E_2-1;
		}
		else if(gPassTrack_Store_Sector>=FLASH_PASTDATA_E_2)
		{
			gPassPage_First=FLASH_PASTDATA_S_2+1;
			gPassPage_End=FLASH_PASTDATA_E_2;
		}
		else
		{
			gPassPage_First=gPassTrack_Store_Sector+2;
			gPassPage_End=FLASH_PASTDATA_E_2;
		}
	}
	#ifdef Debug_GSM_COM1
	Send_COM1_String( (unsigned char *)TIME_START,sizeof(TIME_START) );
	Send_COM1_Byte('[');
   	Send_COM1_Byte(ASCII(gCheck_Track_Time_Start/0x10000000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_Start%0x10000000)/0x1000000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_Start%0x1000000)/0x100000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_Start%0x100000)/0x10000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_Start%0x10000)/0x1000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_Start%0x1000)/0x100));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_Start%0x100)/0x10));
	Send_COM1_Byte(ASCII(gCheck_Track_Time_Start%0x10));
	Send_COM1_Byte(']');
	Send_COM1_String( (unsigned char *)TIME_END,sizeof(TIME_END) );
	Send_COM1_Byte('[');
   	Send_COM1_Byte(ASCII(gCheck_Track_Time_end/0x10000000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_end%0x10000000)/0x1000000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_end%0x1000000)/0x100000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_end%0x100000)/0x10000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_end%0x10000)/0x1000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_end%0x1000)/0x100));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_end%0x100)/0x10));
	Send_COM1_Byte(ASCII(gCheck_Track_Time_end%0x10));
	Send_COM1_Byte(']');
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);

	Send_COM1_String( (unsigned char *)PAGE_S,sizeof(PAGE_S) );
	Send_COM1_Byte('[');
    Send_COM1_Byte(ASCII(gPassPage_First/1000));
    Send_COM1_Byte(ASCII((gPassPage_First%1000)/100));
    Send_COM1_Byte(ASCII(((gPassPage_First%1000)%100)/10));
	Send_COM1_Byte(ASCII(gPassPage_First%10));
	Send_COM1_Byte(']');
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	Send_COM1_String( (unsigned char *)PAGE_E,sizeof(PAGE_E) );
	Send_COM1_Byte('[');
    Send_COM1_Byte(ASCII(gPassPage_End/1000));
    Send_COM1_Byte(ASCII((gPassPage_End%1000)/100));
    Send_COM1_Byte(ASCII(((gPassPage_End%1000)%100)/10));
	Send_COM1_Byte(ASCII(gPassPage_End%10));
	Send_COM1_Byte(']');
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	#endif
	Judge_SMS_Way();
	
}

/********************************************************\
*	函数名：Disp_Dispaly_Info();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Disp_Dispaly_Info(void)
{
    unsigned int i=0;

	if(gDisp_Buffer_Point+gGeneral_Buffer[0]+2+6<=sizeof(gDisp_Buffer))
	{
	    gDisp_Buffer[gDisp_Buffer_Point]=gGeneral_Buffer[0]+2+6;
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]=0;
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]='@';
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]='%';
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]=gGeneral_Buffer[0]+2+4;
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]=DISP_MAIN_WORD_DOWN;
	    gDisp_Buffer_Point++;
	    //从0xFF开始存储数据(转移整个中心的数据包)
	    Copy_String(gGeneral_Buffer+1,gDisp_Buffer+gDisp_Buffer_Point,gGeneral_Buffer[0]);
	    gDisp_Buffer_Point=gDisp_Buffer_Point+gGeneral_Buffer[0];
	    gDisp_Buffer[gDisp_Buffer_Point]='$';
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]='&';
	    gDisp_Buffer_Point++;
	}
	//下面是显示调试信息
	/*
	#ifdef Debug_GSM_COM1
	Send_COM1_Byte(ASCII(gDisp_Buffer[0]/16));
	Send_COM1_Byte(ASCII(gDisp_Buffer[0]%16));
	Send_COM1_Byte('=');
	Send_COM1_Byte(ASCII(gDisp_Buffer_Point/16));
	Send_COM1_Byte(ASCII(gDisp_Buffer_Point%16));
	Send_COM1_Byte('{');
	for(i=0;i<gDisp_Buffer[0];i++)
	{
		Send_COM1_Byte(ASCII(gDisp_Buffer[i]/16));
		Send_COM1_Byte(ASCII(gDisp_Buffer[i]%16));
	}
	Send_COM1_Byte('}');
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	#endif
	*/
}
/********************************************************\
*	函数名：Set_TCP_OnOrOff();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
	补充说明：接收到下行的是设置进入/退出联网状态的命令，
				需要进行如下几个操作，存储需要响应的ACK
				1，改变进入TCP的开关量，并存储扇区0
				2，写入B类数据区此命令的回应上行命令
\********************************************************/
void Set_TCP_OnOrOff(void)
{
	unsigned int i=0,j=0;
	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,SET_TCP_ONOROFF_ECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(ECHO_SMS_2+3,1);
	SRAM_Write(ECHO_SMS_2+4,gGeneral_Buffer[10]);
	Judge_SMS_Way();
}
/********************************************************\
*	函数名：Query_SYS_Ver();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Query_SYS_Ver(void)
{
	unsigned int i=0;

	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,QUERY_VERSION_ECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(ECHO_SMS_2+3,14);
	SRAM_Write(ECHO_SMS_2+4,SYS_EDITION[0]);
	SRAM_Write(ECHO_SMS_2+5,SYS_EDITION[1]);
	for(i=0;i<6;i++)
	{
		SRAM_Write(ECHO_SMS_2+6+i,SYS_TIME[i]);
	}
	for(i=0;i<6;i++)
	{
		SRAM_Write(ECHO_SMS_2+12+i,SYS_UPDATATIME[i]);
	}
	Judge_SMS_Way();
}

/********************************************************\
*	函数名：Deal_Alarm_Echo();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Deal_Alarm_Echo(void)
{
	unsigned int i=0,j=0;
	gSMS_ACK[ALARM_ACK]=gGeneral_Buffer[5];
	gCycle_Alarm = gGeneral_Buffer[10];
	gCycle_Alarm <<= 8;
	gCycle_Alarm += gGeneral_Buffer[11];
	if( gCycle_Alarm == 0)
	{
		gAlarm_Type=NOP_ALARM;
		gCycle_Send_Status=NOP_DATA;
		gAlarm_Count=0;
		gStatus2_Flag&=~ROB_ALARM_F_1;
		Load_Buffer1_Echo(ALARM_END_UP,gGeneral_Buffer[5]);
	}
	else if( gCycle_Alarm <= 5 )	gCycle_Alarm=5;
}

/********************************************************\
*	函数名：Deal_Vehicle_Monitor();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Deal_Vehicle_Monitor(void)
{
	unsigned int i=0,j=0;
	i=10;j=0;
	while(gGeneral_Buffer[i]!=0x00)
	{
		i++;j++;if(j>16) {j=0;break;}
	}
	//接收到下行的监听信令。
	if(j>4)
	{
		SRAM_Init();
		SRAM_Write(PHONE_AT_2+0,VAILD_2);
		SRAM_Write(PHONE_AT_2+1,M_DIAL);
		SRAM_Write(PHONE_AT_2+2,j);
		for(i=0;i<j;i++)
		{
			SRAM_Write(PHONE_AT_2+3+i,gGeneral_Buffer[i+10]);
		}
	}
}


/********************************************************\
*	函数名：Defence_Password_Ask_Echo();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/

void Defence_Password_Ask_Echo(void)
{
	unsigned int i=0;
	unsigned char OldPassWord_Length=0;

	//读取FLASH相应存储密码区域的密码信息,将密码暂时存储在gGeneral_Buffer[]中
	OperateSPIEnd();
	ReadOnePageToBuffer2(FLASH_INFO1_ONE_2);
	OperateSPIEnd();
	gGeneral_Buffer[0]=ReadByteFromBuffer2(PASSWORD1_F_2);
	for(i=1;i<6;i++)
	{
		gGeneral_Buffer[i]=ReadNextFromBuffer2();
	}
	OperateSPIEnd();

	//调试显示出读出的密码是否正确
	#ifdef Debug_GSM_COM1
	Send_COM1_Byte('P');Send_COM1_Byte('[');
	for(i=0;i<6;i++)	Send_COM1_Byte(gGeneral_Buffer[i]);
	Send_COM1_Byte(']');Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	#endif

	//读出的密码存储在gGeneral_Buffer[0]---gGeneral_Buffer[5]
	for(i=0;i<6;i++)
	{
		if( (gGeneral_Buffer[i]<'0')||(gGeneral_Buffer[i]>'9') )	break;
	}
	//知道密码的长度
	OldPassWord_Length=i;

	//调试显示密码的数据
	#ifdef Debug_GSM_COM1
	Send_COM1_Byte(ASCII(OldPassWord_Length/10));
	Send_COM1_Byte(ASCII(OldPassWord_Length%10));
	Send_COM1_Byte('[');
	for(i=0;i<OldPassWord_Length;i++)	Send_COM1_Byte(gGeneral_Buffer[i]);
	Send_COM1_Byte(']');Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	#endif
	SRAM_Init();
	SRAM_Write(OTHER_SMS_2+0,VAILD_2);
	SRAM_Write(OTHER_SMS_2+1,PASSWORD_ASKECHO_UP);
	SRAM_Write(OTHER_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(OTHER_SMS_2+3,OldPassWord_Length+1);
	for(i=0;i<OldPassWord_Length;i++)
	{
		SRAM_Write(OTHER_SMS_2+4+i,gGeneral_Buffer[i]);
	}
	SRAM_Write(OTHER_SMS_2+4+OldPassWord_Length,0x00);
	Judge_SMS_Way();
}


/********************************************************\
*	函数名：Set_Phone_Limit();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Set_Phone_Limit(void)
{
	unsigned char nACK=0;
	nACK=gGeneral_Buffer[5];
	//1，接收判断
	//判断电话呼入限制的功能
	if(gGeneral_Buffer[10]==0)
	{
		//电话呼入禁止
		gON_OFF_Temp0&=~PHONE_IN_ON;
		gStatus2_Flag|=HANDLE_PHONE_IN_LIMIT;
	}
	else if(gGeneral_Buffer[10]==0xFF)
	{
		//电话呼出允许
		gON_OFF_Temp0|=PHONE_IN_ON;
		gStatus2_Flag&=~HANDLE_PHONE_IN_LIMIT;
	}
	//判断电话呼出限制的功能
	if(gGeneral_Buffer[11]==0)
	{
		//电话呼出禁止
		gON_OFF_Temp0&=~PHONE_OUT_ON;
		gStatus2_Flag|=HANDLE_PHONE_OUT_LIMIT;
	}
	else if(gGeneral_Buffer[11]==0xFF)
	{
		//电话呼出允许
		gON_OFF_Temp0|=PHONE_OUT_ON;
		gStatus2_Flag&=~HANDLE_PHONE_OUT_LIMIT;
	}
	//2，处理后的状态存储
	//根据状态的改变，则需要重新更新扇区5中的数据，并重新计算校验核，写入扇区
//	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO2_ONE_2);OperateSPIEnd();
	Deal_Sector(FLASH_INFO2_ONE_2);
	WriteOneByteToBuffer2(ON_OFF_V1_2,gON_OFF_Temp0);
	OperateSPIEnd();
	RFlash_Sector(FLASH_INFO2_ONE_2);
	Re430_Flash(1);
	//3,处理完后的响应
	//下面是处理需要发送接收到终端进入/退出联网状态的响应
	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,SET_PHONE_LIMIT_ECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,nACK);
	SRAM_Write(ECHO_SMS_2+3,2);
	SRAM_Write(ECHO_SMS_2+4,gGeneral_Buffer[10]);
	SRAM_Write(ECHO_SMS_2+5,gGeneral_Buffer[11]);
	gGeneral_Flag&=~LAND_TCP_ERROR_F_1;
	Judge_SMS_Way();
}
/********************************************************\
*	函数名：Set_Center_Num();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/

void Set_Center_Num(void)
{
	unsigned int i=0,j=0,k=0;
	unsigned char cNum1,cNum2,cNum3;
	//先读出Flash相应的第5个扇区的数据到buffer2中
	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO2_ONE_2);OperateSPIEnd();
	//1，处理接收中心服务号码
	//========================================
	i=1+9;
	while(1)
	{
		if(gGeneral_Buffer[i]==0x00)	break;
		i++;
		if(i>1+9+15) return;
		//这里的15表示目标号码1不会超过15个数
	}
	cNum1=i-(1+9);
	//如果中心服务号码的长度没有超过15，则目标号码有效
	if( (i<=1+9+15)&&(cNum1>0) )
	{
		//计算出此中心服务的号码长度并写入buffer2中相应的位置
		OperateSPIEnd();
		WriteOneByteToBuffer2(CENTER_NUM_F_2,cNum1);
		for(j=1+9;j<i;j++)
		{
			WriteNextByteToBuffer2(gGeneral_Buffer[j]);
		}
		OperateSPIEnd();
	}
	OperateSPIEnd();
	//2，处理接收报警电话号码
	//========================================
	j=i+1;
	while(1)
	{
		if(gGeneral_Buffer[j]==0x00)	break;
		j++;
		if(j>1+i+15)	return;
	}
	cNum2=j-i;
	if( (j<=1+i+15)&&(cNum2>0) )
	{
		//计算出此报警号码的号码长度并写入buffer2
		OperateSPIEnd();
		WriteOneByteToBuffer2(ARALM_NUM_2,cNum2);
		for(k=i+1;k<j;k++)
		{
			WriteNextByteToBuffer2(gGeneral_Buffer[k]);
		}
		OperateSPIEnd();
	}
	OperateSPIEnd();
	//3，处理接收监听电话号码
	//========================================
	k=j+1;
	while(1)
	{
		if(gGeneral_Buffer[k]==0x00)	break;
		k++;if(k>1+j+15)	return;
	}
	cNum3=k-j;
	if( (k<=j+1+15)&&(cNum3>0) )
	{
	}
	//4，预留电话号码
	//========================================
	//5，将更新的设置重新写入相应的Flash
	OperateSPIEnd();
	RFlash_Sector(FLASH_INFO2_ONE_2);
	Re430_Flash(1);
	//6，接收处理完毕，则发送设置响应
	//========================================
	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,CENTER_NUMECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(ECHO_SMS_2+3,21);
	Write_PartData_Sram(ECHO_SMS_2+3);
	SRAM_Write(ECHO_SMS_2+3+18,0);
	SRAM_Write(ECHO_SMS_2+3+19,gStatus3_Flag);
	SRAM_Write(ECHO_SMS_2+3+20,gStatus2_Flag);
	SRAM_Write(ECHO_SMS_2+3+21,gStatus1_Flag);
	Judge_SMS_Way();
}

/********************************************************\
*	函数名：Set_Area_Limit();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*	修改历史：（每条详述）
	此程序的处理是将响应的区域参数存储在外部SRAM中，和buffer2中。并计算gArea_Par的值
\********************************************************/
void Set_Area_Limit(void)
{
	unsigned int  i=0;
	unsigned char Buffer2_Temp=0;
	gArea_Par=0;
	//1,存储外部的FLASH中。
	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_AREA_ONE_2);OperateSPIEnd();
	WriteOneByteToBuffer2(FAREA1_LON_1_2,gGeneral_Buffer[1+9]);
	for(i=1;i<68;i++)
	{
		WriteNextByteToBuffer2(gGeneral_Buffer[1+9+i]);
	}
	OperateSPIEnd();
	RFlash_Sector(FLASH_AREA_ONE_2);
	//3，存储外部的SRAM中的相应位置
	SRAM_Init();
	SRAM_Write( SAREA_DATA_FS,0x55);						//存储区域的有效标志
	for(i=0;i<68;i++)
	{
		SRAM_Write( (SAREA1_LON_1_2+i),gGeneral_Buffer[1+9+i]);
	}
	SRAM_Write( SAREA_DATA_FE,0xAA);
	//4,显示当前区域的参数
	gArea_Par=0;
	SRAM_Init();
	Buffer2_Temp=SRAM_Read(SAREA1_PAR_2);
	if( Buffer2_Temp==0x55)		{gArea_Par|=AREA1_ON;}
	else if(Buffer2_Temp==0xAA)	{gArea_Par|=AREA1_ON;gArea_Par|=AREA1_DIR;}
	Buffer2_Temp=SRAM_Read(SAREA2_PAR_2);
	if( Buffer2_Temp==0x55)		{gArea_Par|=AREA2_ON;}
	else if(Buffer2_Temp==0xAA)	{gArea_Par|=AREA2_ON;gArea_Par|=AREA2_DIR;}
	Buffer2_Temp=SRAM_Read(SAREA3_PAR_2);
	if( Buffer2_Temp==0x55)		{gArea_Par|=AREA3_ON;}
	else if(Buffer2_Temp==0xAA)	{gArea_Par|=AREA3_ON;gArea_Par|=AREA3_DIR;}
	Buffer2_Temp=SRAM_Read(SAREA4_PAR_2);
	if( Buffer2_Temp==0x55)		{gArea_Par|=AREA4_ON;}
	else if(Buffer2_Temp==0xAA)	{gArea_Par|=AREA4_ON;gArea_Par|=AREA4_DIR;}
	//4，接收到区域参数的响应
	Load_Buffer1_Echo(ALLOW_AREA_ECHO_UP,gGeneral_Buffer[5]);
	if(gArea_Par!=0)	gStatus2_Flag|=AREA_VAILD_F_1;
	else				gStatus2_Flag&=~AREA_VAILD_F_1;
}

/********************************************************\
*	函数名：Send_RecordData();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*	修改历史：（每条详述）
	补充说明：将外部数据从新组包，发送给记录仪，然后等待回音
\********************************************************/
void Send_RecordData(void)
{
    unsigned int i=0;
	gSMS_ACK[RECORD_EQUIPMENT_ACK]=gGeneral_Buffer[5];
	for(i=11;i<=gGeneral_Buffer[0]-3;i++)
	{
		Send_COM1_Byte(gGeneral_Buffer[i]);
	}
	/*
	#ifdef Debug_GSM_COM1
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	for(i=11;i<=gGeneral_Buffer[0]-3;i++)
	{
		Send_COM1_Byte(ASCII(gGeneral_Buffer[i]/0x10));
		Send_COM1_Byte(ASCII(gGeneral_Buffer[i]%0x10));
	}
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	#endif
	*/

}
/********************************************************\
*	函数名：Set_Distance_Par();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*	修改历史：（每条详述）

\********************************************************/
void Set_Distance_Par(void)
{
	gCycle_Distance = gGeneral_Buffer[1+9];
	gCycle_Distance <<= 8;
	gCycle_Distance += gGeneral_Buffer[1+10];

	if(gGeneral_Buffer[1+11]==1)
	{
		gON_OFF_Temp1 &=~ DISTANCE_TCP_ON_1;
		gStatus3_Flag |= DISTANCE_ON_F_1;
	}
	else if(gGeneral_Buffer[1+11]==0)
	{
		gON_OFF_Temp1 |= DISTANCE_TCP_ON_1;
		gStatus3_Flag &=~ DISTANCE_ON_F_1;
	}
	gCycle_TCP=0;
	gCycle_GSM=0;
	gCycle_Send_Status = NOP_DATA;
	Load_Buffer1_Echo(SET_DISTANCE_DATA_UP,gGeneral_Buffer[5]);
	//根据状态的改变，则需要重新更新扇区0中的数据，并重新计算校验核，写入扇区
	//OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO2_ONE_2);OperateSPIEnd();
	Deal_Sector(FLASH_INFO2_ONE_2);
	WriteOneByteToBuffer2(ON_OFF_V2_2,gON_OFF_Temp1);
	OperateSPIEnd();
	WriteOneByteToBuffer2(TCP_TIMER_2,0);
	WriteNextByteToBuffer2(0);
	WriteNextByteToBuffer2(0);
	WriteNextByteToBuffer2(0);
	WriteNextByteToBuffer2(gGeneral_Buffer[1+9]);
	WriteNextByteToBuffer2(gGeneral_Buffer[1+10]);
	OperateSPIEnd();
	RFlash_Sector(FLASH_INFO2_ONE_2);
	Re430_Flash(1);
	gPublic_Flag &=~ SET_DISTANCE_OK_F_1;
	gSet_Distance_Info_Count=WAIT_DISPINFO_TIME-2;

}
/********************************************************\
*	函数名：Set_Phone_Num();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*	修改历史：（每条详述）
	电话号码本的存储，固定2个扇区，每个扇区存储10个电话本
	一个电话本固定23个字节的长度，8(姓名)+13(电话号码)
	扇区的0xFF处，存储当前页的号码个数，如果值为0，则需要另外存储一个扇区页
\********************************************************/
void Set_Phone_Num(void)
{
	Do_Phone_Note();
	//存储作为第2类信息发送的响应数据的存储
	//服务器端口设置成功后的响应
	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,ASK_PHONE_NUMECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(ECHO_SMS_2+3,2);
	SRAM_Write(ECHO_SMS_2+4,gGeneral_Buffer[1+9]);
	SRAM_Write(ECHO_SMS_2+5,gGeneral_Buffer[1+10]);
	Judge_SMS_Way();
}
/********************************************************\
*	函数名：Watch_Way();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*	修改历史：（每条详述）

\********************************************************/
void Watch_Way(void)
{
	unsigned int i=0;
	gWatch_Circle = gGeneral_Buffer[1+9];
	gWatch_Circle <<= 8;
	gWatch_Circle += gGeneral_Buffer[1+10];

	gWatch_Time = gGeneral_Buffer[1+11];
	gWatch_Time <<= 8;
	gWatch_Time += gGeneral_Buffer[1+12];

	gWatch_Count = gGeneral_Buffer[1+13];
	gWatch_Count <<= 8;
	gWatch_Count += gGeneral_Buffer[1+14];

	if(gWatch_Circle==0)
	{
		if(gCycle_Send_Status==WATCH_DATA)	gCycle_Send_Status=NOP_DATA;
		gWatch_Count=0;gWatch_Time=0;
		gWatch_Type=WATCH_TYPE_NULL;
		Load_Buffer1_Echo(ASK_FREEWATCH_UP,gGeneral_Buffer[5]);
	}
	else
	{
		if(   (gCycle_Send_Status==TRACK_DATA_TIME)
			||(gCycle_Send_Status==TRACK_DATA_DISTANCE)
			||(gCycle_Send_Status==NOP_DATA)  )
		{
			gCycle_Send_Status=WATCH_DATA;
			gPublic_Flag&=~ACCOFF_OK_F_1;
			gPublic_Flag&=~ACC_OFF_UP_F_1;
			gPublic_Flag&=~ACCOFF_TRACK_F_1;
			ACC_OFF_Timer=0;
			if(gWatch_Time!=0)
			{
				gWatch_Count=0;
				gWatch_Type=WATCH_TYPE_TIME;
			}
			else
			{
				if(gWatch_Count!=0)	gWatch_Type=WATCH_TYPE_COUNT;
				else
				{
					Load_Buffer1_Echo(ASK_FREEWATCH_UP,gGeneral_Buffer[5]);
				}
			}
		}
	}
}

/********************************************************\
*	函数名：Line_Data_Deal();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*	修改历史：（每条详述）

\********************************************************/
void Line_Data_Deal(void)
{
	unsigned int i=0,j=0;
	unsigned char nTemp=0;
	unsigned char Line_Num=0;
	nTemp=gGeneral_Buffer[1+10];	//帧数(范围值1,2)
	Line_Num=gGeneral_Buffer[1+11];	//当前线路点的个数(1--30)
	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,LINE_DATA_ECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(ECHO_SMS_2+3,2);
	SRAM_Write(ECHO_SMS_2+4,gGeneral_Buffer[1+9]);
	SRAM_Write(ECHO_SMS_2+5,gGeneral_Buffer[1+10]);
	Judge_SMS_Way();
	if(Line_Num>30 )
	{
		Line_Num=30;
	}
	OperateSPIEnd();
	//表示当前是第一页的数据
	if(nTemp==1)
	{
		ReadOnePageToBuffer2(FLASH_LINEDATA1_ONE_2);
	}
	//表示是当前第二页的数据
	else if(nTemp==2)
	{
		ReadOnePageToBuffer2(FLASH_LINEDATA2_ONE_2);
	}
	OperateSPIEnd();

	//写buffer2中第一个数据
	WriteOneByteToBuffer2(FLINE_DATA_1,gGeneral_Buffer[1+12]);

	for(i=1;i<8*Line_Num;i++)
	{
		WriteNextByteToBuffer2(gGeneral_Buffer[1+12+i]);
	}
	for(i=8*Line_Num+1;i<264;i++)
	{
		WriteNextByteToBuffer2(0xFF);
	}
	OperateSPIEnd();
	//写入当前页点的个数
	WriteOneByteToBuffer2(FLINE_NUM_1,Line_Num);
	WriteNextByteToBuffer2(~Line_Num);
	OperateSPIEnd();
	if(nTemp==1)		RFlash_Sector(FLASH_LINEDATA1_ONE_2);
	else if(nTemp==2)	RFlash_Sector(FLASH_LINEDATA2_ONE_2);
	Clear_Exter_WatchDog();
	OperateSPIEnd();
	//将扇区存储在相应的FLASH中

	//判断是否接收完线路报警，如果接收完，则将FLAHS中的数据转移到SRAM中
	if(gGeneral_Buffer[1+9]==gGeneral_Buffer[1+10])
	{
		Load_Info_Four();
	}
}