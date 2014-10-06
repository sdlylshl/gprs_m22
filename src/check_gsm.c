
/********************************************************\
*	�ļ�����  Check_GSM.C
*	����ʱ�䣺2004��12��7��
*	�����ˣ�  
*	�汾�ţ�  1.00
*	���ܣ�
*	�ļ����ԣ������ļ�
*	�޸���ʷ����ÿ��������
\********************************************************/
#include <msp430x14x.h>
#include "Uart01.h"
#include "General.h"
#include "Define_Bit.h"
#include "Sub_C.h"
#include "M22_AT.h"
#include "Check_GSM.h"
#include "W_Protocol.h"
#include "SPI45DB041.h"
#include "D_Buffer.h"
#include "Other_Define.h"
#include "TA_Uart.h"
#include "Handle_Protocol.h"
#include "Disp_Protocol.h"
#include "Do_Handle.h"
#include "Do_SRAM.h"
/*
#include "Main_Init.h"
#include "Do_Reset.h"
#include "Do_GSM.h"
#include "Msp430_Flash.h"
*/
#define Debug_GSM_COM1
void Do_SMS_Receive(unsigned int iLength,unsigned char iRLength);
void Do_GPRS_Receive(void);
void Deal_Command(void);
unsigned char Check_Command(void);
void IP_SOCKET_Switch(void);
void Do_Parameter_Set(void);
void Position_Echo(void);
void Pass_Data_Echo(void);
void Set_Cycle_Timer(void);
void Disp_Dispaly_Info(void);
void Set_TCP_OnOrOff(void);
void Query_SYS_Ver(void);
void Deal_Alarm_Echo(void);
void Deal_Vehicle_Monitor(void);

void Set_Phone_Limit(void);

void Set_Area_Limit(void);
void Send_RecordData(void);
void Set_Distance_Par(void);
void Set_Phone_Num(void);
void Watch_Way(void);
void Line_Data_Deal(void);
//unsigned char Do_Deal_Area(unsigned char Add_Flash,unsigned char Add_Buffer );

