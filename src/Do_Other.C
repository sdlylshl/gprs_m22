/********************************************************\
*	文件名：  Do_Other.C
*	创建时间：2004年12月7日
*	创建人：  
*	版本号：  1.00
*	功能：	  针对一些其他外部操作，比如报警检测等等
*	文件属性：公共文件
*	修改历史：（每条详述）
\********************************************************/
#include <msp430x14x.h>
#include <math.h>
#include <stdlib.h>
#include "Define_Bit.h"
#include "Other_Define.h"
#include "General.h"
#include "Uart01.h"
#include "W_Protocol.h"
#include "D_Buffer.h"
#include "SPI45DB041.h"
#include "Sub_C.h"

#include "TA_Uart.h"
#include "M22_AT.h"
#include "Do_Other.h"
#include "Do_Handle.h"
#include "Handle_Protocol.h"
#include "Disp_Protocol.h"
#include "Do_SRAM.h"
#include "Record_Protocol.h"
#include "Do_M22.h"
/*
#include "Main_Init.h"
#include "Check_GSM.h"
#include "Do_Reset.h"
#include "Do_GPS.h"
#include "Do_Disp.h"
*/
//#define Debug_GSM_COM1
unsigned int Circle_Data_GSM( unsigned char nCount,unsigned char nCommand,unsigned char nACK,unsigned int iCheckOutTemp );
unsigned int Circle_Data_GSM_Sub(unsigned int iCheckOutTemp);
unsigned int Dichotomy_PassData_First(unsigned int iFirst_Page,unsigned int iEnd_Page);

unsigned long int Max_Result(unsigned long int x,unsigned long int y);
unsigned long int Min_Result(unsigned long int x,unsigned long int y);
unsigned char Check_Area_Alarm(unsigned char Num);
unsigned char Account_Mid_Dot(double x1,double y1,double x2,double y2);
unsigned char  Check_Line_Data(unsigned char S_Num_ID);

