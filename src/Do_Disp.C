/********************************************************\
*	�ļ�����  Do_Disp.h
*	����ʱ�䣺2004��10��20��
*	�����ˣ�  
*	�汾�ţ�  1.00
*	���ܣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
#include <msp430x14x.h>
#include "General.h"
#include "Uart01.h"
#include "Define_Bit.h"
#include "Other_Define.h"
#include "Disp_Protocol.h"
#include "Sub_C.h"
#include "W_Protocol.h"
#include "SPI45DB041.h"
#include "D_Buffer.h"
#include "Do_Other.h"
#include "TA_Uart.h"
#include "Handle_Protocol.h"
#include "Do_Handle.h"
#include "M22_AT.h"
#include "Do_SRAM.h"
/*
#include "Main_Init.h"
#include "Check_GSM.h"
#include "Do_Reset.h"
#include "Do_GSM.h"
#include "Do_Handle.h"
#include "Do_GPS.h"
#include "Do_Disp.h"
#include "Msp430_Flash.h"
*/
unsigned char Check_Disp_Command(void);
void Disp_Ask_Watch_Status_Echo(void);
void Disp_Ask_Info(unsigned char nCommand,unsigned char nKey_ID,unsigned char nACK);
void Disp_Receive_Info_Echo(unsigned char nCommand,unsigned char nInfo_ID1,unsigned char nInfo_ID2,unsigned char nInfo_ID3,unsigned char nInfo_ID4,unsigned char nACK);
void Disp_Ask_Par(void);
/********************************************************\
*	��������Do_Disp_Module()
	�������ⲿ�ļ�����
*	���ܣ�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
	����˵������ʾ�ն����ݲ��ð�˫����ͨѶ��ʽ��ģ�⴮��4�������������ߣ����ֱ���ͨѶ��ʽһ��
\********************************************************/
void Do_Disp_Module(void)
{
    unsigned int i=0,k=0;
 	//���մ���������ʾ�ն˵���Ϣ��
	if( (gDisp_Receive_Over>50)&&(gDisp_Buffer_R_Point>0) )
	{
		#ifdef Debug_GSM_COM1
		Send_COM1_Byte(ASCII(gDisp_Buffer_R_Point/10));
		Send_COM1_Byte(ASCII(gDisp_Buffer_R_Point%10));
		Send_COM1_Byte('[');
		Send_COM1_String(gDisp_Buffer_R,gDisp_Buffer_R_Point);
		Send_COM1_Byte(']');
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		#endif
		k=Check_Disp_Command();
		gDisp_Buffer_R_Point=0;
		switch(k)
		{
			/********************************************************\
			1,��ʾ�ն���������������״̬������
			\********************************************************/
			case	DISP_ASK_WATCH_STATUS_UP:
			{
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				Disp_Ask_Watch_Status_Echo();
				break;
			}
			/********************************************************\
			2,����������ʾ�ն˵���Ϣ������Ҫ���е�����������Ϣ��
			\********************************************************/
			case	DISP_INFO_STATUS_UP:
			{
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				SRAM_Init();
				//1,���ж��Ƿ���δ���������Ϣ������У��򷵻أ���������ɹ�
				i=SRAM_Read(OTHER_SMS_2);
				if(i==VAILD_2)
				{
					gPublic_Flag &=~ DISP_SEND_INFO_F_1;
					if(gDisp_Buffer_Point+7+2<=sizeof(gDisp_Buffer))
					{
						gDisp_Buffer[gDisp_Buffer_Point]=7+2;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=0;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='@';
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='%';
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=7;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=DISP_MAIN_INFO_ECHO_DOWN;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=COMMAND_ERROR;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='$';
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='&';
						gDisp_Buffer_Point++;
					}
				}
				else
				{
					Disp_Ask_Info(DISP_ASK_INFO_UP,gDisp_Buffer_R[4],0);
					gPublic_Flag |= DISP_SEND_INFO_F_1;
				}
				break;
			}
			/********************************************************\
			3������ϢԤ�õ��ϱ�
			\********************************************************/
			case	DISP_SMS_SNED_UP:
			{
				//1,���ж��Ƿ���δ���������Ϣ������У��򷵻أ���������ɹ�
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				SRAM_Init();
				i=SRAM_Read(OTHER_SMS_2);
				if(i==VAILD_2)
				{
					gPublic_Flag &=~ DISP_SEND_INFO_F_1;
					if(gDisp_Buffer_Point+7+2<=sizeof(gDisp_Buffer))
					{
						gDisp_Buffer[gDisp_Buffer_Point]=7+2;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=0;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='@';
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='%';
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=7;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=DISP_MAIN_INFO_ECHO_DOWN;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=COMMAND_ERROR;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='$';
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]='&';
						gDisp_Buffer_Point++;
					}
				}
				else
				{
					Disp_Ask_Info(DISP_SMS_INFO_UP,gDisp_Buffer_R[4],0);
					gPublic_Flag |= DISP_SEND_INFO_F_1;
				}
				break;
			}
			/********************************************************\
			4,����������ʾ�ն˽��յ����ط��͵ļ�ʻԱ��Ϣ�ķ���
			\********************************************************/
			case	DISP_RECEIVE_IC_INFO_UP:
			{
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				gGet_Driver_Info_Count=0;
				gDriver_Info=SEND_DRIVER_INFO_OK;
				break;
			}
			/********************************************************\
			4,����������ʾ�ն˽��յ�һ�����е���Ϣ����ECHO,��Ҫ���е���������
			\********************************************************/
			/*
			case	DISP_RECEIVE_INFO_ECHO_UP:
			{
				Disp_Receive_Info_Echo(DISP_INFO_ECHO_UP,gDisp_Buffer_R[4],gDisp_Buffer_R[5],gDisp_Buffer_R[6],gDisp_Buffer_R[7],0);
				break;
			}
			*/
			/********************************************************\
			5,����������ʾ�ն˽��յ�һ�����е���Ӧ����ʾ�ն˽��յ�һ��ÿһ���ӵ�
				��ʱ�����������Ҫ���ص�һ����Ӧ���ݰ���
			\********************************************************/
			case	DISP_CLRCLESEND_ECHO_UP:
			{
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				break;
			}
			/********************************************************\
			6, ����������ʾ�ն˺ͼ�¼��֮��ͨѶ��Э���֣�����ϵͳֻ����ת����
				��Ҫ�����ظ����͹���
			\********************************************************/
			case	DISP_ASK_RECORDINFO_UP:
			{
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				for(k=0;k<gDisp_Buffer_R[2]-5;k++)
				{
					Send_COM1_Byte(gDisp_Buffer_R[k+4]);
				}
				break;
			}
			/********************************************************\
			7, ����������ʾ�ն˵���Ӧ��0xAE��
			\********************************************************/
			case	DISP_SET_ECHO_UP:
			{
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				//����������Ϣ����ʾ�ն��յ�һ������Ӧ
				if(gDisp_Buffer_R[4]==DISP2_SEND_WORD_ECHO_UP)
				{
					if(gCommon_Flag&ALLOW_OUT_DISP_F_1)
					{
						gCommon_Flag|=DISP_SEND_OK_F_1;
					}
				}
				//����������Ϣȫ����������Ӧ0x4B
				else if(gDisp_Buffer_R[4]==DISP2_SEND_WORD_UP)
				{
					//���ݲ������ڶ��������֣�ID(4���ֽ�)��ACK��
					Disp_Receive_Info_Echo(gDisp_Buffer_R[4],gDisp_Buffer_R[6],gDisp_Buffer_R[7],gDisp_Buffer_R[8],gDisp_Buffer_R[9],gDisp_Buffer_R[5]);
				}
				//�������ĵ�Ԥ�����ö���Ϣ����Ӧ0xAE
				else if(gDisp_Buffer_R[4]==DISP2_SET_SMS_ECHO_UP)
				{
					//���ݲ������ڶ��������֣�ID(1���ֽ�)��ACK
					Disp_Ask_Info(gDisp_Buffer_R[4],gDisp_Buffer_R[6],gDisp_Buffer_R[5]);
					gCommon_Flag|=DISP_SEND_OK_F_1;
				}
				//�����ֱ��˿ڵ�Ԥ�����ö���Ϣ����Ӧ��0x04
				else if(gDisp_Buffer_R[4]==DISP2_HSET_SMS_ECHO_UP)
				{
					Send_COM3_Load(5,HANDLE_SET_SMS_ECHO,COMMAND_OK);
					gCommon_Flag|=DISP_SEND_OK_F_1;
				}
				//�����ֱ��˿ڵĳ�ʼ����ʾ�ն˵�������Ӧ0x03
				else if(gDisp_Buffer_R[4]==DISP2_CLEAR_ECHO_UP)
				{
					//�����ֱ�һ���ɹ��ź�
					Waiting_Allow_Send();
					Send_COM3_SubProgram(5,HANDLE_CLEAR_DISP_ECHO,COMMAND_OK);
				}
				break;
			}
			/********************************************************\
			8, ����������ʾ�ն˲�ѯ�����ն˵�����(��Ҫ������ʾ�ն˵��ظ�����)
			\********************************************************/
			case	DISP_ASK_PAR_UP:
			{
				gDisp_OnLine_Timer=0;
				gInternal_Flag|=DISP_ON_F_1;
				gStatus1_Flag|=DISPLAY_ON_F_1;
				Disp_Ask_Par();
				break;
			}
			/********************************************************\
			9,����
			\********************************************************/
			default:
				break;
		}
	}
	else if(  (P6IN&DISP_TEST)
			&&((gCommon_Flag&ALLOW_SEND_DISP_F_1)==0) 	)
	{
		gCommon_Flag|=ALLOW_SEND_DISP_F_1;
	}
	//��������ʾ�ն˷�����Ϣ���Ĵ���
	else if ( 	(gDisp_Buffer_Point>0)				//���ͻ�������������Ҫ����
			  &&(gDisp_Buffer_R_Point==0)			//���ջ�����������ָ��Ϊ0����ʾû�н�������
			  &&(gHandle_Receive_Over>20)   		//��Ϊ��ͬһ���ж��У����뱣֤���Ե绰û�������ط�������
			  &&(P6IN&DISP_TEST)
			  &&( (gCommon_Flag&ALLOW_R232_F_1)==0)
			  &&(gReceive1_Over_Count>20)
			  &&(gGPS_Receive_Over>20)
			  &&(gDisp_Timer>30)
			  &&(gCommon_Flag&ALLOW_SEND_DISP_F_1)	)
	{
		//1,�����ķ���Ҫ��Ӧ������
		if(   ((gCommon_Flag&ALLOW_OUT_DISP_F_1)==0)
		    &&( (gCommon_Flag&DISP_SEND_OK_F_1)==0)		)
		{
			Send_COM4_String( gDisp_Buffer+2,gDisp_Buffer[0]-2 );
			RX4_Ready();
			gCommon_Flag&=~ALLOW_SEND_DISP_F_1;
			gDisp_Timer=0;
			if( gDisp_Buffer[5]==DISP_MAIN_WORD_DOWN )
			{
				gCommon_Flag|=ALLOW_OUT_DISP_F_1;
				gDo_Speed_Count=0;
			}
			else
			{
				k=gDisp_Buffer[0];
		        Copy_String( gDisp_Buffer+k,gDisp_Buffer,gDisp_Buffer_Point-k );
		        gDisp_Buffer_Point=gDisp_Buffer_Point-k;
			}
		}
		//2,�յ���Ӧ��Ĵ���
		else if(   (gCommon_Flag&DISP_SEND_OK_F_1)
		         &&(gCommon_Flag&ALLOW_OUT_DISP_F_1)  )
		{
			gCommon_Flag&=~DISP_SEND_OK_F_1;
			gCommon_Flag&=~ALLOW_OUT_DISP_F_1;
			k=gDisp_Buffer[0];
	        Copy_String( gDisp_Buffer+k,gDisp_Buffer,gDisp_Buffer_Point-k );
	        gDisp_Buffer_Point=gDisp_Buffer_Point-k;
		}
		//3���ط����ֵĴ���
		else if(   (gCommon_Flag&ALLOW_OUT_DISP_F_1)
		         &&((gCommon_Flag&DISP_SEND_OK_F_1)==0)   )
		{
			if(gSend_Disp_Timer>3)
			{
				gSend_Disp_Timer=0;
				Send_COM4_String( gDisp_Buffer+2,gDisp_Buffer[0]-2 );
				RX4_Ready();
				gCommon_Flag&=~ALLOW_SEND_DISP_F_1;
				gDisp_Timer=0;
				gDisp_ReSend_Count++;
				if(gDisp_ReSend_Count>3)
				{
					gCommon_Flag&=~ALLOW_OUT_DISP_F_1;
					k=gDisp_Buffer[0];
	        		Copy_String( gDisp_Buffer+k,gDisp_Buffer,gDisp_Buffer_Point-k );
	       		 	gDisp_Buffer_Point=gDisp_Buffer_Point-k;
				}
			}
	    }
	}
}
/********************************************************\
*	��������Check_Disp_Command()
	�����򣺱����ļ�����
*	���ܣ�	�������մ���������ʾ�ն˵Ľ�������,�жϳ�������
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/

unsigned char Check_Disp_Command(void)
{
    if( gDisp_Buffer_R_Point < 6) return(0);
//    if( gDisp_Buffer_R_Point != gDisp_Buffer_R[2]) return(0);
    if( gDisp_Buffer_R[0]!='@' ) return(0);
    if( gDisp_Buffer_R[1]!='%' ) return(0);
    if( gDisp_Buffer_R[gDisp_Buffer_R[2]-2]!='$' ) return(0);
    if( gDisp_Buffer_R[gDisp_Buffer_R[2]-1]!='&' ) return(0);
    return( gDisp_Buffer_R[3] );
}
/********************************************************\
*	��������Save_TypeC_Buffer()
	�����򣺱����ļ�����
*	���ܣ�	��������C����Ҫ�ϴ������ݣ�����ָ����������ʾ�ն˵�C�����ݣ�����
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
	����˵����
		0xB1	�洢��ʾ��������SMS�Ƿ�����Ϊ1���ʾ���滹��δ������SMS��Ϊ0���ʾ�Ѿ���������SMS
		0xB2	�洢���ݰ�����������������ΪnCommand
		0xB3	�洢���ݰ���ACK��ֵACK
		0xB4	�洢�������ݰ��ĳ��ȣ���ע��û�а����洢CHK(MSB),CHK(LSB),EOT,�������ֽڣ�
		0xB5  	���⿪ʼ�洢���ݰ�
\********************************************************/