/********************************************************\
*	��������Check_GSM
	�������ⲿ�ļ�����
*	���ܣ�	�жϴ���GSM���ջ���gReceive0_GSM_Buffer�е����ݣ�����ж���0x0d,0x0a��ʾ��
			һ��GSM���ݷ��ء�����һ������ת�Ƶ�������gGeneral_Buffer�н��д���

*	������  ȫ�ֱ�����GSM�Ľ��ջ���gReceive0_GSM_Buffer[]��ָ��gReceive0_GSM_Buffer_Move
					  gReceive0_GSM_Buffer_Point��
					  ���ݴ�����gGeneral_Buffer[]

*	����ֵ��0����ʾ��GSM���ݽ��ջ���δ���κδ���
			1����ʾת��һ�����ݣ����������
			�����ڷ��صĹ����У���д����ĳ��ڴ�����Ҫ��һЩȫ�ֱ������в����������������ϸ����

*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
unsigned int Check_GSM(void)
{
	unsigned int i,j,k;
	for(k=0;k<sizeof(gGeneral_Buffer);k++)	gGeneral_Buffer[k]=0;
	i = 0;
	j = gReceive0_GSM_Buffer_Move;
	if( gReceive0_GSM_Buffer_Point == gReceive0_GSM_Buffer_Move ) return(0);
	gGeneral_Buffer[i] = gReceive0_GSM_Buffer[j];
	i++;j++;
	if( j >= sizeof(gReceive0_GSM_Buffer) ) j=0;
	while(j!=gReceive0_GSM_Buffer_Point)
	{
		gGeneral_Buffer[i] = gReceive0_GSM_Buffer[j];
		i++;j++;
		if( i >= sizeof(gGeneral_Buffer) )
		{
//			Send_COM1_String((unsigned char *)R_OVERTOP,sizeof(R_OVERTOP));
			for(i=0;i<sizeof(gGeneral_Buffer);i++) 	 	gGeneral_Buffer[i]=0;
			for(i=0;i<sizeof(gReceive0_GSM_Buffer);i++)	gReceive0_GSM_Buffer[i]=0;
			gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
			gReceive0_GSM_Buffer_Point=0;
			gReceive0_GSM_Buffer_Move=0;
			return(0);
		}
		if( j >= sizeof(gReceive0_GSM_Buffer) )  	j=0;

		/********************************************************\
		\********************************************************/
		//�ж��з���SMS�ķ���+++++++++++++++++++++++++++++++++++
		if( (gGeneral_Buffer[i-1]==' ')&&(gGeneral_Buffer[i-2]=='>') )
		{
			#ifdef Debug_GSM_COM1
			Send_COM1_String(gGeneral_Buffer,i);
			#endif
			gReceive0_GSM_Buffer_Move=j;				//��ʾһ����Ч֡���ݴ�����
			if(j==0) gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
			else     gReceive0_GSM_Buffer_End=j-1;
			return(1);
		}
		//�ж���һ����Ч֡����++++++++++++++++++++++++++++++++++++
		else if( (gGeneral_Buffer[i-1]==0x0a)&&(gGeneral_Buffer[i-2]==0x0d) )
		{
			#ifdef Debug_GSM_COM1
			Send_COM1_String(gGeneral_Buffer,i);
			#endif
			gReceive0_GSM_Buffer_Move=j;				//��ʾһ����Ч֡���ݴ�����
			if(j==0) gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
			else     gReceive0_GSM_Buffer_End=j-1;

			//*��������һ���������ݰ��ڻ���General_Buffer[]��,���ݰ��ĳ���Ϊi
			/********************************************************\
			1,�ж��Ƿ��е绰���뷽��Ĵ���
			RING

			+CLIP: "13670155704",129,,,,0

			+CLIP: "13670155704",129,,,,0
			\********************************************************/
			//***************************************************************************
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_RING,sizeof(String_RING))!=0)
			{
				return(0);
			}
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_CLIP,sizeof(String_CLIP))!=0)
			{
				if(   ((gGeneral_Flag&RESET_GSM_ON_F_1)==0 )
					&&((gGeneral_Flag&LAND_TCP_F_1)==0)
					&&((gInternal_Flag&GSM_ONEAT_SUCCESS_1)==0)   )
				{
					if((gGeneral_Flag&RING_F_1)==0)			gHangUp_Timer=0;
					gGeneral_Flag|=TCP_PHONE_F_1;
					gStatus1_Flag|=PHONE_ONLINE_F_1;
					gGeneral_Flag|=RING_F_1;
					return(1);
				}
				else return(0);
			}
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_BUSY,sizeof(String_BUSY))!=0)
			{
				return(1);
			}
			/********************************************************\
			2,�ж��Ƿ���AT��ͷ������ATָ��ķ��ط���Ĵ�����߿���
			  �ȷ�˵ATָ��Ļ����ַ�������0x0d,0x0a
			\********************************************************/
			if(Compare_String(gGeneral_Buffer,(unsigned char *)String_AT,sizeof(String_AT))!=0)
			{
				return(0);
			}
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_END,sizeof(String_END))!=0)
			{
				return(0);
			}
			/********************************************************\
			3,�ж��Ƿ���NO CARRIER ���صĴ���
			\********************************************************/
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_NOCARRIER,sizeof(String_NOCARRIER))!=0)
			{
				if( (gM22_Status==TCP_HANG_WORK) )
				{
					if( (gGeneral_Flag&TCP_PHONE_F_1)==0 )
					{
						gM22_Status=GSM_WORK;
						gGSM_Oper_Type=GSM_PHONE;
						gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
						gPhone_Step=0;gTimer_GSM_AT=0;
					}
					else
					{
						if(gPhone_Status==ACTIVE_MONITOR)
						{
							//��������
							gPhone_Status=READY;
							gGeneral_Flag&=~TCP_PHONE_F_1;
							gStatus1_Flag&=~PHONE_ONLINE_F_1;
							gGeneral_Flag|=MONITOR_RESUME_F_1;
							P6OUT&=~SEL_MIC;
							gGeneral_Flag|=TCP_STATUS_F_1;
						}
						else if( (gPhone_Status==ACTIVE_HANDLE)||(gGeneral_Flag&RING_F_1) )
 						{
							//�ֱ�ͨ������
							gPhone_Status=READY;
							gGeneral_Flag&=~RING_F_1;
							gGeneral_Flag&=~TCP_PHONE_F_1;
							gStatus1_Flag&=~PHONE_ONLINE_F_1;
							//����һ���һ��źŸ��ֱ�
						    Send_COM3_Load(4,MAIN_HANG_UP,0);
							gSys_Handle=MAIN_HANG_UP;
							gGeneral_Flag|=TCP_STATUS_F_1;
						}
						gGeneral_Flag&=~DAILING_UP_F_1;
						gGeneral_Flag&=~MONITOR_ON_F_1;
						gPhone_Status=READY;
						gGeneral_Flag&=~TCP_PHONE_F_1;
						gStatus1_Flag&=~PHONE_ONLINE_F_1;
						gGeneral_Flag&=~RING_F_1;
						gGeneral_Flag|=TCP_STATUS_F_1;
						gCommon_Flag&=~CHECK_RINGPHONE_F_1;
					}
				}
				else if(gM22_Status==GSM_WORK)
				{
					if(gPhone_Status==ACTIVE_MONITOR)
					{
						//��������
						gPhone_Status=READY;
						gGeneral_Flag&=~TCP_PHONE_F_1;
						gStatus1_Flag&=~PHONE_ONLINE_F_1;
					}
					else if( (gPhone_Status==ACTIVE_HANDLE)||(gGeneral_Flag&RING_F_1) )
					{
						//�ֱ�ͨ������
						gPhone_Status=READY;
						gGeneral_Flag&=~RING_F_1;
						gGeneral_Flag&=~TCP_PHONE_F_1;
						gStatus1_Flag&=~PHONE_ONLINE_F_1;
				        Send_COM3_Load(4,MAIN_HANG_UP,0);
				        gSys_Handle=MAIN_HANG_UP;

						//����һ���һ��źŸ��ֱ�
					}
					gGeneral_Flag&=~DAILING_UP_F_1;
					gGeneral_Flag&=~MONITOR_ON_F_1;
					gPhone_Status=READY;
					gGeneral_Flag&=~TCP_PHONE_F_1;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
					gGeneral_Flag&=~RING_F_1;
					gGeneral_Flag|=MONITOR_RESUME_F_1;
					gCommon_Flag&=~CHECK_RINGPHONE_F_1;
				}
				return(1);
			}
			/********************************************************\
			4,�ж��Ƿ���+CMT: ���صĴ���(�����жϽ���SMS����Ϣ����)
			+CMT: ,032
			0891683108705505F0040D91683128782840F90004503011613495000CFF000000000000000000001B
			\********************************************************/
			else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_CMT,sizeof(String_CMT))!=0)
			{
		        gSMS_In_Lenth=0;
		        j=4;k=0;
		        while(1)
		        {
		        	if(gGeneral_Buffer[j]==',')	{k=1;break;}
		        	j++;
		        	if(j>i) break;
		        }
		        if(k==1)
		        {
		        	for(k=0;k<3;k++)
		        	{
						gSMS_In_Lenth*=10;
						gSMS_In_Lenth=gSMS_In_Lenth+(gGeneral_Buffer[k+j+1]-0x30);
		        	}
					gInternal_Flag|=RECEIVE_SMS_F_1;
		        }
				return(0);
			}
			else if(gInternal_Flag&RECEIVE_SMS_F_1)
			{
				gInternal_Flag&=~RECEIVE_SMS_F_1;
				Do_SMS_Receive(i,gSMS_In_Lenth);
				return(0);
			}
			/********************************************************\
			5,����
			\********************************************************/
			return(1);
		}//end else if( (gGeneral_Buffer[i-1]==0x0a)&&(gGeneral_Buffer[i-2]==0x0d) )
	}//end while
	return(0);
}
/********************************************************\
*	��������Check_TCP
	�������ⲿ�ļ�����
*	���ܣ�

*	������  ȫ�ֱ�����GSM�Ľ��ջ���gReceive0_GSM_Buffer[]��ָ��gReceive0_GSM_Buffer_Move
					  gReceive0_GSM_Buffer_Point��
					  ���ݴ�����gGeneral_Buffer[]

*	����ֵ��0����ʾ��GSM���ݽ��ջ���δ���κδ���
			1����ʾת��һ�����ݣ����������
			�����ڷ��صĹ����У���д����ĳ��ڴ�����Ҫ��һЩȫ�ֱ������в����������������ϸ����

*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
	����˵����
\********************************************************/
unsigned int Check_TCP(void)
{
	unsigned int i,j,k,x;
	unsigned char nLength;
	unsigned char nFlag;
	nLength=0;
	//�Ѿ�100ms��δ���յ�M22�����ݣ�������жϽ��ջ������е�����
	if(gM22_TCP_ECHO_Timer>60)
	{
		nFlag=0;
		for(k=0;k<sizeof(gGeneral_Buffer);k++)	gGeneral_Buffer[k]=0;
		i = 0;
		j = gReceive0_GSM_Buffer_Move;
		if( gReceive0_GSM_Buffer_Point == gReceive0_GSM_Buffer_Move ) return(0);
		gGeneral_Buffer[i] = gReceive0_GSM_Buffer[j];
		i++;j++;
		if( j >= sizeof(gReceive0_GSM_Buffer) ) j=0;
		while(j!=gReceive0_GSM_Buffer_Point)
		{
			gGeneral_Buffer[i] = gReceive0_GSM_Buffer[j];
			i++;j++;
			if( i >= sizeof(gGeneral_Buffer) )
			{
//				Send_COM1_String((unsigned char *)R_OVERTOP,sizeof(R_OVERTOP));
				for(i=0;i<sizeof(gGeneral_Buffer);i++) 	 	gGeneral_Buffer[i]=0;
				for(i=0;i<sizeof(gReceive0_GSM_Buffer);i++)	gReceive0_GSM_Buffer[i]=0;
				gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
				gReceive0_GSM_Buffer_Point=0;
				gReceive0_GSM_Buffer_Move=0;
				return(0);
			}
			if( j >= sizeof(gReceive0_GSM_Buffer) )  	j=0;
			//�����ִ�е�������Ѿ������ջ��������ת�Ƶ�gGeneral_Buffer�����С�
			for(k=0;k<i;k++)
			{
				//�ж��Ƿ�Ϊ���е��������ݰ�
				//����FF 4F 0B 08 00 01 01 00 08 02 10 0D
				if( (gGeneral_Buffer[k]==SOH)									//�жϰ�ͷ
					&&(gGeneral_Buffer[gGeneral_Buffer[k+3]+3]==EOT)			//�жϰ�β
					&&(gGeneral_Buffer[k+1]==(char)(~gGeneral_Buffer[k+2])) )	//�ж��������Ƿ�ȡ��
				{
//					Send_COM1_String((unsigned char *)TCP_COMMAAND,sizeof(TCP_COMMAAND));
					nLength=gGeneral_Buffer[k+3]+3+1;	//������Ч���ݰ�����
					j = gReceive0_GSM_Buffer_Move;
					for(x=0;x<nLength+k;x++)
					{
						j++;
						if( j >= sizeof(gReceive0_GSM_Buffer) ) j=0;
					}
					gReceive0_GSM_Buffer_Move=j;		//�ƶ�����������ָ��
					if(j==0) gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
					else     gReceive0_GSM_Buffer_End=j-1;
					//�����ʺ�Do_GPRS_Receive()�����gGeneral_Buffer�������ݰ���
					if(k==0)
					{
						for(x=nLength;x>=1;x--)
						{
							gGeneral_Buffer[x]=gGeneral_Buffer[x-1];
						}
						gGeneral_Buffer[0]=nLength;
					}
					else if(k>0)
					{
						gGeneral_Buffer[k-1]=nLength;
						for(x=0;x<=nLength;x++)
						{
							gGeneral_Buffer[x]=gGeneral_Buffer[k-1+x];
						}
					}
					gGeneral_Flag|=LAND_TCP_SUCCESS_F_1;
					Do_GPRS_Receive();
					nFlag=1;
					break;
				}
				//�ж��Ƿ�ΪFF 0D����������
				else if( (gGeneral_Buffer[0]==0xFF)&&(gGeneral_Buffer[0+1]==0x0D) )
				{
					gGeneral_Flag|=LAND_TCP_SUCCESS_F_1;
					gLand_Interval_Timer=0;
					if(gFF0D_Receive_Timer<=2)
					{
						for(i=0;i<sizeof(gGeneral_Buffer);i++) 	 	gGeneral_Buffer[i]=0;
						for(i=0;i<sizeof(gReceive0_GSM_Buffer);i++)	gReceive0_GSM_Buffer[i]=0;
						gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
						gReceive0_GSM_Buffer_Point=0;
						gReceive0_GSM_Buffer_Move=0;
						return(0);
					}
					else
					{
						Send_COM1_String((unsigned char *)TCP_TEST,sizeof(TCP_TEST));
						j = gReceive0_GSM_Buffer_Move;
						for(x=0;x<2;x++)
						{
							j++;
							if( j >= sizeof(gReceive0_GSM_Buffer) ) j=0;
						}
						gReceive0_GSM_Buffer_Move=j;		//�ƶ�����������ָ��
						if(j==0) gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
						else     gReceive0_GSM_Buffer_End=j-1;
						gFF0D_Receive_Timer=0;
						nFlag=1;
					}
					break;
				}
				//�ж��Ƿ����TCP������Ϣ
				//0D 0A 4F 4B 0D 0A 0D 0A 43 4F 4E 4E 45 43 54 0D 0A 0D 0A 4F
				//4B 0D 0A 0D 0A 4F 4B 0D 0A 0D 0A 4E 4F 20 43 41 52 52 49 45 52 0D 0A
				else if( (gGeneral_Buffer[k]==0x4E)
						 &&(gGeneral_Buffer[k+1]==0x4F)
						 &&(gGeneral_Buffer[k+2]==0x20)
						 &&(gGeneral_Buffer[k+3]==0x43)
						 &&(gGeneral_Buffer[k+4]==0x41)
						 &&(gGeneral_Buffer[k+5]==0x52)
						 &&(gGeneral_Buffer[k+6]==0x52)
						 &&(gGeneral_Buffer[k+7]==0x49)
						 &&(gGeneral_Buffer[k+8]==0x45)
						 &&(gGeneral_Buffer[k+9]==0x52)
						 &&(gGeneral_Buffer[k+10]==0x0D)
						 &&(gGeneral_Buffer[k+11]==0x0A) )
			 	{
			 		//�������������˵���Ѿ�TCP����
			 		Send_COM1_String((unsigned char *)TCP_QUIT,sizeof(TCP_QUIT));
					gGeneral_Flag&=~LAND_TCP_SUCCESS_F_1;
					gGSM_Oper_Type=GSM_PHONE;
					gM22_Status=GSM_WORK;
					gReceive0_GSM_Buffer_Move=gReceive0_GSM_Buffer_Point;
					if(gReceive0_GSM_Buffer_Point==0) 	gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
					else     							gReceive0_GSM_Buffer_End=gReceive0_GSM_Buffer_Point-1;
					gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
					gPhone_Step=0;gTimer_GSM_AT=0;
			 		break;
			 	}
			}//for(k=0;k<i;k++)
		}//end while(j!=gReceive0_GSM_Buffer_Point)
	}//end	gM22_GSM_ECHO_Timer>100
	return(0);
}
/********************************************************\
*	��������Do_SMS_Receive
	�����򣺱��ļ�����
*	���ܣ�  ��G20����AT+CMGRָ���ȡ��SMS�����ݣ����ղ�����

*	������  iLength	 ��Ϊ�����������ݵĳ��ȣ����µ����Ӵ�����Ϊ64
        	iRLength ��ΪAT+CMGR��������SMS�ĳ���Ϊ32
        	ȫ�ֱ�������gGeneral_Buffer

*	����ֵ�����ģ���ǽ�������������ֵ�任��洢��ȫ�ֱ���gGeneral_Buffer�У�������
			Deal_Command()ģ�鴦������
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
	//////////////////////////////////////////////////////////

	AT+CMGR=1
	+CMGR: 0,,24
	08                   0D
	0891683108705505F004 0D91683176105507F4 0008 40 0111900093000462 C94E01 (0D 0A)
	----------20-------- ---------20--------- ------20------------ -----8-------
	0891683108705505F0 04 0D91683128782840F9 0004 40 01 03 41 34 5400 0C FF4FB00800302E303002C40D

	i=HEX(GSM_Echo_Buffer[0],GSM_Echo_Buffer[1]);	����ֵΪ08
	j=(i+2)*2;  20��  (0D�ĵ�һ����ַ)
	i=HEX(GSM_Echo_Buffer[j],GSM_Echo_Buffer[j+1]);	����ֵΪ0D
	j=j+(i+1)/2*2+22; (02�ĵ�һ����ַ)
	i=HEX(GSM_Echo_Buffer[j],GSM_Echo_Buffer[j+1]); ����ֵΪ0C,�����Ϊ���ݰ�������
	j++;
	j++;



	+CMT: ,032
	0891683108705505F0040D91683128782840F90004503011613495000CFF000000000000000000001B
\********************************************************/
void Do_SMS_Receive(unsigned int iLength,unsigned char iRLength)
{
	unsigned int i,j,k;
	unsigned int m,n;
	unsigned char y;
	//�ж�SMS�ĳ�������յ������Ƿ����,�������ݰ��Ƿ���ȫ��iLengthΪ�յ������ݰ���
	//��������iRLength=24,iLength=68
	if(ASCIITOHEX(gGeneral_Buffer[0],gGeneral_Buffer[1])+iRLength+2==iLength/2)
	{
		//��������ȡ������Ȼ�洢��General_Buffer[]������
	    i=ASCIITOHEX(gGeneral_Buffer[0],gGeneral_Buffer[1]);
	    j=(i+2)*2;
	    i=ASCIITOHEX(gGeneral_Buffer[j],gGeneral_Buffer[j+1]);

	    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	    for(m=0;m<sizeof(gNum_Phone_Temp);m++)	gNum_Phone_Temp[m]=0;
	    m=j;					//20
	    n=m+(i+1)/2*2+3;		//37
	    y=0;
		gNum_Phone_Temp[y]=0;
		y++;
		if(gGeneral_Buffer[2]=='9')
		{
			gNum_Phone_Temp[y]='+';
			y++;
		}
		m=j+4;
		while(1)
		{
			if(m<n)
			{
				gNum_Phone_Temp[y]=gGeneral_Buffer[m+1];
				y++;
				gNum_Phone_Temp[y]=gGeneral_Buffer[m];
				y++;
				m++;m++;
			}
			else if(m==n)
			{
				gNum_Phone_Temp[y]=gGeneral_Buffer[m];
				y++;
				m++;
			}
			else break;
		}
		for(m=2;m<y;m++)
		{
			if( (gNum_Phone_Temp[m]>'9')||(gNum_Phone_Temp[m]<'0') )
			break;
		}
		gNum_Phone_Temp[0]=m-1;
		if(gNum_Phone_Temp[0]>14)	gNum_Phone_Temp[0]=0;
		/*
		//��ʾ����ǰ�ĺ���洢���Ƿ���ȷ
	    Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		Send_COM1_Byte(ASCII(gNum_Phone_Temp[0]/10));
		Send_COM1_Byte(ASCII(gNum_Phone_Temp[0]%10));
		Send_COM1_Byte('<');
		for(m=1;m<=gNum_Phone_Temp[0];m++)
		{
			Send_COM1_Byte(gNum_Phone_Temp[m]);
		}
		Send_COM1_Byte('>');
		Send_COM1_Byte(0x0d); Send_COM1_Byte(0x0a);
		*/
		//�����ڻ���gNum_Phone_Temp[]�д洢�����������Ϣ����Դ����
	    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	    j=j+(i+1)/2*2+22;
	    i=ASCIITOHEX(gGeneral_Buffer[j],gGeneral_Buffer[j+1]);//��ʾ���������ݵĳ���
	    //���ʱ��j��ָ������������ĵ�һ���ֽڣ���ʼת��
	    //�洢�����������ĳ��ȣ���������¼���ȵĵ�һ���ֽ�
	    gGeneral_Buffer[0]=i;
		for(k=1;k<=i;k++)
		{
			gGeneral_Buffer[k]=ASCIITOHEX(gGeneral_Buffer[j+2*k],gGeneral_Buffer[j+2*k+1]);
		}
		Deal_Command();
	}
}
/********************************************************\
*	��������Do_GPRS_Receive
	�����򣺱��ļ�����
*	���ܣ�
*	������  ȫ�ֱ�������gGeneral_Buffer

*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Do_GPRS_Receive(void)
{
	Deal_Command();
}
/********************************************************\
*	��������Deal_Command
	�����򣺱��ļ�����
*	���ܣ�	ͨ������Check_Commandģ�飬��������Э��ķ��������ݲ�ͬ��Э�����
			�����ģ���н�����Ӧ�Ĵ���Ͳ����������ÿ����ͬ�������ϸ����
*	������  ȫ�ֱ�������gGeneral_Buffer[]

*	����ֵ�����ݲ�ͬ����Ҫ��һЩȫ�ֱ������½��и�ֵ��������ϸ����ͬ����Ĵ���ʽ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
    ��ϸ˵����
	Deal_Command()��ģ�����˵��:�Ѿ�ͨ������֪���������֣�����Ҫ������Բ�
						ͬ�������ֽ�����Ӧ�Ĳ���
	˵���������Ǵ�GSM����GPRS�������������������������ӳ����ʱ��General_Buffer
		�����д洢���ݰ��ĸ�ʽ��һ���ģ�������
	0891683108705505F004 0D91683176105507F400 0840907271356100 02 00 41 (0x0d 0x0a)
	08                   0D                                    02
	���Ǵ�GSM���յ������ݣ��������ɵĽ��General_Buffer�����д洢�Ľ��Ϊ
	gGeneral_Buffer[0]=2   �����ȣ�
	gGeneral_Buffer[1]=0
	gGeneral_Buffer[2]=41
	ͬ������GPRS���յ�����
	+MIPRTCP: 1,0,0041
	gGeneral_Buffer[0]=2   �����ȣ�
	gGeneral_Buffer[1]=0
	gGeneral_Buffer[2]=41
\********************************************************/
void Deal_Command(void)
{
	unsigned char nCmd;
	unsigned char nSpeed=0;
//	Send_COM1_Byte('S');
	#ifdef Debug_GSM_COM1
	Send_COM1_Byte(ASCII(gGeneral_Buffer[0]/16));
	Send_COM1_Byte(ASCII(gGeneral_Buffer[0]%16));
	Send_COM1_Byte('[');
	for(nCmd=1;nCmd<=gGeneral_Buffer[0];nCmd++)
	{
		if(nCmd==0)	break;
		Send_COM1_Byte(ASCII(gGeneral_Buffer[nCmd]/16));
		Send_COM1_Byte(ASCII(gGeneral_Buffer[nCmd]%16));
		Clear_Exter_WatchDog();
	}
	Send_COM1_Byte(']');
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	#endif

	nCmd=Check_Command();
	//����˵����������ǰ�ļ��ݣ�����������е���Ϣ�У�������о��ȣ�γ�ȵ��������ݣ����ں�������4��״̬�����ֽ�
	switch(nCmd)
	{//�����ɿ������Լ�����
		//1��������½��Ӧ(����״̬λ)
		case	VEHICLE_ENTRY_ECHO_DOWN:
		{
			break;
		}
		//2��������������(����״̬λ)
		case	PARAMETER_SET_DOWN:
		{
			break;
		}
		//3�������ϴ�λ��ʱ���� (����״̬λ������д)
		case	TIME_INTERVAL_DOWN:
		{
			break;
		}
		//4�����÷�������ַ���˿� (����״̬λ)
		case	SET_ADDRESS_PORT_DOWN:
		{
		        break;
		}
		//5��������λ��ѯ ( ��״̬λ )
		case 	POSITION_ASK_DOWN:
		{
			break;
		}
		//6������Ӧ��(����״̬λ)
		case	ALARM_ECHO_DOWN:
		{
			break;
		}
		//7����������(����״̬λ)
		case	VEHICLE_MONITOR_DOWN:
		{
			break;
		}
		//8����Ӳ������������(����״̬λ)
		case	EXTER_EQUIPMENT_DOWN:
		{
			break;
		}
		//9-0,����ʾ�ն���ص���ص�����(������Ϣ�Ĵ���)(����״̬λ)
		case	WORD_INFORMATION_DOWN:
		{
			break;
		}
		//9-1�����ض�̬�˵�
		case	SET_MENU_DOWN:
		{
			break;
		}
		//9-2,�����趨����Ϣ����������
		case	SET_SMS_COMMAND_DOWN:
		{
			break;
		}
		//10,���ǵǼ���Ӧ(����״̬λ)
		case	OUTCITY_ECHO_DOWN:
		{
			break;
		}
		//11,��ʷ�켣��ѯ����������(����״̬λ),
		case	TRACK_ASK_DOWN:
		{
			break;
		}
		//12,�������е������ն˽���/�˳�״̬������	(����״̬λ)����д
		case	SET_TCP_ONOROFF_DOWN:
		{
			break;
		}
		//13,�����ն˵İ汾��ѯ (����״̬λ)
		case	QUERY_VERSION_DOWN:
		{
			break;
		}
		//14�����������˳�TCP��½����Ӧ(����״̬λ)
		case	ALLOW_TCP_QUIT_ECHO_DOWN:
		{
			break;
		}
		//15,������ȡ�������������������(����״̬λ)
		case	PASSWORD_ASK_DOWN:
		{
			break;
		}
		//16������ǿ��������߽�����״̬����������(����״̬λ)
		case	FORCE_FORTIFY_DOWN:
		{
			break;
		}
		//17������ǿ���ƶ�����������(����״̬λ)
		case	SET_STOP_COMMAND_DOWN:
		{
			break;
		}
		//18������ǿ�ƽ���ƶ�����������(����״̬λ)
		case	FREE_STOP_COMMAND_DOWN:
		{
			break;
		}
		//19,�յ��������ʱ�ٵ���������
		case	SET_MAXSPEED_DOWN:
		{
			break;
		}
		//20,���õ绰��Ȩ�޵���������
		case	SET_PHONE_LIMIT_DOWN:
		{
			break;
		}
		//21,�յ�����������̳�ʼֵ����������
		case	SET_DISTANCE_INIT_DOWN:
		{
			break;
		}
		//22,�������ķ������
		case	SET_CENTER_NUM_DOWN:
		{
			break;
		}
		//23,�����������������
		case	SET_ALLOW_AREA_DOWM:
		{
			break;
		}
		//24,���ü������Ĳ���
		case	SET_DISTANCE_DATA_DOWN:
		{
			break;
		}
		//25,���յ����еĵ绰���뱾�Ĳ���
		case	CARRY_PHONE_NUM_DOWN:
		{
			break;
		}
		//26�����յ��ص��ص�����
		case	VEHICLE_WATCH_DOWN:
		{
			break;
		}
		//27�����յ���·ƫ�����ݵ�����
		case	LINE_DATA_DOWN:
		{
			break;
		}
		//28,���յ���·ƫ�����ݵľ�������ֵ������
		case	LINE_LIMIT_DISTANCE_DOWN:
		{
			break;
		}
		//29,�յ�������Ϣ�Ĳ���SMS
		case	TEST_SMS_COMMAND_DOWN:
		{
			break;
		}
		default:
		{
			break;
		}
	}
}
/********************************************************\
*	��������Check_Command
	�����򣺱��ļ�����
*	���ܣ�  ���ȫ�ֱ�������gGeneral_Buffer[]�е����ݽ���������Ч���жϣ������Ч�򷵻�
			��Ч���ݰ��е�������

*	������  ��Ҫ��ȫ�ֱ�������gGeneral_Buffer[]���в���

*	����ֵ��0����Ч�����ݰ�
			gGeneral_Buffer[2]�����ݰ���Ч����������������

*	�����ˣ�
*
*	�޸���ʷ����ÿ��������

	����˵����	У�����ݰ�����ȡ�����֣����жϷ��������֣���Ҫ�����gGeneral_Buffer[]
				gGeneral_Buffer[]�ľ���ֲ����£�
				gGeneral_Buffer[0]��ʾ�������ݰ��ĳ��ȣ���������������ȱ�������ֽڣ�
				gGeneral_Buffer[1]��Ϣͷ0XFF
				gGeneral_Buffer[2]������ gGeneral_Buffer[2]�����ֵ�ȡ��
				............

\********************************************************/
unsigned char Check_Command(void)
{
	unsigned int i,j,k;
	if(gGeneral_Buffer[0]==0)	return(0);
	if(gGeneral_Buffer[1]!=0xFF)	return(0);
	if(gGeneral_Buffer[2]!=(char)(~gGeneral_Buffer[3]) ) return(0);
	//����У���
	i=gGeneral_Buffer[4]+1;							//���ݰ�����
    k=0;
    for( j=1;j<=i;j++ ) k=k+gGeneral_Buffer[j];		//����У���
    if( k/256!=gGeneral_Buffer[gGeneral_Buffer[4]+2] ) return(0);
    if( k%256!=gGeneral_Buffer[gGeneral_Buffer[4]+3] ) return(0);
    return( gGeneral_Buffer[2] );
}
/********************************************************\
*	��������IP_SOCKET_Switch
	�����򣺱��ļ�����
*	���ܣ� 	�����ǽ��ӽ��յ��Ĳ��������е�IP��ַ�Ͷ˿ںţ�long int���͵����ݡ�
			IP��ֱַ����N��ASCII�ַ�����ֻ��Ҫֱ�Ӵ洢��FLASH��buffer1��
			SOCKET��Ϊ��long int����ֵ������Ҫת����ASCLL�ַ�����ʽ��ʾ��������������
			���5λ���ֵ�ASCII�����洢��Flash ��buffer1��
*	������

*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������

	����˵��1:
	long int����ΪIP_Port_Num,�������������ʾ����ֵת��Ϊ���5λ��ASCII������ʽ
	����������Flash��buffer1�д洢�Ͽ�����
	��������ӳ��������յ����е����÷�������ַ�Ͷ˿ں��Ժ󣬴�������ݺ����õ��Ľ��
	�ȴ���IP��ַ�����ݣ�
	������һ�������ֽڣ�General_Buffer[0]��

	����˵��2��	����������ӳ����ʱ���ж���gGeneral_Buffer[]�е����������������÷�������ַ���˿�
				gGeneral_Buffer[]�����ݽṹ�ֲ����£�
				gGeneral_Buffer[0]��ʾ�������ݰ��ĳ��ȣ�����������������ֽڱ���
				gGeneral_Buffer[1]----gGeneral_Buffer[9] 				ǰ��̶��ģ�������Ϣͷ����Ϣ�ŵȵȣ�
				gGeneral_Buffer[10]----gGeneral_Buffer[10+N]			IP��ַ���ַ�������0x00��Ϊ�������ţ�ASCII�ַ�����
				gGeneral_Buffer[10+N+1]----gGeneral_Buffer[10+N+1+4]	Ϊ������ͨѶ�˿ںţ�4���ֽڣ�Ϊһ��long int�����ݣ�

	//����ڶ�����GPRS���ݵ���ϸ����

	//0x7B----0xB6�����洢���������Ϣ�ķ�����Ϣ�����Ȳ��̶�������Э�飬�˴洢�ռ���60���ֽڴ洢
	//#define  ECHO_SMS_2	0x7B
	//0x7B	�洢��ʾ��������SMS�Ƿ���Ϊ1(VAILD_2)���ʾ���滹��δ�����SMS��Ϊ0(NULL_2)���ʾ�Ѿ���������SMS
	//0x7C	�洢���ݰ�����������������ΪnCommand
	//0x7D	�洢���ݰ���ACK��ֵACK
	//0x7E	�洢�������ݰ��ĳ��ȣ���ע��û�д洢CHK(MSB),CHK(LSB),EOT,�������ֽڣ�
	//0x7F  ���⿪ʼ�洢���ݰ�
	����������е����ݰ�Ϊ����gGeneral_Buffer[10]��ʼ
\********************************************************/
void IP_SOCKET_Switch(void)
{
	unsigned int i,j,k;
	unsigned char APN_Length=0;
	unsigned char Check_Data=0;
	//��gGeneral_Buffer[i]�б�ʾIP��ַ�ĵ�һ����
	i=10;
	while(1)
	{
		if(gGeneral_Buffer[i]==0x00) break;
		i++;
		if(i>gGeneral_Buffer[0]) 	return;
	}
	//�洢IP��ַ�ĳ��ȣ����������ȱ���
	SRAM_Init();
	SRAM_Write(GPRS_IP_ADDRESS1_2,i-10);
	//һ��+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//1,IP�ķ�������ַ�Ĵ���
	Check_Data=0;
	for(j=10;j<i;j++)
	{
		SRAM_Write(GPRS_IP_ADDRESS1_2+j-9,gGeneral_Buffer[j]);
		Check_Data += gGeneral_Buffer[j];
	}
	SRAM_Write(GPRS_IP_ADDRESS1_CHECK_2,Check_Data);

	//2����IP��ַ��SOCKET���ݴ洢��buffer1��Ӧ�Ļ������У���Ϊ����gGeneral_Buffer[i]=0x00
	gIP_Port_Num = gGeneral_Buffer[i+1];
	gIP_Port_Num<<=8;
	gIP_Port_Num += gGeneral_Buffer[i+2];
	gIP_Port_Num<<=8;
	gIP_Port_Num += gGeneral_Buffer[i+3];
	gIP_Port_Num<<=8;
	gIP_Port_Num += gGeneral_Buffer[i+4];
	//3����GPRS��APN�����ݴ洢��buffer1��
	k=i+5;
	while(1)
	{
		if(gGeneral_Buffer[k]==0x00) break;
		k++;
		if(k>gGeneral_Buffer[0]) return;
	}
	//4���洢APN��ַ�ĳ��ȣ����������ȱ�������ֽڵ�����
	APN_Length=k-i-5;
	if( (APN_Length>=4)&&(APN_Length<=25) )
	{
		SRAM_Write(GPRS_APN_ADDRESS2_2,APN_Length);
		//5���洢APN������ֵ����Ӧ��BUFFER1��
		Check_Data=0;
		for(j=i+5;j<k;j++)
		{
			SRAM_Write(GPRS_APN_ADDRESS2_2+j-i-4,gGeneral_Buffer[j]);
			Check_Data += gGeneral_Buffer[j];
		}
		SRAM_Write(GPRS_APN_END_2,Check_Data);
	}
	else
	{
		SRAM_Write(GPRS_APN_ADDRESS2_2,sizeof(APN_DEFAULT));
		//5���洢APN������ֵ����Ӧ��BUFFER1��
		Check_Data=0;
		for(i=0;i<sizeof(APN_DEFAULT);i++)
		{
			SRAM_Write(GPRS_APN_ADDRESS2_2+i+1,APN_DEFAULT[i]);
			Check_Data += APN_DEFAULT[i];
		}
		SRAM_Write(GPRS_APN_END_2,Check_Data);
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//�洢��Ϊ��2����Ϣ���͵���Ӧ���ݵĴ洢
	//�������˿����óɹ������Ӧ
	SRAM_Write(ECHO_SMS_2,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,ADDRESS_PORT_ECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(ECHO_SMS_2+3,k-10);
	for(j=10;j<k;j++)
	{
		SRAM_Write(ECHO_SMS_2+4+j-10,gGeneral_Buffer[j]);
	}
	//��Ϊ�˿ں�Ϊһ��long int �͵����ݣ�����Ҫת��ΪASCII�ַ�����ʽ��ʵ��Ӧ��
	//��������ܱ�ʾ5λ�Ķ˿ںţ�
	Check_Data=0;
	SRAM_Write(GPRS_SOCKET1_2+0,ASCII(gIP_Port_Num/10000));
	Check_Data += ASCII(gIP_Port_Num/10000);
	SRAM_Write(GPRS_SOCKET1_2+1,ASCII((gIP_Port_Num%10000)/1000));
	Check_Data += ASCII((gIP_Port_Num%10000)/1000);
	SRAM_Write(GPRS_SOCKET1_2+2,ASCII((gIP_Port_Num%1000)/100));
	Check_Data += ASCII((gIP_Port_Num%1000)/100);
	SRAM_Write(GPRS_SOCKET1_2+3,ASCII((gIP_Port_Num%100)/10));
	Check_Data +=ASCII((gIP_Port_Num%100)/10);
	SRAM_Write(GPRS_SOCKET1_2+4,ASCII(gIP_Port_Num%10));
	Check_Data += ASCII(gIP_Port_Num%10);
	SRAM_Write(GPRS_SOCKET1_END_2,Check_Data);
	gTimer=0;Clear_Exter_WatchDog();
	while(gTimer<100){}
	Clear_Exter_WatchDog();
	//����++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//ע�⣬�����ڳ�����ϵ���ʼ�Ѿ�������FLASH�ĵ�1����2����������
	//����������Ҫ�ڼ������
	//����һ�����������ݵ�buffer2��
//	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO1_ONE_2);OperateSPIEnd();
	Deal_Sector(FLASH_INFO1_ONE_2);
	//1����IP��ַ�����Ѿ�д����Ӧ��buffer2��
	i=10;
	while(1)
	{
		if(gGeneral_Buffer[i]==0x00) break;
		i++;
		if(i>26) return;
	}
	//1-1,дIP��ַ���ݳ���
	OperateSPIEnd();
	WriteOneByteToBuffer2(GPRS_IP_ADDRESS1_F_2,i-10);
	//1-2��дIP��ַ���ݰ�
	for(j=10;j<i;j++)
	{
		WriteNextByteToBuffer2(gGeneral_Buffer[j]);
	}
	OperateSPIEnd();
	gGeneral_Flag|=TCP_IP_VALID_1;
	gStatus2_Flag|=IP_VAILD_F_1;
	//2����SOCKET������д����Ӧ��buffer2��
	i=i+4;//��ʱ��gGeneral_Buffer[i]=0x00
	i++;
	OperateSPIEnd();
	WriteOneByteToBuffer2(GPRS_SOCKET1_F_2,ASCII(gIP_Port_Num/10000));
	WriteNextByteToBuffer2(ASCII((gIP_Port_Num%10000)/1000));
	WriteNextByteToBuffer2(ASCII((gIP_Port_Num%1000)/100));
	WriteNextByteToBuffer2(ASCII((gIP_Port_Num%100)/10));
	WriteNextByteToBuffer2(ASCII(gIP_Port_Num%10));
	OperateSPIEnd();
	//3,��APN������д����Ӧ��buffer2��
	OperateSPIEnd();
	//3-1,д��APN���ݵĳ���
	WriteOneByteToBuffer2(GPRS_APN_DATA_F_2,APN_Length);
	//3-2��дAPN���ݰ�
	for(j=i;j<i+APN_Length;j++)
	{
		WriteNextByteToBuffer2(gGeneral_Buffer[j]);
	}
	OperateSPIEnd();
	RFlash_Sector(FLASH_INFO1_ONE_2);
	Re430_Flash(0);
	if(gM22_Status!=GSM_WORK)	gOther_Flag|=QUIT_TCP_F_1;
	gCheck_Par_Timer=CHECK_SYS_PAR_TIME+1;
	Judge_SMS_Way();
}

/********************************************************\
*	��������Do_Parameter_Set
	�����򣺱��ļ�����
*	���ܣ�  �����Deal_Command()ģ�����ж����ڴ������е�����������Ϊ
			�������ã����������������д����յ��Ĳ�����
			���ͣ������𣬳��ţ�Ŀ�����1(��0x00����)��Ŀ�����2(����)(��0x00����)
			����������Ϣ�Ĵ���һ�Ǵ洢��Flash��Ӧ������һ�Ǵ洢��buffer1��Ӧ������
			ͬʱ���ò���������Ӧ�ı�־�����͵�һ���SMS�����洢��ACK��gSMS_ACK[]��Ӧ������
*	������	ȫ�ֱ���gGeneral_Buffer[]�е�����

*	����ֵ�����кͲ�����ص�����

*	�����ˣ�
*
*	�޸���ʷ����ÿ��������

	����˵����	����������ӳ����ʱ���ж���gGeneral_Buffer[]�е������ǲ������ã���ʱ
				gGeneral_Buffer[]�����ݽṹ�ֲ����£�
				gGeneral_Buffer[0]��ʾ�������ݰ��ĳ��ȣ�����������������ֽڱ���
				gGeneral_Buffer[1]----gGeneral_Buffer[9] 			ǰ��̶��ģ�������Ϣͷ����Ϣ�ŵȵȣ�
				gGeneral_Buffer[10]----gGeneral_Buffer[14]  		��Ҫ���õĳ��ͣ������𣬳���
				gGeneral_Buffer[15]----gGeneral_Buffer[15+N]		���ĵĶ���Ϣ���պ��루��0x00��Ϊ��������
				gGeneral_Buffer[15+N+1]-----gGeneral_Buffer[15+N+M]	���ĵĶ���Ϣ���ձ��ú��� ����0x00��Ϊ��������

	����յ��������õ�������������ȵ���Ҫ�ָ�����ʱ�����ã�����0����������1����������2�������������ERASE
	ִ��������ֻ�д���GSM������ʽ�²Ŵ���
\********************************************************/
void Do_Parameter_Set(void)
{
	//���в���������Ӧ���������ݷ��ͣ�
	//��ʾ��һ��SMS�����ݷ���:����������Ӧ
	//���÷��Ͳ���������Ӧ�ı�־
	Load_Buffer1_Echo(PARAMETER_SETECHO_UP,gGeneral_Buffer[5]);
	Do_Par();
}

/********************************************************\
*	��������Position_Echo
	�����򣺱��ļ�����
*	���ܣ�  �յ����е���Ҫ��λ��ѯ�����������صĴ���
			����ʱ�˿̵�Ч���ľ��ȣ�γ�ȣ�ʱ����Ϣ������ݰ�
			�洢�ڵڶ���SMS���ݵĴ洢����buffer1��0x7B----0xB6����
			0x7B����Ϊ1��VAILD_2��,�������ݰ����δ洢�ں�����ֽڻ����С�
			�������ݰ�����ʽΪ

*	������	ȫ�ֱ���gGeneral_Buffer[]�е�����

*	����ֵ�����кͲ�����ص�����

*	�����ˣ�
*
*	�޸���ʷ����ÿ��������

	����˵����

	����˵����������buffer1�д洢��2��SMS��������ʽ���£�0x7B----0xB6����������������С��
			  0x7B	�洢��ʾ��������SMS�Ƿ���Ϊ1���ʾ���滹��δ�����SMS��Ϊ0���ʾ�Ѿ���������SMS
			  0x7C	�洢���ݰ�����������������ΪnCommand
			  0x7D	�洢���ݰ���ACK��ֵACK
			  0x7E	�洢�������ݰ��ĳ��ȣ���ע��û�д洢CHK(MSB),CHK(LSB),EOT,�������ֽڣ�
			  0x7F  ���⿪ʼ�洢���ݰ�
�����������ݣ�
LONGTITUDE	4 bytes���޷�������	����
LATITUDE	4 bytes���޷�������	γ��
SPEED	1 byte�� �޷�������	�ٶȣ���λ���ף���
AZIMUTH	1 byte�� �޷�������	��λ�ǣ�15����λ����
FLAG	1 byte�� �޷�������	��־
TIME	6 bytes���ַ���	ʱ��(YYMMDDHHMISS)

\********************************************************/
void Position_Echo(void)
{
	unsigned char nCheck=0;
	unsigned int i=0;
	//1��������ĺ�����Ч�����������ʱ�ķ��ط���
	if(   ((gGeneral_Flag&NUM1_VAILD_F_1)==0)
		&&(gNum_Phone_Temp[0]<=14) )
	{
		//1,�洢Ŀ�����
		SRAM_Init();
		SRAM_Write(TARGET_NUMBER1_2+0,gNum_Phone_Temp[0]);
		for(i=1;i<=gNum_Phone_Temp[0];i++)
		{
			SRAM_Write(TARGET_NUMBER1_2+i,gNum_Phone_Temp[i]);
			nCheck += gNum_Phone_Temp[i];
		}
		SRAM_Write(TARGET_NUMBER1_CHECK_2,nCheck);
		//2,�洢���飬���࣬���ͣ�����
		gKind=gGeneral_Buffer[6];
		gGroup=gGeneral_Buffer[7];
		gVID = gGeneral_Buffer[8];
		gVID<<=8;
		gVID += gGeneral_Buffer[9];
		gGeneral_Flag|=NUM1_VAILD_F_1;
	}
	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,POSITION_ASKECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(ECHO_SMS_2+3,17+4);
	Write_PartData_Sram(ECHO_SMS_2+3);	//17
	SRAM_Write(ECHO_SMS_2+3+18,0);
	SRAM_Write(ECHO_SMS_2+3+19,0);
	SRAM_Write(ECHO_SMS_2+3+20,gStatus2_Flag);
	SRAM_Write(ECHO_SMS_2+3+21,gStatus1_Flag);
	Judge_SMS_Way();
}

/********************************************************\
*	��������Set_Cycle_Timer
	�����򣺱����ļ�����
*	���ܣ�	�յ������ϴ�λ��ʱ�����������������һЩ��صĴ���
			һ�Ǹ�������gCycle_GPRS��gCycle_GSM�����Ǵ洢����Ӧ��Flash����
			gCycle_GPRS�����洢������GPRS���ߵ�״̬�·�����ʻ״̬����������Ϣ��ʱ����
			gCycle_GSM�����洢������GSM���ߵĵ�GPRS�����ߵ�״̬�·�����ʻ״̬����������Ϣ��ʱ����

*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Set_Cycle_Timer(void)
{
	unsigned int i=0,j=0;
	unsigned char nACK=0;
	nACK=gGeneral_Buffer[5];
	gCycle_TCP = gGeneral_Buffer[1+9];
	gCycle_TCP <<= 8;
	gCycle_TCP += gGeneral_Buffer[1+10];

	gCycle_GSM = gGeneral_Buffer[1+11];
	gCycle_GSM <<= 8;
	gCycle_GSM += gGeneral_Buffer[1+12];

	if( (gCycle_TCP>0)&&(gCycle_TCP<5) )	gCycle_TCP=5;
	if( (gCycle_GSM>0)&&(gCycle_GSM<5) )	gCycle_GSM=5;

	//�����������������,�򽫼����������Ϊ0
	if( gCycle_TCP>=5 )
	{
		gCycle_Distance=0;
	}
	gPublic_Flag &=~ SET_DISTANCE_OK_F_1;
	gSet_Distance_Info_Count=WAIT_DISPINFO_TIME-2;
//	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO2_ONE_2);OperateSPIEnd();
	Deal_Sector(FLASH_INFO2_ONE_2);
	WriteOneByteToBuffer2(TCP_TIMER_2,gGeneral_Buffer[1+9]);
	WriteNextByteToBuffer2(gGeneral_Buffer[1+10]);
	WriteNextByteToBuffer2(gGeneral_Buffer[1+11]);
	WriteNextByteToBuffer2(gGeneral_Buffer[1+12]);
	WriteNextByteToBuffer2(0);
	WriteNextByteToBuffer2(0);
	OperateSPIEnd();
	RFlash_Sector(FLASH_INFO2_ONE_2);
	Re430_Flash(1);
	OperateSPIEnd();
	Clear_Exter_WatchDog();

	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,TIME_INTERVAL_ECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,nACK);
	SRAM_Write(ECHO_SMS_2+3,21);
	Write_PartData_Sram(ECHO_SMS_2+3);	//17
	SRAM_Write(ECHO_SMS_2+3+18,0);
	SRAM_Write(ECHO_SMS_2+3+19,0);
	SRAM_Write(ECHO_SMS_2+3+20,gStatus2_Flag);
	SRAM_Write(ECHO_SMS_2+3+21,gStatus1_Flag);
	Judge_SMS_Way();

	//1��IP��ַ��Ч��������2��TCP�Ŀ������Ƿ���
	if( (gGeneral_Flag&TCP_IP_VALID_1)&&((gON_OFF_Temp0&TCP_LAND_ON_OFF_1)==0) )
	{
		if( (gCycle_TCP>0)&&(gCycle_Send_Status!=ALARM_DATA)&&(gCycle_Send_Status!=PASS_DATA) )
		{
			gCycle_Send_Status=TRACK_DATA_TIME;
		}
		else if( (gCycle_TCP==0)&&(gCycle_Send_Status==TRACK_DATA_TIME))
		{
			gCycle_Send_Status=NOP_DATA;
		}
	}
	//1��IP��ַ��Ч������TCP�Ŀ������ر�
	else
	{
		if( (gCycle_GSM>0)&&(gCycle_Send_Status!=ALARM_DATA)&&(gCycle_Send_Status!=PASS_DATA) )
		{
			gCycle_Send_Status=TRACK_DATA_TIME;
		}
		else if( (gCycle_GSM==0)&&(gCycle_Send_Status==TRACK_DATA_TIME))
		{
			gCycle_Send_Status=NOP_DATA;
		}
	}
}
/********************************************************\
*	��������Pass_Data_Echo()
	�����򣺱����ļ�����
*	���ܣ�	���յ����еĲ�ѯ��ʷ�켣������
			����Ҫ��ѯ����ʷ�켣����ʼʱ�丽������Check_Track_Time_Start
			����Ҫ��ѯ����ʷ�켣�Ľ���ʱ�丽������Check_Track_Time_end
			Ȼ����������Ҫ������ʷ�켣�����ı�־λ gInternal_Flag|=PASS_DATA_F_1

*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
	����˵�������ʱ����Ҫ�����gGeneral_Buffer[]�����е�����
				SMS_ACK[TRACK_ASKECHO_ACK]=gGeneral_Buffer[5]   �洢ACK


\********************************************************/
void Pass_Data_Echo(void)
{
	
	unsigned int i=0;
	//�洢������ʷ�켣�Ŀ�ʼ��ACK
	gSMS_ACK[TRACK_ASKECHO_ACK]=gGeneral_Buffer[5];
	//�洢������ʷ�켣�Ŀ�ʼʱ��
	gCheck_Track_Time_Start=0;
	gCheck_Track_Time_Start= ( (gGeneral_Buffer[10]%100)/10*16+ (gGeneral_Buffer[10]%10)  );
	gCheck_Track_Time_Start <<= 8;
	gCheck_Track_Time_Start += ( (gGeneral_Buffer[11]%100)/10*16+ (gGeneral_Buffer[11]%10) );
	gCheck_Track_Time_Start <<= 8;
	gCheck_Track_Time_Start += ( (gGeneral_Buffer[12]%100)/10*16+ (gGeneral_Buffer[12]%10) );
	gCheck_Track_Time_Start <<= 8;
	gCheck_Track_Time_Start += ( (gGeneral_Buffer[13]%100)/10*16+ (gGeneral_Buffer[13]%10) );

    //�洢������ʷ�켣�Ľ���ʱ��
    gCheck_Track_Time_end=0;
	gCheck_Track_Time_end= ( (gGeneral_Buffer[15]%100)/10*16+ (gGeneral_Buffer[15]%10) );
	gCheck_Track_Time_end <<= 8;
	gCheck_Track_Time_end += ( (gGeneral_Buffer[16]%100)/10*16+ (gGeneral_Buffer[16]%10) );
	gCheck_Track_Time_end <<= 8;
	gCheck_Track_Time_end += ( (gGeneral_Buffer[17]%100)/10*16+ (gGeneral_Buffer[17]%10) );
	gCheck_Track_Time_end <<= 8;
	gCheck_Track_Time_end+= ( (gGeneral_Buffer[18]%100)/10*16+ (gGeneral_Buffer[18]%10) );;


	//������յ�����ʷ�켣��ʼ��ѯʱ��ͽ���ʱ����ͬ�����ʾ���յ����ǽ�����ʷ�켣��ѯ������
	if(gCheck_Track_Time_Start==gCheck_Track_Time_end)
	{
		gCycle_Send_Status=NOP_DATA;
		gCircle_Buffer_Point=0;
		for(i=0;i<sizeof(gCircle_Buffer);i++)	gCircle_Buffer[i]=0;
		gOther_Flag&=~FIND_PASSPAGE_F_1;gOther_Flag&=~FIRST_PASSPAGE_F_1;gOther_Flag&=~SEND_PASSPAGE_F_1;
		gOther_Flag&=~PASS_DATA_LASE_F_1;
		gOther_Flag&=~SEND_PASSPAGE_F_1;
		//���÷�����ʷ�켣�����ı�־
		Load_Buffer1_Echo(TRACK_ENDECHO_UP,gGeneral_Buffer[5]);
		Send_COM1_String( (unsigned char *)PASS_END,sizeof(PASS_END) );
		return;
	}
	if (gCycle_Send_Status!=ALARM_DATA )
	{

		gCycle_Send_Status=PASS_DATA;gOther_Flag|=FIND_PASSPAGE_F_1;
		gOther_Flag&=~FIRST_PASSPAGE_F_1;gOther_Flag&=~SEND_PASSPAGE_F_1;
		if(gPassTrack_Store_Sector==FLASH_PASTDATA_E_2-1)
		{
			gPassPage_First=FLASH_PASTDATA_S_2;
			gPassPage_End=FLASH_PASTDATA_E_2-1;
		}
		else if(gPassTrack_Store_Sector>=FLASH_PASTDATA_E_2)
		{
			gPassPage_First=FLASH_PASTDATA_S_2+1;
			gPassPage_End=FLASH_PASTDATA_E_2;
		}
		else
		{
			gPassPage_First=gPassTrack_Store_Sector+2;
			gPassPage_End=FLASH_PASTDATA_E_2;
		}
	}
	#ifdef Debug_GSM_COM1
	Send_COM1_String( (unsigned char *)TIME_START,sizeof(TIME_START) );
	Send_COM1_Byte('[');
   	Send_COM1_Byte(ASCII(gCheck_Track_Time_Start/0x10000000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_Start%0x10000000)/0x1000000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_Start%0x1000000)/0x100000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_Start%0x100000)/0x10000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_Start%0x10000)/0x1000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_Start%0x1000)/0x100));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_Start%0x100)/0x10));
	Send_COM1_Byte(ASCII(gCheck_Track_Time_Start%0x10));
	Send_COM1_Byte(']');
	Send_COM1_String( (unsigned char *)TIME_END,sizeof(TIME_END) );
	Send_COM1_Byte('[');
   	Send_COM1_Byte(ASCII(gCheck_Track_Time_end/0x10000000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_end%0x10000000)/0x1000000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_end%0x1000000)/0x100000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_end%0x100000)/0x10000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_end%0x10000)/0x1000));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_end%0x1000)/0x100));
    Send_COM1_Byte(ASCII((gCheck_Track_Time_end%0x100)/0x10));
	Send_COM1_Byte(ASCII(gCheck_Track_Time_end%0x10));
	Send_COM1_Byte(']');
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);

	Send_COM1_String( (unsigned char *)PAGE_S,sizeof(PAGE_S) );
	Send_COM1_Byte('[');
    Send_COM1_Byte(ASCII(gPassPage_First/1000));
    Send_COM1_Byte(ASCII((gPassPage_First%1000)/100));
    Send_COM1_Byte(ASCII(((gPassPage_First%1000)%100)/10));
	Send_COM1_Byte(ASCII(gPassPage_First%10));
	Send_COM1_Byte(']');
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	Send_COM1_String( (unsigned char *)PAGE_E,sizeof(PAGE_E) );
	Send_COM1_Byte('[');
    Send_COM1_Byte(ASCII(gPassPage_End/1000));
    Send_COM1_Byte(ASCII((gPassPage_End%1000)/100));
    Send_COM1_Byte(ASCII(((gPassPage_End%1000)%100)/10));
	Send_COM1_Byte(ASCII(gPassPage_End%10));
	Send_COM1_Byte(']');
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	#endif
	Judge_SMS_Way();
	
}