//unsigned int ADC_NORMAIL_VCC[2]={ 0x0850,0x0EF0 };
//unsigned int ADC_STOP_VCC[2]={ 0x01F0,0x02EF };
/********************************************************\
补充说明：ADC_NORMAIL_VCC[2]根据实际的电路(分压电阻的不同)在正常状态下检测ADC转换的数值的范围
		  ADC_STOP_VCC[2]根据实际电路，已经执行了制动后，检测ADC转换的数值的范围
\********************************************************/
void GSM_WORK_Data(void);
void TCP_WORK_Data(unsigned int Timer,unsigned char nCommand);
/********************************************************\
*	函数名：Do_Other_Module
	作用域：外部文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Do_Other_Module(void)
{
	static unsigned char Test_Alarm_Step=0;
	static unsigned char Ring_Test_Step=0;
	static unsigned char Test_Status_Step=0;
	unsigned int i=0,j=0,k=0,h=0;
	unsigned long int x=0,y=0;
	unsigned char Check_Result=0;
	unsigned char Area_All=0;
	unsigned char Check_Record=0;
	unsigned char Area_Out_Num=0;
	double Distance_Temp=0;
	unsigned long int Dis_Data=0;

	//每间隔30分钟验证存储在buffer1中的参数是否有效
	if(gCheck_Par_Timer>CHECK_SYS_PAR_TIME)
	{
		gCheck_Par_Timer=0;
		k=Check_ParNum();
		if( (k==0)||(k==1) )
		{
			i=Check_Vaild(GPRS_IP_ADDRESS1_2,GPRS_IP_ADDRESS1_CHECK_2);
			j=Check_Port();
			h=Check_Vaild(GPRS_APN_ADDRESS2_2,GPRS_APN_END_2);
			if( (i==1)||(j==1)||(h==1))
			{
				//事先得先保存短消息中心号码，防止操作擦除掉目标号码
				gGeneral_Buffer[CENTER_NUM_2]=SRAM_Read(CENTER_NUM_2);
				for(i=CENTER_NUM_2+1;i<=CENTER_NUM_CHECK_2;i++)
				{
					gGeneral_Buffer[i]=SRAM_Read(i);
				}
				OperateSPIEnd();
				Load_Flash_Par_Buffer1();
			}
		}
		if(gInternal_Flag&SET_CSCA_F_1)	gTimer_CSQ=GSM_CHECKCSQ_TIME-2;
	}
	if(gCommon_Flag&ALLOW_HANGUP_PHONE_F_1)
	{
		gCommon_Flag&=~ALLOW_HANGUP_PHONE_F_1;
		SRAM_Init();
		SRAM_Write(PHONE_AT_2+0,VAILD_2);
		SRAM_Write(PHONE_AT_2+1,H_HUNGUP);
		SRAM_Write(PHONE_AT_2+2,0);
		Send_COM3_Load(4,MAIN_HANG_UP,0);
		gSys_Handle=MAIN_HANG_UP;
	}
	/********************************************************\
	A0;增加处理在来电或者拨出状态下的超时处理
	\********************************************************/
	if(gHangUp_Timer>HANGUP_COUNT)
	{
		if(	(gGeneral_Flag&RING_F_1)||(gGeneral_Flag&DAILING_UP_F_1)||(gGeneral_Flag&MONITOR_ON_F_1) )
		{
			gHangUp_Timer=0;
			SRAM_Init();
			SRAM_Write(PHONE_AT_2+0,VAILD_2);
			SRAM_Write(PHONE_AT_2+1,H_HUNGUP);
			SRAM_Write(PHONE_AT_2+2,0);
			Send_COM3_Load(4,MAIN_HANG_UP,0);
			gSys_Handle=MAIN_HANG_UP;
		}
	}
	/********************************************************\
	A1;如果未接收到登陆响应，则进行再次发送登陆命令
	\********************************************************/
	if(   (gLand_Interval_Timer>M22_LAND_TIME)
		&&((gGeneral_Flag&LAND_TCP_SUCCESS_F_1)==0)	)
	{
		gOther_Flag|=ALLOW_SENDLAND_F_1;
		gLand_Interval_Timer=0;
		gWait_Land_Echo_Count++;
		if(gWait_Land_Echo_Count>10)
		{
			gWait_Land_Echo_Count=0;
			TCP_Land_Error_Return();
		}
	}
	/********************************************************\
	A2;清除相关数据
	\********************************************************/
	if( ((gGeneral_Flag&TCP_PHONE_F_1)==0)&&(gDTMF_SAVE[0]!=0 ) )
	{
		for(i=0;i<sizeof(gDTMF_SAVE);i++)	gDTMF_SAVE[i]=0;
	}
	/********************************************************\
	A2-1;上报数据的状态位,1,无周期数据，2，按时间间隔上报数据
						3，按行使间隔上报数据，4，报警类型数据
	\********************************************************/
	//1,无周期数据
	if(gCycle_Send_Status==NOP_DATA)
	{
		gStatus3_Flag&=~M22_WORK_0_F_1;
		gStatus3_Flag&=~M22_WORK_1_F_1;
	}
	//2，按时间间隔上报数据
	else if(gCycle_Send_Status==TRACK_DATA_TIME)
	{
		gStatus3_Flag|=M22_WORK_0_F_1;
		gStatus3_Flag&=~M22_WORK_1_F_1;
	}
	//3，按行使间隔上报数据
	else if(gCycle_Send_Status==TRACK_DATA_DISTANCE)
	{
		gStatus3_Flag&=~M22_WORK_0_F_1;
		gStatus3_Flag|=M22_WORK_1_F_1;
	}
	//4，报警类型的上报数据
	else if(gCycle_Send_Status==ALARM_DATA)
	{
		gStatus3_Flag|=M22_WORK_0_F_1;
		gStatus3_Flag|=M22_WORK_1_F_1;
	}
	/********************************************************\
	A2-2;进行线路报警的标志
	\********************************************************/
	if(   (gCommon_Flag&ALLOW_LINE_DATA_F_1)
		&&(gCommon_Flag&ALLOW_LINE_DIS_F_1)  )
	{
		gStatus3_Flag|=LINE_LIMIT_F_1;
	}
	else
	{
		gStatus3_Flag&=~LINE_LIMIT_F_1;
		if(gAlarm_Type==OVER_LINE_ALARM)
		{
			gAlarm_Type=NOP_ALARM;
			gCycle_Send_Status=NOP_DATA;
			gAlarm_Count=0;
			gStatus2_Flag&=~ROB_ALARM_F_1;
			gCommon_Flag&=~STOP_OVERLINE_F_1;
			gCircle_Timer=0;
			gOver_Line_Count=0;
		}
	}
	/********************************************************\
	A3;每间隔3秒向显示终端发送一次系统状态（GPS的数据有效果无效，GSM的信号强度，TCP的在线状态，当前速度值）
		已经转移到下面的速度处理部分，以1秒发送一个速度处理,已经放在下一个部分处理
	\********************************************************/
	/********************************************************\
	A3-1;每间隔10秒向手柄发送一个场强信号
	\********************************************************/
	if(gHandle_Single_Count>10)
	{
		if( (gM22_Status==TCP_ONLINE_WORK)&&(gSignal<10) ) 	gSignal=30;
		gHandle_Single_Count=0;
		Send_Handle_Signal(gSignal);
	}
	/********************************************************\
	A4;如果在接收记录仪大量数据的存储区中还有未发送完的数据，则在这里处理
	\********************************************************/
	if(   (gOther_Flag&RECORD_DATA_F_1)
		&&(gSend_RecordData_Timer>=3)
		&&((gOther_Flag&ALLOW_SEND_RECORD_F_1)==0)  )
	{
		gSend_RecordData_Timer=0;
		gOther_Flag|=ALLOW_SEND_RECORD_F_1;
	}
	/********************************************************\
	A4;处理每一秒速度相关的处理
	\********************************************************/
	if(gDo_Speed_Count>=1)
	{
		gDo_Speed_Count=0;
		i=0;
		if( (gPublic_Flag&RECORD_NULL_F_1)==0 )
		{
			gSpeed_Disp=gSpeed_Record;
			gSpeed=(gSpeed_Record/3.6)+0.5;
			i=1;
		}
		else if(  (gPublic_Flag&RECORD_NULL_F_1)
				&&( (gPublic_Flag&GPS_NULL_F_1)==0)	)
		{
			gSpeed_Disp=(gSpeed_Gps*3.6)+0.5;
			gSpeed=gSpeed_Gps;
			i=1;
		}
		if(i==1)
		{
			//1;判断与超速报警相关的处理
			if( (gSpeed>=gMAX_Speed)&&(gSpeed<80)&&(gMAX_Speed>=10) )
			{
				gOver_Speed_Count++;
			}
			else
			{
				gOver_Speed_Count=0;
				if(gAlarm_Type==OVER_SPEED_ALARM)	gInternal_Flag|=STOP_OVERSPEED_F_1;
			}
			//2;判断关于制动方面的处理,在GPS数据有效的状态下，判断速度是否小于某一个值，并且持续5秒
			if( (gSpeed==0)||(gSpeed<=2) )	gSTOP_Speed_Count++;
			else							gSTOP_Speed_Count=0;
		}

		//每秒向显示终端发送数据
		//===================================================
		if(  (gDisp_Buffer_Point+14<=sizeof(gDisp_Buffer))
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
			gDisp_Buffer[gDisp_Buffer_Point]=DISP_MAIN_SYS_STATUS_DOWN;
			gDisp_Buffer_Point++;
			if(gGeneral_Flag&GPS_VALID_F_1)
			{
				gDisp_Buffer[gDisp_Buffer_Point]=0x01;			//GPS有效
			}
			else
			{
				gDisp_Buffer[gDisp_Buffer_Point]=0x00;			//GPS无效
			}
			gDisp_Buffer_Point++;
			gDisp_Buffer[gDisp_Buffer_Point]=gSignal;			//信号强度
			gDisp_Buffer_Point++;
			if(gM22_Status==TCP_ONLINE_WORK)
			{
				gDisp_Buffer[gDisp_Buffer_Point]=1;				//TCP的在线状态
			}
			else
			{
				gDisp_Buffer[gDisp_Buffer_Point]=0;
			}
			gDisp_Buffer_Point++;
			gDisp_Buffer[gDisp_Buffer_Point]=gSpeed_Disp;		//当前速度
			gDisp_Buffer_Point++;
			gDisp_Buffer[gDisp_Buffer_Point]=gDisp_Status;		//预留
			gDisp_Buffer_Point++;
			gDisp_Buffer[gDisp_Buffer_Point]=0;					//预留
			gDisp_Buffer_Point++;
			gDisp_Buffer[gDisp_Buffer_Point]='$';
			gDisp_Buffer_Point++;
			gDisp_Buffer[gDisp_Buffer_Point]='&';
			gDisp_Buffer_Point++;
		}
	}
	/********************************************************\
	A5;	判断是否产生超速度报警的条件和结束超速报警的条件
	\********************************************************/
	if( gOver_Speed_Count>4 )
	{
		gOver_Speed_Count=0;
		if(gAlarm_Type==NOP_ALARM)
		{
			gAlarm_Type = OVER_SPEED_ALARM;
			gCycle_Send_Status=ALARM_DATA;
			gCircle_Timer=0;
			gCycle_Alarm=ALARM_CIRCLE_TIME;
			gSMS_ACK[ALARM_ACK]=0;
			Circle_Data_TCP(ALARM_UP,gSMS_ACK[ALARM_ACK]);
			Send_Over_Alarm_to_Disp(gAlarm_Type);
			gInternal_Flag|=SEND_CIRCLE_F_1;
			Judge_SMS_Way();
		}
	}
	else if( (gAlarm_Type==OVER_SPEED_ALARM)&&(gInternal_Flag&STOP_OVERSPEED_F_1) )
	{
		gAlarm_Type=NOP_ALARM;
		gCycle_Send_Status=NOP_DATA;
		gAlarm_Count=0;
		gStatus2_Flag&=~ROB_ALARM_F_1;
		gInternal_Flag&=~STOP_OVERSPEED_F_1;
		gCircle_Timer=0;
		gOver_Speed_Count=0;
		Circle_Data_TCP(VEHICLE_RUNDATA_UP,0);
		gInternal_Flag|=SEND_CIRCLE_F_1;
		Judge_SMS_Way();
	}
	/********************************************************\
	A6;判断是否产生疲劳驾驶报警和结束疲劳驾驶报警的条件
	\********************************************************/
	//A6-1:产生疲劳驾驶报警的条件
	if(	gOver_Tire_Count>4 )
	{
		gOver_Speed_Count=0;
		if(gAlarm_Type==NOP_ALARM)
		{
			gAlarm_Type = OVER_TIRE_ALARM;
			gCycle_Send_Status=ALARM_DATA;
			gCircle_Timer=0;
			gCycle_Alarm=ALARM_CIRCLE_TIME;
			gSMS_ACK[ALARM_ACK]=0;
			Circle_Data_TCP(ALARM_UP,gSMS_ACK[ALARM_ACK]);
			Send_Over_Alarm_to_Disp(gAlarm_Type);
			gInternal_Flag|=SEND_CIRCLE_F_1;
			Judge_SMS_Way();
		}
	}
	//A6-2:结束疲劳驾驶报警的条件
	else if	(gStop_Tire_Count>4)
	{
		gStop_Tire_Count=0;
		if(gAlarm_Type==OVER_TIRE_ALARM)
		{
			gAlarm_Type = NOP_ALARM;
			gCycle_Send_Status = NOP_DATA;
			gAlarm_Count=0;
			gCircle_Timer=0;
			gCycle_Alarm=0;
			Circle_Data_TCP(VEHICLE_RUNDATA_UP,0);
			gInternal_Flag|=SEND_CIRCLE_F_1;
 			Judge_SMS_Way();
		}
	}
	/********************************************************\
	A7;如果ACC检测线处于开启的状态，则需要进行开启模块电源的操作
		但如果处于关闭状态，则进行时间统计，如果超过48小时仍然为关闭状态，
		则需要进行关闭两个模块的电源供电，以达到节省用电的目的
	\********************************************************/
	//ACC检测处于开启状态
	if(gStatus1_Flag&ACC_ON_F_1)
	{
		if(   (gPublic_Flag&ACCOFF_TRACK_F_1)
			||(gPublic_Flag&ACCOFF_OK_F_1) )
		{
			if((gCommon_Flag&ALLOW_RESET_MCU_F_1)==0)
			{
				P6OUT&=~POWER_ON_OFF;
				gCommon_Flag|=ALLOW_RESET_MCU_F_1;
				gRload_Flash_Par_Timer=RLAOD_FLASH_TIME-5;
			}
//	    	Send_COM1_String((unsigned char *)POWER_OFF,sizeof(POWER_OFF));
//			gOther_Flag|=RESET_PROGRAME_F_1;
//			while(1);
		}
		else if(gPublic_Flag&ACC_OFF_UP_F_1)
		{
			gPublic_Flag&=~ACC_OFF_UP_F_1;
		}
	}
	//ACC检测处于关闭状态
	else
	{
		//1，先发送一个信息上传
		if(   ((gPublic_Flag&ACC_OFF_UP_F_1)==0)
			&&((gPublic_Flag&ACCOFF_TRACK_F_1)==0)
			&&((gPublic_Flag&ACCOFF_OK_F_1)==0)
			&&(gCycle_Send_Status!=WATCH_DATA)
			&&(gCycle_Send_Status!=ALARM_DATA) )
		{
			gPublic_Flag|=ACC_OFF_UP_F_1;
			ACC_OFF_Timer=0;
			Circle_Data_TCP(VEHICLE_RUNDATA_UP,0);
			gInternal_Flag|=SEND_CIRCLE_F_1;
			Send_COM1_String( (unsigned char *)OFF_ACC,sizeof(OFF_ACC) );
 			Judge_SMS_Way();
		}
		//2，ACC持续关闭时间达到满足改变上传行车轨迹的时间间隔
		else if(  (ACC_OFF_Timer<ACC_LASTOFF_TIME)
		        &&(ACC_OFF_Timer>=ACC_CHANGEOFF_TIME)
				&&(gPublic_Flag&ACC_OFF_UP_F_1 )
				&&((gPublic_Flag&ACCOFF_TRACK_F_1)==0)
				&&((gPublic_Flag&ACCOFF_OK_F_1)==0)
				&&(gCycle_Send_Status!=WATCH_DATA)
				&&(gCycle_Send_Status!=ALARM_DATA) 	)
		{
			if(	  (gCycle_TCP<ACCOFF_TRACK_TIME)
				&&(gCycle_TCP>0) )
			{
				gCycle_TCP=ACCOFF_TRACK_TIME;
			}
			gPublic_Flag|=ACCOFF_TRACK_F_1;
		}

		//3，持续规定的一段时间后，则需要关闭模块电源
		else if(   (ACC_OFF_Timer>ACC_LASTOFF_TIME)
			&&(gPublic_Flag&ACC_OFF_UP_F_1)
			&&(gPublic_Flag&ACCOFF_TRACK_F_1 )
			&&((gPublic_Flag&ACCOFF_OK_F_1)==0)
			&&(gCycle_Send_Status!=WATCH_DATA)
			&&(gCycle_Send_Status!=ALARM_DATA)  )
		{
			P6OUT|=POWER_ON_OFF;
			gPublic_Flag|=ACCOFF_OK_F_1;
			gReset_M22_On_Setp=0;
			gGeneral_Flag&=~RESET_GSM_ON_F_1;
			gM22_Status=GSM_WORK;
			gGeneral_Flag&=~LAND_TCP_SUCCESS_F_1;
			gGSM_Oper_Type=GSM_PHONE;
			gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
			gPhone_Step=0;gTimer_GSM_AT=0;
		}

		//3，关闭模块电源
		else if( gPublic_Flag&ACCOFF_OK_F_1 )
		{
			P6OUT|=POWER_ON_OFF;
			gGeneral_Flag&=~OFF_M22_F_1;
			gGeneral_Flag&=~OFF_GPS_F_1;
			gM22_GSM_ECHO_Timer=0;
			gFF0D_Receive_Timer=0;
			gGPS_No_Output_Timer=0;
			gGPS_Invaild_Timer=0;
			gGeneral_Flag&=~RESET_GSM_ON_F_1;
		}
	}
	/********************************************************\
	A8;判断是否可以执行制动或者解除制动操作,制动方面的信息不存储在Flash中
	\********************************************************/
	//1,是否允许执行制动操作
	/*
	if( (gM22_Stop_Status==ALLOW_SET_STOP)&&(gSTOP_Speed_Count>=5) )
	{
		gM22_Stop_Status=ASK_SET_STOP;
		P6OUT|=STOP_Q1;
		P6OUT|=STOP_Q2;

		P6OUT|=STOP_Q1;
		P6OUT|=STOP_Q2;
		gOK_Count=0;gError_Count=0;
	}
	//2,是否允许执行解除制动操作
	else if(gM22_Stop_Status==ALLOW_FREE_STOP)
	{
		gM22_Stop_Status=ASK_FREE_STOP;
		P6OUT&=~STOP_Q1;
		P6OUT&=~STOP_Q2;

		P6OUT&=~STOP_Q1;
		P6OUT&=~STOP_Q2;
		gOK_Count=0;gError_Count=0;
	}
	*/
	/********************************************************\
	A9;	制动结果的检测和判断
	\********************************************************/
	if(gCheck_Stop_Timer>500)
	{
		gCheck_Stop_Timer=0;
		/*
		if(   (gM22_Stop_Status==ASK_SET_STOP)		//执行制动动作的状态下
			||(gM22_Stop_Status==STOP_STATUS) )		//已经制动的状态下
		{
			if(P2IN&CHECK_ON_OFF)	gOK_Count++;
			else					gError_Count++;

			if( (gOK_Count+gError_Count)>60)
			{
				if(gOK_Count>45)
				{
					gStatus1_Flag|=STOP_SUCCESS_F_1;
					gM22_Stop_Status=STOP_STATUS;
				}
				else
				{
					gStatus1_Flag&=~STOP_SUCCESS_F_1;
					gM22_Stop_Status=NOP_READY;
				}
				gOK_Count=0;gError_Count=0;
			}
		}
		else if(   (gM22_Stop_Status==ASK_FREE_STOP)	//执行解除制动动作的状态下
				 ||(gM22_Stop_Status==NOP_READY) )		//已经确定解除制动的状态下
		{
			if( (P2IN&CHECK_ON_OFF)==0 )	gOK_Count++;
			else							gError_Count++;

			if( (gOK_Count+gError_Count)>60)
			{
				if(gOK_Count>45)
				{
					gStatus1_Flag&=~STOP_SUCCESS_F_1;
					gM22_Stop_Status=NOP_READY;
				}
				else
				{
					gStatus1_Flag|=STOP_SUCCESS_F_1;
					gM22_Stop_Status=STOP_STATUS;
				}
				gOK_Count=0;gError_Count=0;
			}
		}
		*/
	}
	/********************************************************\
	A12;判断周期数据发送的状态(这里是处理如果有行车轨迹，后来又有了类似报警，后来报警解除后的行车轨迹的继续上传的操作)
	\********************************************************/
	if( (gGeneral_Flag&TCP_IP_VALID_1)&&((gON_OFF_Temp0&TCP_LAND_ON_OFF_1)==0) )
	{
		if( (gCycle_TCP>0)&&(gCycle_Send_Status==NOP_DATA) )
		{
			gCycle_Send_Status=TRACK_DATA_TIME;gCircle_Timer=0;
			gInternal_Flag&=~SEND_CIRCLE_F_1;gGSM_Work_Count=0;gCheck_Data=0;
		}
		else if(    (gCycle_Distance>0)
				  &&(gCycle_Send_Status==NOP_DATA) )
		{
			gCycle_Send_Status=TRACK_DATA_DISTANCE;gCircle_Timer=0;
			gInternal_Flag&=~SEND_CIRCLE_F_1;gGSM_Work_Count=0;gCheck_Data=0;
		}
	}
	else
	{
		if(   (gCycle_GSM>0)
			&&(gCycle_Send_Status==NOP_DATA) )
		{
			gCycle_Send_Status=TRACK_DATA_TIME;gCircle_Timer=0;
			gInternal_Flag&=~SEND_CIRCLE_F_1;gGSM_Work_Count=0;gCheck_Data=0;
		}
		else if(   (gCycle_Distance>0)
				 &&(gCycle_Send_Status==NOP_DATA) )
		{
			gCycle_Send_Status=TRACK_DATA_DISTANCE;gCircle_Timer=0;
			gInternal_Flag&=~SEND_CIRCLE_F_1;gGSM_Work_Count=0;gCheck_Data=0;
		}
	}
	/********************************************************\
	A13;在TCP在线的工作方式下，如果检测到有来电（通过管脚电平变化的检测）
	\********************************************************/
	if(gM22_Status==TCP_ONLINE_WORK)
	{
		if(Ring_Test_Step==0)
		{
			if( (P1IN&RING_TEST)==0 )
			{
				Ring_Test_Step=1;
				gRing_Test_Timer=0;
			}
		}
		else if(Ring_Test_Step==1)
		{
			if(P1IN&RING_TEST)
			{
				Ring_Test_Step=0;
				gRing_Test_Timer=0;
			}
			else if(gRing_Test_Timer>RING_TEST_TIME)
			{
				if( (gGeneral_Flag&TCP_PHONE_F_1)==0 )
				{
					if((gGeneral_Flag&RING_F_1)==0)			gHangUp_Timer=0;
					gGeneral_Flag|=TCP_PHONE_F_1;
					gStatus1_Flag|=PHONE_ONLINE_F_1;
					gGeneral_Flag|=RING_F_1;
				}
				Ring_Test_Step=0;gRing_Test_Timer=0;
			}
		}
		else
		{
			Ring_Test_Step=0;
			gRing_Test_Timer=0;
		}
	}
	/********************************************************\
	A14;如果判断接收驾驶员信息未成功，则需要周期向记录仪发送查询驾驶员信息。
		直到设置成功为止
		这里取消GPS主动查询驾驶员信息的程序操作
	\********************************************************/
	/*
	//1,判断是否需要查询记录仪的驾驶员信息数据
	if(   (gGet_Driver_Info_Count>GET_DRIVER_INFO_TIME)
		&&(gDriver_Info==READY_READ_DRIVER_INFO)
		&&(gStatus==NORMAL_TYPE)   )
	{
    	Send_COM1_Byte(gStatus);
    	Check_Record = gStatus;
    	Send_COM1_Byte(0xAA);
    	Check_Record ^= 0xAA;
    	Send_COM1_Byte(0x75);
    	Check_Record ^= 0x75;
    	Send_COM1_Byte(K_GET_DRIVEDATA_E_ECHO);
    	Check_Record ^= K_GET_DRIVEDATA_E_ECHO;
    	Send_COM1_Byte(0x00);
    	Check_Record ^= 0;
    	Send_COM1_Byte(0x00);
    	Check_Record ^= 0;
    	Send_COM1_Byte(0x00);
    	Check_Record ^= 0;
    	Send_COM1_Byte(Check_Record);					//校验核
    	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);		//包尾
    	gGet_Driver_Info_Count=0;
    	gDriver_Info=WAIT_RETURN_DRIVER_INFO;

	}
	//2,等待查询记录仪的驾驶员信息返回失败，则需要从新开始计算时间等下一个周期的查询
	else if(  (gDriver_Info==WAIT_RETURN_DRIVER_INFO)
			&&(gGet_Driver_Info_Count>WAIT_RETURNINFO_TIME) )
	{
		gGet_Driver_Info_Count=0;
		gDriver_Info=READY_READ_DRIVER_INFO;
	}
	*/
	//3,判断是否满足需要想显示终端发送驾驶员信息的数据条件
	/*
	if(   (gGet_Driver_Info_Count>READY_SENDINFO_TIME)
			 &&(gDriver_Info==GET_DRIVER_INFO)
			 &&(gInternal_Flag&DISP_ON_F_1)     )
	{
		//发送记录仪的驾驶员当前信息给显示屏
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
			gDriver_Info=WAIT_SEND_DRIVER_INFO;
		}
	}
	//4，如果等待显示屏幕接收到驾驶员信息超时间无响应，则应该重新发送
	else if(   (gGet_Driver_Info_Count>WAIT_DISPINFO_TIME)
			 &&(gDriver_Info==WAIT_SEND_DRIVER_INFO)	 )
	{
		gGet_Driver_Info_Count=0;
		gDriver_Info=GET_DRIVER_INFO;
	}
	*/
	/********************************************************\
	A15,判断接收记录仪是否长时间没有向主控发送数据
	\********************************************************/
	if( gRecord_Data_Timer > OVER_RECORD_DATA_TIME )
	{
		//超时，如果记录仪显示为正常状态，则认为记录仪故障
		if( (gStatus2_Flag&RECORD_VAILD_F_1)==0 )
		{
			gStatus2_Flag|=RECORD_VAILD_F_1;
			gRecord_Status=0;
		}
	}
	/********************************************************\
	A16,每间隔12个小时，需要重新启动程序一次
		这部分的功能已经转移到中断中去执行
	\********************************************************/
	/********************************************************\
	A17;如果需要向记录仪发送设置间隔距离的是否成功，如果成功收到记录仪
	的响应，则不需要进行此操作，如果超过5秒仍然未收到响应，则间隔发送设置命令
	连续发送5次，如果仍然无响应，则停止发送
	\********************************************************/
	if(   ((gPublic_Flag&SET_DISTANCE_OK_F_1)==0)
		&&(gSet_Distance_Info_Count>WAIT_DISPINFO_TIME)
		&&(gStatus==NORMAL_TYPE)
		&&(gSend_Record_Count<5) )
	{
		gSend_Record_Count++;
    	Send_COM1_Byte(gStatus);
    	Check_Record = gStatus;
    	Send_COM1_Byte(0xAA);
    	Check_Record ^= 0xAA;
    	Send_COM1_Byte(0x75);
    	Check_Record ^= 0x75;
    	Send_COM1_Byte(K_SET_INTERDISTANCE);
    	Check_Record ^= K_SET_INTERDISTANCE;
    	Send_COM1_Byte(0x00);
    	Check_Record ^= 0;
    	Send_COM1_Byte(0x02);							//长度
    	Check_Record ^= 0x02;
    	Send_COM1_Byte(0x00);							//预留
    	Check_Record ^= 0;
    	Send_COM1_Byte(gCycle_Distance/0x100);			//距离值的高位字节
    	Check_Record ^= gCycle_Distance/0x100;
    	Send_COM1_Byte(gCycle_Distance%0x100);			//距离值的低位字节
    	Check_Record ^= gCycle_Distance%0x100;
    	Send_COM1_Byte(Check_Record);					//校验核
    	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);		//包尾
    	gSet_Distance_Info_Count=0;
	}
	/********************************************************\
	A18;判断是否向显示屏发送显示屏上行信息的响应,
	\********************************************************/
	if(gPublic_Flag&DISP_SEND_INFO_F_1)
	{
		i=SRAM_Read(OTHER_SMS_2);
		if(i==VAILD_2);
		else
		{
			gPublic_Flag &= ~DISP_SEND_INFO_F_1;
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
				gDisp_Buffer[gDisp_Buffer_Point]=COMMAND_OK;
				gDisp_Buffer_Point++;
				gDisp_Buffer[gDisp_Buffer_Point]='$';
				gDisp_Buffer_Point++;
				gDisp_Buffer[gDisp_Buffer_Point]='&';
				gDisp_Buffer_Point++;
			}
		}
	}
	/********************************************************\
	B1;关于判断ACC检测和车载的空载/重载方面的程序检测

	补充说明：
	1， Test_Status_Step=0的时候检测ACC的检测线的状态，分为两种情况，如果此时ACC检测线处于开启状态标志或者关闭状态的标志，
		则需要检测ACC检测线的实际状态量是否发生变化，如果没有，则直接设置Test_Status_Step=2，如果有则设置Test_Status_Step=1。
	2，Test_Status_Step=1的时候，则说明检测到ACC的检测线实际状态量有变化，则在这个步骤中检测状态量的变化是否持续符合条件的时间
		如果检测符合时间条件，则改变ACC的状态量标志，设置Test_Status_Step=0;
	3，Test_Status_Step=2的时候，则说明ACC的检测无变化，则进行车载的空载/重载线的状态检测，如果分为两种情况如果此时检测空载/重载是处于空或者
		重，则需要检测空载/重载线的实际状态量是否改变，如过检测到有改变则设置Test_Status_Step=3.否则设置Test_Status_Step=0
	4，Test_Status_Step=3的时候，则说明空载/重载线检测到有所变化，则在这个步骤中检测状态量的变化持续时间是否符合要求，如果符合要求
		则改变空载/重载线的状态量的标志。同时设置Test_Status_Step=0
	\********************************************************/
	if(Test_Status_Step==0)
	{
		//1，如果ACC检测线开启标志的状态下，检测是否ACC检测线关闭
		if(gStatus1_Flag&ACC_ON_F_1)
		{
			if(P3IN&ACC_TEST)
			{
				gTest_Status_Timer=0;
				Test_Status_Step=1;
			}
		}
		//2，如果已经存在ACC线关闭标志的状态下，检测是否ACC检测线开启
		else
		{
			if( (P3IN&ACC_TEST)==0)
			{
				gTest_Status_Timer=0;
				Test_Status_Step=2;
			}
		}
	}
	else if(Test_Status_Step==1)
	{
		if( (P3IN&ACC_TEST)==0 )
		{
			gTest_Status_Timer=0;
			Test_Status_Step=0;
		}
		//判断延迟时间内检测到，则认为ACC关闭
		else if(gTest_Status_Timer>TEST_ACC_TIME)
		{
			gStatus1_Flag&=~ACC_ON_F_1;
			gTest_Status_Timer=0;
			Test_Status_Step=0;
		}
	}
	else if(Test_Status_Step==2)
	{
		if(P3IN&ACC_TEST)
		{
			gTest_Status_Timer=0;
			Test_Status_Step=0;
		}
		//判断延迟时间内检测到，则认为ACC开启
		else if(gTest_Status_Timer>TEST_ACC_TIME)
		{
			gStatus1_Flag|=ACC_ON_F_1;
			gTest_Status_Timer=0;
			Test_Status_Step=0;
		}
	}
	else
	{
		gTest_Status_Timer=0;
		Test_Status_Step=0;
	}
	/********************************************************\
	B3;关于判断是否被劫报警方面的程序处理
	\********************************************************/
	if(Test_Alarm_Step==0)
	{
		if((P2IN&ARALM)==0 )
		{
			Test_Alarm_Step=1;
			gTest_Alarm_Timer=0;
		}
	}
	else if(Test_Alarm_Step==1)
	{
		if(P2IN&ARALM)
		{
			Test_Alarm_Step=0;
			gTest_Alarm_Timer=0;
		}
		else if(gTest_Alarm_Timer>=ROB_TIME)
		{
			if(gM22_Status==GSM_WORK)	gAlarm_Count=ALARM_COUNT;
			if( gAlarm_Type!=ROB_ALARM )
			{
				gAlarm_Type=ROB_ALARM;
				gStatus2_Flag|=ROB_ALARM_F_1;
				#ifdef Debug_GSM_COM1
				Send_COM1_Byte('S');Send_COM1_Byte('O');Send_COM1_Byte('S');
				Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
				#endif
				gCycle_Alarm=ALARM_CIRCLE_TIME;
				gCycle_Send_Status=ALARM_DATA;
				//被劫报警的上报的处理方式
				//1,组织数据包
				gCircle_Timer=0;
				Circle_Data_TCP(ALARM_UP,gSMS_ACK[ALARM_ACK]);
				gInternal_Flag|=SEND_CIRCLE_F_1;
				Judge_SMS_Way();
			}
			Test_Alarm_Step=0;
			gTest_Alarm_Timer=0;
		}
	}
	else
	{
		Test_Alarm_Step=0;
		gTest_Alarm_Timer=0;
	}
	/********************************************************\
	B4;判断是否有报警报警信息的发送(暂时增加一个条件)
	\********************************************************/
	if(   (gAlarm_Type!=NOP_ALARM)
	    &&(gCycle_Send_Status==ALARM_DATA)
	    &&(gCycle_Alarm>4) )
	{
		if( gCircle_Timer>gCycle_Alarm )
		{
			gCircle_Timer=0;
			Circle_Data_TCP(ALARM_UP,gSMS_ACK[ALARM_ACK]);
			//如果属于超速报警,则同时发送一个信息给显示终端
			if(   (gAlarm_Type==OVER_AREA_ALARM)		//区域报警
				||(gAlarm_Type==OVER_SPEED_ALARM)		//超速报警
				||(gAlarm_Type==OVER_TIRE_ALARM)		//疲劳驾驶报警
				||(gAlarm_Type==OVER_LINE_ALARM)	)	//线路偏移报警
			{
				Send_Over_Alarm_to_Disp(gAlarm_Type);
			}
			gInternal_Flag|=SEND_CIRCLE_F_1;
			if(gM22_Status==GSM_WORK)
			{
				if(gAlarm_Count==0)
				{
					gInternal_Flag&=~SEND_CIRCLE_F_1;
					gInternal_Flag&=~ALLOW_DATA_F_1;
					gCircle_Buffer_Point=0;
				}
				else
				{
					gAlarm_Count--;
					gInternal_Flag|=ALLOW_DATA_F_1;
				}
			}
		}
	}
	/********************************************************\
	B4-1;判断是否处于重点监控的传输方式
	\********************************************************/
	if( gCycle_Send_Status==WATCH_DATA )
	{
		//1，如果处于按定时间传输数据的方式
		if(gWatch_Type==WATCH_TYPE_TIME)
		{
			if(gWatch_Time==0)
			{
				gWatch_Count=0;gWatch_Time=0;
				gCycle_Send_Status=NOP_DATA;
				gWatch_Type=WATCH_TYPE_NULL;
				Load_Buffer1_Echo(ASK_FREEWATCH_UP,gGeneral_Buffer[5]);
			}
			else
			{
				TCP_WORK_Data(gWatch_Circle,VEHICLE_WATCHECHO_UP);
				if(gM22_Status==GSM_WORK)	gInternal_Flag|=ALLOW_DATA_F_1;
			}
		}
		else if(gWatch_Type==WATCH_TYPE_COUNT)
		{
			if(gWatch_Count==0)
			{
				gWatch_Count=0;gWatch_Time=0;
				gCycle_Send_Status=NOP_DATA;
				gWatch_Type=WATCH_TYPE_NULL;
				Load_Buffer1_Echo(ASK_FREEWATCH_UP,gGeneral_Buffer[5]);
			}
			else
			{
				TCP_WORK_Data(gWatch_Circle,VEHICLE_WATCHECHO_UP);
				if(   (gInternal_Flag&SEND_CIRCLE_F_1)
					&&( gWatch_Count>0 ) )
				{
					if(gM22_Status==GSM_WORK)	gInternal_Flag|=ALLOW_DATA_F_1;
				}
			}
		}
	}

	/********************************************************\
	B5;关于区域报警的检测程序
		说明：区域报警方式和超速度报警方式一样，每隔5秒种进行一次区域报警的检测
		累计5次，检测有效，则认为属于区域报警，一旦检测无满足条件的区域
		则立刻停止区域报警。(所有的检测条件必须是在GPS数据有效的状态下判断)
	\********************************************************/
	if( (gTimer_Area_Test>INTER_ATRA_TEST)&&(gGeneral_Flag&GPS_VALID_F_1)&&(gArea_Par!=0) )
	{
		gTimer_Area_Test=0;
		Area_All=0;
		Check_Result=0;
//		Send_COM1_Byte('(');
		for(i=1;i<=4;i++)
		{
			//依次检查4个区域的数据
			Check_Result=Check_Area_Alarm(i);
			//Send_COM1_Byte(ASCII(Check_Result));
			//返回1，则说明当前点在禁入区域内
			if(Check_Result==1)
			{
				//满足条件，则跳出此循环
				Area_All=1;break;
			}
			//返回2，则说明当前点的在禁出区域内
			else if(Check_Result==2)
			{
				Area_All=0;
				//go on
			}
			//返回3，则说明当前点不在禁入区域内
			else if(Check_Result==3)
			{
				//go on
			}
			//返回4，则说明当前点不在禁出区域内
			else if(Check_Result==4)
			{
				//需要判断完所有的禁出区域，如果当前点只要在一个禁出区域内，则认为无满足条件
				Area_Out_Num++;
			}
			//返回0。则说明当前判断区域数据无效
			else if(Check_Result==0)
			{
				//go on
			}
		}
		/*
		Send_COM1_Byte(')');
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		*/
		SRAM_Init();
		j=SRAM_Read(SAREA_OUT_NUM);
		if( (Area_All==0)&&(Area_Out_Num>=j)&&(j<=4)&&(j>0))
		{
			Area_All=1;
		}
		else if( (Area_All==0)&&(gAlarm_Type==OVER_AREA_ALARM))
		{
			gAlarm_Type=NOP_ALARM;
			gCycle_Send_Status=NOP_DATA;
			gAlarm_Count=0;
			gCircle_Timer=0;
			gArea_No_Count=0;
			Circle_Data_TCP(VEHICLE_RUNDATA_UP,0);
			gInternal_Flag|=SEND_CIRCLE_F_1;
 			Judge_SMS_Way();
		}
		if( (Area_All==1)&&(gAlarm_Type==NOP_ALARM) )
		{
			gArea_No_Count++;
		}
	}
	if( (gArea_No_Count>5)&&(gAlarm_Type==NOP_ALARM) )
	{
		gArea_No_Count=0;
		gAlarm_Type = OVER_AREA_ALARM;
		gCycle_Send_Status=ALARM_DATA;
		gCircle_Timer=0;
		gCycle_Alarm=ALARM_CIRCLE_TIME;
		gSMS_ACK[ALARM_ACK]=0;
		Circle_Data_TCP(ALARM_UP,gSMS_ACK[ALARM_ACK]);
		Send_Over_Alarm_to_Disp(gAlarm_Type);
		gInternal_Flag|=SEND_CIRCLE_F_1;
		if(gM22_Status==GSM_WORK)
		{
			gAlarm_Count=ALARM_COUNT;
		}
		Judge_SMS_Way();
	}
	/********************************************************\
	B6;与行车轨迹相关的一些处理流程:
		1,先判断IP地址和开关量
		 1-1：如果IP地址有效果，且开关量处于开启状态
		 1-2：IP地址无效或者开关量未开启
		 	1-1-1：如果gCycle_GSM>4且为行车轨迹状态
	\********************************************************/
	//IP地址有效，开关量开启
	if( (gGeneral_Flag&TCP_IP_VALID_1)&&((gON_OFF_Temp0&TCP_LAND_ON_OFF_1)==0) )
	{
		//如果处于GSM工作方式
		if(gM22_Status==GSM_WORK)
		{
			//如果间隔属于大于20分钟
			if( (gCycle_TCP>=TCP_TRACK_MINTIME)&&(gCycle_Send_Status==TRACK_DATA_TIME) )
			{
				TCP_WORK_Data(gCycle_TCP,VEHICLE_RUNDATA_UP);
			}
			//如果间隔属于小于20分钟
			else if( (gCycle_TCP>4)&&(gCycle_TCP<TCP_TRACK_MINTIME)&&(gCycle_Send_Status==TRACK_DATA_TIME) )
			{
				TCP_WORK_Data(SMS_DATA_TIME,VEHICLE_RUNDATA_UP);
			}
		}
		//如果处于TCP的工作方式
		else if( gM22_Status==TCP_ONLINE_WORK )
		{
			if(gCycle_Send_Status==TRACK_DATA_TIME)
			{
				TCP_WORK_Data(gCycle_TCP,VEHICLE_RUNDATA_UP);
			}
		}
	}
	//如果IP地址无效，或者开关量关闭
	else
	{
		if( (gCycle_GSM>4)
			&&(gCycle_Send_Status==TRACK_DATA_TIME)
			&&(gM22_Status==GSM_WORK) )
		{
			GSM_WORK_Data();
		}
	}
	/********************************************************\
	\********************************************************/
	/********************************************************\
	B7;	关于线路报警的程序部分
		说明：
	\********************************************************/
	if(   (gStatus3_Flag&LINE_LIMIT_F_1)
		&&(gGeneral_Flag&GPS_VALID_F_1)
		&&(gLine_Num>0)&&(gLine_Num<=60) )
	{
		if(gLine_Timer>8)
		{
			gLine_Timer=0;
			gCommon_Flag&=~LINE_OUT_F_1;
			for(i=0;i<gLine_Num-1;i++)
			{
				Check_Result=Check_Line_Data(i);
				Clear_Exter_WatchDog();
				//Send_COM1_Byte(ASCII(Check_Result%10));
				//计算垂直点在当前线段内，属于需要统计计算距离
				//1,返回0，则表示异常
				if(Check_Result==0);
				//2,返回1，则表示垂直点没落在当前线段内
				else if(Check_Result==1);
				//3,返回2，则说明垂直点落在当前线段内，但距离大于最大限制的值
				else if(Check_Result==2)
				{
					gOver_Line_Count++;
					gCommon_Flag|=LINE_OUT_F_1;
				}
				//4,返回3，说明不满足偏离线路的条件
				else if(Check_Result==3)
				{
					gOver_Line_Count=0;
					gCommon_Flag|=LINE_OUT_F_1;
					if(gAlarm_Type==OVER_LINE_ALARM)
					{
						gCommon_Flag|=STOP_OVERLINE_F_1;
					}
					break;
				}
			}
			if(   (i==gLine_Num-1)
				&&((gCommon_Flag&LINE_OUT_F_1)==0) )
			{
				gOver_Line_Count++;
			}
		}
	}
	if(gOver_Line_Count>2)
	{
		//满足条件，则产生偏离报警
		gOver_Line_Count=0;
		if(gAlarm_Type==NOP_ALARM)
		{
			gAlarm_Type = OVER_LINE_ALARM;
			gCycle_Send_Status=ALARM_DATA;
			gCircle_Timer=0;
			gCycle_Alarm=ALARM_CIRCLE_TIME;
			gSMS_ACK[ALARM_ACK]=0;
			Circle_Data_TCP(ALARM_UP,gSMS_ACK[ALARM_ACK]);
			Send_Over_Alarm_to_Disp(gAlarm_Type);
			gInternal_Flag|=SEND_CIRCLE_F_1;
			if(gM22_Status==GSM_WORK)
			{
				gAlarm_Count=ALARM_COUNT;
			}
			Judge_SMS_Way();
		}
	}
	else if( (gAlarm_Type==OVER_LINE_ALARM)&&(gCommon_Flag&STOP_OVERLINE_F_1) )
	{
		gAlarm_Type=NOP_ALARM;
		gCycle_Send_Status=NOP_DATA;
		gAlarm_Count=0;
		gStatus2_Flag&=~ROB_ALARM_F_1;
		gCommon_Flag&=~STOP_OVERLINE_F_1;
		gCircle_Timer=0;
		gOver_Line_Count=0;
		Circle_Data_TCP(VEHICLE_RUNDATA_UP,0);
		gInternal_Flag|=SEND_CIRCLE_F_1;
		Judge_SMS_Way();
	}
}
/********************************************************\
		下面是程序 的子模块部分
\********************************************************/
/********************************************************\
*	函数名：Circle_Data_TCP
	作用域：本地文件调用
*	功能：  在TCP的工作方式下，如果是报警，或者行车轨迹，或者监控数据，
			组织到gCircle_Buffer[]缓冲中
*	参数：

*	返回值：

*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/

void Circle_Data_TCP(unsigned char nCommand,unsigned char nACK )
{
	unsigned char nInput=29+4;
	unsigned int iCheckOut=0;
	unsigned char nFlag=0;
	nFlag=gAlarm_Type;

	gCircle_Buffer_Point=0;
	gCircle_Buffer[gCircle_Buffer_Point]=SOH;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=nCommand;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=(char)(~nCommand);
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=nInput-4;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=nACK;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gKind;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gGroup;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gVID/0x100;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gVID%0x100;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;

	gCircle_Buffer[gCircle_Buffer_Point]=gLongitude/0x1000000;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gLongitude%0x1000000/0x10000;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gLongitude%0x10000/0x100;

	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gLongitude%0x100;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;

	gCircle_Buffer[gCircle_Buffer_Point]=gLatitude/0x1000000;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gLatitude%0x1000000/0x10000;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gLatitude%0x10000/0x100;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gLatitude%0x100;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;

	gCircle_Buffer[gCircle_Buffer_Point]=gSpeed;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gAzimuth;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;

	if(gGeneral_Flag&GPS_VALID_F_1)		nFlag|=FLAG_GPSVAILD_1;
	else								nFlag&=~FLAG_GPSVAILD_1;

	gCircle_Buffer[gCircle_Buffer_Point]=nFlag;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;

	gCircle_Buffer[gCircle_Buffer_Point]=gYear;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gMonth;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gDate;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gHour;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gMinute;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gSecond;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;

	//增加4个状态量的发送
	gCircle_Buffer[gCircle_Buffer_Point]=0;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gStatus3_Flag;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gStatus2_Flag;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gStatus1_Flag;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;

	//增加long int型的里程数据
	/*
	gCircle_Buffer[gCircle_Buffer_Point]=gALL_Distance/0x1000000;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gALL_Distance%0x1000000/0x10000;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gALL_Distance%0x10000/0x100;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gALL_Distance%0x100;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	*/
	//==================================================
	gCircle_Buffer[gCircle_Buffer_Point]=iCheckOut/0x100;
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=iCheckOut%0x100;
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=EOT;
	gCircle_Buffer_Point++;
	//gCircle_Buffer_Point的值是数据区的长度
}

