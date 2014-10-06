/********************************************************\
*	�ļ�����  M22_AT.C
*	����ʱ�䣺2004��12��7��
*	�����ˣ�  
*	�汾�ţ�  1.00
*	���ܣ�	  �й�FLASH��Buffer1����ص�һЩλ�ö���
*	�ļ����ԣ������ļ�
*	�޸���ʷ����ÿ��������
	2004��12��14�գ��޸�
\********************************************************/

#ifndef __M22_AT_H
#define __M22_AT_H

extern const unsigned char  AT[3];								//Communication
extern const unsigned char  ATZ[4];						//�ָ�����ʱ����������
extern const unsigned char  ATE1[5]; 					//Ĭ�ϣ�the echo should be enabled
extern const unsigned char  ATE0[5];
extern const unsigned char  AT_CGMM[8];	//��ʾ��ʾ��M22ģ��
extern const unsigned char  AT_CGMR[8];	//��ʾM22�İ汾��
extern const unsigned char  AT_CGSN[8];	//��ʾM22��Ʒ���к�
extern const unsigned char  AT_CMGF0[10];	//����PDUģʽ
extern const unsigned char  AT_CREG[9];		//��ѯ�źŸ���
extern const unsigned char  AT_COPS_C[9];		//��ѯ��SIM��������״̬
extern const unsigned char  AT_CIMI[8];			//��ʾSIM���ı�ʶ
extern const unsigned char  AT_CSCA_C[9];		//��ʾ�˵��Ĳ������ã������PDF
extern const unsigned char  AT_CSCA[8];
extern const unsigned char  AT_NOSLEEP1[13];	//����ģ�鲻�ô�������״̬
extern const unsigned char  AT_COLP1[10];
extern const unsigned char  AT_CLIP1[10];
extern const unsigned char  AT_AUPATH[14];
extern const unsigned char  AT_AUGAIN[10];
extern const unsigned char  AT_CNMI[18];
extern const unsigned char  AT_TIMEOUT[16];
extern const unsigned char  AT_CMGD[12];

//��绰�йص�ATָ��
extern const unsigned char  ATD[3];				//���Ŵ���
extern const unsigned char  ATH[4];		//�һ�ATָ��
extern const unsigned char  ATA[4];		//����ATָ��

extern const unsigned char  AT_AUVOL[9];	//����������,�����PDF�ĵ�
extern const unsigned char  AT_CSQ[7];
extern const unsigned char  AT_CLCC[8];			//��ѯCall Status Message
extern const unsigned char  AT_VTS[7];					//���������DTMF�Ĵ���
//���½TCP��ص�ָ��
extern const unsigned char  AT_CGDCONT[11];
extern const unsigned char  APN_DEFAULT[14];
extern const unsigned char  AT_CGPCO[21];
extern const unsigned char  AT_DESTINFO[12];
extern const unsigned char  ATD_97[8];
extern const unsigned char  HANG_TCP[3];
extern const unsigned char  ATO[4];
//�뷢��SMS��ص�ATָ��
extern const unsigned char  AT_CMGS[8];

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						Motorola G20---ATָ��س����ַ�˵��
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
//GPSģ�鷵�ص��ַ���
extern const unsigned char String_GPRMC[6];			//GPS�������ݵ�֡ͷ

//M22���صĳ��õ��ַ���
extern const unsigned char String_OK[2];
extern const unsigned char String_ER[5];
extern const unsigned char String_Input_Label[2];
extern const unsigned char String_CLCC[6];
extern const unsigned char String_CMGS[6];
extern const unsigned char String_CSQ[5];
extern const unsigned char String_CSCA[5];
extern const unsigned char String_CREG[6];
extern const unsigned char String_RING[4];
extern const unsigned char String_CLIP[6];
extern const unsigned char String_COLP[6];
extern const unsigned char String_CMT[5];
extern const unsigned char String_NOCARRIER[10];
extern const unsigned char String_L_TCP[16];
extern const unsigned char String_DESTINFO[10];
extern const unsigned char String_COPS[6];
extern const unsigned char String_CONNECT[7];
extern const unsigned char String_BUSY[4];
extern const unsigned char String_NOANSWER[9];
extern const unsigned char String_AT[2];
extern const unsigned char String_END[2];


//������������ʾ���ַ���
extern const unsigned char M22_RESET_ON_OK[21];

//��Ӧ�̣�������̩ɭ�������޹�˾
extern const unsigned char MADE[32];
//��Ʒ�ͺ�
extern const unsigned char MODEL[8];
//��������
extern const unsigned char DESIGN_DATA[8];

