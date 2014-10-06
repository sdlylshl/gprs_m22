/********************************************************\
*	�ļ�����  Do_GSM.C
*	����ʱ�䣺2004��12��7��
*	�����ˣ�  
*	�汾�ţ�  1.00
*	���ܣ�	  ���GPS�������ݵĴ���
*	�ļ����ԣ������ļ�
*	�޸���ʷ����ÿ��������
\********************************************************/
#include <msp430x14x.h>
#include "TA_Uart.h"
#include "General.h"
#include "M22_AT.h"
#include "Sub_C.h"
#include "Define_Bit.h"
#include "Uart01.h"
#include "Handle_Protocol.h"
#include "SPI45DB041.h"
#include "D_Buffer.h"
#include "Disp_Protocol.h"
#include "Other_Define.h"
#include "W_Protocol.h"
#include "Do_Other.h"
#include "Other_Define.h"
#include "Record_Protocol.h"
#include "Msp430_Flash.h"
#include "Do_SRAM.h"
/*
#include "Main_Init.h"
#include "Check_GSM.h"
#include "Do_Reset.h"
#include "Do_GSM.h"
#include "Do_Handle.h"
#include "Do_GPS.h"
#include "Do_Disp.h"

*/
//#define Debug_GPS_COM1

unsigned char GPS_Buffer[80];		//���մ洢GPS���ݣ�ֻ�ܽ���һ֡����d
unsigned char GPS_Buffer_Point=0;	//����ָ��
unsigned char GPS_Send_Count=0;
const unsigned char Tab[4][12]={ { 31,29,31,30,31,30,31,31,30,31,30,31 },
                                 { 31,28,31,30,31,30,31,31,30,31,30,31 },
                                 { 31,28,31,30,31,30,31,31,30,31,30,31 },
                                 { 31,28,31,30,31,30,31,31,30,31,30,31 } };

