/********************************************************\
*	�ļ�����  General.C
*	����ʱ�䣺2004��10��20��
*	�����ˣ�  
*	�汾�ţ�  1.00
*	���ܣ�	  ������Ŀ�ļ���Ҫ�õ���ȫ�ֱ���
*	�ļ����ԣ������ļ�
*	�޸���ʷ����ÿ��������

\********************************************************/
unsigned long int gLongitude_mid=0,gLatitude_mid=0;
//unsigned char gFlash_W_Word_Item=0,gFlash_R_Word_Item=0;
//unsigned int  gPassTrack_Store_Sector=0;	//��Ϊ��ʾ�洢��ʷ�켣��ҳ����һ��ҳΪ��ʷ�켣�洢�ļ���ҳ��
//unsigned int  gPassTrack_Read_Sector=0;		//��Ϊ�ڲ�ѯ��ʷ�켣��ʱ��ָʾ��ǰ��ȡ�Ĺ켣ҳ
//unsigned char gPassTrack_Store_Item=0;		//��Ϊ�洢����ʷ�켣ҳ��ǰҳ�Ĺ켣������һ��ҳ���洢15����¼��
unsigned char gON_OFF_Temp0=0xFF;
unsigned char gON_OFF_Temp1=0xFF;
unsigned char gON_OFF_Temp2=0xFF;			//��Ϊ�洢ϵͳ����������������
unsigned char gTCP_Status_Temp=0;
unsigned int  gTimer=0;
unsigned int  gInternal_Flag=0; 			//�ڲ��Ĳ��������������洢��¼��λG20�ı�־����λGPS�ı�־��G20���縴λ�ı�־��
unsigned int  gGeneral_Flag=0;				//������¼�ڲ�����һЩ��Ҫλ�����ı����ı�־
unsigned int  gOther_Flag=0;
unsigned int  gPublic_Flag=0;
unsigned char gType=0,gKind=0,gGroup=0;
unsigned int  gVID=0;
unsigned char gPhone_Status=0;
unsigned char gReset_M22_On_Setp=0;
unsigned char gOFF_Power_S=0;				//�����ڿ���2576����Ĳ������
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
unsigned char gGPS_No_Output_Timer=0;		//��ΪGPS�������ʱ��ͳ��

unsigned char gTimer_CSQ=0;
unsigned char gTimer_Area_Test=0;
unsigned char gSignal=0;					//�ź�ǿ��
unsigned char gVolume=0;					//�ֱ������Ĵ�С��ֵ

unsigned char gSMS_ACK[5];					//�洢��ص�ACK���ݣ���ʼ����ʱ��ֵ��Ϊ0
unsigned char gDTMF_SAVE[20];
unsigned int  gTCP_FF0D_Send_Timer=0;		//��Ϊ����FF 0D������
unsigned long int gIP_Port_Num=0;			//��Ϊ�洢��Flash�д洢��IP�Ķ˿ں�
unsigned long int ACC_OFF_Timer=0;			//��Ϊͳ��ACC�رյ�ʱ����
unsigned long int gLongitude=0,gLatitude=0;
unsigned char gYear=0,gMonth=0,gDate=0,gHour=0,gMinute=0,gSecond=0;
unsigned char gSpeed=0,gAzimuth=0;


unsigned char gWatch_Type=0;				//�洢��Ҫ�ϴ��ļ����Ϣ�ı���
unsigned int  gADCvalue_A1=0;				//��ΪADCת��ͨ��A1ÿ��ת���洢�����ݱ���

unsigned char gMAX_Speed=0xFF;

unsigned char gGPS_Receive_Over=0;			//ÿ���յ�һ��GPS���ݣ��˱������㣬����ÿһ�������Ӽ���
unsigned char gHandle_Receive_Over=0;		//ÿ���յ�һ�������ֱ������ݣ��˱������㣬����ÿһ�������Ӽ���
unsigned char gDisp_Receive_Over=0;
unsigned char gW_TrackData_Page=0;			//��Ϊ�洢�г��켣��дָ��
unsigned char gR_TrackData_Page=0;			//��Ϊ�洢�г��켣�Ķ�ָ��

