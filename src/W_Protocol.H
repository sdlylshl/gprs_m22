/********************************************************\
*	�ļ�����  W_Protocol.H
*	����ʱ�䣺2004��12��7��
*	�����ˣ�  
*	�汾�ţ�  1.00
*	���ܣ�	  ���峵���ն���ϵͳ����֮�������Э��
*	�ļ����ԣ������ļ�
*	�޸���ʷ����ÿ��������

\********************************************************/

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						GPS���ܼ�ص���ϵͳ---ͨѶЭ��˵���飺
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
һ��ͨѶЭ��ĸ�ʽ���壺
1.1 ��Ϣ�ṹ
	�ֽ���			�ֶ���			����(Byte)			˵��
	0				SOH				1					��Ϣͷ
	1				MSG#			1					��Ϣ��
	2				CMPL-MSG#		1
	3				Length			1					��Ϣ����
	4				ACK				1					��Ӧ��
	5				Class			1					���
	6				gGroup			1					���
	7				gVID(MSB)		1
	8				gVID(LSB)		1					����
	9				Data Block		N					���ݿ飬������
	9+N				CHK(MSB)		1
	10+N			CHK(LSB)		1					У���
	11+N			EOT				1					��Ϣβ

1.2 Э��˵��
	���к����е���Ϣ��������ͬ��ͨ�Ÿ�ʽ�����ݴ������GPRS��ʽ��SMS��ʽ������SMS��ʽ���͵�����
	���140���ֽڡ�

	SOH����Ϣ�ĵ�һ���ֽ� 0xFF;
	ACK: ��Ӧ�ַ������ص���Ϣ�б�������������ACK��
	Length:���ݰ����ȣ���SOH��ʼ��CHK��ǰ�����ݵ��ֽ�����
	[Class]+[gGroup]+[gVID],��4���ֽڣ���ʾһ�������ĳ���ʶ����
	CHK��16-bitУ��ͣ�����SOH��ʼ��CHK��ǰ�������޷���8-bit�ַ���������Ӷ���
	������Ϣ����һ���ֽڵ����ݣ������á���λ��ǰ����λ�ں󡱵�˳����
	EOT��Ϊ��Ϣ�����һ���ֽ�0x0D

����������Ϣ����Э��
	//////////////////////////////////////////////////////////////////////////
	���					��Ϣ��					��Ϣ����

	1						0x21					��ʱӦ������
	2						0x23					��ʱӦ���ɳ�
	3						0x25					ԤԼӦ������
	4						0x27					ԤԼӦ���ɳ�
	5						0x41					�������
	6						0x43					��λ��ѯ
	7						0x45					���ѯ
	8						0x47					��������
	9						0x49					����Ӧ��
	10						0x4b					������Ϣ����
	11						0x4d					������ȡ����
	12						0x4f					������½��Ӧ
	13						0x61					�����г�����
	14						0x63					���ó���Χ
	15						0x65					�绰Ȩ������
	16						0x67					������������
	17						0x81					���ǵǼ���Ӧ
	18						0x82					�����������
	19						0x8a					������Ӧ��
	20						0x8e					�ƶ�����
	21						0x8f					����ƶ�����
	22						0xa1					�����ϴ�λ��ʱ����
	23						0xa5					�¹��ɵ����ݲ�ѯ
	24						0xa9					�ն��Լ챨���ѯ
	25						0xab					����������
	26						0xaf					���ö�̬�˵�
	27						0xb1					�����ŵ�½��Ӧ
	28						0xb3					������ǩ����Ӧ
	29						0xb5					���ó����ն˹�����ʽ
	30						0xb7					���üƼ���������ʽ
	31						0xb9					���ص绰���뱾
	32						0xbb					�������ĺ���
	33						0xbd					ͨ����¼���ݷ���
	34						0xc3					���ó�����������
	35						0xc5					�������ʱ��
	36						0xc7					����ǿ�����״̬
	37						0xcb					���÷�������ַ���˿ں�
	38						0xcf					���³����ն˳���
	39						0xd1					�����ն˵İ汾��ѯ
	40						0xd3					���ü�ʻԱ�б�
	41						0xd5					������̲�ѯ
	42						0xd7					�Ƽ�����ʱӪ��������Ӧ
	43						0xd9					�Ƽ����������ݲ�ѯ
	44						0xdb					�Ƽ�����ʷӪ�����ݲ�ѯ
	45						0xdd					�Ƽ���Уʱ
	46						0xdf					�Ƽ����������
	47						0xe1					ˢ��������������Ӧ
	48						0xe3					ˢ������ʷ�������ݲ�ѯ
	49						0xe5					ˢ�������״�����ݲ�ѯ
	50						0xe7					����ˢ����������
	51						0xe9					ˢ�����������
	/////////////////////////////////////////////////////////////////////////