/********************************************************\
*	��������Disp_Hand_Free_Defence()
	�����򣺱����ļ�����
*	���ܣ�	���յ�������ʾ�ն˵������������Ϣ�Ĵ���
			1�����жϽ������������Ƿ���ȷ��
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
	����˵����
	1�����жϳ����ն˵������Ƿ���Ч�������δ�洢�����룬���жϽ���������ȷ�ԣ�ֱ�ӽ�����
	2����������ն˵�������Ч�����жϽ�������Ƿ���ȷ��������Ҫ�ص�˵�����ǣ���Ϊ��ʾ�ն˵��������
		������6λ�����㴦������Ϊ0x20�����Ժ�����ͨ���ֱ�������������ʼ�հ�6λ������������6λ��
		�����0X20��
	3�����������ȷ��������ʾ�ն˷���������ȷ�����ҽ�����(����FLASH�������־�����)
	4��������벻��ȷ��������ʾ�жϷ������벻��ȷ����������3���������벻��ȷ�������̲�������������Ϣ
		�������־��Ȼ����

\********************************************************/


/********************************************************\
*	��������Disp_Ask_Watch_Status_Echo()
	�����򣺱����ļ�����
*	���ܣ�	���յ�������ʾ�ն������ı����������
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Disp_Ask_Watch_Status_Echo(void)
{
	if(gDisp_Buffer_Point+14<=sizeof(gDisp_Buffer))
	{
		gDisp_Buffer[gDisp_Buffer_Point]=14;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=0;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='@';
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='%';
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=12;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=DISP_MAIN_STATUS_DOWN;
		gDisp_Buffer_Point++;
		if(gGeneral_Flag&GPS_VALID_F_1)
		{
			gDisp_Buffer[gDisp_Buffer_Point]=gYear;
			gDisp_Buffer_Point++;
			gDisp_Buffer[gDisp_Buffer_Point]=gMonth;
			gDisp_Buffer_Point++;
			gDisp_Buffer[gDisp_Buffer_Point]=gDate;
		}
		else
		{
			gDisp_Buffer[gDisp_Buffer_Point]=0x05;
			gDisp_Buffer_Point++;
			gDisp_Buffer[gDisp_Buffer_Point]=0x01;
			gDisp_Buffer_Point++;
			gDisp_Buffer[gDisp_Buffer_Point]=0x01;
		}
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=gHour;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=gMinute;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=gSecond;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='$';
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='&';
		gDisp_Buffer_Point++;
	}
}

/********************************************************\
*	��������Disp_Ask_Info()
	�����򣺱����ļ�����
*	���ܣ�	���յ�������ʾ�ն˵İ���������Ϣ����Ҫ����Щ��Ϣ���и�����
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Disp_Ask_Info(unsigned char nCommand,unsigned char nKey_ID,unsigned char nACK)
{
	unsigned char Key_ID=0;
	Key_ID=nKey_ID;
	SRAM_Init();
	SRAM_Write(OTHER_SMS_2+0,VAILD_2);
	SRAM_Write(OTHER_SMS_2+1,nCommand);
	SRAM_Write(OTHER_SMS_2+2,nACK);
	SRAM_Write(OTHER_SMS_2+3,22);
	SRAM_Write(OTHER_SMS_2+4,Key_ID);
	Write_PartData_Sram(OTHER_SMS_2+4);
	SRAM_Write(OTHER_SMS_2+4+18,0);
	SRAM_Write(OTHER_SMS_2+4+19,gStatus3_Flag);
	SRAM_Write(OTHER_SMS_2+4+20,gStatus2_Flag);
	SRAM_Write(OTHER_SMS_2+4+21,gStatus1_Flag);
	Judge_SMS_Way();
}
/********************************************************\
*	��������Disp_Ask_Info()
	�����򣺱����ļ�����
*	���ܣ�	���յ�������ʾ�ն˵İ���������Ϣ����Ҫ����Щ��Ϣ���и�����
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Disp_Receive_Info_Echo(unsigned char nCommand,unsigned char nInfo_ID1,unsigned char nInfo_ID2,unsigned char nInfo_ID3,unsigned char nInfo_ID4,unsigned char nACK)
{
	SRAM_Init();
	SRAM_Write(OTHER_SMS_2+0,VAILD_2);
	SRAM_Write(OTHER_SMS_2+1,nCommand);
	SRAM_Write(OTHER_SMS_2+2,nACK);
	SRAM_Write(OTHER_SMS_2+3,17+8);
	//4���ֽڵ���Ϣ����ID��
	SRAM_Write(OTHER_SMS_2+4,nInfo_ID1);
	SRAM_Write(OTHER_SMS_2+5,nInfo_ID2);
	SRAM_Write(OTHER_SMS_2+6,nInfo_ID3);
	SRAM_Write(OTHER_SMS_2+7,nInfo_ID4);
	Write_PartData_Sram(OTHER_SMS_2+7);
	SRAM_Write(OTHER_SMS_2+7+18,0);
	SRAM_Write(OTHER_SMS_2+7+19,0);
	SRAM_Write(OTHER_SMS_2+7+20,gStatus2_Flag);
	SRAM_Write(OTHER_SMS_2+7+21,gStatus1_Flag);
	Judge_SMS_Way();
}

/********************************************************\
*	��������Disp_Ask_Par(()
	�����򣺱����ļ�����
*	���ܣ�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Disp_Ask_Par(void)
{
	unsigned int i=0;
	unsigned char Data_Temp[12];
	unsigned int iData=0;
	Ask_Par();
	//����ʾ�����͵����ݰ���Ҫ���⴦��һ��
	/*
	gGeneral_Buffer[0]�洢���Ƿ������ݵĳ���
	��Ҫ��gGeneral_Buffer[1]-----gGeneral_Buffer[5]������ת��ΪASCII�ַ���ʾ��ʽ
	���̣�
	40 25 50 13 30 30 30 31 30 31 30 30 30 30 30 35 31 33 36 39 31 38 37 38 35 33 34 00 32 31 30 2E 32 31 2E 32 34 34 2E 33 39 00 30 39 39 38 38 31 2C 22 49 50 22 2C 22 43 4D 4E 45 54 22 00 31 30 30 30 32 30 30 30 32 30 30 30 30 30 30 30 24 26
	*/
	Data_Temp[0]=ASCII(gGeneral_Buffer[1]/10);
	Data_Temp[1]=ASCII(gGeneral_Buffer[1]%10);		//����

	Data_Temp[2]=ASCII(gGeneral_Buffer[2]/10);
	Data_Temp[3]=ASCII(gGeneral_Buffer[2]%10);		//����

	Data_Temp[4]=ASCII(gGeneral_Buffer[3]/10);
	Data_Temp[5]=ASCII(gGeneral_Buffer[3]%10);		//����

	iData = gGeneral_Buffer[4];
	iData <<=8;
	iData +=gGeneral_Buffer[5];

	Data_Temp[6]=ASCII(iData/100000);
	Data_Temp[7]=ASCII( (iData%100000)/10000 );
	Data_Temp[8]=ASCII( (iData%10000)/1000);
	Data_Temp[9]=ASCII( (iData%1000)/100);
	Data_Temp[10]=ASCII( (iData%100)/10);
	Data_Temp[11]=ASCII( iData%10 );				//����

	//��������gGeneral_Buffer����
	for(i=gGeneral_Buffer[0]+7;i>=13;i--)
	{
		gGeneral_Buffer[i]=gGeneral_Buffer[i-7];
	}
	//�����������ݳ���
	gGeneral_Buffer[0]=gGeneral_Buffer[0]+7;
	for(i=0;i<12;i++)
	{
		gGeneral_Buffer[1+i]=Data_Temp[i];
	}
	if(gDisp_Buffer_Point+gGeneral_Buffer[0]+1+5+2<=sizeof(gDisp_Buffer))
	{
		gDisp_Buffer[gDisp_Buffer_Point]=gGeneral_Buffer[0]+1+5+2;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=0;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='@';
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='%';
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=gGeneral_Buffer[0]+1+5;
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]=DISP_ASK_PAR_DOWM;
		gDisp_Buffer_Point++;
		for(i=1;i<=gGeneral_Buffer[0];i++)
		{
			gDisp_Buffer[gDisp_Buffer_Point]=gGeneral_Buffer[i];
			gDisp_Buffer_Point++;
		}
		gDisp_Buffer[gDisp_Buffer_Point]='$';
		gDisp_Buffer_Point++;
		gDisp_Buffer[gDisp_Buffer_Point]='&';
		gDisp_Buffer_Point++;
	}
}