unsigned char gM22_GSM_ECHO_Timer=0;		//��Ϊ��ʱM22ģ���ڳ��������ڼ����κη������ݵ�ʱ����������˱������ĳһ��ֵ��˵��G20��Ҫ��λ
unsigned int  gM22_TCP_ECHO_Timer=0;
unsigned int  gFF0D_Receive_Timer=0;
unsigned int  gCycle_TCP=0;
unsigned int  gCycle_GSM=0;
unsigned int  gCycle_Distance=0;
unsigned char gStatus1_Flag=0;
unsigned char gStatus2_Flag=0;
unsigned char gStatus3_Flag=0;

unsigned char gError_Count=0;				//ADCת������ֵ����������Χ�ڵĴ���ͳ��
unsigned char gOK_Count=0;					//ADCת������ֵ��������Χ�ڵ���ȷͳ��
unsigned int  gCheck_Stop_Timer=0;

unsigned char gCycle_Send_Status=0;			//���������Ϊ��ʾ5�������������ݵı�־��=0��ʾ���κ�������������
unsigned int  gLand_Interval_Timer=0;


unsigned char gHandle_Buffer[120];			//��Ϊ���ؽ��մ洢���ص绰�����ط��͵���Ϣ��
unsigned char gHandle_Buffer_Point=0;
unsigned int  gTest_Alarm_Timer=0;
unsigned int  gTest_Status_Timer=0;


unsigned int  gCycle_Alarm=0;				//��Ϊ�����󣬴洢���еı�����Ӧ�ĵı������ڵ�����
unsigned char gCircle_Buffer[256];			//��Ϊ�洢��������GSMģʽ�µļ�أ���ʷ�켣�Ĳ�ѯ���ݵĻ���
unsigned char gCircle_Buffer_Point=0;
unsigned int  gCircle_Timer=0;				//��Ϊ��ʱ��Ҫ����gCircle_Buffer���������ݵ�ʱ�����
unsigned char gW_TrackData_Item=0;

unsigned char gRing_Test_Timer=0;
unsigned char gGSM_Work_Count=0;
unsigned int  gCheck_Data=0;

unsigned char gDelay_Flash_Timer=0;

unsigned char gDisp_Buffer[256];			//�����������ʾ�ն˷��͵���Ϣ��
unsigned int  gDisp_Buffer_Point=0;

unsigned char gDisp_Buffer_R[50];			//��Ž��յ���ʾ�ն������ط��͵���Ϣ��
unsigned int  gDisp_Buffer_R_Point=0;

unsigned long int gCheck_Track_Time_Start=0;//�洢��Ҫ��ѯ��ʷ�켣�Ŀ�ʼʱ��
unsigned long int gCheck_Track_Time_end=0;	//�洢��Ҫ��ѯ��ʷ�켣�Ľ���ʱ��
unsigned long int gALL_Distance=0;			//��Ϊͳ����̵ı����ۼ�
unsigned int  gGPS_Invaild_Timer=0;

unsigned int  gHandle_OnLine_Timer=0;		//��Ϊ�ж��ֱ��Ƿ�ժ����ʱ���ʱ
unsigned int  gDisp_OnLine_Timer=0;			//��Ϊ�ж���ʾ���Ƿ�ػ���ʱ���ʱ
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
unsigned char gLand_TCP_ERROR_Count=0;		//��Ϊ�ۼƺ�������½TCPʧ�ܵ�ͳ��
unsigned char gM22_ReturnER_SIM_Count=0;
unsigned char gM22_Stop_Status=0;
unsigned char gSTOP_Speed_Count=0;
unsigned char gArea_Par=0;					//����������÷���Ĳ��������忴���λ�Ķ���


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
unsigned char gAlarm_Type=0;					//�洢��Ҫ�ϴ���һЩ������Ϣ�ı���
unsigned int  gRecord_Data_Timer=0;				//ͳ�Ƽ�¼��¼�Ƕ೤ʱ��δ�����ط�������
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