/********************************************************\
*	��������Do_GPS_Module
	�������ⲿ�ļ�����
*	���ܣ�	���ֱ�ͨѶģ�顣
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
	����˵����
			����GPS���ݵĽ��մ�����������жϽ��մ����ķ�ʽ��һ���жϽ�����
			���ݺ���Do_GPS_Module�ﴦ�����յ���GPS���ݣ�GPS_Buffer[]����
			�������GPS���ݣ���ֻ�ܴ洢һ��GPS���ݰ���ȫ�ֱ���gGPS_Receive_over
			��Ϊ�ж��Ƿ��������ȫ����ɵļ�����һ���жϽ��յ�һ���ֽڵ�����
			��gGPS_Receive_over=0,������ݽ����꣬�򲻻��ڽ����ж��ˣ���
			gGPS_Receive_over���ڶ�ʱ�ж���++������һ��ʱ�䣬��Ȼδ�ڽ������
			�жϽ��յ����ݣ����ʾһ��������GPS���ݰ������꣬����봦��
			GPS���ݴ������֡�
\********************************************************/
void Do_GPS_Module(void)
{
	unsigned int i=0,j=0,k=0;
	unsigned char CYear=0x05,CMonth=0x01,CDate=0x01,CHour=0x12,CMinute=0x00,CSecond=0x00;
//	unsigned char nFlag=0;
	unsigned char Check_Data=0;
	unsigned char Check_Record=0;
//	nFlag=gAlarm_Type;
	/********************************************************\
	A���ж��Ƿ���Ҫ����GPS�ĸ�λ����
		�ж��Ƿ����㸴λGPS�ı�־λ��λ
		�����GPS��λ�ı�־λ��������ÿһ�ε���Ч����gGPS_Invaild_Timer++
		�������GPS_INVALID_COUNT��ʱ���Ϊ��Ч�㣬����Ҫ�ø�λGPS�ı�־λGPS_RESET_F_1
		��һ������Ч�㣬����Ҫ����Ч��ļ�����Ϊ0��

		������㸴λGPS������������Ҫ��GPS�ĸ�λ�ܽ�����Ϊ�͵�ƽ������GPS_LOW_TIMEʱ������ø�
	\********************************************************/
	/********************************************************\
	A���������100�룬GPS��������������ʽ��Ч�������һ��GPSģ����紦��
		�������5�ν����������Ĳ��������Ժ��ڽ���GPSģ��ĵ��紦�������ֱ������
		GPSģ����ϵı�־λ��
	\********************************************************/
	if(gGPS_No_Output_Timer>NO_OUTPUT_TIME)
	{
		gGPS_No_Output_Timer=0;
		gNO_GPS_Reset_Count=0;
		gCommon_Flag|=GPSNO_RESET_F_1;
		gStatus3_Flag|=GPS_OUTPUT_WAY_F_1;
		//���GPS�������ͬʱ�жϵ���ʾ�ն˺��ֱ����κ�ʱ����е�ͨѶ������Ҫ�رտ��Ź���ʹ�ó�����������
		if(  ((gStatus1_Flag&DISPLAY_ON_F_1)==0)
		   &&((gStatus1_Flag&HANDLE_ON_0_F_1)==0)
		   &&((gStatus1_Flag&HANDLE_ON_1_F_1)==0)	)
		{
			gOther_Flag|=RESET_PROGRAME_F_1;
		}
	}
	/********************************************************\
	B ���GPS����5����������������ǰ����Ч��־������Ҫ��������Ϊ
		��Ч��־
	\********************************************************/
	else if((gGPS_No_Output_Timer>20)&&(gGeneral_Flag&GPS_VALID_F_1) )
	{
		gGeneral_Flag&=~GPS_VALID_F_1;
	}
	/********************************************************\
	D���ж��Ƿ����������:
		������GPS�����ݴ����С�
		1����Ч����ۼƺ͸�λGPS��־���ж�
		2���������״̬�£��ٶȳ���3.6*4����Ч����ۼ�
		���������ж�������30����Ч�������������ɣ���������������Ϣ��
	\********************************************************/
	else if( (gGPS_Receive_Over>90)&&(GPS_Buffer_Point>1) )
	{
		//�жϽ��յ���ȷ�İ�ͷ����β
        if((Compare_String((unsigned char *)String_GPRMC,GPS_Buffer,sizeof(String_GPRMC))==1)&&(GPS_Buffer[GPS_Buffer_Point-1]==0x0a))
		{
			//��GPS�����ж�
			//�Ƚ���A���жϹر�
			//TAUARTCTL&=~TAUARTIE;
			if((gGeneral_Flag&TCP_PHONE_F_1)==0)
			{
				GPS_Send_Count++;
				if(GPS_Send_Count>10)
				{
					GPS_Send_Count=0;
					Send_COM1_String(GPS_Buffer,6);
				}
			}
			gNO_GPS_Reset_Count=0;
			gGPS_No_Output_Timer=0;
			gGeneral_Flag&=~GPS_VALID_F_1;
			gCommon_Flag&=~GPSNO_RESET_F_1;
			gStatus3_Flag&=~GPS_OUTPUT_WAY_F_1;
			i=0;
			while(1)
			{
				//����ʱ���֣�����Ϣ
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				else
				{
					CHour  =ASCIITOHEX( GPS_Buffer[i+1],GPS_Buffer[i+2] );
					CMinute=ASCIITOHEX( GPS_Buffer[i+3],GPS_Buffer[i+4] );
					CSecond=ASCIITOHEX( GPS_Buffer[i+5],GPS_Buffer[i+6] );
					i++;
				}
				//����GPS���ݵ���Ч��
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')		break;
				if(GPS_Buffer[i+1]!='A')
				{
					gGeneral_Flag &=~GPS_VALID_F_1;
				}
				else
				{
					gGeneral_Flag |= GPS_VALID_F_1;
					gGPS_Invaild_Timer=0;
				}
				i++;
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				else
				{	//γ��,ֻ������Ч��״̬�£��Ÿ���γ��ֵ
					if(gGeneral_Flag&GPS_VALID_F_1)
					{
						gLatitude=ASCIITOHEX( 0,GPS_Buffer[i+1] )*10+ASCIITOHEX( 0,GPS_Buffer[i+2] );
						gLatitude=gLatitude*60;
						gLatitude=gLatitude+ASCIITOHEX( 0,GPS_Buffer[i+3] )*10+ASCIITOHEX( 0,GPS_Buffer[i+4] );
						gLatitude=gLatitude*10+ASCIITOHEX( 0,GPS_Buffer[i+6] );
						gLatitude=gLatitude*10+ASCIITOHEX( 0,GPS_Buffer[i+7] );
						gLatitude=gLatitude*10+ASCIITOHEX( 0,GPS_Buffer[i+8] );
						gLatitude=gLatitude*10+ASCIITOHEX( 0,GPS_Buffer[i+9] );
						gLatitude=gLatitude*6;
					}
					i++;
				}
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				i++;
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				else
				{	//����,ֻ������Ч��״̬�²Ÿ��¾���ֵ
					if(gGeneral_Flag&GPS_VALID_F_1)
					{
						gLongitude=ASCIITOHEX( 0,GPS_Buffer[i+1] );
						gLongitude=gLongitude*10+ASCIITOHEX( 0,GPS_Buffer[i+2] );
						gLongitude=gLongitude*10+ASCIITOHEX( 0,GPS_Buffer[i+3] );
						gLongitude=gLongitude*60;
						gLongitude=gLongitude+ASCIITOHEX( 0,GPS_Buffer[i+4] )*10+ASCIITOHEX( 0,GPS_Buffer[i+5] );
						gLongitude=gLongitude*10+ASCIITOHEX( 0,GPS_Buffer[i+7] );
						gLongitude=gLongitude*10+ASCIITOHEX( 0,GPS_Buffer[i+8] );
						gLongitude=gLongitude*10+ASCIITOHEX( 0,GPS_Buffer[i+9] );
						gLongitude=gLongitude*10+ASCIITOHEX( 0,GPS_Buffer[i+10] );
						gLongitude=gLongitude*6;
					}
					i++;
				}
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				i++;
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				else
				{
					j=0;
					gSpeed_Gps=0;
					while(1)
					{
					    if(GPS_Buffer[i+j+1]=='.') break;
					    if(GPS_Buffer[i+j+1]==',') break;
					    gSpeed_Gps=gSpeed_Gps*10+ASCIITOHEX(0,GPS_Buffer[i+j+1]);
					    j++;
					    if( j>=GPS_Buffer_Point ) break;
					}
					gSpeed_Gps=gSpeed_Gps/2;
					i++;
				}
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				else
				{
					j=0;
					k=0;
					while(1)
					{
					    if( GPS_Buffer[i+j+1]=='.' )break;
					    if( GPS_Buffer[i+j+1]==',' )break;
					    k=k*10+ASCIITOHEX( 0,GPS_Buffer[i+j+1] );
					    j++;
					    if( j>=GPS_Buffer_Point ) break;
					}
					gAzimuth=k/15;
					i++;
				}
				while( i<GPS_Buffer_Point )
				{
				    if( GPS_Buffer[i]==',' ) break;i++;
				}
				if(GPS_Buffer[i]!=',')	break;
				else
				{
					CDate = ASCIITOHEX( GPS_Buffer[i+1],GPS_Buffer[i+2] );
					CMonth =ASCIITOHEX( GPS_Buffer[i+3],GPS_Buffer[i+4] );
					CYear = ASCIITOHEX( GPS_Buffer[i+5],GPS_Buffer[i+6] );
					i++;
				}
				break;
			}
			/********************************************************\
			1;	�����Ǵ���������GPS����
				��������Ҫ���н��յ�GPS���ݽ��и���һ���Ĵ���
				��һ���Ĵ���ʱ�䣬�꣬�£���
			\********************************************************/
			while(1)
			{
				i=CHour/16*10+CHour%16;
				i=i+8;
				if( i<24 ){ CHour=i/10*16+i%10;break; }
				i=i-24;
				CHour=i/10*16+i%10;
				i=CDate/16*10+CDate%16;
				j=CYear/16*10+CYear%16;
				k=CMonth/16*10+CMonth%16;
				i++;
				if( i<=Tab[j/4][k-1] ){ CDate=i/10*16+i%10;break; }
				CDate=1;
				k++;
				if( k<=12 ){ CMonth=k/10*16+k%10;break; }
				CMonth=1;
				j++;
				CYear=j/10*16+j%10;
				break;
			}
			/********************************************************\
			2,���CSecond=0������Ҫ���ֱ�����ʾ������һ��ʱ�����
			\********************************************************/
			if(CSecond==0)
			{
				SRAM_Init();
				SRAM_Write(SHANDLE_DATA_VAILD+0,VAILD_2);
				SRAM_Write(SHANDLE_DATA_VAILD+1,0x24);
				SRAM_Write(SHANDLE_DATA_VAILD+2,0x09);
				SRAM_Write(SHANDLE_DATA_VAILD+3,MAIN_TIME);
				SRAM_Write(SHANDLE_DATA_VAILD+4,CYear);
				SRAM_Write(SHANDLE_DATA_VAILD+5,CMonth);
				SRAM_Write(SHANDLE_DATA_VAILD+6,CDate);
				SRAM_Write(SHANDLE_DATA_VAILD+7,CHour);
				SRAM_Write(SHANDLE_DATA_VAILD+8,CMinute);
				SRAM_Write(SHANDLE_DATA_VAILD+9,0x0d);

				if(   (gDisp_Buffer_Point+14<=sizeof(gDisp_Buffer))
				    &&((gCommon_Flag&ALLOW_OUT_DISP_F_1)==0)   )
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
						gDisp_Buffer[gDisp_Buffer_Point]=CYear;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=CMonth;
						gDisp_Buffer_Point++;
						gDisp_Buffer[gDisp_Buffer_Point]=CDate;
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
					gDisp_Buffer[gDisp_Buffer_Point]=CHour;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=CMinute;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]=CSecond;
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='$';
					gDisp_Buffer_Point++;
					gDisp_Buffer[gDisp_Buffer_Point]='&';
					gDisp_Buffer_Point++;
				}
			}
			/********************************************************\
			3�����GPS������Ч,����е��йش���
			\********************************************************/
		    if(gGeneral_Flag&GPS_VALID_F_1)
		    {
				/*
		    	//��ʾ��ǰGPS�ľ���γ��ֵ
				Send_COM1_Byte('O');Send_COM1_Byte('[');
				Send_COM1_Byte(ASCII((gLongitude/0x1000000)/0x10));
				Send_COM1_Byte(ASCII((gLongitude/0x1000000)%0x10));
				Send_COM1_Byte(ASCII((gLongitude%0x1000000/0x10000)/0x10));
				Send_COM1_Byte(ASCII((gLongitude%0x1000000/0x10000)%0x10));
				Send_COM1_Byte(ASCII((gLongitude%0x10000/0x100)/0x10));
				Send_COM1_Byte(ASCII((gLongitude%0x10000/0x100)%0x10));
				Send_COM1_Byte(ASCII((gLongitude%0x100)/0x10));
				Send_COM1_Byte(ASCII((gLongitude%0x100)%0x10));
				Send_COM1_Byte(']');
				Send_COM1_Byte('A');Send_COM1_Byte('[');
				Send_COM1_Byte(ASCII((gLatitude/0x1000000)/0x10));
				Send_COM1_Byte(ASCII((gLatitude/0x1000000)%0x10));
				Send_COM1_Byte(ASCII((gLatitude%0x1000000/0x10000)/0x10));
				Send_COM1_Byte(ASCII((gLatitude%0x1000000/0x10000)%0x10));
				Send_COM1_Byte(ASCII((gLatitude%0x10000/0x100)/0x10));
				Send_COM1_Byte(ASCII((gLatitude%0x10000/0x100)%0x10));
				Send_COM1_Byte(ASCII((gLatitude%0x100)/0x10));
				Send_COM1_Byte(ASCII((gLatitude%0x100)%0x10));
				Send_COM1_Byte(']');
				Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
				*/
				/////////////////////////////////////////
                gYear=CYear;
                gMonth=CMonth;
                gDate=CDate;
                gHour=CHour;
                gMinute = CMinute;
                gSecond=CSecond;
                gGPS_Invaild_Timer=0;
				gGps_Null_Count=0;
				gPublic_Flag &=~ GPS_NULL_F_1;
				//�ж��Ƿ��������������м�¼�ǵ�ʱ��������õ�����
				if(   (gPublic_Flag&SET_RECORD_TIME_F_1)
					&&(gSecond==0)
					&&(gStatus==NORMAL_TYPE)
					&&((gPublic_Flag&CHECK_TIME_F_1)==0)  )
				{
					Send_COM1_String( (unsigned char *)CHECK_TIME,sizeof(CHECK_TIME) );
			    	Send_COM1_Byte(gStatus);
			    	Check_Record = gStatus;
			    	Send_COM1_Byte(0xAA);
			    	Check_Record ^= 0xAA;
			    	Send_COM1_Byte(0x75);
			    	Check_Record ^= 0x75;
			    	Send_COM1_Byte(G_SET_TIME);		//������
			    	Check_Record ^= G_SET_TIME;
			    	Send_COM1_Byte(0);
			    	Check_Record ^= 0;
			    	Send_COM1_Byte(6);		//����6
			    	Check_Record ^= 6;
			    	Send_COM1_Byte(0x00);
			    	Check_Record ^= 0x00;	//�����ֽ�
			    	Send_COM1_Byte(gYear);
			    	Check_Record ^= gYear;
			    	Send_COM1_Byte(gMonth);
			    	Check_Record ^= gMonth;
			    	Send_COM1_Byte(gDate);
			    	Check_Record ^= gDate;
			    	Send_COM1_Byte(gHour);
			    	Check_Record ^= gHour;
			    	Send_COM1_Byte(gMinute);
			    	Check_Record ^= gMinute;
			    	Send_COM1_Byte(gSecond);
			    	Check_Record ^= gSecond;
			    	Send_COM1_Byte(Check_Record);					//У���
			    	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);		//��β

				}
				/*
                //D:�洢һ����һ�������ʷ�켣
                if( (CMinute!=CMinute_Temp)&&(gCycle_Send_Status!=PASS_DATA) )
                {
                	gPassTrack_Store_Item++;
                	//��ָ����һ����ʷ��Ĵ洢λ�ã�����ж��Ѿ�����ҳβ������Ҫ���¿�ʼ��һ��ҳ�Ĵ洢
                    if( ( gPassTrack_Store_Item>=16 )||( gPassTrack_Store_Item<=0 ) )
                    {
                    	gPassTrack_Store_Item=1;
                    	//1,�ж��Ƿ��˹涨�洢��ʷ�켣��������һ������
						if((gPassTrack_Store_Sector>=FLASH_PASTDATA_E_2)||(gPassTrack_Store_Sector<FLASH_PASTDATA_S_2) )
						{
							gPassTrack_Store_Sector=FLASH_PASTDATA_S_2;
						}
						else 	gPassTrack_Store_Sector++;
						//2,����һ������������д�ɼ���ҳ
						OperateSPIEnd();
						WriteOneByteToBuffer2(0xFF,0xaa);
						OperateSPIEnd();
						WriteBuffer2ToPage(gPassTrack_Store_Sector+1);
						gTimer=0;Clear_Exter_WatchDog();
						while(gTimer<500){}
						Clear_Exter_WatchDog();
						OperateSPIEnd();
                    }
					OperateSPIEnd();
					//����ӦgPassTrack_Store_Sector�����е����ݵ�buffer2.
					ReadOnePageToBuffer2(gPassTrack_Store_Sector);
					//���������������Ҫ�浽����һ��λ��
					i=(gPassTrack_Store_Item-1)*17;
					OperateSPIEnd();
					//long int gLatitude  gLongitude �ĸ��ֽ�
					WriteOneByteToBuffer2( (i+0),gLongitude/0x1000000);
					WriteNextByteToBuffer2( gLongitude%0x1000000/0x10000 );
					WriteNextByteToBuffer2( gLongitude%0x10000/0x100 );
					WriteNextByteToBuffer2( gLongitude%0x100 );

					WriteNextByteToBuffer2( gLatitude/0x1000000 );
					WriteNextByteToBuffer2( gLatitude%0x1000000/0x10000 );
					WriteNextByteToBuffer2( gLatitude%0x10000/0x100 );
					WriteNextByteToBuffer2( gLatitude%0x100 );

					WriteNextByteToBuffer2( gSpeed );
					WriteNextByteToBuffer2( gAzimuth );
					WriteNextByteToBuffer2( nFlag|FLAG_GPSVAILD_1 );
					WriteNextByteToBuffer2( gYear );
					WriteNextByteToBuffer2( gMonth );
					WriteNextByteToBuffer2( gDate );
					WriteNextByteToBuffer2( gHour );
					WriteNextByteToBuffer2( gMinute );
					WriteNextByteToBuffer2( gSecond );
					OperateSPIEnd();
					//д�Ѿ��洢������
					WriteOneByteToBuffer2( 0xFF,gPassTrack_Store_Item );

					//����洢�������ͳ������
					WriteNextByteToBuffer2( gALL_Distance/0x1000000 );
					WriteNextByteToBuffer2( gALL_Distance%0x1000000/0x10000 );
					WriteNextByteToBuffer2( gALL_Distance%0x10000/0x100 );
					WriteNextByteToBuffer2( gALL_Distance%0x100 );
					OperateSPIEnd();
					//��buffer2�е�����д����Ӧ��Flash��������
					if( (gPassTrack_Store_Sector>=FLASH_PASTDATA_S_2)&&(gPassTrack_Store_Sector<=FLASH_PASTDATA_E_2) )
					{
						WriteBuffer2ToPage(gPassTrack_Store_Sector);
						gTimer=0;Clear_Exter_WatchDog();
						while(gTimer<500){}
						Clear_Exter_WatchDog();
					}
					OperateSPIEnd();
					CMinute_Temp=CMinute;
                }
           		*/
		    }
			/********************************************************\
			4�����GPS������Ч���������صĴ���
			\********************************************************/
		    else
		    {
				gGPS_Invaild_Timer++;
				//�������1800�������Ч�㣬�����µ��紦��GPSģ��
				if(gGPS_Invaild_Timer>GPS_INVALID_COUNT)
				{
					gGPS_Invaild_Timer=0;
					gGeneral_Flag|=OFF_GPS_F_1;
					gOFF_Power_S=1;
				}
		    }
		}
		GPS_Buffer_Point=0;
		gGPS_Receive_Over=0;
		for(i=0;i<sizeof(GPS_Buffer);i++) GPS_Buffer[i]=0;
	}
	TAUARTCTL|=TAUARTIE;       //�Ƚ���A���ж�����
}
/********************************************************\
*	��������Receive_COM2(�ж��ӳ���)
	�����򣺱����ļ�����
*	���ܣ�	���ñȽ���A���ж�����������ģ�⴮��2�Ľ��մ���,��������GPS������
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
interrupt [TAUART_VECTOR] void Receive_COM2(void)
{
	TAUARTCTL&=~TAUARTIFG;	//����Ƚ���A���жϱ�־λ
	if(RTI2&RECEIVE_FLAG)	//��ʾ���յ�һ���ֽ����
	{
		GPS_Buffer[GPS_Buffer_Point]=SBUFIN2;
		GPS_Buffer_Point++;
		if(GPS_Buffer_Point>=sizeof(GPS_Buffer))	GPS_Buffer_Point=0;
		gGPS_Receive_Over=0;
		RX2_Ready();
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
*	�޸���ʷ����ÿ������
   Dim Instring As String
   Dim Buffer(10) As Byte
   Dim IObuffer(14) As Byte
   Dim InBuffer(10) As Byte
   Dim Savebuffer(5) As Byte

   Dim I

   Buffer(0) = &H10
   Buffer(1) = &H7A
   Buffer(2) = &H0
   Buffer(3) = &H1
   Buffer(4) = &H0
   Buffer(5) = &H0
   Buffer(6) = &H1  ' RMC
   Buffer(7) = &H0
   'buffer(7)=&H1C ������GSA��GSV��VTG��ʱ��buffer(7)=&H1C
   Buffer(8) = &H10
   Buffer(9) = &H3


   IObuffer(0) = &H10
   IObuffer(1) = &HBC
   IObuffer(2) = &H0
   IObuffer(3) = &H7
   IObuffer(4) = &H7
   IObuffer(5) = &H3
   IObuffer(6) = &H0
   IObuffer(7) = &H0
   IObuffer(8) = &H0
   IObuffer(9) = &H2
   IObuffer(10) = &H4
   IObuffer(11) = &H0
   IObuffer(12) = &H10
   IObuffer(13) = &H3

   Savebuffer(0) = &H10
   Savebuffer(1) = &H8E
   Savebuffer(2) = &H26
   Savebuffer(3) = &H10
   Savebuffer(4) = &H3


   ' ʹ�� COM1��
   MSComm1.CommPort = 1
   ' 9600 ���أ�����żУ�飬8 λ���ݣ�һ��ֹͣλ��
   MSComm1.Settings = "9600,O,8,1"
   ' ������ռ��ʱ��
   ' ���߿ؼ�����������������
   MSComm1.InputLen = 0
   ' �򿪶˿ڡ�
   MSComm1.PortOpen = True
   ' �� attention �����͵����ƽ������
   MSComm1.Output = Buffer ' "ATV1Q0" & Chr$(13) ' ȷ��

   MSComm1.Output = IObuffer

   'MSComm1.Output = Savebuffer

   MSComm1.PortOpen = False

   ''''''''''''''
    MSComm1.CommPort = 1
   ' 9600 ���أ�����żУ�飬8 λ���ݣ�һ��ֹͣλ��
   MSComm1.Settings = "9600,N,8,1"
   ' ������ռ��ʱ��
   ' ���߿ؼ�����������������
   MSComm1.InputLen = 0
   ' �򿪶˿ڡ�
   MSComm1.PortOpen = True
   ' �� attention �����͵����ƽ������

   MSComm1.Output = Savebuffer

   MSComm1.PortOpen = False

\********************************************************/