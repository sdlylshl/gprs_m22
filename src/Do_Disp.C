/********************************************************\
*	文件名：  Do_Disp.h
*	创建时间：2004年10月20日
*	创建人：  
*	版本号：  1.00
*	功能：
*
*	修改历史：（每条详述）
\********************************************************/
#include <msp430x14x.h>
#include "General.h"
#include "Uart01.h"
#include "Define_Bit.h"
#include "Other_Define.h"
#include "Disp_Protocol.h"
#include "Sub_C.h"
#include "W_Protocol.h"
#include "SPI45DB041.h"
#include "D_Buffer.h"
#include "Do_Other.h"
#include "TA_Uart.h"
#include "Handle_Protocol.h"
#include "Do_Handle.h"
#include "M22_AT.h"
#include "Do_SRAM.h"
/*
#include "Main_Init.h"
#include "Check_GSM.h"
#include "Do_Reset.h"
#include "Do_GSM.h"
#include "Do_Handle.h"
#include "Do_GPS.h"
#include "Do_Disp.h"
#include "Msp430_Flash.h"
*/
unsigned char Check_Disp_Command(void);
void Disp_Ask_Watch_Status_Echo(void);
void Disp_Ask_Info(unsigned char nCommand,unsigned char nKey_ID,unsigned char nACK);
void Disp_Receive_Info_Echo(unsigned char nCommand,unsigned char nInfo_ID1,unsigned char nInfo_ID2,unsigned char nInfo_ID3,unsigned char nInfo_ID4,unsigned char nACK);
void Disp_Ask_Par(void);
/********************************************************\
*	函数名：Do_Disp_Module()
	作用域：外部文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	补充说明：显示终端数据采用半双工的通讯方式，模拟串口4，不采用握手线，和手柄的通讯方式一样
\********************************************************/
void Do_Disp_Module(void)
{
    unsigned int i=0,k=0;
 	//接收处理来自显示终端的消息包
	if( (gDisp_Receive_Over>50)&&(gDisp_Buffer_R_Point>0) )
	{
		#ifdef Debug_GSM_COM1
		Send_COM1_Byte(ASCII(gDisp_Buffer_R_Point/10));
		Send_COM1_Byte(ASCII(gDisp_Buffer_R_Point%10));
		Send_COM1_Byte('[');
		Send_COM1_String(gDisp_Buffer_R,gDisp_Buffer_R_Point);
		Send_COM1_Byte(']');
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		#endif
		k=Check_Disp_Command();
		gDisp_Buffer_R_Point=0;
		switch(k)
		{
			/********************************************************\
			1,显示终端向主控请求主控状态的命令
			\********************************************************/
			case	DISP_ASK_WATCH_STATUS_UP:
			{
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				Disp_Ask_Watch_Status_Echo();
				break;
			}
			/********************************************************\
			2,接收来自显示终端的信息请求（需要上行到中心请求信息）
			\********************************************************/
			case	DISP_INFO_STATUS_UP:
			{
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				SRAM_Init();
				//1,先判断是否有未处理完的信息，如果有，则返回，接收命令不成功
				i=SRAM_Read(OTHER_SMS_2);
				if(i==VAILD_2)
				{
					gPublic_Flag &=~ DISP_SEND_INFO_F_1;
					if(gDisp_Buffer_Point+7+2<=sizeof(gDisp_Buffer))
					{
						gDisp_Buffer[gDisp_Buffer_Point]=7+2;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=0;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='@';
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='%';
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=7;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=DISP_MAIN_INFO_ECHO_DOWN;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=COMMAND_ERROR;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='$';
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='&';
						gDisp_Buffer_Point++;
					}
				}
				else
				{
					Disp_Ask_Info(DISP_ASK_INFO_UP,gDisp_Buffer_R[4],0);
					gPublic_Flag |= DISP_SEND_INFO_F_1;
				}
				break;
			}
			/********************************************************\
			3，短消息预置的上报
			\********************************************************/
			case	DISP_SMS_SNED_UP:
			{
				//1,先判断是否有未处理完的信息，如果有，则返回，接收命令不成功
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				SRAM_Init();
				i=SRAM_Read(OTHER_SMS_2);
				if(i==VAILD_2)
				{
					gPublic_Flag &=~ DISP_SEND_INFO_F_1;
					if(gDisp_Buffer_Point+7+2<=sizeof(gDisp_Buffer))
					{
						gDisp_Buffer[gDisp_Buffer_Point]=7+2;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=0;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='@';
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='%';
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=7;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=DISP_MAIN_INFO_ECHO_DOWN;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=COMMAND_ERROR;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='$';
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='&';
						gDisp_Buffer_Point++;
					}
				}
				else
				{
					Disp_Ask_Info(DISP_SMS_INFO_UP,gDisp_Buffer_R[4],0);
					gPublic_Flag |= DISP_SEND_INFO_F_1;
				}
				break;
			}
			/********************************************************\
			4,接收来自显示终端接收到主控发送的驾驶员信息的反馈
			\********************************************************/
			case	DISP_RECEIVE_IC_INFO_UP:
			{
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				gGet_Driver_Info_Count=0;
				gDriver_Info=SEND_DRIVER_INFO_OK;
				break;
			}
			/********************************************************\
			4,接收来自显示终端接收到一个下行的信息包的ECHO,需要上行到中心请求
			\********************************************************/
			/*
			case	DISP_RECEIVE_INFO_ECHO_UP:
			{
				Disp_Receive_Info_Echo(DISP_INFO_ECHO_UP,gDisp_Buffer_R[4],gDisp_Buffer_R[5],gDisp_Buffer_R[6],gDisp_Buffer_R[7],0);
				break;
			}
			*/
			/********************************************************\
			5,接收来自显示终端接收到一个下行的响应（显示终端接收到一个每一分钟的
				的时间矫正后，则需要返回的一个响应数据包）
			\********************************************************/
			case	DISP_CLRCLESEND_ECHO_UP:
			{
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				break;
			}
			/********************************************************\
			6, 接收来自显示终端和记录仪之间通讯的协议字（主控系统只负责转发）
				需要增加重复发送功能
			\********************************************************/
			case	DISP_ASK_RECORDINFO_UP:
			{
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				for(k=0;k<gDisp_Buffer_R[2]-5;k++)
				{
					Send_COM1_Byte(gDisp_Buffer_R[k+4]);
				}
				break;
			}
			/********************************************************\
			7, 接收来自显示终端的响应（0xAE）
			\********************************************************/
			case	DISP_SET_ECHO_UP:
			{
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				//下行文字信息的显示终端收到一屏的响应
				if(gDisp_Buffer_R[4]==DISP2_SEND_WORD_ECHO_UP)
				{
					if(gCommon_Flag&ALLOW_OUT_DISP_F_1)
					{
						gCommon_Flag|=DISP_SEND_OK_F_1;
					}
				}
				//下行文字信息全部收完后的响应0x4B
				else if(gDisp_Buffer_R[4]==DISP2_SEND_WORD_UP)
				{
					//传递参数：第二级命令字；ID(4个字节)；ACK，
					Disp_Receive_Info_Echo(gDisp_Buffer_R[4],gDisp_Buffer_R[6],gDisp_Buffer_R[7],gDisp_Buffer_R[8],gDisp_Buffer_R[9],gDisp_Buffer_R[5]);
				}
				//来自中心的预先设置短消息的响应0xAE
				else if(gDisp_Buffer_R[4]==DISP2_SET_SMS_ECHO_UP)
				{
					//传递参数：第二级命令字；ID(1个字节)；ACK
					Disp_Ask_Info(gDisp_Buffer_R[4],gDisp_Buffer_R[6],gDisp_Buffer_R[5]);
					gCommon_Flag|=DISP_SEND_OK_F_1;
				}
				//来自手柄端口的预先设置短消息的响应字0x04
				else if(gDisp_Buffer_R[4]==DISP2_HSET_SMS_ECHO_UP)
				{
					Send_COM3_Load(5,HANDLE_SET_SMS_ECHO,COMMAND_OK);
					gCommon_Flag|=DISP_SEND_OK_F_1;
				}
				//来自手柄端口的初始化显示终端的命令响应0x03
				else if(gDisp_Buffer_R[4]==DISP2_CLEAR_ECHO_UP)
				{
					//返回手柄一个成功信号
					Waiting_Allow_Send();
					Send_COM3_SubProgram(5,HANDLE_CLEAR_DISP_ECHO,COMMAND_OK);
				}
				break;
			}
			/********************************************************\
			8, 接收来自显示终端查询主控终端的请求(需要增加显示终端的重复发送)
			\********************************************************/
			case	DISP_ASK_PAR_UP:
			{
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				Disp_Ask_Par();
				break;
			}
			/********************************************************\
			9,其他
			\********************************************************/
			default:
				break;
		}
	}
	else if(  (P6IN&DISP_TEST)
			&&((gCommon_Flag&ALLOW_SEND_DISP_F_1)==0) 	)
	{
		gCommon_Flag|=ALLOW_SEND_DISP_F_1;
	}
	//处理向显示终端发送消息包的处理
	else if ( 	(gDisp_Buffer_Point>0)				//发送缓冲区中有数据要发送
			  &&(gDisp_Buffer_R_Point==0)			//接收缓冲区的数据指针为0，表示没有接收数据
			  &&(gHandle_Receive_Over>20)   		//因为在同一个中断中，必须保证车栽电话没有向主控发送数据
			  &&(P6IN&DISP_TEST)
			  &&( (gCommon_Flag&ALLOW_R232_F_1)==0)
			  &&(gReceive1_Over_Count>20)
			  &&(gGPS_Receive_Over>20)
			  &&(gDisp_Timer>30)
			  &&(gCommon_Flag&ALLOW_SEND_DISP_F_1)	)
	{
		//1,正常的非需要响应的数据
		if(   ((gCommon_Flag&ALLOW_OUT_DISP_F_1)==0)
		    &&( (gCommon_Flag&DISP_SEND_OK_F_1)==0)		)
		{
			Send_COM4_String( gDisp_Buffer+2,gDisp_Buffer[0]-2 );
			RX4_Ready();
			gCommon_Flag&=~ALLOW_SEND_DISP_F_1;
			gDisp_Timer=0;
			if( gDisp_Buffer[5]==DISP_MAIN_WORD_DOWN )
			{
				gCommon_Flag|=ALLOW_OUT_DISP_F_1;
				gDo_Speed_Count=0;
			}
			else
			{
				k=gDisp_Buffer[0];
		        Copy_String( gDisp_Buffer+k,gDisp_Buffer,gDisp_Buffer_Point-k );
		        gDisp_Buffer_Point=gDisp_Buffer_Point-k;
			}
		}
		//2,收到响应后的处理
		else if(   (gCommon_Flag&DISP_SEND_OK_F_1)
		         &&(gCommon_Flag&ALLOW_OUT_DISP_F_1)  )
		{
			gCommon_Flag&=~DISP_SEND_OK_F_1;
			gCommon_Flag&=~ALLOW_OUT_DISP_F_1;
			k=gDisp_Buffer[0];
	        Copy_String( gDisp_Buffer+k,gDisp_Buffer,gDisp_Buffer_Point-k );
	        gDisp_Buffer_Point=gDisp_Buffer_Point-k;
		}
		//3，重发部分的处理
		else if(   (gCommon_Flag&ALLOW_OUT_DISP_F_1)
		         &&((gCommon_Flag&DISP_SEND_OK_F_1)==0)   )
		{
			if(gSend_Disp_Timer>3)
			{
				gSend_Disp_Timer=0;
				Send_COM4_String( gDisp_Buffer+2,gDisp_Buffer[0]-2 );
				RX4_Ready();
				gCommon_Flag&=~ALLOW_SEND_DISP_F_1;
				gDisp_Timer=0;
				gDisp_ReSend_Count++;
				if(gDisp_ReSend_Count>3)
				{
					gCommon_Flag&=~ALLOW_OUT_DISP_F_1;
					k=gDisp_Buffer[0];
	        		Copy_String( gDisp_Buffer+k,gDisp_Buffer,gDisp_Buffer_Point-k );
	       		 	gDisp_Buffer_Point=gDisp_Buffer_Point-k;
				}
			}
	    }
	}
}
/********************************************************\
*	函数名：Check_Disp_Command()
	作用域：本地文件调用
*	功能：	用来接收处理来自显示终端的接收数据,判断出命令字
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/

unsigned char Check_Disp_Command(void)
{
    if( gDisp_Buffer_R_Point < 6) return(0);
//    if( gDisp_Buffer_R_Point != gDisp_Buffer_R[2]) return(0);
    if( gDisp_Buffer_R[0]!='@' ) return(0);
    if( gDisp_Buffer_R[1]!='%' ) return(0);
    if( gDisp_Buffer_R[gDisp_Buffer_R[2]-2]!='$' ) return(0);
    if( gDisp_Buffer_R[gDisp_Buffer_R[2]-1]!='&' ) return(0);
    return( gDisp_Buffer_R[3] );
}
/********************************************************\
*	函数名：Save_TypeC_Buffer()
	作用域：本地文件调用
*	功能：	用来处理C类需要上传的数据（这里指的是来自显示终端的C类数据（））
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	补充说明：
		0xB1	存储表示这个区域的SMS是否处理，为1则表示后面还有未处理的SMS，为0则表示已经处理完了SMS
		0xB2	存储数据包的命令字在这里则为nCommand
		0xB3	存储数据包的ACK的值ACK
		0xB4	存储后面数据包的长度（但注意没有包括存储CHK(MSB),CHK(LSB),EOT,这三个字节）
		0xB5  	从这开始存储数据包
\********************************************************/