/********************************************************\
*	函数名：Circle_Data_GSM
	作用域：本地文件调用
*	功能：  在GSM的工作方式下，如果是报警，或者行车轨迹，或者监控数据，
			组织到gCircle_Buffer[]缓冲中
*	参数：

*	返回值：

*	创建人：
*
*	修改历史：（每条详述）
AT+CMGS=045
> 001100|0D 91683196818735F4 000400 1D FF43BC19010101001018739A9A04D63C2200008003093010164306460D

\********************************************************/
unsigned int Circle_Data_GSM( unsigned char nCount,unsigned char nCommand,unsigned char nACK,unsigned int iCheckOutTemp )
{
	unsigned char nInput;
	unsigned int  iCheckOut;
	unsigned int  i;
	iCheckOut=iCheckOutTemp;
	nInput=12+4+17*2;
	if(nCount==1)
	{
		gCircle_Buffer_Point=0;
		gCircle_Buffer[gCircle_Buffer_Point]=SOH;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=nCommand;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=(char)(~nCommand);
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=nInput-4;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=nACK;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=gKind;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=gGroup;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=gVID/0x100;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=gVID%0x100;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		i=Circle_Data_GSM_Sub(iCheckOut);
		return(i);
	}
	else if(nCount==2)
	{
		i=Circle_Data_GSM_Sub(iCheckOut);
		return(i);
	}
	/*
	else if(nCount==3)
	{
		i=Circle_Data_GSM_Sub(iCheckOut);
		return(i);
	}
	else if(nCount==4)
	{
		i=Circle_Data_GSM_Sub(iCheckOut);
		return(i);
	}
	*/
	else if(nCount==3)
	{
		i=Circle_Data_GSM_Sub(iCheckOut);
		return(i);
	}
	else if(nCount==4)
	{
		/////////////////////////////////////////////////
		i=Circle_Data_GSM_Sub(iCheckOut);
		//增加4个车载终端的状态量
		gCircle_Buffer[gCircle_Buffer_Point]=0;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=gStatus3_Flag;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=gStatus2_Flag;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=gStatus1_Flag;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		//增加long int型的里程数据
		/*
		gCircle_Buffer[gCircle_Buffer_Point]=gALL_Distance/0x1000000;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=gALL_Distance%0x1000000/0x10000;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=gALL_Distance%0x10000/0x100;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=gALL_Distance%0x100;
		iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
		gCircle_Buffer_Point++;
		*/
		//////////////////////////////////////////////
		gCircle_Buffer[gCircle_Buffer_Point]=i/0x100;
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=i%0x100;
		gCircle_Buffer_Point++;
		gCircle_Buffer[gCircle_Buffer_Point]=EOT;
		gCircle_Buffer_Point++;
		return(0);
	}
	else
	{
	  return(0);
	}
}
/********************************************************\
*	函数名：Circle_Data_GSM_Sub
	作用域：本地文件调用
*	功能：  在GSM的工作方式下，如果是报警，或者行车轨迹，或者监控数据，
			组织到gCircle_Buffer[]缓冲中
*	参数：
*	返回值：
*	创建人：
*	修改历史：（每条详述）
\********************************************************/