extern const unsigned char EDIT_DATE[13];
//�����汾
extern const unsigned char SOFT_EDITION[8];
extern const unsigned char GPS_2005A[16];
extern const unsigned char UPDATE_TIME[21];
//�����ߣ��װ���
extern const unsigned char AUTHOR[16];
//����GPSС�ն�
extern const unsigned char MODEL_TYPE1[13];
//����GPS	GPRS-M22-2005A
extern const unsigned char MODEL_TYPE2[30];

extern const unsigned char R_OVERTOP[24];
extern const unsigned char JR_OVERTOP[24];
extern const unsigned char CLEAR_SRAM[7];
extern const unsigned char SYS_EDITION[2];
extern const unsigned char SYS_TIME[6];
extern const unsigned char SYS_UPDATATIME[6];
//extern const unsigned char LAND_TCP[8];
//extern const unsigned char PASS_W_PAGE[10];
//extern const unsigned char ALL_DISTANCEP[12];
extern const unsigned char MAXSPEED[12];
extern const unsigned char LIMIT_DIS[14];
//extern const unsigned char PASS_R_PAGE[9];
//extern const unsigned char TIME_START[11];
//extern const unsigned char TIME_END[9];
//extern const unsigned char PASS_SEND_ONE_PAGE[19];
//extern const unsigned char PASS_END[18];
extern const unsigned char MAINSYS[26];
//extern const unsigned char PAGE_S[12];
//extern const unsigned char PAGE_E[10];
extern const unsigned char String_A[60];
//extern const unsigned char CHANGE_PAGE[11];
extern const unsigned char String_TestGPRS[4];
extern const unsigned char SMS_Head_1[4];
extern const unsigned char SMS_Head_2[6];
extern const unsigned char TYPE[7];
extern const unsigned char TCP_QUIT[16];
extern const unsigned char TCP_TEST[13];
extern const unsigned char TCP_COMMAAND[18];
extern const unsigned char TRACK[6];
extern const unsigned char ALARM[6];
extern const unsigned char DISTANCE[8];
extern const unsigned char WATCH[5];
extern const unsigned char POWER_OFF[20];
extern const unsigned char PHONE[17];
extern const unsigned char Handle_VOL[5];
extern const unsigned char PRO_EDITION[18];
extern const unsigned char SYS[9];
extern const unsigned char String_EDIT[60];
extern const unsigned char TCP_LAND_ERROR[16];
extern const unsigned char RECEIVE1[16];
extern const unsigned char RECEIVE2[16];
extern const unsigned char GET_DRIVERINFO_OK[18];
extern const unsigned char SEND_DRIVERINFO_OK[19];
extern const unsigned char CENTER_RELOAD[17];
extern const unsigned char OFF_ACC[12];
extern const unsigned char ASK_DRIVERINFO[17];
extern const unsigned char SET_DISTANCE_INFO[14];
extern const unsigned char RESET_PRO[14];
extern const unsigned char CONNCET_TCP[13];
extern const unsigned char INFO_ERROR[16];
extern const unsigned char INFO_DATA[6];
extern const unsigned char DISTANCE_DATA[18];
extern const unsigned char CHECK_TIME[12];
extern const unsigned char DISP_SHOW_ALARM[17];
extern const unsigned char AREA_PAR[9];
extern const unsigned char LINE_PAR[9];

extern const unsigned char LOAD_ERROR[17];


extern const unsigned char VTYPE[5];
extern const unsigned char VKIND[5];
extern const unsigned char VGROUP[6];
extern const unsigned char VID[4];
extern const unsigned char NUM1[5];
extern const unsigned char IP_ADDRESS[3];
extern const unsigned char PORT[5];
extern const unsigned char TCP_APN[4];

extern const unsigned char ON_OFF0[8];
extern const unsigned char ON_OFF1[8];
extern const unsigned char ON_OFF2[8];

extern const unsigned char FLAG[5];
extern const unsigned char TCP_TIME[4];
extern const unsigned char GSM_TIME[4];
extern const unsigned char DIS[4];


extern const unsigned char	PHONE1[3];
extern const unsigned char	PHONE2[3];
extern const unsigned char	PHONE3[3];
extern const unsigned char	PHONE4[4];
extern const unsigned char	PHONE5[4];
extern const unsigned char	PHONE6[4];
extern const unsigned char	PHONE7[11];

#endif