/********************************************************\
*	��������Disp_Dispaly_Info();
	�����򣺱����ļ�����
*	���ܣ�
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Disp_Dispaly_Info(void)
{
    unsigned int i=0;

	if(gDisp_Buffer_Point+gGeneral_Buffer[0]+2+6<=sizeof(gDisp_Buffer))
	{
	    gDisp_Buffer[gDisp_Buffer_Point]=gGeneral_Buffer[0]+2+6;
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]=0;
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]='@';
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]='%';
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]=gGeneral_Buffer[0]+2+4;
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]=DISP_MAIN_WORD_DOWN;
	    gDisp_Buffer_Point++;
	    //��0xFF��ʼ�洢����(ת���������ĵ����ݰ�)
	    Copy_String(gGeneral_Buffer+1,gDisp_Buffer+gDisp_Buffer_Point,gGeneral_Buffer[0]);
	    gDisp_Buffer_Point=gDisp_Buffer_Point+gGeneral_Buffer[0];
	    gDisp_Buffer[gDisp_Buffer_Point]='$';
	    gDisp_Buffer_Point++;
	    gDisp_Buffer[gDisp_Buffer_Point]='&';
	    gDisp_Buffer_Point++;
	}
	//��������ʾ������Ϣ
	/*
	#ifdef Debug_GSM_COM1
	Send_COM1_Byte(ASCII(gDisp_Buffer[0]/16));
	Send_COM1_Byte(ASCII(gDisp_Buffer[0]%16));
	Send_COM1_Byte('=');
	Send_COM1_Byte(ASCII(gDisp_Buffer_Point/16));
	Send_COM1_Byte(ASCII(gDisp_Buffer_Point%16));
	Send_COM1_Byte('{');
	for(i=0;i<gDisp_Buffer[0];i++)
	{
		Send_COM1_Byte(ASCII(gDisp_Buffer[i]/16));
		Send_COM1_Byte(ASCII(gDisp_Buffer[i]%16));
	}
	Send_COM1_Byte('}');
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	#endif
	*/
}
/********************************************************\
*	��������Set_TCP_OnOrOff();
	�����򣺱����ļ�����
*	���ܣ�
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
	����˵�������յ����е������ý���/�˳�����״̬�����
				��Ҫ�������¼����������洢��Ҫ��Ӧ��ACK
				1���ı����TCP�Ŀ����������洢����0
				2��д��B��������������Ļ�Ӧ��������
\********************************************************/
void Set_TCP_OnOrOff(void)
{
	unsigned int i=0,j=0;
	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,SET_TCP_ONOROFF_ECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(ECHO_SMS_2+3,1);
	SRAM_Write(ECHO_SMS_2+4,gGeneral_Buffer[10]);
	Judge_SMS_Way();
}
/********************************************************\
*	��������Query_SYS_Ver();
	�����򣺱����ļ�����
*	���ܣ�
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Query_SYS_Ver(void)
{
	unsigned int i=0;

	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,QUERY_VERSION_ECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(ECHO_SMS_2+3,14);
	SRAM_Write(ECHO_SMS_2+4,SYS_EDITION[0]);
	SRAM_Write(ECHO_SMS_2+5,SYS_EDITION[1]);
	for(i=0;i<6;i++)
	{
		SRAM_Write(ECHO_SMS_2+6+i,SYS_TIME[i]);
	}
	for(i=0;i<6;i++)
	{
		SRAM_Write(ECHO_SMS_2+12+i,SYS_UPDATATIME[i]);
	}
	Judge_SMS_Way();
}

/********************************************************\
*	��������Deal_Alarm_Echo();
	�����򣺱����ļ�����
*	���ܣ�
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Deal_Alarm_Echo(void)
{
	unsigned int i=0,j=0;
	gSMS_ACK[ALARM_ACK]=gGeneral_Buffer[5];
	gCycle_Alarm = gGeneral_Buffer[10];
	gCycle_Alarm <<= 8;
	gCycle_Alarm += gGeneral_Buffer[11];
	if( gCycle_Alarm == 0)
	{
		gAlarm_Type=NOP_ALARM;
		gCycle_Send_Status=NOP_DATA;
		gAlarm_Count=0;
		gStatus2_Flag&=~ROB_ALARM_F_1;
		Load_Buffer1_Echo(ALARM_END_UP,gGeneral_Buffer[5]);
	}
	else if( gCycle_Alarm <= 5 )	gCycle_Alarm=5;
}

/********************************************************\
*	��������Deal_Vehicle_Monitor();
	�����򣺱����ļ�����
*	���ܣ�
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Deal_Vehicle_Monitor(void)
{
	unsigned int i=0,j=0;
	i=10;j=0;
	while(gGeneral_Buffer[i]!=0x00)
	{
		i++;j++;if(j>16) {j=0;break;}
	}
	//���յ����еļ������
	if(j>4)
	{
		SRAM_Init();
		SRAM_Write(PHONE_AT_2+0,VAILD_2);
		SRAM_Write(PHONE_AT_2+1,M_DIAL);
		SRAM_Write(PHONE_AT_2+2,j);
		for(i=0;i<j;i++)
		{
			SRAM_Write(PHONE_AT_2+3+i,gGeneral_Buffer[i+10]);
		}
	}
}


/********************************************************\
*	��������Defence_Password_Ask_Echo();
	�����򣺱����ļ�����
*	���ܣ�
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/

void Defence_Password_Ask_Echo(void)
{
	unsigned int i=0;
	unsigned char OldPassWord_Length=0;

	//��ȡFLASH��Ӧ�洢���������������Ϣ,��������ʱ�洢��gGeneral_Buffer[]��
	OperateSPIEnd();
	ReadOnePageToBuffer2(FLASH_INFO1_ONE_2);
	OperateSPIEnd();
	gGeneral_Buffer[0]=ReadByteFromBuffer2(PASSWORD1_F_2);
	for(i=1;i<6;i++)
	{
		gGeneral_Buffer[i]=ReadNextFromBuffer2();
	}
	OperateSPIEnd();

	//������ʾ�������������Ƿ���ȷ
	#ifdef Debug_GSM_COM1
	Send_COM1_Byte('P');Send_COM1_Byte('[');
	for(i=0;i<6;i++)	Send_COM1_Byte(gGeneral_Buffer[i]);
	Send_COM1_Byte(']');Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	#endif

	//����������洢��gGeneral_Buffer[0]---gGeneral_Buffer[5]
	for(i=0;i<6;i++)
	{
		if( (gGeneral_Buffer[i]<'0')||(gGeneral_Buffer[i]>'9') )	break;
	}
	//֪������ĳ���
	OldPassWord_Length=i;

	//������ʾ���������
	#ifdef Debug_GSM_COM1
	Send_COM1_Byte(ASCII(OldPassWord_Length/10));
	Send_COM1_Byte(ASCII(OldPassWord_Length%10));
	Send_COM1_Byte('[');
	for(i=0;i<OldPassWord_Length;i++)	Send_COM1_Byte(gGeneral_Buffer[i]);
	Send_COM1_Byte(']');Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	#endif
	SRAM_Init();
	SRAM_Write(OTHER_SMS_2+0,VAILD_2);
	SRAM_Write(OTHER_SMS_2+1,PASSWORD_ASKECHO_UP);
	SRAM_Write(OTHER_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(OTHER_SMS_2+3,OldPassWord_Length+1);
	for(i=0;i<OldPassWord_Length;i++)
	{
		SRAM_Write(OTHER_SMS_2+4+i,gGeneral_Buffer[i]);
	}
	SRAM_Write(OTHER_SMS_2+4+OldPassWord_Length,0x00);
	Judge_SMS_Way();
}


/********************************************************\
*	��������Set_Phone_Limit();
	�����򣺱����ļ�����
*	���ܣ�
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Set_Phone_Limit(void)
{
	unsigned char nACK=0;
	nACK=gGeneral_Buffer[5];
	//1�������ж�
	//�жϵ绰�������ƵĹ���
	if(gGeneral_Buffer[10]==0)
	{
		//�绰�����ֹ
		gON_OFF_Temp0&=~PHONE_IN_ON;
		gStatus2_Flag|=HANDLE_PHONE_IN_LIMIT;
	}
	else if(gGeneral_Buffer[10]==0xFF)
	{
		//�绰��������
		gON_OFF_Temp0|=PHONE_IN_ON;
		gStatus2_Flag&=~HANDLE_PHONE_IN_LIMIT;
	}
	//�жϵ绰�������ƵĹ���
	if(gGeneral_Buffer[11]==0)
	{
		//�绰������ֹ
		gON_OFF_Temp0&=~PHONE_OUT_ON;
		gStatus2_Flag|=HANDLE_PHONE_OUT_LIMIT;
	}
	else if(gGeneral_Buffer[11]==0xFF)
	{
		//�绰��������
		gON_OFF_Temp0|=PHONE_OUT_ON;
		gStatus2_Flag&=~HANDLE_PHONE_OUT_LIMIT;
	}
	//2��������״̬�洢
	//����״̬�ĸı䣬����Ҫ���¸�������5�е����ݣ������¼���У��ˣ�д������
//	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO2_ONE_2);OperateSPIEnd();
	Deal_Sector(FLASH_INFO2_ONE_2);
	WriteOneByteToBuffer2(ON_OFF_V1_2,gON_OFF_Temp0);
	OperateSPIEnd();
	RFlash_Sector(FLASH_INFO2_ONE_2);
	Re430_Flash(1);
	//3,����������Ӧ
	//�����Ǵ�����Ҫ���ͽ��յ��ն˽���/�˳�����״̬����Ӧ
	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,SET_PHONE_LIMIT_ECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,nACK);
	SRAM_Write(ECHO_SMS_2+3,2);
	SRAM_Write(ECHO_SMS_2+4,gGeneral_Buffer[10]);
	SRAM_Write(ECHO_SMS_2+5,gGeneral_Buffer[11]);
	gGeneral_Flag&=~LAND_TCP_ERROR_F_1;
	Judge_SMS_Way();
}
/********************************************************\
*	��������Set_Center_Num();
	�����򣺱����ļ�����
*	���ܣ�
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/

void Set_Center_Num(void)
{
	unsigned int i=0,j=0,k=0;
	unsigned char cNum1,cNum2,cNum3;
	//�ȶ���Flash��Ӧ�ĵ�5�����������ݵ�buffer2��
	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO2_ONE_2);OperateSPIEnd();
	//1������������ķ������
	//========================================
	i=1+9;
	while(1)
	{
		if(gGeneral_Buffer[i]==0x00)	break;
		i++;
		if(i>1+9+15) return;
		//�����15��ʾĿ�����1���ᳬ��15����
	}
	cNum1=i-(1+9);
	//������ķ������ĳ���û�г���15����Ŀ�������Ч
	if( (i<=1+9+15)&&(cNum1>0) )
	{
		//����������ķ���ĺ��볤�Ȳ�д��buffer2����Ӧ��λ��
		OperateSPIEnd();
		WriteOneByteToBuffer2(CENTER_NUM_F_2,cNum1);
		for(j=1+9;j<i;j++)
		{
			WriteNextByteToBuffer2(gGeneral_Buffer[j]);
		}
		OperateSPIEnd();
	}
	OperateSPIEnd();
	//2��������ձ����绰����
	//========================================
	j=i+1;
	while(1)
	{
		if(gGeneral_Buffer[j]==0x00)	break;
		j++;
		if(j>1+i+15)	return;
	}
	cNum2=j-i;
	if( (j<=1+i+15)&&(cNum2>0) )
	{
		//������˱�������ĺ��볤�Ȳ�д��buffer2
		OperateSPIEnd();
		WriteOneByteToBuffer2(ARALM_NUM_2,cNum2);
		for(k=i+1;k<j;k++)
		{
			WriteNextByteToBuffer2(gGeneral_Buffer[k]);
		}
		OperateSPIEnd();
	}
	OperateSPIEnd();
	//3��������ռ����绰����
	//========================================
	k=j+1;
	while(1)
	{
		if(gGeneral_Buffer[k]==0x00)	break;
		k++;if(k>1+j+15)	return;
	}
	cNum3=k-j;
	if( (k<=j+1+15)&&(cNum3>0) )
	{
	}
	//4��Ԥ���绰����
	//========================================
	//5�������µ���������д����Ӧ��Flash
	OperateSPIEnd();
	RFlash_Sector(FLASH_INFO2_ONE_2);
	Re430_Flash(1);
	//6�����մ�����ϣ�����������Ӧ
	//========================================
	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,CENTER_NUMECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(ECHO_SMS_2+3,21);
	Write_PartData_Sram(ECHO_SMS_2+3);
	SRAM_Write(ECHO_SMS_2+3+18,0);
	SRAM_Write(ECHO_SMS_2+3+19,gStatus3_Flag);
	SRAM_Write(ECHO_SMS_2+3+20,gStatus2_Flag);
	SRAM_Write(ECHO_SMS_2+3+21,gStatus1_Flag);
	Judge_SMS_Way();
}

/********************************************************\
*	��������Set_Area_Limit();
	�����򣺱����ļ�����
*	���ܣ�
*	������  ��
*	����ֵ����
*	�����ˣ�
*	�޸���ʷ����ÿ��������
	�˳���Ĵ����ǽ���Ӧ����������洢���ⲿSRAM�У���buffer2�С�������gArea_Par��ֵ
\********************************************************/
void Set_Area_Limit(void)
{
	unsigned int  i=0;
	unsigned char Buffer2_Temp=0;
	gArea_Par=0;
	//1,�洢�ⲿ��FLASH�С�
	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_AREA_ONE_2);OperateSPIEnd();
	WriteOneByteToBuffer2(FAREA1_LON_1_2,gGeneral_Buffer[1+9]);
	for(i=1;i<68;i++)
	{
		WriteNextByteToBuffer2(gGeneral_Buffer[1+9+i]);
	}
	OperateSPIEnd();
	RFlash_Sector(FLASH_AREA_ONE_2);
	//3���洢�ⲿ��SRAM�е���Ӧλ��
	SRAM_Init();
	SRAM_Write( SAREA_DATA_FS,0x55);						//�洢�������Ч��־
	for(i=0;i<68;i++)
	{
		SRAM_Write( (SAREA1_LON_1_2+i),gGeneral_Buffer[1+9+i]);
	}
	SRAM_Write( SAREA_DATA_FE,0xAA);
	//4,��ʾ��ǰ����Ĳ���
	gArea_Par=0;
	SRAM_Init();
	Buffer2_Temp=SRAM_Read(SAREA1_PAR_2);
	if( Buffer2_Temp==0x55)		{gArea_Par|=AREA1_ON;}
	else if(Buffer2_Temp==0xAA)	{gArea_Par|=AREA1_ON;gArea_Par|=AREA1_DIR;}
	Buffer2_Temp=SRAM_Read(SAREA2_PAR_2);
	if( Buffer2_Temp==0x55)		{gArea_Par|=AREA2_ON;}
	else if(Buffer2_Temp==0xAA)	{gArea_Par|=AREA2_ON;gArea_Par|=AREA2_DIR;}
	Buffer2_Temp=SRAM_Read(SAREA3_PAR_2);
	if( Buffer2_Temp==0x55)		{gArea_Par|=AREA3_ON;}
	else if(Buffer2_Temp==0xAA)	{gArea_Par|=AREA3_ON;gArea_Par|=AREA3_DIR;}
	Buffer2_Temp=SRAM_Read(SAREA4_PAR_2);
	if( Buffer2_Temp==0x55)		{gArea_Par|=AREA4_ON;}
	else if(Buffer2_Temp==0xAA)	{gArea_Par|=AREA4_ON;gArea_Par|=AREA4_DIR;}
	//4�����յ������������Ӧ
	Load_Buffer1_Echo(ALLOW_AREA_ECHO_UP,gGeneral_Buffer[5]);
	if(gArea_Par!=0)	gStatus2_Flag|=AREA_VAILD_F_1;
	else				gStatus2_Flag&=~AREA_VAILD_F_1;
}