unsigned int Circle_Data_GSM_Sub(unsigned int iCheckOutTemp)
{
	unsigned int iCheckOut;
	unsigned char nFlag;
	nFlag=gAlarm_Type;
	iCheckOut=iCheckOutTemp;
	gCircle_Buffer[gCircle_Buffer_Point]=gLongitude/0x1000000;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gLongitude%0x1000000/0x10000;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gLongitude%0x10000/0x100;

	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gLongitude%0x100;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;

	gCircle_Buffer[gCircle_Buffer_Point]=gLatitude/0x1000000;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gLatitude%0x1000000/0x10000;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gLatitude%0x10000/0x100;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gLatitude%0x100;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;

	gCircle_Buffer[gCircle_Buffer_Point]=gSpeed;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gAzimuth;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;

	if(gGeneral_Flag&GPS_VALID_F_1)		nFlag|=FLAG_GPSVAILD_1;
	else								nFlag&=~FLAG_GPSVAILD_1;

	gCircle_Buffer[gCircle_Buffer_Point]=nFlag;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gYear;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gMonth;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gDate;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gHour;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gMinute;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	gCircle_Buffer[gCircle_Buffer_Point]=gSecond;
	iCheckOut += gCircle_Buffer[gCircle_Buffer_Point];
	gCircle_Buffer_Point++;
	return(iCheckOut);
}

