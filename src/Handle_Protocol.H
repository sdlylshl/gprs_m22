/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						����ϵͳ��HANDLE֮���Э������

���к����е���Ϣ�ṹһ������Ϣͷ����Ϣβ�̶�����Ϣͷ '$'0x24 ��ʾΪ$��ASCII�ַ���
��Ϣ���ȱ�ʾΪ������Ϣ���ĳ��ȣ���������Ϣͷ����Ϣβ����Ϣ�ż�Ϊ�����֣�
����������ִ�ASCII�ַ���ʼ
'1'0x31 һֱ�����š�

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
//--------------�ֱ������ص�������
#ifndef __Handle_Protocol_H
#define __Handle_Protocol_H

/********************************************************\
�ֱ������ص�����
\********************************************************/
//�ֱ�����
#define			HANDLE_DIAL			0x31			//�ֱ�����
#define			HANDLE_ANSWER		0x32			//�ֱ�����
#define			HANDLE_HANGUP		0x33			//�ֱ��һ�
#define			HANDLE_VOL			0x34			//�ֱ���������
#define			HANDLE_DTMF			0x35			//�ֱ�DTMF
#define			HANDLE_READ_DIAL	0x36			//�ֱ������²����绰����
#define			HANDLE_UP_DIAL		0x37			//�ֱ���ǰһ�������绰����
#define			HANDLE_DOWN_DIAL	0x38			//�ֱ�����һ�������绰����
#define			HANDLE_READ_TALK	0x39			//�ֱ�������һ�ε�ͨ����¼
#define			HANDLE_UP_TALK		0x40			//�ֱ���ǰһ��ͨ����¼
#define			HANDLE_DOWN_TALK	0x41			//�ֱ�����һ��ͨ����¼
#define			HANDLE_SEND_TALK	0x42			//�ֱ��͸����ش洢��ͨ����¼
#define			HANDLE_SET_DEFENCE	0x43			//�ֱ��������������
#define		HANDLE_ASK_CENTER_NUM	0x44			//�ֱ���ȡ����Ϣ���ĺ��루��ʱȡ����
#define		HANDLE_SET_CENTER_NUM	0x45			//�ֱ����ö���Ϣ���ĺ��루��ʱȡ����
#define			HANDLE_DEFAULT_SET	0x46			//�ֱ��Գ����ն˽��г�������(��ʱ��Ϊ����ָ��)

#define			HANDLE_STATUS		0x47			//�ֱ������Ƿ񿪻���״̬
#define			HANDLE_PASSWORD		0x48			//�ֱ�������������
#define			HANDLE_FREE_DEFENCE	0x49			//�ֱ�����������
#define			HANDLE_PHONE_NOTE	0x50			//�ֱ���ѯָ��λ�õĵ绰���뱾
#define			HANDLE_ECHO			0x51			//�ֱ�����ķ���

//��������
#define		HANDLE_ERASE_PAGEDATA	0x60			//ͨ���ֱ�(COM3)����һ��ָ������ҳ������
#define		HANDLE_READ_PAGEDATA	0x61			//ͨ���ֱ�(COM3)��ȡһ��ָ������ҳ������
#define		HANDLE_SET_PHONE_NUM	0x62			//ͨ���ֱ�(COM3)����Ŀ��绰����1������
#define		HANDLE_CHECK_FLASH		0x63			//ͨ���ֱ�(COM3)���ͼ������FLASH������
#define		HANDLE_CHECK_GPS		0x64			//ͨ���ֱ�(COM3)���ͼ��GPS���ݸ�ʽ��Ч������
#define		HANDLE_CHECK_SMS		0x65			//ͨ���ֱ�(COM3)ʹGSMģ���Է�����һ��SMS������
#define		HANDLED_ACTIVE_MONITOR	0x66			//ͨ���ֱ�(com3)���м�����·�ĵ���
#define		HANDLE_ERASE_ALLPASS	0x67			//ͨ���ֱ�(COM3)������ʷ�켣�����ݵļ���ҳ
#define		HANDLE_SET_TCP			0x68			//ͨ���ֱ�(COM3)����TCP��ز���
#define		HNADLE_READ_EDITION		0x99

//��������1
#define		HANDLE_ERROR_NUM		0X70
#define		HANDLE_ERROR_CENTER		0X71
#define		HANDLE_ERROR_IP			0X72
#define		HANDLE_ERROR_PORT		0X73
#define		HANDLE_SET_ALARM		0X74
#define 	HANDLE_READ_SRAM		0X81
#define		HANDLE_TEST_SRAM		0x7C			//�����ⲿSRAM