/********************************************************\
*	��������Send_RecordData();
	�����򣺱����ļ�����
*	���ܣ�
*	������  ��
*	����ֵ����
*	�����ˣ�
*	�޸���ʷ����ÿ��������
	����˵�������ⲿ���ݴ�����������͸���¼�ǣ�Ȼ��ȴ�����
\********************************************************/
void Send_RecordData(void)
{
    unsigned int i=0;
	gSMS_ACK[RECORD_EQUIPMENT_ACK]=gGeneral_Buffer[5];
	for(i=11;i<=gGeneral_Buffer[0]-3;i++)
	{
		Send_COM1_Byte(gGeneral_Buffer[i]);
	}
	/*
	#ifdef Debug_GSM_COM1
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	for(i=11;i<=gGeneral_Buffer[0]-3;i++)
	{
		Send_COM1_Byte(ASCII(gGeneral_Buffer[i]/0x10));
		Send_COM1_Byte(ASCII(gGeneral_Buffer[i]%0x10));
	}
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	#endif
	*/

}
/********************************************************\
*	��������Set_Distance_Par();
	�����򣺱����ļ�����
*	���ܣ�
*	������  ��
*	����ֵ����
*	�����ˣ�
*	�޸���ʷ����ÿ��������

\********************************************************/
void Set_Distance_Par(void)
{
	gCycle_Distance = gGeneral_Buffer[1+9];
	gCycle_Distance <<= 8;
	gCycle_Distance += gGeneral_Buffer[1+10];

	if(gGeneral_Buffer[1+11]==1)
	{
		gON_OFF_Temp1 &=~ DISTANCE_TCP_ON_1;
		gStatus3_Flag |= DISTANCE_ON_F_1;
	}
	else if(gGeneral_Buffer[1+11]==0)
	{
		gON_OFF_Temp1 |= DISTANCE_TCP_ON_1;
		gStatus3_Flag &=~ DISTANCE_ON_F_1;
	}
	gCycle_TCP=0;
	gCycle_GSM=0;
	gCycle_Send_Status = NOP_DATA;
	Load_Buffer1_Echo(SET_DISTANCE_DATA_UP,gGeneral_Buffer[5]);
	//����״̬�ĸı䣬����Ҫ���¸�������0�е����ݣ������¼���У��ˣ�д������
	//OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO2_ONE_2);OperateSPIEnd();
	Deal_Sector(FLASH_INFO2_ONE_2);
	WriteOneByteToBuffer2(ON_OFF_V2_2,gON_OFF_Temp1);
	OperateSPIEnd();
	WriteOneByteToBuffer2(TCP_TIMER_2,0);
	WriteNextByteToBuffer2(0);
	WriteNextByteToBuffer2(0);
	WriteNextByteToBuffer2(0);
	WriteNextByteToBuffer2(gGeneral_Buffer[1+9]);
	WriteNextByteToBuffer2(gGeneral_Buffer[1+10]);
	OperateSPIEnd();
	RFlash_Sector(FLASH_INFO2_ONE_2);
	Re430_Flash(1);
	gPublic_Flag &=~ SET_DISTANCE_OK_F_1;
	gSet_Distance_Info_Count=WAIT_DISPINFO_TIME-2;

}
/********************************************************\
*	��������Set_Phone_Num();
	�����򣺱����ļ�����
*	���ܣ�
*	������  ��
*	����ֵ����
*	�����ˣ�
*	�޸���ʷ����ÿ��������
	�绰���뱾�Ĵ洢���̶�2��������ÿ�������洢10���绰��
	һ���绰���̶�23���ֽڵĳ��ȣ�8(����)+13(�绰����)
	������0xFF�����洢��ǰҳ�ĺ�����������ֵΪ0������Ҫ����洢һ������ҳ
\********************************************************/
void Set_Phone_Num(void)
{
	Do_Phone_Note();
	//�洢��Ϊ��2����Ϣ���͵���Ӧ���ݵĴ洢
	//�������˿����óɹ������Ӧ
	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,ASK_PHONE_NUMECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(ECHO_SMS_2+3,2);
	SRAM_Write(ECHO_SMS_2+4,gGeneral_Buffer[1+9]);
	SRAM_Write(ECHO_SMS_2+5,gGeneral_Buffer[1+10]);
	Judge_SMS_Way();
}
/********************************************************\
*	��������Watch_Way();
	�����򣺱����ļ�����
*	���ܣ�
*	������  ��
*	����ֵ����
*	�����ˣ�
*	�޸���ʷ����ÿ��������

\********************************************************/
void Watch_Way(void)
{
	unsigned int i=0;
	gWatch_Circle = gGeneral_Buffer[1+9];
	gWatch_Circle <<= 8;
	gWatch_Circle += gGeneral_Buffer[1+10];

	gWatch_Time = gGeneral_Buffer[1+11];
	gWatch_Time <<= 8;
	gWatch_Time += gGeneral_Buffer[1+12];

	gWatch_Count = gGeneral_Buffer[1+13];
	gWatch_Count <<= 8;
	gWatch_Count += gGeneral_Buffer[1+14];

	if(gWatch_Circle==0)
	{
		if(gCycle_Send_Status==WATCH_DATA)	gCycle_Send_Status=NOP_DATA;
		gWatch_Count=0;gWatch_Time=0;
		gWatch_Type=WATCH_TYPE_NULL;
		Load_Buffer1_Echo(ASK_FREEWATCH_UP,gGeneral_Buffer[5]);
	}
	else
	{
		if(   (gCycle_Send_Status==TRACK_DATA_TIME)
			||(gCycle_Send_Status==TRACK_DATA_DISTANCE)
			||(gCycle_Send_Status==NOP_DATA)  )
		{
			gCycle_Send_Status=WATCH_DATA;
			gPublic_Flag&=~ACCOFF_OK_F_1;
			gPublic_Flag&=~ACC_OFF_UP_F_1;
			gPublic_Flag&=~ACCOFF_TRACK_F_1;
			ACC_OFF_Timer=0;
			if(gWatch_Time!=0)
			{
				gWatch_Count=0;
				gWatch_Type=WATCH_TYPE_TIME;
			}
			else
			{
				if(gWatch_Count!=0)	gWatch_Type=WATCH_TYPE_COUNT;
				else
				{
					Load_Buffer1_Echo(ASK_FREEWATCH_UP,gGeneral_Buffer[5]);
				}
			}
		}
	}
}