/********************************************************\
*	函数名：Dichotomy_PassData_First
	作用域：本地文件调用
*	功能：  二分法寻找历史轨迹的第一个满足条件的扇区
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned int Dichotomy_PassData_First(unsigned int iFirst_Page,unsigned int iEnd_Page)
{
	unsigned int iMiddle_Page;
	iMiddle_Page=(iFirst_Page+iEnd_Page)/2;
	return(iMiddle_Page);
}

/********************************************************\
*	函数名：GSM_WORK_Data
	作用域：
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void GSM_WORK_Data(void)
{
	if(gCircle_Timer>0)
	{
		if( (gGeneral_Flag&GPS_VALID_F_1)&&( (gInternal_Flag&LOAD_ONEVALID_F_1)==0) )
		{
			gInternal_Flag|=LOAD_ONEVALID_F_1;
			//在0--gCycle_GSM内判断到一个有效的数据
			gGSM_Work_Count++;
			gCheck_Data += Circle_Data_GSM(gGSM_Work_Count,VEHICLE_RUNDATA_UP,0,gCheck_Data);
			if(gGSM_Work_Count>=4)
			{
				gInternal_Flag|=SEND_CIRCLE_F_1;gGSM_Work_Count=0;gCheck_Data=0;
				Judge_SMS_Way();
			}
		}
		else if( gCircle_Timer>gCycle_GSM )
		{
			if( gInternal_Flag&LOAD_ONEVALID_F_1 )
			{
				gCircle_Timer=0;
				gInternal_Flag&=~LOAD_ONEVALID_F_1;
			}
			else
			{
				gCircle_Timer=0;
				gInternal_Flag&=~LOAD_ONEVALID_F_1;
				//在0--gCycle_GSM内仍然未判断到一个有效的数据，则将这个作为一个点存储
				gGSM_Work_Count++;
				gCheck_Data += Circle_Data_GSM(gGSM_Work_Count,VEHICLE_RUNDATA_UP,0,gCheck_Data);
				if(gGSM_Work_Count>=4)
				{
					gInternal_Flag|=SEND_CIRCLE_F_1;gGSM_Work_Count=0;gCheck_Data=0;
					Judge_SMS_Way();
				}
			}
		}
	}
}
/********************************************************\
*	函数名：TCP_WORK_Data
	作用域：
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/

void TCP_WORK_Data(unsigned int Timer,unsigned char nCommand)
{
	if(gCircle_Timer>0)
	{
		if( (gGeneral_Flag&GPS_VALID_F_1)&&( (gInternal_Flag&LOAD_ONEVALID_F_1)==0) )
		{
			gInternal_Flag|=LOAD_ONEVALID_F_1;
			//在0--gCycle_TCP内判断到一个有效的数据
			Circle_Data_TCP(nCommand,0);
			gInternal_Flag|=SEND_CIRCLE_F_1;
			Judge_SMS_Way();
		}
		else if( gCircle_Timer>Timer )
		{
			if( gInternal_Flag&LOAD_ONEVALID_F_1 )
			{
				gCircle_Timer=0;
				gInternal_Flag&=~LOAD_ONEVALID_F_1;
			}
			else
			{
				gCircle_Timer=0;
				gInternal_Flag&=~LOAD_ONEVALID_F_1;
				//在0--gCycle_TCP内仍然未判断到一个有效的数据，则将这个作为一个点存储
				Circle_Data_TCP(nCommand,0);
				gInternal_Flag|=SEND_CIRCLE_F_1;
				Judge_SMS_Way();
			}
		}
	}
}

/********************************************************\
*	函数名：Max_Result()
	作用域：
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned long int Max_Result(unsigned long int x,unsigned long int y)
{
	if(x>y)	return(x);
	else	return(y);
}
/********************************************************\
*	函数名：Min_Result()
	作用域：
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned long int Min_Result(unsigned long int x,unsigned long int y)
{
	if(x<y)	return(x);
	else	return(y);
}

/********************************************************\
*	函数名：Account_Mid_Dot()
	作用域：
*	功能：
*	参数：
*	返回值：
*	创建人：
*
\********************************************************/
unsigned char Account_Mid_Dot(double x1,double y1,double x2,double y2)
{
	unsigned char Line_Flag=0;
	double k,b,k1,b1;
	double ak;
	double Max_gLongitude=0,Max_gLatitude=0;
	double Min_gLongitude=0,Min_gLatitude=0;
	double Longitude_Temp=0,Latitude_Temp=0;
	unsigned long int nLength=0;

	//先判断当前点与判断的线段的第一个点的距离,如果此点的距离就不满足偏离条件，则返回5
	//1,计算两点经度差值所表示的实际距离
	nLength=labs(gLatitude-(long int)(y1*3600000) );
	Latitude_Temp=(double)((nLength/1000.0)*DIS_LAT);
	//2,计算两点纬度差值所表示的实际距离
	nLength=labs(gLongitude-(long int)(x1*3600000) );
	Longitude_Temp=(double)((nLength/1000.0)*DIS_LON);

	if( (Longitude_Temp<gMax_Limit_Distance)&&(Latitude_Temp<gMax_Limit_Distance) )
	{
		Latitude_Temp=Latitude_Temp*Latitude_Temp+Longitude_Temp*Longitude_Temp;
		Latitude_Temp=sqrt(Latitude_Temp);
		if(Latitude_Temp<gMax_Limit_Distance)		return(5);
	}
	/////////////////////////////////////////////////////////////////////

	Longitude_Temp=(double)(gLongitude/3600000.0);
	Latitude_Temp=(double)(gLatitude/3600000.0);

	if(x1>=x2)
	{
		Max_gLongitude=x1;Min_gLongitude=x2;
	}
	else
	{
		Max_gLongitude=x2;Min_gLongitude=x1;
	}
	if(y1>=y2)
	{
		Max_gLatitude=y1;Min_gLatitude=y2;
	}
	else
	{
		Max_gLatitude=y2;Min_gLatitude=y1;
	}

	if(   ((Max_gLongitude-Min_gLongitude)<0.0003)
		&&((Max_gLatitude-Min_gLatitude)>0.0003)	 )
	{
		Line_Flag=1;
	}
	else if(   ((Max_gLongitude-Min_gLongitude)>0.0003)
			 &&((Max_gLatitude-Min_gLatitude)<0.0003)	 )
	{
		Line_Flag=2;
	}
	else if(   ((Max_gLongitude-Min_gLongitude)>0.0003)
			 &&((Max_gLatitude-Min_gLatitude)>0.0003)	 )
	{
		k=(y2-y1)/(x2-x1);
		ak=fabs(k);
		if(ak>1000)				Line_Flag=1;	//相当于垂直，相当于x相同
		else if(ak<0.001)		Line_Flag=2;	//相当于平行，相当于y相同
		else					Line_Flag=0;	//可以有计算的斜率
	}
	else	//两个点重合
	{
		return(0);
	}
	/*
	k=(y2-y1)/(x2-x1);
	ak=fabs(k);
	if(ak>100)				Line_Flag=1;	//相当于垂直，相当于x相同
	else if(ak<0.01)		Line_Flag=2;	//相当于平行，相当于y相同
	else					Line_Flag=0;	//可以有计算的斜率
	*/
	//1，如果两个点的纬度数据相同，则判断经度，y相同，判断x
	if(Line_Flag==2)		//gLatitude
	{
		if(Longitude_Temp>Max_gLongitude)		return(1);
		else if(Longitude_Temp<Min_gLongitude)	return(1);
		else
		{
			gLongitude_mid=gLongitude;
			gLatitude_mid=(long int)(3600000*(y1+y2)/2);
			return(2);
		}
	}
	//2，如果两个点的经度数据相同，则判断纬度，x相同，判断y
	else if(Line_Flag==1)	//gLongitude
	{
		if(Latitude_Temp>Max_gLatitude)			return(1);
		else if(Latitude_Temp<Min_gLatitude)	return(1);
		else
		{
			gLongitude_mid=(long int)(3600000*(x1+x2)/2);					//经度,相当于y
			gLatitude_mid=gLatitude;								//纬度,相当于x
			return(3);
		}
	}
	//3,如果可以计算斜率
	else if(Line_Flag==0)
	{
		b=y1-k*x1;
		k1=-1/k;
		b1=(double)(gLatitude/3600000.0)-k1*(double)(gLongitude/3600000.0);//b1=

		Longitude_Temp=(b1-b)/(k-k1);
		Latitude_Temp= k1*Longitude_Temp+b1;

		gLongitude_mid=(long int)(Longitude_Temp*3600000);
		gLatitude_mid=(long int)(Latitude_Temp*3600000);

		if(gLongitude_mid>(long int)(Max_gLongitude*3600000))			return(1);
		else if(gLongitude_mid<(long int)(Min_gLongitude*3600000))		return(1);
		else if(gLatitude_mid>(long int)(Max_gLatitude*3600000))		return(1);
		else if(gLatitude_mid<(long int)(Min_gLatitude*3600000))		return(1);
		else
		{
			return(4);
		}
	}
	return(0);
	//返回0，则属于异常
	//返回1，表示垂线的交点不在当前线段内，则此线段的判断结束，进行下一个线段的判断
	//返回2，垂线的交点在当前线段内，但两个点的经度相同，纬度不同
	//返回3，垂线的交点在当前线段内，但两个点的纬度相同，经度不同
	//返回4，垂线的交点在单前线段内，并且两个点的经度，纬度君不相同
	//返回5，说明当前点离线段的第一个点的距离已经不满足偏离距离的条件
}
/********************************************************\
*	函数名：Check_Sram_AreaData()
	作用域：
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）

\********************************************************/
/*
unsigned char Check_Sram_AreaData(void)
{
	unsigned int i=0;
	SRAM_Init();
	if( (SRAM_Read(SAREA_DATA_FS)==0x55)&&(SRAM_Read(SAREA_DATA_FE)==0xAA) )
	{
		return(1);
	}
	else
	{
		return(0);
	}
}
*/
/********************************************************\
*	函数名：Check_Area_Alarm()
	作用域：
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）

\********************************************************/
unsigned char Check_Area_Alarm(unsigned char Num)
{
	unsigned long int gLongitude_1=0,gLatitude_1=0;
	unsigned long int gLongitude_2=0,gLatitude_2=0;
	unsigned long int Max_gLongitude=0,Max_gLatitude=0;
	unsigned long int Min_gLongitude=0,Min_gLatitude=0;

	unsigned char Area_Flag=0;
	unsigned int Area_Add=0;
	unsigned int Aflag_Add=0;

	Area_Add=SAREA1_LON_1_2+(Num-1)*17;			//区域的首地址
	Aflag_Add=Area_Add+16;						//区域标志的地址

	//先判断对应的Num区域是禁出还是禁入
	SRAM_Init();
	Area_Flag=SRAM_Read(Aflag_Add);
	if( (Area_Flag!=0xaa)&&(Area_Flag!=0x55) )	return(0);
	//2，第一个点的经，纬度
	gLongitude_1 = SRAM_Read(Area_Add);
	gLongitude_1<<=8;
	gLongitude_1 += SRAM_Read(Area_Add+1);
	gLongitude_1<<=8;
	gLongitude_1 += SRAM_Read(Area_Add+2);
	gLongitude_1<<=8;
	gLongitude_1 += SRAM_Read(Area_Add+3);

	gLatitude_1 = SRAM_Read(Area_Add+4);
	gLatitude_1<<=8;
	gLatitude_1 += SRAM_Read(Area_Add+5);
	gLatitude_1<<=8;
	gLatitude_1 += SRAM_Read(Area_Add+6);
	gLatitude_1<<=8;
	gLatitude_1 += SRAM_Read(Area_Add+7);
	//3，第二个点的经，纬度
	gLongitude_2 = SRAM_Read(Area_Add+8);
	gLongitude_2<<=8;
	gLongitude_2 += SRAM_Read(Area_Add+9);
	gLongitude_2<<=8;
	gLongitude_2 += SRAM_Read(Area_Add+10);
	gLongitude_2<<=8;
	gLongitude_2 += SRAM_Read(Area_Add+11);

	gLatitude_2 = SRAM_Read(Area_Add+12);
	gLatitude_2<<=8;
	gLatitude_2 += SRAM_Read(Area_Add+13);
	gLatitude_2<<=8;
	gLatitude_2 += SRAM_Read(Area_Add+14);
	gLatitude_2<<=8;
	gLatitude_2 += SRAM_Read(Area_Add+15);

	Max_gLongitude=Max_Result(gLongitude_1,gLongitude_2);
	Max_gLatitude=Max_Result(gLatitude_1,gLatitude_2);
	Min_gLongitude=Min_Result(gLongitude_1,gLongitude_2);
	Min_gLatitude=Min_Result(gLatitude_1,gLatitude_2);

	//判断对应点在这个区域范围内
	if(   (gLongitude>Min_gLongitude)&&(gLongitude<Max_gLongitude)
		&&(gLatitude>Min_gLatitude)&&(gLatitude<Max_gLatitude)	)
	{
		//当前点在此区域范围内
		//1,如果此区域属于禁入标志
		if(Area_Flag==0xaa)			return(1);	//可以产生报警
		//2,如果此区域属于禁出标志，
		else if(Area_Flag==0x55)	return(2);	//
	}
	//判断这个点对应不在这个区域内
	else
	{
		//当前点不在这个区域范围内
		//1,如果此区域属于禁入标志，
		if(Area_Flag==0xaa)			return(3);	//此区域PASS
		//2,如果此区域属于禁出标志，则返回2
		else if(Area_Flag==0x55)	return(4);	//此区域计数
	}
	return(0);
}
/********************************************************\
*	函数名：Account_Distance()
	作用域：
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	统计计算告诉了两个精度，两个纬度，计算出，中间点的精度，纬度值
\********************************************************/