����������Ϣ����Э��
	//////////////////////////////////////////////////////////////////////////
	���					��Ϣ��					��Ϣ����

	1						0x21					��ʱӦ����Ӧ
	2						0x23					�ӿͳɹ�
	3						0x25					ԤԼӦ����Ӧ
	4						0x41					���������Ӧ--�������
	5						0x43					��λ��ѯ��Ӧ
	6						0x45					���ѯ��Ӧ--��λ����
	7						0x46					���ѯ��Ӧ--���ݷ��ͽ���
	8						0x49					��������
	9						0x4d					����������ȡ��Ӧ
	10						0x4f					������½��Ϣ
	11						0x61					�г�����������Ӧ
	12						0x65					�绰Ȩ��������Ӧ
	13						0x67					��������������Ӧ
	14						0x81					���ǵǼ�
	15						0x83					�����˳���½
	16						0x85					��������
	17						0x87					·����Ϣ
	18						0x89					�¹���Ϣ
	19						0x8a					������
	20						0x8b					ȡ�����
	21						0x8c					���������
	22						0x8d					��������
	23						0x8e					�ƶ�������Ӧ
	24						0x8f					����ƶ�������Ӧ
	25						0xa1					��ʻ״̬����
	26						0xa5					�¹��ɵ�����
	27						0xa9					�Լ챨��
	28						0xab					�����Ķ��ر�
	29						0xaf					��̬�˵�������Ӧ
	30						0xb1					�����ŵ�½
	31						0xb3					������ǩ��
	32						0xb5					���ó����ն˹�����ʽ��Ӧ
	33						0xb7					���üƼ���������ʽ��Ӧ
	34						0xb8					�������ص绰���뱾
	35						0xb9					���ص绰���뱾��Ӧ
	36						0xbb					���ĺ��������Ӧ
	37						0xbd					ͨ����¼����
	38						0xc3					������������������Ӧ
	39						0xc5					�������ʱ����Ӧ
	40						0xc7					����ǿ�����״̬��Ӧ
	41						0xcb					���÷�������ַ���˿ں���Ӧ
	42						0xd1					�����ն˵İ汾����
	43						0xd3					���ؼ�ʻԱ�б�������Ӧ
	44						0xd5					�����������
	45						0xd7					�Ƽ�����ʱӪ������
	46						0xd9					�Ƽ�����������
	47						0xdb					�Ƽ�����ʷӪ������
	48						0xdd					�Ƽ���Уʱ��Ӧ
	49						0xdf					�Ƽ������������Ӧ
	50						0xe1					ˢ������������
	51						0xe3					ˢ������ʷ��������
	52						0xe5					ˢ�������״������
	53						0xe7					ˢ����������������Ӧ
	54						0xe9					ˢ�������������Ӧ
	/////////////////////////////////////////////////////////////////////////

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#ifndef __W_Protocol_H
#define __W_Protocol_H

#define			SOH                     0xff		//��Ϣͷ
#define 		EOT                     0x0d		//��Ϣβ
//������Ϣ����Э��Ķ���
#define			MUSTER_ASK_DOWN			0x21		//��ʱӦ������
#define			MUSTER_SEND_DOWN		0x23		//��ʱӦ���ɳ�
#define			PREMUSTER_ASK_DOWN		0x25		//ԤԼӦ������
#define			PREMUSTER_SEND_DOWN		0x27		//ԤԼӦ���ɳ�
#define			VEHICLE_WATCH_DOWN		0x41		//�������
#define			POSITION_ASK_DOWN		0x43		//��λ��ѯ
#define			TRACK_ASK_DOWN			0x45		//�켣��ѯ
#define			VEHICLE_MONITOR_DOWN	0x47		//��������
#define			ALARM_ECHO_DOWN			0x49		//����Ӧ��
#define			WORD_INFORMATION_DOWN	0x4b		//������Ϣ����
#define			PASSWORD_ASK_DOWN		0x4d		//������ȡ����
#define			VEHICLE_ENTRY_ECHO_DOWN	0x4f		//������¼��Ӧ
#define			SET_ALLOW_AREA_DOWM		0x61		//�����г�����
#define			SET_DISTANCE_DATA_DOWN	0x68		//���ü�������ϴ����ݵ����������

