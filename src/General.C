/********************************************************\
*	文件名：  General.C
*	创建时间：2004年10月20日
*	创建人：  
*	版本号：  1.00
*	功能：	  整个项目文件需要用到的全局变量
*	文件属性：公共文件
*	修改历史：（每条详述）

\********************************************************/
unsigned long int gLongitude_mid=0,gLatitude_mid=0;
//unsigned char gFlash_W_Word_Item=0,gFlash_R_Word_Item=0;
//unsigned int  gPassTrack_Store_Sector=0;	//作为表示存储历史轨迹的页（下一个页为历史轨迹存储的记忆页）
//unsigned int  gPassTrack_Read_Sector=0;		//作为在查询历史轨迹的时候，指示当前读取的轨迹页
//unsigned char gPassTrack_Store_Item=0;		//作为存储在历史轨迹页当前页的轨迹条数（一个页最多存储15条记录）
unsigned char gON_OFF_Temp0=0xFF;
unsigned char gON_OFF_Temp1=0xFF;
unsigned char gON_OFF_Temp2=0xFF;			//作为存储系统开关量的三个变量
unsigned char gTCP_Status_Temp=0;
unsigned int  gTimer=0;
unsigned int  gInternal_Flag=0; 			//内部的参数变量，用来存储记录复位G20的标志，复位GPS的标志，G20掉电复位的标志，
unsigned int  gGeneral_Flag=0;				//用来记录内部其他一些需要位操作的变量的标志
unsigned int  gOther_Flag=0;
unsigned int  gPublic_Flag=0;
unsigned char gType=0,gKind=0,gGroup=0;
unsigned int  gVID=0;
unsigned char gPhone_Status=0;
unsigned char gReset_M22_On_Setp=0;
unsigned char gOFF_Power_S=0;				//属于在控制2576掉电的步骤变量
unsigned char gPhone_Step=0;
unsigned char gSMS_IN_Step=0;
unsigned char gSMS_OUT_Step=0;
unsigned char gLand_GPRS_Step=0;
unsigned char gGPRS_Data_Step=0;
unsigned char gGSM_OTHER_Step=0;
unsigned char gGeneral_Buffer[320];
unsigned char gSMS_In_Lenth=0;
unsigned char gReceive0_GSM_Buffer[320];
unsigned int  gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
unsigned int  gReceive0_GSM_Buffer_Point=0;
unsigned int  gReceive0_GSM_Buffer_Move=0;
unsigned int  gTimer_GSM_AT=0;

unsigned char gGSM_Oper_Type=0;          	//G20
unsigned char gGPS_No_Output_Timer=0;		//作为GPS无输出的时间统计

unsigned char gTimer_CSQ=0;
unsigned char gTimer_Area_Test=0;
unsigned char gSignal=0;					//信号强度
unsigned char gVolume=0;					//手柄音量的大小数值

unsigned char gSMS_ACK[5];					//存储相关的ACK数据，初始化的时候值均为0
unsigned char gDTMF_SAVE[20];
unsigned int  gTCP_FF0D_Send_Timer=0;		//作为发送FF 0D的数据
unsigned long int gIP_Port_Num=0;			//作为存储在Flash中存储的IP的端口号
unsigned long int ACC_OFF_Timer=0;			//作为统计ACC关闭的时间检测
unsigned long int gLongitude=0,gLatitude=0;
unsigned char gYear=0,gMonth=0,gDate=0,gHour=0,gMinute=0,gSecond=0;
unsigned char gSpeed=0,gAzimuth=0;


unsigned char gWatch_Type=0;				//存储需要上传的监控信息的变量
unsigned int  gADCvalue_A1=0;				//作为ADC转换通道A1每次转换存储的数据变量

unsigned char gMAX_Speed=0xFF;

unsigned char gGPS_Receive_Over=0;			//每接收到一个GPS数据，此变量清零，否则每一毫秒增加计数
unsigned char gHandle_Receive_Over=0;		//每接收到一个来自手柄的数据，此变量清零，否则每一毫秒增加计数
unsigned char gDisp_Receive_Over=0;
unsigned char gW_TrackData_Page=0;			//作为存储行车轨迹的写指针
unsigned char gR_TrackData_Page=0;			//作为存储行车轨迹的读指针

unsigned char gM22_GSM_ECHO_Timer=0;		//作为记时M22模块在程序运行期间无任何返回数据的时间间隔，如果此变量大过某一个值，说明G20需要复位
unsigned int  gM22_TCP_ECHO_Timer=0;
unsigned int  gFF0D_Receive_Timer=0;
unsigned int  gCycle_TCP=0;
unsigned int  gCycle_GSM=0;
unsigned int  gCycle_Distance=0;
unsigned char gStatus1_Flag=0;
unsigned char gStatus2_Flag=0;
unsigned char gStatus3_Flag=0;

unsigned char gError_Count=0;				//ADC转换的数值不在正常范围内的错误统计
unsigned char gOK_Count=0;					//ADC转换的数值在正常范围内的正确统计
unsigned int  gCheck_Stop_Timer=0;

