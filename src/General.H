

#ifndef __General_H
#define __General_H

extern unsigned long int gLongitude_mid,gLatitude_mid;
//extern unsigned char gFlash_W_Word_Item,gFlash_R_Word_Item;
//extern unsigned int  gPassTrack_Store_Sector;
//extern unsigned int  gPassTrack_Read_Sector;
//extern unsigned char gPassTrack_Store_Item;
extern unsigned char gON_OFF_Temp0;
extern unsigned char gON_OFF_Temp1;
extern unsigned char gON_OFF_Temp2;			//��Ϊ�洢ϵͳ����������������
extern unsigned char gTCP_Status_Temp;
extern unsigned int  gTimer;
extern unsigned int  gInternal_Flag; 		//�ڲ��Ĳ��������������洢��¼��λG20�ı�־����λGPS�ı�־��G20���縴λ�ı�־��
extern unsigned int  gGeneral_Flag;			//������¼�ڲ�����һЩ��Ҫλ�����ı����ı�־
extern unsigned int  gOther_Flag;
extern unsigned int  gPublic_Flag;
extern unsigned char gType,gKind,gGroup;
extern unsigned int  gVID;
extern unsigned char gPhone_Status;
extern unsigned char gReset_M22_On_Setp;
extern unsigned char gOFF_Power_S;			//�����ڿ���2576����Ĳ������
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
extern unsigned char gSignal;					//�ź�ǿ��
extern unsigned char gVolume;					//�ֱ������Ĵ�С��ֵ

extern unsigned char gSMS_ACK[5];				//�洢��ص�ACK���ݣ���ʼ����ʱ��ֵ��Ϊ0
extern unsigned char gDTMF_SAVE[20];
extern unsigned int  gTCP_FF0D_Send_Timer;		//��Ϊ������ڷ��ͼ���GPRS�����GPRS����
extern unsigned long int gIP_Port_Num;			//��Ϊ�洢��Flash�д洢��IP�Ķ˿ں�
extern unsigned long int ACC_OFF_Timer;			//��Ϊͳ��ACC�رյ�ʱ����
extern unsigned long int gLongitude,gLatitude;
extern unsigned char gYear,gMonth,gDate,gHour,gMinute,gSecond;

extern unsigned char gSpeed,gAzimuth;
extern unsigned char gWatch_Type;
extern unsigned int  gADCvalue_A1;				//��ΪADCת��ͨ��A1ÿ��ת���洢�����ݱ���

extern unsigned char gMAX_Speed;

extern unsigned char gGPS_Receive_Over;			//��ΪGPS�Ľ����������ж��еĴ��������յ�һ���ֽں󽫴˼�������
												//��GPS��������ѭ���У�����жϴ˼�������ĳһ��ֵ�����жϽ��յ�GPS����

extern unsigned int  gFF0D_Receive_Timer;
extern unsigned char gHandle_Receive_Over;		//ÿ���յ�һ�������ֱ������ݣ��˱������㣬����ÿһ�������Ӽ���
extern unsigned char gDisp_Receive_Over;

extern unsigned char gW_TrackData_Page;			//��Ϊ�洢�г��켣��дָ��
extern unsigned char gR_TrackData_Page;			//��Ϊ�洢�г��켣�Ķ�ָ��


extern unsigned char gM22_GSM_ECHO_Timer;		//��Ϊ��ʱG20ģ���ڳ��������ڼ����κη������ݵ�ʱ����������˱������ĳһ��ֵ��˵��G20��Ҫ��λ
extern unsigned int	 gM22_TCP_ECHO_Timer;
extern unsigned int  gCycle_TCP;				//��Ϊ�洢���ڷ�����ʻ���ݵ�ʱ������
extern unsigned int  gCycle_GSM;
extern unsigned int  gCycle_Distance;

extern unsigned char gStatus1_Flag;
extern unsigned char gStatus2_Flag;
extern unsigned char gStatus3_Flag;

extern unsigned char gError_Count;				//ADCת������ֵ����������Χ�ڵĴ���ͳ��
extern unsigned char gOK_Count;					//ADCת������ֵ��������Χ�ڵ���ȷͳ��
extern unsigned int  gCheck_Stop_Timer;
extern unsigned char gCycle_Send_Status;		//���������Ϊ��ʾ5�������������ݵı�־��=0��ʾ���κ�������������
extern unsigned int  gLand_Interval_Timer;	 	//��Ϊ���δ�յ���½��Ӧ������ÿһ��ʱ����е�½��Ӧ��ʱ���ʱ

//�������ֱ��������ݵĻ���
extern unsigned char gHandle_Buffer[120];
extern unsigned char gHandle_Buffer_Point;

extern unsigned int  gTest_Alarm_Timer;
extern unsigned int  gTest_Status_Timer;

extern unsigned int  gCycle_Alarm;					//��Ϊ�����󣬴洢���еı�����Ӧ�ĵı������ڵ�����
extern unsigned char gCircle_Buffer[256];			//��Ϊ�洢��������GSMģʽ�µļ�أ���ʷ�켣�Ĳ�ѯ���ݵĻ���
extern unsigned char gCircle_Buffer_Point;
extern unsigned int  gCircle_Timer;					//��Ϊ��ʱ��Ҫ����gCircle_Buffer���������ݵ�ʱ�����,����Ϊ��λ��ʱ
extern unsigned char gW_TrackData_Item;
extern unsigned char gRing_Test_Timer;

extern unsigned char gGSM_Work_Count;
extern unsigned int  gCheck_Data;

extern unsigned char  gDelay_Flash_Timer;

extern unsigned char gDisp_Buffer[256];
extern unsigned int  gDisp_Buffer_Point;


extern unsigned char gDisp_Buffer_R[50];
extern unsigned int  gDisp_Buffer_R_Point;

extern unsigned long int gCheck_Track_Time_Start;	//�洢��Ҫ��ѯ��ʷ�켣�Ŀ�ʼʱ��
extern unsigned long int gCheck_Track_Time_end;		//�洢��Ҫ��ѯ��ʷ�켣�Ľ���ʱ��

extern unsigned long int gALL_Distance;				//��Ϊͳ����̵ı����ۼ�


extern unsigned int  gGPS_Invaild_Timer;

extern unsigned int  gHandle_OnLine_Timer;			//��Ϊ�ж��ֱ��Ƿ�ժ����ʱ���ʱ
extern unsigned int  gDisp_OnLine_Timer;			//��Ϊ�ж���ʾ���Ƿ�ػ���ʱ���ʱ
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
extern unsigned char gLand_TCP_ERROR_Count;		//��Ϊ�ۼƺ�������½TCPʧ�ܵ�ͳ��
extern unsigned char gNO_Land_TCP_Timer;
extern unsigned char gM22_ReturnER_SIM_Count;
extern unsigned char gM22_Stop_Status;
extern unsigned char gSTOP_Speed_Count;
extern unsigned char gArea_Par;					//����������÷���Ĳ���


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
extern unsigned char gAlarm_Type;						//�洢��Ҫ�ϴ���һЩ��Ϣ�ı�־
extern unsigned int  gRecord_Data_Timer;				//ͳ�Ƽ�¼��¼�Ƕ೤ʱ��δ�����ط�������
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