/********************************************************\
*	�ļ�����  Other_Define.H
*	����ʱ�䣺2005��3��22��
*	�����ˣ�  
*	�汾�ţ�  V2.1
*	���ܣ�	  �йط����һЩ���壨����ɿ���ϸϸ�ڣ�

*	�ļ����ԣ������ļ�
*	�޸���ʷ����ÿ��������
	����˵��������M22ģ����ص�һЩ�ܽ�����

\********************************************************/


#ifndef __Other_Define_H
#define __Other_Define_H
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			һЩ���ƹܽŵĶ���
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

//�汾V3.8
#define		WDI					0x01		//P6.0  out ι����������ת�ⲿ���Ź��������ƽ�ܽ�
#define		POWER_ON_OFF		0x40		//P6.6  out ����GPS��GSM��Դ����
#define		STOP_Q1				0x20		//P6.5  out ִ���ƶ��Ŀ��ƹܽ�1
#define		STOP_Q2				0x10		//P6.4  out ִ���ƶ��Ŀ��ƹܽ�2
#define		ACC_TEST			0x01		//P3.0	in	��⳵��ACC��ƽ
#define		ARALM				0x40		//P2.6	in  ����Ƿ��б��ٱ����Ĺܽ�
#define		DISP_TEST			0x08		//P6.3	in	����Ƿ�����������ʾƾ���ݵĹܽ�
#define		SEL_MIC				0x80		//P6.7  out	ѡ������ͨ��
#define		MCU_STATUS			0x10		//P1.4	out
#define		RING_TEST			0x01		//P1.0	in	��Ϊ�����TCP���ߵ�״̬�£��Ƿ��е绰����
#define		NL_CHK				0x10		//P2.7	in  ����/���صļ����
#define		CHECK_ON_OFF		0x20		//P2.5	in  �ƶ�ϵͳִ�н�����

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				16λ������д�ACK��SMS������Ҫ��Ӧ���е�SMS�д���ACK��ͼ��
				���ݴ洢��gSMS_ACK[]��Ӧ��������
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define			TRACK_ASKECHO_ACK		0			//�켣��ѯ��Ӧ--��λ���ݵ�ACK
#define			ALARM_ACK				1			//��������ACK
#define		RECORD_EQUIPMENT_ACK		2			//��Ӳ�����ACK����
#define		SET_PHONE_LIMIT_ECHO_ACK	3			//�������õ绰Ȩ�����õ���Ӧ��ACK

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			    ���������ĵ�λ����
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define			ONE						1
#define			TWO						2
#define			THREE					3
#define			FOUR					4

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			����һЩ�����Ķ���
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		DIS_LON				28.67
#define		DIS_LAT				30.92


#define		OFF_TIME			15000		//�ض�2576��ʱ�䣨 ���룩
#define		DELAY_TIME			10000		//����2576����ӳ�ʱ�䣨���룩

#define		M22_NULECHO_TIME	200			//G20 ��200�������κη��أ�����Ҫ��λG20
#define		M22_LAND_TIME		30			//���δ�ɹ��յ����еĵ�½��Ϣ����ÿ��30����е�½����
#define		TCP_TEST_TIME		60			//��GPRS��������״̬�£�����һ�����ݰ�(���з���GPRS�����ݰ�)�ļ�����룩
#define		ALARM_CIRCLE_TIME	60			//1���ӵı�������
#define		GPS_INVALID_COUNT	3600 		//gps�������������Сʱ��Ȼ����Ч�㣬����Ҫ��GPSģ����и�λ����
#define		NO_OUTPUT_TIME		40			//ͳ��GPS���κ������ʱ������λΪ��
#define		GPS_LOW_TIME		10			//gps��RESET�ܽų����͵�ƽ��ʱ�䣨�룩
#define		STEAL_ALARM_COUNT	30			//���㱻���������ٶȴ���3.6*4�����������ļ���
#define		SEARCH_PASS_TIME	200			//����ʷ�켣��ѯ�Ĺ����У���Ҫ���500������е���һҳ�Ĳ�ѯ
#define		ROB_TIME			3000		//��ʾ���±��ٱ������صĳ���ʱ�䣨��λΪ���룩
#define		HANDLE_OUT_TIME		130			//���������ʱ����5���ӣ����յ��ֱ����κ��������Ϊ�ֱ����ڱ�ժ��״̬
#define		DISP_OUT_TIME		130			//�������4���ӣ����յ���ʾ�ն˵������������Ϊ��ʾ�ն˴��ڹر�״̬
#define		ALLOW_STOP_SPEED	3			//���������ƶ����ٶ�
#define		OVER_TESTSMS_TIME	300			//����SMS�ķ��͵ĳ���5����
#define		OVER_TCP_TIME		200			//����200��δ���յ����е���������
#define		GSM_CHECKCSQ_TIME	30			//��GSM������ʽ�²�ѯCSQ��ʱ����30��
#define 	TCP_CHECKCSQ_TIME	200			//��TCP������ʽ�²�ѯCSQ��ʱ����200��
#define		TCP_TRACK_MINTIME	1200		//���TCP���г��켣�Ƿ������ߵķ�ˮ��
#define		RING_TEST_TIME		220			//��TCP���ߵ�״̬�£�����ƽ��ʱ��
#define		RELAND_TCP_TIME		180			//4���ӵ�ʱ����������һ�ε�TCP�ĵ�½