//��������
#define		HANDLE_SET_PAR			0x75			//ͨ���ֱ����û�������
#define		HANDLE_SET_TCPIP		0x76			//ͨ���ֱ�����TCP��صĲ���
#define		HANDLE_SET_TIME			0x77			//ͨ���ֱ�����ʱ��������
#define		HANDLE_ASK_PAR			0x78			//ͨ���ֱ���ѯ�������ò���
#define		HANDLE_SET_SMS			0x79			//ͨ���ֱ�����Ԥ���趨�Ķ���Ϣ
#define		HANDLE_SET_PHONE_NOTE	0x7A			//ͨ���ֱ����ô洢�����ض˵ĵ绰���뱾
#define		HANDLE_SET_DISTANCE		0x7B

#define		HANDLE_CLEAR_DISP		0x80			//ͨ���ֱ���ʼ����ʾ�ն�

/********************************************************\
���ص��ֱ��������ּ���Э���
\********************************************************/

//���ص��ֱ������
#define			MAIN_HELLO				0X31					//��������
#define			MAIN_TALKING			0x32					//�Է�����
#define			MAIN_HANG_UP			0x33					//�Է��һ�
#define			MAIN_PHONE_IN			0x34					//�Է�����
#define			MAIN_BUSY				0x35					//�绰��æ(ռ��)
#define			MAIN_STATUS				0x36					//״̬������ǿ�����������״̬��GPS��״̬
#define			MAIN_TIME				0x37					//һ����ʱ��Ķ�ʱ����
#define			MAIN_TALKRECORD			0x38					//ͨ����¼�ķ���
#define			MAIN_DIALRECORD			0x39					//�����绰����ķ���
#define			MAIN_RETURN_NO_CALLIN	0X40					//���ط��ص绰��ֹ��������
#define			MAIN_RETURN_SETPASSWORD	0x41					//���ط���������������Ƿ�ɹ�
#define			MAIN_RETURN_NO_CALLOUT	0x42					//���ط��ص绰��ֹ��������
#define			MAIN_RETURN_NUM			0x43					//���ط��ض���Ϣ���ĺ��루��ʱȡ����
#define			MAIN_RETURN_SETNUM		0x44					//���ط��ض���Ϣ���ĺ����Ƿ����óɹ�����ʱȡ����
#define			MAIN_RETURN_INIT		0x45					//���ط����ֱ���ʼ��ϵͳ�Ƿ�ɹ�����ʱ��Ϊ����ָ�
#define			MAIN_FREE_DEFENCE		0x46					//�����ֱ��������Ƿ�ɹ�
#define			MAIN_RETURN_PHONE_NOTE	0x47					//���ط��ص�ǰָ��λ�õĺ��뱾
#define			MAIN_RETURN_PHONE_ECHO	0x48					//���ط��ؽ��������ţ��Ҷϵ���Ӧ

//���������
#define			MAIN_ERASEPAGE_ECHO		0x60					//����ϵͳָ��ҳ��ECHO
#define			MAIN_READPAGE_ECHO		0x61					//��ȡϵͳָ��ҳ��ECHO
#define			MAIN_SETNUM_ECHO		0x62					//����Ŀ��绰����1��ECHO
#define			MAIN_CHECKFLASH_ECHO	0x63					//�������FLASH�������ECHO
#define			MAIN_CHECHGPS_ECHO		0x64					//���GPS���ݸ�ʽ����Ч�Ե�ECHO
#define			MAIN_CHECKSMS_ECHO		0x65					//���GSM��SMS���Է����յ�ECHO
#define			MAIN_ERASEPASS_ECHO		0x67					//������ʷ�켣�ļ���ҳ��ECHO
#define			MAIN_SET_TCP_ECHO		0x68					//����TCP��ز�������Ӧ
#define			MAIN_SEND_STATUS		0x69
#define			HANDLE_TEST_SRAM_ECHO	0x7C
#define			HANDLE_CLEAR_DISP_ECHO	0x80


//��������ķ���
#define		HANDLE_SET_PAR_ECHO			0x75			//ͨ���ֱ����û�������
#define		HANDLE_SET_TCPIP_ECHO		0x76			//ͨ���ֱ�����TCP��صĲ���
#define		HANDLE_SET_TIME_ECHO		0x77			//ͨ���ֱ�����ʱ��������
#define		HANDLE_ASK_PAR_ECHO			0x78			//ͨ���ֱ���ѯ���ò����ķ���
#define		HANDLE_SET_SMS_ECHO			0x79			//ͨ���ֱ�����Ԥ���趨�Ķ���Ϣ����Ӧ
#define		HANDLE_SET_PHONE_NOTE_ECHO	0x7A			//ͨ���ֱ����ô洢�����ض˵ĵ绰���뱾����Ӧ
#define		HANDLE_SET_DISTANCE_ECHO	0x7B

#endif