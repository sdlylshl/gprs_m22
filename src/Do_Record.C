/********************************************************\
*	�ļ�����  Do_Record.h
*	����ʱ�䣺2005��6��21��
*	�����ˣ�  
*	�汾�ţ�  1.00
*	���ܣ�
*
*	�޸���ʷ����ÿ��������
	����˵��������ӵ���ʹ��¼�������ӣ�����1��9600����У�飬ȫ˫��ͨѶ��ʽ
\********************************************************/
#include <msp430x14x.h>
#include <stdlib.h>
#include "General.h"
#include "Do_SRAM.h"
#include "D_Buffer.h"
#include "Other_Define.h"
#include "Uart01.h"
#include "Sub_C.h"
#include "Record_Protocol.h"
#include "Define_Bit.h"
#include "M22_AT.h"
#include "Disp_Protocol.h"
#include "Do_Other.h"
#include "W_Protocol.h"
#include "Record_Protocol.h"
#include "TA_Uart.h"
/*
#include "SPI45DB041.h"
#include "W_Protocol.h"
#include "Handle_Protocol.h"
#include "Do_Record.h"
#include "Define_Bit.h"
#include "Main_Init.h"
#include "Check_GSM.h"
#include "Do_Reset.h"
#include "Do_M22.h"
#include "Do_Handle.h"
#include "Do_GPS.h"
#include "Do_Disp.h"
#include "Do_Other.h"
*/
void Deal_Record_Data(unsigned int Length);
unsigned char Check_Record_Time(void);
/********************************************************\
*	��������Do_Record_Module()
	�������ⲿ�ļ�����
*	���ܣ�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������

\********************************************************/
void Do_Record_Module(void)
{
	unsigned int i;
	unsigned int k;
	unsigned char DataTemp=0;
	if( (gReceive1_Over_Count>60)&&(gReceive1_Point>SRAM_RECORD_S)&&((gOther_Flag&RECORD_DATA_F_1)==0) )
	{
		//�ж��Ƿ���ȷ�����ݰ�
		i=gReceive1_Point;
			//���жϰ�β
		if( (SRAM_Read(i-1)==0x0a)&&(SRAM_Read(i-2)==0x0d) )
		{
			//�����β��ȷ������Ҫ�жϰ���
			k=0;
			k=SRAM_Read(SRAM_RECORD_S+4);
			k<<=8;
			k=k+SRAM_Read(SRAM_RECORD_S+5);
			k=k+10;
			if( (i-SRAM_RECORD_S)==k )
			{
				if((gGeneral_Flag&TCP_PHONE_F_1)==0 )
				{
					#ifdef Debug_GSM_COM1
		            Send_COM1_Byte(ASCII((i-SRAM_RECORD_S)/0x100));
		            Send_COM1_Byte(ASCII(((i-SRAM_RECORD_S)%0x100)/0x10));
		            Send_COM1_Byte(ASCII((i-SRAM_RECORD_S)%0x10));
		            Send_COM1_Byte(':');
		            Send_COM1_Byte(ASCII(k/0x100));
		            Send_COM1_Byte(ASCII((k%0x100)/0x10));
		            Send_COM1_Byte(ASCII(k%0x10));
					Send_COM1_Byte('[');
					for(n=SRAM_RECORD_S;n<i;n++)
					{
						DataTemp=SRAM_Read(n);
						Send_COM1_Byte(ASCII(DataTemp/0x10));
						Send_COM1_Byte(ASCII(DataTemp%0x10));
						Clear_Exter_WatchDog();
					}
					Send_COM1_Byte(']');
					Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
					#endif
				}
				Deal_Record_Data(i-SRAM_RECORD_S);
				gNO_Sram_Data_Timer=0;
			}
		}
		gReceive1_Point=SRAM_RECORD_S;
	}
	//�������20���޼�¼�ǵ����ݷ��ͣ�����Ҫ����ⲿSRAM.
	if(gNO_Sram_Data_Timer>20)
	{
		gNO_Sram_Data_Timer=0;
		gReceive1_Point=SRAM_RECORD_S;
		gReceive1_Over_Count=0;
		Send_COM1_String((unsigned char *)CLEAR_SRAM,sizeof(CLEAR_SRAM));
	}
}
/********************************************************\
*	��������Deal_Record_Command()
	�����򣺱����ļ�����
*	���ܣ�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
	����˵���������յ������������ݰ��Ѿ�����
	�ⲿSRAM��SRAM_DO_S-----SRAM_DO_E����
	������ӳ��򲿷�����Ҫ����������ȡ����������
\********************************************************/
void Deal_Record_Data(unsigned int Length)
{
	unsigned char nCmd=0;
	unsigned int iLength=0,i=0;
	unsigned char nStatus;
	unsigned char nTemp=0;
	unsigned char Check_Record=0;
	iLength=Length;
	SRAM_Init();
	nStatus=SRAM_Read(SRAM_RECORD_S);
	switch(nStatus)
	{
		//1����Ϣ����Ϊ0����Ϣ��
		case	NORMAL_TYPE:
		{
			gRecord_Data_Timer=0;
			gStatus2_Flag&=~RECORD_VAILD_F_1;
			gReceive_Record_Info_Timer=0;
			gStatus=NORMAL_TYPE;
			SRAM_Init();
			//��ȡ������
			nCmd=SRAM_Read(SRAM_RECORD_S+3);
			/**************************************************************************************\
			1,	���ؼ�¼�Ƿ��͵ļ�¼��ϵͳ����:��Ϣ�У���ǰ�ٶ�(��/��)
				�꣬�£��գ�ʱ���֣��룬��¼��״̬�ֽ�1����¼��״̬�ֽ�2
				����˵����A���ٶ���Ҫ���������ٺ��ƶ�����Ĵ�����ͬʱ��������ʾ����ʾ
						  B������жϼ�¼�ǵ�ʱ���GPS��Ч��״̬�£�����2������Ҫ�Լ�¼�ǵ�ʱ����н�����
						     ͬʱ�����GPS��Ч����״̬�£�GPS��ʱ��Ҳ��Ч������ת����¼�ǵ�ʱ�䵽��ʾ����
						  C����¼�ǵ�״̬�ֽ�1����Ϣ�������£�	BIT0����������Ϸ�������ʱ�䵽��
						  										BIT1��ƣ�ͼ�ʻ��ʾ��־��
						  										BIT2��ƣ�ͱ�����־
						  										BIT3����¼�Ǵ�ӡ���Ĺ���״̬
						  										BIT4����¼��USB�Ĺ���״̬

			00 55 7A 40 00 09 00 00 05 09 02 11 37 09 02 00 45 0D 0A
			\************************************************************************************/
			if(nCmd==K_RECEIVE_RECORDDATA)
			{
				//1���ٶȵĴ���
				SRAM_Init();
				gSpeed_Record=SRAM_Read(SRAM_RECORD_S+7);
				gSpeed_Disp=gSpeed_Record;
				gRecord_Null_Count=0;
				gPublic_Flag &=~ RECORD_NULL_F_1;
				//2��������ʱ����Ĵ���,����жϼ�¼�ǵ�ʱ����GPS��ʱ������������Ҫ�Լ�¼�ǽ���ʱ�����������,��ֻ����һ��
				i=Check_Record_Time();
				if(   (i==1)
					&&((gPublic_Flag&CHECK_TIME_F_1)==0)	)
				{
					gPublic_Flag|=SET_RECORD_TIME_F_1;
				}
				//------------------------------
				//3��״̬λ�Ĵ���
				gRecord_Status=SRAM_Read(SRAM_RECORD_S+14);
				//3-1���ж�BIT0��������Ҫ���͵�ǰ��λ�������
				if(   (gRecord_Status&J_DISTANCE_DATA_F_1)
					&&(gCycle_Send_Status==TRACK_DATA_DISTANCE)
					&&((gPublic_Flag&ALLOW_SEND_DISTANCE_F_1)==0)	 )
				{
//					Send_COM1_String( (unsigned char *)DISTANCE_DATA,sizeof(DISTANCE_DATA) );
					gPublic_Flag|=ALLOW_SEND_DISTANCE_F_1;
				}
				else
				{
					if(gPublic_Flag&ALLOW_SEND_DISTANCE_F_1)
					{
						if( (gInternal_Flag&SEND_CIRCLE_F_1)==0 )
						{
							Circle_Data_TCP(VEHICLE_RUNDATA_UP,0);
							gInternal_Flag|=SEND_CIRCLE_F_1;
							gPublic_Flag&=~ALLOW_SEND_DISTANCE_F_1;
							//����GSM�Ĺ�����ʽ��
							if(gM22_Status==GSM_WORK)
							{
								//���ڳ������ߵķ�ʽ
								if((gON_OFF_Temp1&DISTANCE_TCP_ON_1)==0)
								{
									gInternal_Flag&=~SEND_CIRCLE_F_1;
									gInternal_Flag&=~ALLOW_DATA_F_1;
									gCircle_Buffer_Point=0;
								}
								//���ڲ������ߵķ�ʽ������Ҫ��½
								else
								{
									//�ڵ�½�����д������ж�
								}
							}
						}
					}
				}
				//3-2,�ж�BIT1,����ƣ�ͼ�ʻ��ʾ�ı�־
				if(gRecord_Status&J_TIRE_NOTICE_F_1)
				{
					gFore_Tire_Count++;
					if(gFore_Tire_Count>20)
					{
						gDisp_Status|=S_TIRE_NOTICE_F_1;
						gFore_Tire_Count=0;
					}
					else
					{
						gDisp_Status&=~S_TIRE_NOTICE_F_1;
					}
				}
				else
				{
					gFore_Tire_Count=0;
					gDisp_Status&=~S_TIRE_NOTICE_F_1;
				}
				//3-3,�ж�BIT2������ƣ�ͼ�ʻ�����ı�־
				if(gRecord_Status&J_TIRE_ALARM_F_1)
				{
					gOver_Tire_Count++;
					gStop_Tire_Count=0;
				}
				else
				{
					gOver_Tire_Count=0;
					gStop_Tire_Count++;
				}
				//3-4,�ж�BIT3�����ڴ�ӡ���Ĺ���״̬
				if(gRecord_Status&J_PRINTER_STATUS_F_1)	gDisp_Status|=S_PRINTER_STATUS_F_1;
				else									gDisp_Status&=~S_PRINTER_STATUS_F_1;

				//3-5,�ж�BIT4������USB�Ĺ���״̬
				if(gRecord_Status&J_USB_STATUS_F_1)		gDisp_Status|=S_USB_STATUS_F_1;
				else									gDisp_Status&=~S_USB_STATUS_F_1;

				//4,���յ���¼�ǵ����ݣ�����Ҫ����һ��״̬�����������ȣ�γ��.
				//------------------------------
		    	Send_COM1_Byte(gStatus);
		    	Check_Record = gStatus;
		    	Send_COM1_Byte(0xAA);
		    	Check_Record ^= 0xAA;
		    	Send_COM1_Byte(0x75);
		    	Check_Record ^= 0x75;
		    	Send_COM1_Byte(K_SEND_GPSDATA);
		    	Check_Record ^= K_SEND_GPSDATA;
		    	Send_COM1_Byte(0);
		    	Check_Record ^= 0;
		    	Send_COM1_Byte(10);
		    	Check_Record ^= 10;
		    	Send_COM1_Byte(0x00);
		    	Check_Record ^= 0x00;
		    	if(gGeneral_Flag&GPS_VALID_F_1)
		    	{
		    		Send_COM1_Byte(0x01);
		    		Check_Record ^= 0x01;
		    	}
				else
				{
					Send_COM1_Byte(0x00);
					Check_Record ^= 0x00;
				}
		    	Send_COM1_Byte(gLongitude/0x1000000);
		    	Check_Record ^= gLongitude/0x1000000;
		    	Send_COM1_Byte(gLongitude%0x1000000/0x10000);
		    	Check_Record ^= gLongitude%0x1000000/0x10000;
		    	Send_COM1_Byte(gLongitude%0x10000/0x100);
		    	Check_Record ^= gLongitude%0x10000/0x100;
		    	Send_COM1_Byte(gLongitude%0x100);
		    	Check_Record ^= gLongitude%0x100;
		    	Send_COM1_Byte(gLatitude/0x1000000);
		    	Check_Record ^= gLatitude/0x1000000;
		    	Send_COM1_Byte(gLatitude%0x1000000/0x10000);
		    	Check_Record ^= gLatitude%0x1000000/0x10000;
		    	Send_COM1_Byte(gLatitude%0x10000/0x100);
		    	Check_Record ^= gLatitude%0x10000/0x100;
		    	Send_COM1_Byte(gLatitude%0x100);
		    	Check_Record ^= gLatitude%0x100;
		    	Send_COM1_Byte(gRecord_Status);
		    	Check_Record ^= gRecord_Status;
		    	Send_COM1_Byte(0);
		    	Check_Record ^= 0;
		    	Send_COM1_Byte(Check_Record);					//У���
		    	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);		//��β
			}
			/**************************************************************************************\
			2,���ؽ��ռ�¼�ǵļ�ʻԱ��Ϣ����Ӧ������������������ѯ�Ļ��Ǽ�¼���������͵ģ���Ϊͬһ�������֣�
				��Ϊ��Ҫ��ʾ��ʵʱ����ʾ��ǰ��¼��IC���еļ�ʻԱ��Ϣ��������һЩ����Ĵ�����ʽ
				A������ϵͳ�����ϵ������ϵͳ��������������ʱ������Ҫ�Ƚ���һ�ν��вɼ���¼�ǵ�ǰ��ʻԱ��Ϣ
					�����
				B�������Ѿ��������������еĹ����У������ʱ��¼�ǵļ�ʻԱ��Ϣ���£�����Ҫ��¼����������һ��
					��ǰ��ʻԱ��Ϣ����������ء�

				������ͨ��ʲô��ʽ������ϵͳֻҪ�õ�һ�ε�ǰ��¼�ǵļ�ʻԱ��Ϣ������Ҫ���˼�ʻԱ����Ϣ
				�洢��FLASH�С���������Ҫ���ͼ�ʻԱ��Ϣ����ʾ���ı�־��

				��ϵͳ�������й����У�����ж���ʾ�жϴ��ڿ�����״̬�����з��ͼ�ʻԱ��־����ʾ����
			\**************************************************************************************/
			else if(nCmd==K_GET_DRIVEDATA_E_ECHO)
			{
				gReceive_Record_Info_Timer=0;
				SRAM_Init();
				SRAM_Write(SRAM_DRIVER_INFO_VAILD,VAILD_2);
				for(i=0;i<29;i++)
				{
					nTemp=SRAM_Read(SRAM_RECORD_S+7+i);
					SRAM_Write(SRAM_DRIVER_INFO_CODE+i,nTemp);
				}
				///////////////////////////////
				gDisp_Buffer_Point=0;
				if(gDisp_Buffer_Point+37<=sizeof(gDisp_Buffer))
				{
					gDisp_Buffer[gDisp_Buffer_Point]=37;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=0;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='@';
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='%';
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=35;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=DISP_MAIN_IC_INFO_DOWN;
					gDisp_Buffer_Point++;
					SRAM_Init();
					for(i=0;i<29;i++)
					{
		 				gDisp_Buffer[gDisp_Buffer_Point]=SRAM_Read(SRAM_DRIVER_INFO_CODE+i);
						gDisp_Buffer_Point++;
					}
					gDisp_Buffer[gDisp_Buffer_Point]='$';
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='&';
					gDisp_Buffer_Point++;
					gGet_Driver_Info_Count=0;
					//gDriver_Info=WAIT_SEND_DRIVER_INFO;
				}
				/////////////////////////////////
			}
			/**************************************************************************************\
			3,�յ��������¼�����ü����¼�ļ�¼�ǵ���Ӧ
			\**************************************************************************************/
			else if(nCmd==K_SET_INTERDISTANCE_ECHO)
			{
				gPublic_Flag|=SET_DISTANCE_OK_F_1;
				gSet_Distance_Info_Count=0;
			}
			/**************************************************************************************\
			4,�յ��������¼������ʱ��ļ�¼�ǵ���Ӧ
			\**************************************************************************************/
			else if(nCmd==G_SET_TIME_EHCO)
			{
				gPublic_Flag&=~SET_RECORD_TIME_F_1;
				gPublic_Flag|=CHECK_TIME_F_1;
			}
			break;
		}
		/*
		========================================================================
		������Ϣ����Ϊ1����Ϣ��(ת�Ƹ����ĵ�����)
			�����¼�������ݷ��͸����ģ���������Ҫ����TCP��½����
			�����½����ʧ�ܣ�������ж��Ƿ�����SMS����ʽ����
		========================================================================
		*/
		case	CENTER_TYPE:
		{
			gRecord_Data_Timer=0;
			gStatus2_Flag&=~RECORD_VAILD_F_1;
			gReceive_Record_Info_Timer=0;
			gStatus=CENTER_TYPE;
			gOther_Flag|=RECORD_DATA_F_1;
			gOther_Flag|=ALLOW_SEND_RECORD_F_1;
			SRAM_Init();
			SRAM_Write(SRAM_SEND_VAILD,VAILD_2);
			SRAM_Write(SRAM_SEND_LENGTH_H,(iLength-3)/0x100);
			SRAM_Write(SRAM_SEND_LENGTH_L,(iLength-3)%0x100);
			for(i=0;i<iLength-3;i++)
			{
				nTemp=SRAM_Read(SRAM_RECORD_S+1+i);
				SRAM_Write(SRAM_SEND_S+i,nTemp);
			}
			Judge_SMS_Way();
			break;
		}
		/*
		========================================================================
		������Ϣ����Ϊ2����Ϣ����ת�Ƹ���ʾ�������ݣ�
		========================================================================
		*/
		case	DISP_TYPE:
		{
			gRecord_Data_Timer=0;
			gStatus2_Flag&=~RECORD_VAILD_F_1;
			gStatus=DISP_TYPE;
			if(gDisp_Buffer_Point+iLength+8<=sizeof(gDisp_Buffer))
			{
				gDisp_Buffer[gDisp_Buffer_Point]=iLength+8;
				gDisp_Buffer_Point++;
				gDisp_Buffer[gDisp_Buffer_Point]=0;
				gDisp_Buffer_Point++;
				gDisp_Buffer[gDisp_Buffer_Point]='@';
				gDisp_Buffer_Point++;
				gDisp_Buffer[gDisp_Buffer_Point]='%';
				gDisp_Buffer_Point++;
				gDisp_Buffer[gDisp_Buffer_Point]=iLength+6;
				gDisp_Buffer_Point++;
				gDisp_Buffer[gDisp_Buffer_Point]=DISP_ASK_RECORD_INFO_ECHO_DOWM;
				gDisp_Buffer_Point++;
				for(i=0;i<iLength;i++)
				{
					gDisp_Buffer[gDisp_Buffer_Point] = SRAM_Read(SRAM_RECORD_S+i) ;
					gDisp_Buffer_Point++;
				}
				gDisp_Buffer[gDisp_Buffer_Point]='$';
				gDisp_Buffer_Point++;
				gDisp_Buffer[gDisp_Buffer_Point]='&';
				gDisp_Buffer_Point++;
			}
			break;
		}
		default:
			break;
	}
}
/********************************************************\
*	��������Deal_Record_Command()
	�����򣺱����ļ�����
*	���ܣ�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
unsigned char Check_Record_Time(void)
{
	SRAM_Init();
	if(   (gGeneral_Flag&GPS_VALID_F_1)
		&&((gPublic_Flag&SET_RECORD_TIME_F_1)==0)
		&&(gSecond>=55) )
	{
		if(gYear!=SRAM_Read(SRAM_RECORD_S+8) )				return(1);
		else if(gMonth!=SRAM_Read(SRAM_RECORD_S+9))			return(1);
		else if(gDate !=SRAM_Read(SRAM_RECORD_S+10))		return(1);
		else if(gHour !=SRAM_Read(SRAM_RECORD_S+11))		return(1);
		else if(gMinute!=SRAM_Read(SRAM_RECORD_S+12))		return(1);
		else												return(0);
	}
	else
	{
		return(0);
	}
}