#define			SET_PHONE_LIMIT_DOWN	0x63		//�绰Ȩ������
#define			PARAMETER_SET_DOWN		0x67		//������������
#define			OUTCITY_ECHO_DOWN		0x81		//���ǵǼ���Ӧ
#define			FREE_STRAL_ALARM_DOWN	0x82		//�����������
#define		ALLOW_TCP_QUIT_ECHO_DOWN	0x83		//�����˳�TCP����״̬
#define			ASK_WATCH_ECHO_DOWN		0x8a		//������Ӧ��
#define			SET_STOP_COMMAND_DOWN	0x8e		//�ƶ�����
#define			FREE_STOP_COMMAND_DOWN	0x8f		//����ƶ�����
#define			TIME_INTERVAL_DOWN		0xa1		//�����ϴ�λ��ʱ����
#define			ACCIDENT_DATA_ASK_DOWN	0xa5		//�¹��ɵ����ݲ�ѯ
#define		DEVICE_CHECK_REPORT_DOWN	0xa9		//�ն��Լ챨���ѯ
#define			MESSAGE_FEEDBACK_DOWN	0xab		//�������ı���
#define			SET_MENU_DOWN			0xaf		//���ö�̬�˵�
#define			LAND_ECHO_DOWN			0xb1		//�����ŵ�½��Ӧ
#define			QUIT_ECHO_DOWN			0xb3		//������ǩ����Ӧ
#define			SET_DEVICE_WORKING_DOWN	0xb5		//���ó����ն˹�����ʽ
#define			SET_TAXI_WORKING_DOWN	0xb7		//���üƼ���������ʽ
#define			CARRY_PHONE_NUM_DOWN	0xb9		//���ص绰���뱾
#define			SET_CENTER_NUM_DOWN		0xbb		//�������ĺ���
#define			CALL_LOG_ECHO_DOWN		0xbd		//ͨ����¼���ݷ���
#define			VEHICLE_STARTUP_DOWN	0xc3		//���ó�����������
#define			SET_MAXSPEED_DOWN		0xc5		//�������ʱ��
#define			FORCE_FORTIFY_DOWN		0xc7		//����ǿ�����״̬
#define			SET_ADDRESS_PORT_DOWN	0xcb		//���÷�������ַ���˿�
#define			SET_TCP_ONOROFF_DOWN	0xcd		//�����ն˽���/�˳�������״̬
#define			UPGRADE_SYSTEM_DOWN		0xcf		//���³����ն˳���
#define			QUERY_VERSION_DOWN		0xd1		//�����ն˵İ汾��ѯ
#define			SET_DRIVER_LIST_DOWN	0xd3		//���ü�ʻԱ�б�
#define			SET_DISTANCE_INIT_DOWN	0xD5		//������̳�ʼ����ֵ
#define			EXTER_EQUIPMENT_DOWN	0xd7		//��Ӳ������ݵ�����
#define			TEST_SMS_COMMAND_DOWN	0x10		//��Ϊ���յ�����������
#define			SET_SMS_COMMAND_DOWN	0xae		//��Ϊ����Ϣ�̶����õ������������ת������ʾ�ն�
#define			LINE_DATA_DOWN			0x65		//��·���ݵ���������
#define		LINE_LIMIT_DISTANCE_DOWN	0x66		//��·���ݵ����ƾ��������
/*
#define			TAXI_DATA_ECHO_DOWN		0xd7		//�Ƽ�����ʱӪ��������Ӧ
#define			TAXI_PARAMETER_DOWN		0xd9		//�Ƽ����������ݲ�ѯ
#define			TAXI_PASTDATA_ECHO_DOWN	0xdb		//�Ƽ�����ʷӪ�����ݲ�ѯ
#define			TAXI_CHECKOUT_DOWN		0xdd		//�Ƽ���Уʱ
#define			TAXI_UPGRADE_DOWN		0xdf		//�Ƽ����������
#define			CARD_DATA_ECHO_DOWN		0xe1		//ˢ��������������Ӧ
#define			CARD_PASTDATA_ECHO_DOWN	0xe3		//ˢ������ʷ�������ݲ�ѯ
#define			CARD_DEALDATA_ECHO_DOWN	0xe5		//ˢ�������״�����ݲ�ѯ
#define			LOAD_BLACKLIST_DOWN		0xe7		//����ˢ����������
#define			CARD_UPGRADE_DOWN		0xe9		//ˢ�����������
*/

/////////////////////////////////////////////////////////////////////////////
//������Ϣ����Э��Ķ��壺
#define			MUSTER_ASK_UP			0x21		//��ʱӦ����Ӧ:20
#define			MUSTER_SUCCESS_UP		0x23		//�ӿͳɹ�:12
#define			PREMUSTER_ECHO_UP		0x25		//ԤԼӦ����Ӧ:20