/********************************************************\
*	函数名：Send_Over_Speed_to_Disp
	作用域：
*	功能：  定期送给显示终端超速报警信息
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Send_Over_Alarm_to_Disp(unsigned char Alarm_Style)
{
	if(gDisp_Buffer_Point+8+2<=sizeof(gDisp_Buffer))
	{
		gDisp_Buffer[gDisp_Buffer_Point]=8+2;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=0;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='@';
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='%';
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=8;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=DISP_MAIN_ARALM_DOWN;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=Alarm_Style;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=0;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='$';
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='&';
		gDisp_Buffer_Point++;
	}
}
/********************************************************\
*	函数名：Check_Line_Data
	作用域：
*	功能：  定期送给显示终端超速报警信息
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	S_Num_ID的值的范围为1----61
\********************************************************/
unsigned char  Check_Line_Data(unsigned char S_Num_ID)
{
	unsigned long int gLatitude_1,gLatitude_2;
	unsigned long int gLongitude_1,gLongitude_2;
	unsigned long int nLength=0;
	unsigned char nR_Value=0;
	double	gLat_1,gLon_1,gLat_2,gLon_2;
	unsigned int SLine_Address;
	double 	Distance_LAT=0,Distance_LON=0;

	//先根据ID号计算相邻的两个点的第一个点的在SRAM中的位置
	SLine_Address=SLINE_DATA_1+8*S_Num_ID;
	//依次读取数据
	SRAM_Init();
	//第一个点的经，纬度
	gLongitude_1 = SRAM_Read(SLine_Address);
	gLongitude_1<<=8;
	gLongitude_1 += SRAM_Read(SLine_Address+1);
	gLongitude_1<<=8;
	gLongitude_1 += SRAM_Read(SLine_Address+2);
	gLongitude_1<<=8;
	gLongitude_1 += SRAM_Read(SLine_Address+3);

	gLatitude_1 = SRAM_Read(SLine_Address+4);
	gLatitude_1<<=8;
	gLatitude_1 += SRAM_Read(SLine_Address+5);
	gLatitude_1<<=8;
	gLatitude_1 += SRAM_Read(SLine_Address+6);
	gLatitude_1<<=8;
	gLatitude_1 += SRAM_Read(SLine_Address+7);
	//第二个点的经度，纬度
	gLongitude_2 = SRAM_Read(SLine_Address+8);
	gLongitude_2<<=8;
	gLongitude_2 += SRAM_Read(SLine_Address+9);
	gLongitude_2<<=8;
	gLongitude_2 += SRAM_Read(SLine_Address+10);
	gLongitude_2<<=8;
	gLongitude_2 += SRAM_Read(SLine_Address+11);

	gLatitude_2 = SRAM_Read(SLine_Address+12);
	gLatitude_2<<=8;
	gLatitude_2 += SRAM_Read(SLine_Address+13);
	gLatitude_2<<=8;
	gLatitude_2 += SRAM_Read(SLine_Address+14);
	gLatitude_2<<=8;
	gLatitude_2 += SRAM_Read(SLine_Address+15);
//==============
//==============

	//开始计算两个点的相关数据
	gLat_1=(double)(gLatitude_1/3600000.0);
	gLon_1=(double)(gLongitude_1/3600000.0);
	gLat_2=(double)(gLatitude_2/3600000.0);
	gLon_2=(double)(gLongitude_2/3600000.0);

	nR_Value=Account_Mid_Dot(gLon_1,gLat_1,gLon_2,gLat_2);
	//1,异常
	if(nR_Value==0)			return(0);
	//2,垂直点落在线段外
	else if(nR_Value==1)	return(1);
	//3,当前离第一个判断点的距离就不满足偏离条件
	else if(nR_Value==5)	return(3);
	//4,有需要进行计算距离的，计算的垂直的交点在当前线段内
	else
	{
		//1,计算两点经度差值所表示的实际距离
		nLength=labs(gLatitude_mid-gLatitude);
		Distance_LAT=(double)((nLength/1000.0)*DIS_LAT);
		//2,计算两点纬度差值所表示的实际距离
		nLength=labs(gLongitude_mid-gLongitude);
		Distance_LON=(double)((nLength/1000.0)*DIS_LON);

		//A：需要计算两个点经度相同
		if(nR_Value==2)
		{
			if(Distance_LAT>gMax_Limit_Distance)	return(2);
			else									return(3);
		}
		//B：需要计算两个点纬度相同
		else if(nR_Value==3)
		{
			if(Distance_LON>gMax_Limit_Distance)	return(2);
			else									return(3);
		}
		//C：需要计算两个点的经度，纬度均不相同
		else if(nR_Value==4)
		{
			if( (Distance_LAT>gMax_Limit_Distance)||(Distance_LON>gMax_Limit_Distance) )
			{
				return(2);
			}
			else
			{
				Distance_LAT=Distance_LAT*Distance_LAT+Distance_LON*Distance_LON;
				Distance_LAT=sqrt(Distance_LAT);
				if(Distance_LAT>gMax_Limit_Distance)	return(2);
				else
				{
					return(3);
				}
				//计算两点的距离
			}
		}
		return(0);
	}
}