#define		TEST_ACC_TIME		5000		//�������ACC�ļ��ʱ��
#define		ACC_LASTOFF_TIME	600//1800    	//���ACC�رճ�������1Сʱ��ر���ģ��Ĺ���
#define		ACC_CHANGEOFF_TIME	240			//���ACC�رճ���20���ӣ���ı��г��켣���ϴ����
#define		ACCOFF_TRACK_TIME	1800		//��ΪACC�رպ���Ҫת���г��켣�ϴ���ķֽ�㣬���Сʱһ�ϴ����ݵ�
#define		DISTANCE_COMPASTATE	12			//��Ϊ��ʱ��GPS������Ч��������ACC���ڿ���״̬�µ����ͳ�ƵĲ���
#define		INTER_ATRA_TEST		2			//��Ϊ������򱨾���ʱ����
#define		ALLOW_PHONE_TIME	1800		//��������30���ӵ�ͨ��
//#define	 GET_DRIVER_INFO_TIME	10			//���û�гɹ���¼�ǵļ�ʻԱ��Ϣ������Ҫ���ڷ��͵�ʱ����(��λ��)
//#define	 WAIT_RETURNINFO_TIME	30			//���¼�Ƿ��Ͳ�ѯ��ʻԱ��Ϣ�󣬵ȴ���Ӧ��ʱ��
#define	 READY_SENDINFO_TIME	5			//׼������ʾ�����ͼ�ʻԱ��Ϣ�ĵȴ�ʱ��
#define  WAIT_DISPINFO_TIME		10			//
#define		RSET_GPS_TIME		60			//���GPS����60�룬�����ʽ����ȷ
#define	 OVER_RECORD_DATA_TIME	300			//����300��û�н��յ���¼�ǵ�ʱ��
#define	 MAX_TCP_DATA_LENGHT	1000		//�����TCP�����ݵĳ���
#define		RLAOD_FLASH_TIME	36000		//10��Сʱ���ܴ���ʲô״̬����Ҫ������������
#define		MAX_LING_DISTANCE	300			//��·ƫ���������ƾ��루�ף�
#define		ALARM_COUNT			10			//����ͨ��SMS���ͱ������ݵĴ���
#define		HANGUP_COUNT		40			//���40�������κν������߹Ҷ���������������Ҷϵ绰

#define		SMS_DATA_TIME		1800		//���TCP�����ߵģ�ͨ��SMS��ʽ�ϱ����ݵļ��20����
#define		CHECK_SYS_PAR_TIME	1800
//��GSM�����йصĲ������
#define		GSM_PHONE			1			//��绰�й�������ش���
#define		GSM_OTHER			2			//��������CLCC��CSQ��CSCA��صĴ���
#define		GSM_SMS_IN			3			//��GSM���������йص���ش���
#define		GSM_SMS_OUT			4			//��GSM���������йص���ش���
#define		LAND_TCP_OPER		5			//�붨�ڼ��GPRS�����йص���ش���
#define		GPRS_DATA_OPER		6			//��ͨ��GPRS���緢�����ݵ���ش���
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			�ֱ�״̬�Ķ��壨M22�����״̬������
			unsigned char  gPhone_Status=0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#define 	READY				0			//����״̬
#define 	INCOMING			1			//����״̬
#define 	DAILING				2			//���ڲ���״̬(�����ֱ����ͼ�������״̬)
#define 	ACTIVE_HANDLE		3			//�ֱ�����ͨ��״̬
#define 	ACTIVE_MONITOR		4			//����״̬

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			����GPS���¼��֮���ͨѶ����
			unsigned char  gStatus=0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		NORMAL_TYPE			0			//������������GPS���¼��ͨѶ��ʽ
#define		DISP_TYPE			2			//������ʾ�ն����¼��ͨѶ��ʽ
#define		CENTER_TYPE			1			//���������������¼��ͨѶ��ʽ

