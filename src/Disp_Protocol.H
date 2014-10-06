
#ifndef __Disp_Protocol_H
#define __Disp_Protocol_H

//��ʾ�ն��������ն˷��͵���Ϣ������
#define		DISP_ASK_WATCH_STATUS_UP			0x80			//��ʾ�ն�������״̬
//#define		DISP_RECEIVE_INFO_ECHO_UP			0x81			//��ʾ�ն˽��յ�һ��������������Ϣ���ECHO
#define		DISP_INFO_STATUS_UP					0x0A			//��ʾ�ն˵����е�������Ϣ
#define		DISP_ASK_RECORDINFO_UP				0x09			//��ʾ�ն���Ҫ���¼��ͨѶ������
#define    	DISP_CLRCLESEND_ECHO_UP				0x0B			//��ʾ�ն˶��ڵ������ط��ص���Ӧ
#define		DISP_RECEIVE_IC_INFO_UP				0x11			//��ʾ�ն��յ�IC�����ݺ����Ӧ
#define		DISP_SET_TCPSTATUS_UP				0x12			//��ʾ�ն�����������TCP�Ĺ���״̬
#define		DISP_ASK_PAR_UP						0x13			//��ʾ�ն�Ҫ���ѯ�ն˲���
#define		DISP_SMS_SNED_UP					0xAE			//��ʾ�ն�Ԥ��SMS���ϱ���Ϣ

#define		DISP_SET_ECHO_UP					0xFA			//��ʾ�ն˵���Ӧ(��������������)
//�����Ƕ��������֣���Ҫ���ж���ʾ�ն���Ӧ������е���Ӧ����һ�������֣�
#define		DISP2_SEND_WORD_ECHO_UP				0x4A			//��ʾ�ն˽��յ���������������Ϣ����Ӧ������Ƕ�����Ϣ��ʱ���յ�һ��Ҳ��Ӧ�������ӣ�
#define		DISP2_SEND_WORD_UP					0x4B			//��ʾ�ն˽��յ�ȫ�������е�������Ϣ����Ӧ
#define		DISP2_SET_SMS_ECHO_UP				0xAE			//��ʾ�ն˽��յ�����Ԥ�����ö��ŵ���Ӧ
#define		DISP2_HSET_SMS_ECHO_UP				0x15			//��ʾ�ն˽��������ֱ��˿����õĶ���ϢԤ�õ���Ӧ
#define		DISP2_CLEAR_ECHO_UP					0x03
/////////////////////////////////////////////////////////////////////////////////////////
//��������ʾ�ն˷��͵���Ϣ������
#define		DISP_MAIN_ARALM_DOWN				0x50			//���ط�����ʾ�ն��豸ϵͳ���ڵı�����Ϣ
#define		DISP_MAIN_STATUS_DOWN				0xF7			//��������ʾ�ն˷���ʱ�䣨ÿ���ӽ���һ�Σ�
#define		DISP_ASK_RECORD_INFO_ECHO_DOWM		0x09			//��������ʾ�ն�ת����¼�ǵ�������
#define		DISP_MAIN_SYS_STATUS_DOWN			0x01			//��������ʾ�ն˶��ڷ��͵�ϵͳ״̬
#define		DISP_MAIN_IC_INFO_DOWN				0x02			//��������ʾ�ն˷�����IC����������ݰ�

#define		DISP_MAIN_WORD_DOWN					0xFB			//��������ʾ�ն˷���������Ϣ
#define		DISP_MAIN_INFO_ECHO_DOWN			0x12			//��������ʾ�ն˷��ͽ��մ�����Ϣ����Ӧ
#define		DISP_MAIN_MAIN_DOWN					0x83			//��������ʾת����̬�˵�������
#define		DISP_MAIN_SET_SMS_DOWN				0x15			//��������ʾת���ն�����Ԥ��SMS������
#define		DISP_ASK_PAR_DOWM					0x13			//��������ʾ�ն˷�����ʾ��ѯ����,�����ķ��ؽ��
#define		DISP_RECEIVE_ECHO_DOWN				0x14			//�����յ���ʾ�ն˵���Ҫת������󣬷��ص���Ӧ

#define		DISP_CLEAR_DOWN						0x03			//���س�ʼ����ʾ�ն˵�ָ��


#endif