unsigned char gCycle_Send_Status=0;			//这个变量作为表示5种周期类型数据的标志，=0表示无任何周期数据类型
unsigned int  gLand_Interval_Timer=0;


unsigned char gHandle_Buffer[120];			//作为主控接收存储车载电话向主控发送的消息包
unsigned char gHandle_Buffer_Point=0;
unsigned int  gTest_Alarm_Timer=0;
unsigned int  gTest_Status_Timer=0;


unsigned int  gCycle_Alarm=0;				//作为报警后，存储下行的报警响应的的报警周期的设置
unsigned char gCircle_Buffer[256];			//作为存储报警，在GSM模式下的监控，历史轨迹的查询数据的缓冲
unsigned char gCircle_Buffer_Point=0;
unsigned int  gCircle_Timer=0;				//作为记时需要发送gCircle_Buffer缓冲区数据的时间变量
unsigned char gW_TrackData_Item=0;

unsigned char gRing_Test_Timer=0;
unsigned char gGSM_Work_Count=0;
unsigned int  gCheck_Data=0;

unsigned char gDelay_Flash_Timer=0;

unsigned char gDisp_Buffer[256];			//存放主控向显示终端发送的消息包
unsigned int  gDisp_Buffer_Point=0;

unsigned char gDisp_Buffer_R[50];			//存放接收到显示终端向主控发送的消息包
unsigned int  gDisp_Buffer_R_Point=0;

unsigned long int gCheck_Track_Time_Start=0;//存储需要查询历史轨迹的开始时间
unsigned long int gCheck_Track_Time_end=0;	//存储需要查询历史轨迹的结束时间
unsigned long int gALL_Distance=0;			//作为统计里程的变量累加
unsigned int  gGPS_Invaild_Timer=0;

unsigned int  gHandle_OnLine_Timer=0;		//作为判断手柄是否被摘除的时间计时
unsigned int  gDisp_OnLine_Timer=0;			//作为判断显示屏是否关机的时间计时
unsigned char gSteal_Vaild_Count=0;
unsigned char gOver_Speed_Count=0;
unsigned char gOver_Line_Count=0;
unsigned char gArea_No_Count=0;
unsigned int  gPassPage_First=0;
unsigned int  gPassPage_End=0;

unsigned int  gTest_SMS_Timer=0;
unsigned char gPassword_Count=0;

unsigned char gM22_Status=0;
unsigned char gChange_TCP_Status=0;
unsigned int  gLand_TCP_Count=0;
unsigned char gLand_TCP_ERROR_Count=0;		//作为累计和连续登陆TCP失败的统计
unsigned char gM22_ReturnER_SIM_Count=0;
unsigned char gM22_Stop_Status=0;
unsigned char gSTOP_Speed_Count=0;
unsigned char gArea_Par=0;					//相关区域设置方面的参数，具体看相关位的定义


unsigned char gReceive1_Over_Count=0;
unsigned int  gReceive1_Point=0;

unsigned char gSend_Type1=0;
unsigned char gStatus=0;
unsigned char gRecord_Status=0;
unsigned char gSpeed_Record=0;
unsigned char gHandle_Single_Count=0;
unsigned char gSpeed_Disp=0;
unsigned char gDisp_Status=0;
unsigned char gOver_Tire_Count=0,gStop_Tire_Count=0,gFore_Tire_Count=0;
unsigned char gDriver_Info=0;
unsigned char gGet_Driver_Info_Count=0;
unsigned char gSet_Distance_Info_Count=0;
unsigned char gReceive_Record_Info_Timer=0;


unsigned char gSpeed_Gps=0;
unsigned char gAlarm_Type=0;					//存储需要上传的一些报警信息的变量
unsigned int  gRecord_Data_Timer=0;				//统计记录记录仪多长时间未向主控返回数据
unsigned int  gRload_Flash_Par_Timer=0;
unsigned char gSend_RecordData_Timer=0;
unsigned char gGps_Null_Count=0;
unsigned char gRecord_Null_Count=0;
unsigned char gDo_Speed_Count=0;
unsigned int  gLast_Phone_Timer=0;
unsigned int  gCommon_Flag=0;

unsigned int  gWatch_Circle=0;
unsigned int  gWatch_Time=0;
unsigned int  gWatch_Count=0;
unsigned char gLine_Num=0;
unsigned char gLine_Timer=0;
unsigned char gLine_Count=0;
unsigned char gNO_GPS_Reset_Count=0;
unsigned char gNO_Sram_Data_Timer=0;
unsigned char gWait_Land_Echo_Count=0;
unsigned int  gMax_Limit_Distance=0;
unsigned char gAlarm_Count=0;
unsigned char gDelay_Com_Timer=0;
unsigned char gHangUp_Timer=0;
unsigned char gAllow_R232_Timer=0;
unsigned char gSys_Handle=0;
unsigned char gSend_Handle_Timer=0;
unsigned char gRe_Send_Count=0;
unsigned int  gCheck_Par_Timer=0;
unsigned char gSend_Disp_Timer=0;
unsigned char gDisp_ReSend_Count=0;
unsigned char gSend_Record_Count=0;
unsigned char gNum_Phone_Temp[20];
unsigned char gDisp_Timer=0;