//�洢��buffer1��0x30---0x4f��صĹ����ֱ���ATָ��ı�־���洢��0x31
#define		H_DIAL				0x01		//ATDָ��ı�־
#define		H_ANSWER			0x02		//ATAָ��ı�־
#define		H_HUNGUP			0x03		//ATHָ��ı�־
#define		H_VOLADJ			0x04		//�������ڵı�־
#define		H_DTMF				0x05		//DTMF�ı�־
#define		M_DIAL				0x06		//����ָ��
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			�������͵Ķ���洢�������͵ı���gFlag Ϊchar�ͣ���Ϊ���λ����������ʾ
			GPS���ݵ���Ч����Ч��־�����ܱ�ʾ�������͵��Ǻ�7λ��������ܱ�ʾ127�����͵ı���
			unsigned char gAlarm_Type=0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		NOP_ALARM			0			//���κα�����Ϣ
#define		ROB_ALARM			1			//���ٱ���
#define		STEAL_ALARM			2			//��������
#define		OVER_AREA_ALARM		3			//���򱨾�
#define		OVER_SPEED_ALARM	4			//���ٱ���
#define		OVER_TIRE_ALARM		10			//ƣ�ͼ�ʻ����
#define		OVER_LINE_ALARM		5			//��·ƫ�뱨��
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			�洢��Circle_Buffer[]����Ҫ���ڷ��͵����ݵ�����
			������unsigned char gCycle_Send_Status=0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		NOP_DATA			0			//�������κ����͵�����
#define		TRACK_DATA_TIME		1			//���ڰ�ʱ���ϴ��г��켣�������
#define		TRACK_DATA_DISTANCE	2			//���ڰ���������ϴ��켣�������
#define		ALARM_DATA			3			//���ڱ�������
#define		WATCH_DATA			4			//���ڼ������
#define		PASS_DATA			5			//������ʷ�켣����
#define		SAVE_DATA			6			//����ä����������
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			��ʾ��ص�����
			������unsigned char gWatch_Type=0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		WATCH_TYPE_NULL			0			//��ʾ���κ����͵ļ��
#define		WATCH_TYPE_TIME 		1			//��ʾ��ʱ�������
#define		WATCH_TYPE_COUNT		2			//��ʾ�����������

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			��ʾ���յ�TCP���еĽ���/�˳�����״̬����������
			������unsigned char gTCP_Status_Temp=0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		TCP_STATUS_READY				0			//��ʾδ���յ�����������������֮���
#define		TCP_STATUS_LAND_ECHO			1			//��ʾ���յ����������Ҫ������½TCP�ı�־
#define		TCP_STATUS_QUIT_ECHO			2			//��ʾ���յ����������Ҫ�˳���½��TCP��־
#define		TCP_STATUS_ALLOW_LAND			3
#define		TCP_STATUS_ALLOW_QUIT			4
#define		TCP_STATUS_QUIT_READY			5

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			��ʾ���״̬�ĳ���
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		DEFENCE_NO						0			//��ʾ�����ն�δ�������״̬
#define		DEFENCE_YES						1			//��ʾ�����ն��Ѿ��������״̬

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			��ʾ�����ECHO�ĳ���
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		COMMAND_ERROR					0			//��ʾ��������ز��ɹ�
#define		COMMAND_OK						1			//��ʾ��������سɹ�

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			��ʾM22��ǰ״̬�ĳ���  gM22_Status
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		GSM_WORK						0			//M22����GSM�Ĺ�����ʽ��
#define		TCP_HANG_WORK					1			//M22����TCP�Ѿ���½�ϣ������ڹ���״̬
#define		TCP_ONLINE_WORK					2			//M22����TCP�Ѿ���½�ϣ����ڷǹ���״̬
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			��ʾM22��ǰ״̬�ĳ���  gM22_Stop_Status
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		NOP_READY						0			//����ƶ����ڴ���״̬
#define		ALLOW_SET_STOP					1			//���յ����е������ƶ���������״̬
#define		ASK_SET_STOP					2			//ִ���ƶ��󣬼���ƶ������״̬
#define		ALLOW_FREE_STOP					3			//���յ����е���������ƶ���������״̬
#define		ASK_FREE_STOP					4			//ִ�н���ƶ��󣬼�����ƶ�������״̬
#define		STOP_STATUS						5			//�����豸�Ѿ����������Ķ��Ͷϵ��״̬

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			��ʾM22��ǰ״̬�ĳ���  gDriver_Info
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#define		READY_READ_DRIVER_INFO			0
#define		WAIT_RETURN_DRIVER_INFO			1
#define		GET_DRIVER_INFO					2
#define		WAIT_SEND_DRIVER_INFO			3
#define		SEND_DRIVER_INFO_OK				4

#endif