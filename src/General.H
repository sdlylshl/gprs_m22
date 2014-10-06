

#ifndef __General_H
#define __General_H

extern unsigned long int gLongitude_mid,gLatitude_mid;
//extern unsigned char gFlash_W_Word_Item,gFlash_R_Word_Item;
//extern unsigned int  gPassTrack_Store_Sector;
//extern unsigned int  gPassTrack_Read_Sector;
//extern unsigned char gPassTrack_Store_Item;
extern unsigned char gON_OFF_Temp0;
extern unsigned char gON_OFF_Temp1;
extern unsigned char gON_OFF_Temp2;			//作为存储系统开关量的三个变量
extern unsigned char gTCP_Status_Temp;
extern unsigned int  gTimer;
extern unsigned int  gInternal_Flag; 		//内部的参数变量，用来存储记录复位G20的标志，复位GPS的标志，G20掉电复位的标志，
extern unsigned int  gGeneral_Flag;			//用来记录内部其他一些需要位操作的变量的标志
extern unsigned int  gOther_Flag;
extern unsigned int  gPublic_Flag;
extern unsigned char gType,gKind,gGroup;
extern unsigned int  gVID;
extern unsigned char gPhone_Status;
extern unsigned char gReset_M22_On_Setp;
extern unsigned char gOFF_Power_S;			//属于在控制2576掉电的步骤变量
extern unsigned char gPhone_Step;
extern unsigned char gSMS_IN_Step;
extern unsigned char gSMS_OUT_Step;
extern unsigned char gLand_GPRS_Step;
extern unsigned char gGPRS_Data_Step;
extern unsigned char gGSM_OTHER_Step;
extern unsigned char gGeneral_Buffer[320];
extern unsigned char gSMS_In_Lenth;
extern unsigned char gReceive0_GSM_Buffer[320];
extern unsigned int  gReceive0_GSM_Buffer_End;
extern unsigned int  gReceive0_GSM_Buffer_Point;
extern unsigned int  gReceive0_GSM_Buffer_Move;
extern unsigned int  gTimer_GSM_AT;
extern unsigned char gGSM_Oper_Type;
extern unsigned char  gGPS_No_Output_Timer;

extern unsigned char gTimer_CSQ;
extern unsigned char gTimer_Area_Test;
extern unsigned char gSignal;					//信号强度
extern unsigned char gVolume;					//手柄音量的大小数值

extern unsigned char gSMS_ACK[5];				//存储相关的ACK数据，初始化的时候值均为0
extern unsigned char gDTMF_SAVE[20];
extern unsigned int  gTCP_FF0D_Send_Timer;		//作为间隔定期发送检验GPRS网络的GPRS数据
extern unsigned long int gIP_Port_Num;			//作为存储在Flash中存储的IP的端口号
extern unsigned long int ACC_OFF_Timer;			//作为统计ACC关闭的时间检测
extern unsigned long int gLongitude,gLatitude;
extern unsigned char gYear,gMonth,gDate,gHour,gMinute,gSecond;

extern unsigned char gSpeed,gAzimuth;
extern unsigned char gWatch_Type;
extern unsigned int  gADCvalue_A1;				//作为ADC转换通道A1每次转换存储的数据变量

extern unsigned char gMAX_Speed;

extern unsigned char gGPS_Receive_Over;			//作为GPS的接收数据在中断中的处理，接收到一个字节后将此计数清零
												//在GPS的主处理循环中，如果判断此记数大于某一个值，则判断接收的GPS数据

extern unsigned int  gFF0D_Receive_Timer;
extern unsigned char gHandle_Receive_Over;		//每接收到一个来自手柄的数据，此变量清零，否则每一毫秒增加计数
extern unsigned char gDisp_Receive_Over;

extern unsigned char gW_TrackData_Page;			//作为存储行车轨迹的写指针
extern unsigned char gR_TrackData_Page;			//作为存储行车轨迹的读指针


extern unsigned char gM22_GSM_ECHO_Timer;		//作为记时G20模块在程序运行期间无任何返回数据的时间间隔，如果此变量大过某一个值，说明G20需要复位
extern unsigned int	 gM22_TCP_ECHO_Timer;
extern unsigned int  gCycle_TCP;				//作为存储周期发送行驶数据的时间周期
extern unsigned int  gCycle_GSM;
extern unsigned int  gCycle_Distance;

extern unsigned char gStatus1_Flag;
extern unsigned char gStatus2_Flag;
extern unsigned char gStatus3_Flag;

extern unsigned char gError_Count;				//ADC转换的数值不在正常范围内的错误统计
extern unsigned char gOK_Count;					//ADC转换的数值在正常范围内的正确统计
extern unsigned int  gCheck_Stop_Timer;
extern unsigned char gCycle_Send_Status;		//这个变量作为表示5种周期类型数据的标志，=0表示无任何周期数据类型
extern unsigned int  gLand_Interval_Timer;	 	//作为如果未收到登陆响应，则间隔每一段时间进行登陆响应的时间计时