/********************************************************\
*	函数名：Disp_Hand_Free_Defence()
	作用域：本地文件调用
*	功能：	接收到来自显示终端的主动解除防信息的处理
			1，先判断解除设防的密码是否正确，
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	补充说明：
	1，先判断车载终端的密码是否有效，如果还未存储过密码，则不判断解除密码的正确性，直接解除设防
	2，如果车载终端的密码有效，则判断解除密码是否正确，这里需要重点说明的是，因为显示终端的密码如果
		不满足6位，则不足处不会填为0x20，但以后的如果通过手柄来处理密码则始终按6位来处理，不足6位的
		则会填0X20。
	3，如果密码正确，则向显示终端发送密码正确，并且解除设防(包括FLASH区设防标志的清除)
	4，如果密码不正确，则向显示中断发送密码不正确，并且连续3次密码输入不正确，则立刻产生被盗报警信息
		但设防标志仍然存在

\********************************************************/


/********************************************************\
*	函数名：Disp_Ask_Watch_Status_Echo()
	作用域：本地文件调用
*	功能：	接收到来自显示终端主动改变密码的信令
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Disp_Ask_Watch_Status_Echo(void)
{
	if(gDisp_Buffer_Point+14<=sizeof(gDisp_Buffer))
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
			gDisp_Buffer[gDisp_Buffer_Point]=gYear;
			gDisp_Buffer_Point++;
			gDisp_Buffer[gDisp_Buffer_Point]=gMonth;
			gDisp_Buffer_Point++;
			gDisp_Buffer[gDisp_Buffer_Point]=gDate;
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
		gDisp_Buffer[gDisp_Buffer_Point]=gHour;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=gMinute;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=gSecond;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='$';
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='&';
		gDisp_Buffer_Point++;
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
*	修改历史：（每条详述）
\********************************************************/
void Disp_Ask_Info(unsigned char nCommand,unsigned char nKey_ID,unsigned char nACK)
{
	unsigned char Key_ID=0;
	Key_ID=nKey_ID;
	SRAM_Init();
	SRAM_Write(OTHER_SMS_2+0,VAILD_2);
	SRAM_Write(OTHER_SMS_2+1,nCommand);
	SRAM_Write(OTHER_SMS_2+2,nACK);
	SRAM_Write(OTHER_SMS_2+3,22);
	SRAM_Write(OTHER_SMS_2+4,Key_ID);
	Write_PartData_Sram(OTHER_SMS_2+4);
	SRAM_Write(OTHER_SMS_2+4+18,0);
	SRAM_Write(OTHER_SMS_2+4+19,gStatus3_Flag);
	SRAM_Write(OTHER_SMS_2+4+20,gStatus2_Flag);
	SRAM_Write(OTHER_SMS_2+4+21,gStatus1_Flag);
	Judge_SMS_Way();
}
/********************************************************\
*	函数名：Disp_Ask_Info()
	作用域：本地文件调用
*	功能：	接收到来自显示终端的按键请求信息，需要将这些信息上行给中心
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Disp_Receive_Info_Echo(unsigned char nCommand,unsigned char nInfo_ID1,unsigned char nInfo_ID2,unsigned char nInfo_ID3,unsigned char nInfo_ID4,unsigned char nACK)
{
	SRAM_Init();
	SRAM_Write(OTHER_SMS_2+0,VAILD_2);
	SRAM_Write(OTHER_SMS_2+1,nCommand);
	SRAM_Write(OTHER_SMS_2+2,nACK);
	SRAM_Write(OTHER_SMS_2+3,17+8);
	//4个字节的信息包的ID号
	SRAM_Write(OTHER_SMS_2+4,nInfo_ID1);
	SRAM_Write(OTHER_SMS_2+5,nInfo_ID2);
	SRAM_Write(OTHER_SMS_2+6,nInfo_ID3);
	SRAM_Write(OTHER_SMS_2+7,nInfo_ID4);
	Write_PartData_Sram(OTHER_SMS_2+7);
	SRAM_Write(OTHER_SMS_2+7+18,0);
	SRAM_Write(OTHER_SMS_2+7+19,0);
	SRAM_Write(OTHER_SMS_2+7+20,gStatus2_Flag);
	SRAM_Write(OTHER_SMS_2+7+21,gStatus1_Flag);
	Judge_SMS_Way();
}

/********************************************************\
*	函数名：Disp_Ask_Par(()
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Disp_Ask_Par(void)
{
	unsigned int i=0;
	unsigned char Data_Temp[12];
	unsigned int iData=0;
	Ask_Par();
	//向显示屏发送的数据包需要另外处理一下
	/*
	gGeneral_Buffer[0]存储的是发送数据的长度
	需要将gGeneral_Buffer[1]-----gGeneral_Buffer[5]的数据转换为ASCII字符显示形式
	例程：
	40 25 50 13 30 30 30 31 30 31 30 30 30 30 30 35 31 33 36 39 31 38 37 38 35 33 34 00 32 31 30 2E 32 31 2E 32 34 34 2E 33 39 00 30 39 39 38 38 31 2C 22 49 50 22 2C 22 43 4D 4E 45 54 22 00 31 30 30 30 32 30 30 30 32 30 30 30 30 30 30 30 24 26
	*/
	Data_Temp[0]=ASCII(gGeneral_Buffer[1]/10);
	Data_Temp[1]=ASCII(gGeneral_Buffer[1]%10);		//车型

	Data_Temp[2]=ASCII(gGeneral_Buffer[2]/10);
	Data_Temp[3]=ASCII(gGeneral_Buffer[2]%10);		//车组

	Data_Temp[4]=ASCII(gGeneral_Buffer[3]/10);
	Data_Temp[5]=ASCII(gGeneral_Buffer[3]%10);		//车类

	iData = gGeneral_Buffer[4];
	iData <<=8;
	iData +=gGeneral_Buffer[5];

	Data_Temp[6]=ASCII(iData/100000);
	Data_Temp[7]=ASCII( (iData%100000)/10000 );
	Data_Temp[8]=ASCII( (iData%10000)/1000);
	Data_Temp[9]=ASCII( (iData%1000)/100);
	Data_Temp[10]=ASCII( (iData%100)/10);
	Data_Temp[11]=ASCII( iData%10 );				//车号

	//重新整理gGeneral_Buffer缓冲
	for(i=gGeneral_Buffer[0]+7;i>=13;i--)
	{
		gGeneral_Buffer[i]=gGeneral_Buffer[i-7];
	}
	//重新整理数据长度
	gGeneral_Buffer[0]=gGeneral_Buffer[0]+7;
	for(i=0;i<12;i++)
	{
		gGeneral_Buffer[1+i]=Data_Temp[i];
	}
	if(gDisp_Buffer_Point+gGeneral_Buffer[0]+1+5+2<=sizeof(gDisp_Buffer))
	{
		gDisp_Buffer[gDisp_Buffer_Point]=gGeneral_Buffer[0]+1+5+2;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=0;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='@';
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='%';
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=gGeneral_Buffer[0]+1+5;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=DISP_ASK_PAR_DOWM;
		gDisp_Buffer_Point++;
		for(i=1;i<=gGeneral_Buffer[0];i++)
		{
			gDisp_Buffer[gDisp_Buffer_Point]=gGeneral_Buffer[i];
			gDisp_Buffer_Point++;
		}
		gDisp_Buffer[gDisp_Buffer_Point]='$';
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='&';
		gDisp_Buffer_Point++;
	}
}