/********************************************************\
*	��������Line_Data_Deal();
	�����򣺱����ļ�����
*	���ܣ�
*	������  ��
*	����ֵ����
*	�����ˣ�
*	�޸���ʷ����ÿ��������

\********************************************************/
void Line_Data_Deal(void)
{
	unsigned int i=0,j=0;
	unsigned char nTemp=0;
	unsigned char Line_Num=0;
	nTemp=gGeneral_Buffer[1+10];	//֡��(��Χֵ1,2)
	Line_Num=gGeneral_Buffer[1+11];	//��ǰ��·��ĸ���(1--30)
	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,LINE_DATA_ECHO_UP);
	SRAM_Write(ECHO_SMS_2+2,gGeneral_Buffer[5]);
	SRAM_Write(ECHO_SMS_2+3,2);
	SRAM_Write(ECHO_SMS_2+4,gGeneral_Buffer[1+9]);
	SRAM_Write(ECHO_SMS_2+5,gGeneral_Buffer[1+10]);
	Judge_SMS_Way();
	if(Line_Num>30 )
	{
		Line_Num=30;
	}
	OperateSPIEnd();
	//��ʾ��ǰ�ǵ�һҳ������
	if(nTemp==1)
	{
		ReadOnePageToBuffer2(FLASH_LINEDATA1_ONE_2);
	}
	//��ʾ�ǵ�ǰ�ڶ�ҳ������
	else if(nTemp==2)
	{
		ReadOnePageToBuffer2(FLASH_LINEDATA2_ONE_2);
	}
	OperateSPIEnd();

	//дbuffer2�е�һ������
	WriteOneByteToBuffer2(FLINE_DATA_1,gGeneral_Buffer[1+12]);

	for(i=1;i<8*Line_Num;i++)
	{
		WriteNextByteToBuffer2(gGeneral_Buffer[1+12+i]);
	}
	for(i=8*Line_Num+1;i<264;i++)
	{
		WriteNextByteToBuffer2(0xFF);
	}
	OperateSPIEnd();
	//д�뵱ǰҳ��ĸ���
	WriteOneByteToBuffer2(FLINE_NUM_1,Line_Num);
	WriteNextByteToBuffer2(~Line_Num);
	OperateSPIEnd();
	if(nTemp==1)		RFlash_Sector(FLASH_LINEDATA1_ONE_2);
	else if(nTemp==2)	RFlash_Sector(FLASH_LINEDATA2_ONE_2);
	Clear_Exter_WatchDog();
	OperateSPIEnd();
	//�������洢����Ӧ��FLASH��

	//�ж��Ƿ��������·��������������꣬��FLAHS�е�����ת�Ƶ�SRAM��
	if(gGeneral_Buffer[1+9]==gGeneral_Buffer[1+10])
	{
		Load_Info_Four();
	}
}