//主控与手柄处理数据的缓冲
extern unsigned char gHandle_Buffer[120];
extern unsigned char gHandle_Buffer_Point;

extern unsigned int  gTest_Alarm_Timer;
extern unsigned int  gTest_Status_Timer;

extern unsigned int  gCycle_Alarm;					//作为报警后，存储下行的报警响应的的报警周期的设置
extern unsigned char gCircle_Buffer[256];			//作为存储报警，在GSM模式下的监控，历史轨迹的查询数据的缓冲
extern unsigned char gCircle_Buffer_Point;
extern unsigned int  gCircle_Timer;					//作为记时需要发送gCircle_Buffer缓冲区数据的时间变量,以秒为单位计时
extern unsigned char gW_TrackData_Item;
extern unsigned char gRing_Test_Timer;

extern unsigned char gGSM_Work_Count;
extern unsigned int  gCheck_Data;

extern unsigned char  gDelay_Flash_Timer;

extern unsigned char gDisp_Buffer[256];
extern unsigned int  gDisp_Buffer_Point;


extern unsigned char gDisp_Buffer_R[50];
extern unsigned int  gDisp_Buffer_R_Point;

extern unsigned long int gCheck_Track_Time_Start;	//存储需要查询历史轨迹的开始时间
extern unsigned long int gCheck_Track_Time_end;		//存储需要查询历史轨迹的结束时间

extern unsigned long int gALL_Distance;				//作为统计里程的变量累加


extern unsigned int  gGPS_Invaild_Timer;

extern unsigned int  gHandle_OnLine_Timer;			//作为判断手柄是否被摘除的时间计时
extern unsigned int  gDisp_OnLine_Timer;			//作为判断显示屏是否关机的时间计时
extern unsigned char gSteal_Vaild_Count;
extern unsigned char gOver_Speed_Count;
extern unsigned char gOver_Line_Count;
extern unsigned char gArea_No_Count;
extern unsigned int  gPassPage_First;
extern unsigned int  gPassPage_End;
extern unsigned int  gTest_SMS_Timer;
extern unsigned char gPassword_Count;

extern unsigned char gM22_Status;
extern unsigned char gChange_TCP_Status;
extern unsigned int  gLand_TCP_Count;
extern unsigned char gLand_TCP_ERROR_Count;		//作为累计和连续登陆TCP失败的统计
extern unsigned char gNO_Land_TCP_Timer;
extern unsigned char gM22_ReturnER_SIM_Count;
extern unsigned char gM22_Stop_Status;
extern unsigned char gSTOP_Speed_Count;
extern unsigned char gArea_Par;					//相关区域设置方面的参数


extern unsigned char gReceive1_Over_Count;
extern unsigned int  gReceive1_Point;

extern unsigned char gSend_Type1;
extern unsigned char gStatus;
extern unsigned char gRecord_Status;
extern unsigned char gHandle_Single_Count;
extern unsigned char gSpeed_Disp;
extern unsigned char gDisp_Status;
extern unsigned char gOver_Tire_Count,gStop_Tire_Count,gFore_Tire_Count;
extern unsigned char gGet_Driver_Info_Count;
extern unsigned char gSet_Distance_Info_Count;
extern unsigned char gDriver_Info;
extern unsigned char gReceive_Record_Info_Timer;

extern unsigned char gSpeed_Gps;
extern unsigned char gSpeed_Record;
extern unsigned char gAlarm_Type;						//存储需要上传的一些信息的标志
extern unsigned int  gRecord_Data_Timer;				//统计记录记录仪多长时间未向主控返回数据
extern unsigned int  gRload_Flash_Par_Timer;
extern unsigned char gSend_RecordData_Timer;
extern unsigned char gGps_Null_Count;
extern unsigned char gRecord_Null_Count;
extern unsigned char gDo_Speed_Count;
extern unsigned int  gLast_Phone_Timer;
extern unsigned int  gCommon_Flag;
extern unsigned int  gWatch_Circle;
extern unsigned int  gWatch_Time;
extern unsigned int  gWatch_Count;
extern unsigned char gLine_Num;
extern unsigned char gLine_Timer;
extern unsigned char gLine_Count;
extern unsigned char gNO_GPS_Reset_Count;
extern unsigned char gNO_Sram_Data_Timer;

extern unsigned char gWait_Land_Echo_Count;
extern unsigned int  gMax_Limit_Distance;
extern unsigned char gAlarm_Count;
extern unsigned char gDelay_Com_Timer;
extern unsigned char gHangUp_Timer;
extern unsigned char gAllow_R232_Timer;
extern unsigned char gSys_Handle;
extern unsigned char gSend_Handle_Timer;
extern unsigned char gRe_Send_Count;
extern unsigned int  gCheck_Par_Timer;
extern unsigned char gSend_Disp_Timer;
extern unsigned char gDisp_ReSend_Count;
extern unsigned char gSend_Record_Count;
extern unsigned char gNum_Phone_Temp[20];
extern unsigned char gDisp_Timer;
#endif