#define			VEHICLE_WATCHECHO_UP	0x41		//���������Ӧ--��λ����:����
#define			POSITION_ASKECHO_UP		0x43		//��λ��ѯ��Ӧ:25
#define			TRACK_ASKECHO_UP		0x45		//�켣��ѯ��Ӧ--��λ����:����
#define			TRACK_ENDECHO_UP		0x46		//�켣��ѯ��Ӧ--���ݷ��ͽ���:8
#define			ALARM_UP				0x49		//��������:����
#define			PASSWORD_ASKECHO_UP		0x4d		//����������ȡ��Ӧ:16
#define			VEHICLE_ENTRY_UP		0x4f		//������¼��Ϣ:8
#define			ALLOW_AREA_ECHO_UP		0x61		//�г�����������Ӧ:8
#define			PHONE_LIMIT_ECHO_UP		0x65		//�绰Ȩ��������Ӧ:8
#define			PARAMETER_SETECHO_UP	0x67		//��������������Ӧ:8
#define			OUTCITY_UP				0x81		//���ǵǼ�:30
#define			VEHICLE_QUIT_UP			0x83		//�����˳���¼:8
#define			VEHICLE_HELP_UP			0x85		//��������:22
#define			VEHICLE_ROAD_UP			0x87		//·����Ϣ:22
#define			VEHICLE_ACCIDENT_UP		0x89		//�¹���Ϣ:22
#define			ASK_WATCH_UP			0x8a		//������:����
#define			ASK_FREEWATCH_UP		0x8b		//ȡ�����:8
#define			ASK_WATCHEND_UP			0x8c		//���������:8
#define			ALARM_END_UP			0x8d		//��������:8
#define		SET_STOP_COMMAND_ECHO_UP	0x8e		//�ƶ�������Ӧ:15
#define		FREE_STOP_COMMAND_ECHO_UP	0x8f		//����ƶ�������Ӧ:15
#define			VEHICLE_RUNDATA_UP		0xa1		//��ʻ״̬����:����
#define			TIME_INTERVAL_ECHO_UP	0xa3		//����ʱ���ϴ��������Ӧ��30
#define			ACCIDENT_DATA_UP		0xa5		//�¹��ɵ�����:����
#define		    DEVICE_CHECK_REPORT_UP	0xa9		//�Լ챨��:12
#define			MESSAGE_FEEDBACK_UP		0xab		//�����Ķ��ر�:19
#define			SET_MENU_ECHO_UP		0xaf		//��̬�˵�������Ӧ:8
#define			LAND_UP					0xb1		//�����ŵ�½:15
#define			QUIT_UP					0xb3		//������ǩ��:15
#define			DEVICE_WORKINGECHO_UP	0xb5		//���ó����ն˹�����ʽ��Ӧ:9
#define			TAXI_WORKINGECHO_UP		0xb7		//���üƼ���������ʽ��Ӧ:9
#define			ASK_PHONE_NUM_UP		0xb8		//�������ص绰���뱾:8
#define			ASK_PHONE_NUMECHO_UP	0xb9		//���ص绰���뱾��Ӧ:8
#define			CENTER_NUMECHO_UP		0xbb		//���ĺ��������Ӧ:8
#define			CALL_LOG_UP				0xbd		//ͨ����¼����:12
#define			VEHICLE_STARTUP_ECHO_UP	0xc3		//������������������Ӧ:9
#define			SET_MAXSPEED_ECHO_UP	0xc5		//�������ʱ����Ӧ:8
#define			FORCE_FORTIFY_ECHO_UP	0xc7		//����ǿ�����״̬��Ӧ:12
#define			ADDRESS_PORT_ECHO_UP	0xcb		//���÷�������ַ���˿���Ӧ������
#define			SET_TCP_ONOROFF_ECHO_UP	0xcd		//������е������ն˽���/�˳���������Ӧ
#define			QUERY_VERSION_ECHO_UP	0xd1		//�����ն˵İ汾����:22
#define			DRIVER_LIST_ECHO_UP		0xd3		//���ؼ�ʻԱ�б�������Ӧ:8
#define			DISTANCE_INIT_ECHO_UP	0xD5		//������̳�ʼ����ֵ
#define			EXTER_EQUIPMENT_UP		0xd7		//��Ӳ������ݵ�����

#define			DISP_ASK_INFO_UP		0x84		//��ʾ�ն˵���Ϣ�ϱ�����
#define			DISP_SMS_INFO_UP		0x86
#define			DISP_INFO_ECHO_UP		0x4b		//��ʾ�ն˵���Ϣ�ϱ�������Ӧ
#define			SET_PHONE_LIMIT_ECHO_UP 0x63
#define			LINE_DATA_ECHO_UP		0x65
#define	 LINE_LIMIT_DISTANCE_ECHO_UP	0x66


/*
*/
#define			SET_DISTANCE_DATA_UP	0x68		//���ü���������Ӧ
#define			ASK_PAR_SET_UP			0xF0

#define			TEST_SMS_COMMAND_UP		0x10		//�ڲ���SMS�У��̶���Ϊ�������ݵ�������
#endif