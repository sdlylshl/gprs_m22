/********************************************************\
*	�ļ�����  Do_Reset.h
*	����ʱ�䣺2004��10��20��
*	�����ˣ�  
*	�汾�ţ�  1.00
*	���ܣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
#include <msp430x14x.h>
#include "General.h"
#include "Other_Define.h"
#include "Main_Init.h"
#include "Do_Reset.h"
#include "M22_AT.h"
#include "Check_GSM.h"
#include "Sub_C.h"
#include "Define_Bit.h"
#include "Uart01.h"
#include "SPI45DB041.h"
#include "D_Buffer.h"
#include "W_Protocol.h"
#include "Do_SRAM.h"
/*
#include "Disp_Protocol.h"
*/
//#define Debug_GSM_COM1
void Wait_Return_M22(unsigned char Next_Step);
/********************************************************\
*	��������Do_OFF_Power
	�������ⲿ�ļ�����
*	���ܣ�  ����GPS��GSMģ����������Ĳ�����
*	������
*	����ֵ����Ҫ��һЩ��ص�ȫ�ֱ������и�ֵ
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Do_OFF_Power(void)
{
	//���øߵ�ƽ���Թض�2576���
	Clear_Exter_WatchDog();
	gM22_GSM_ECHO_Timer=0;
	gGet_Driver_Info_Count=0;
	gRecord_Data_Timer=0;
	if(gOFF_Power_S==1)
	{
		P6OUT|=POWER_ON_OFF;
	    gTimer=0;
	    gOFF_Power_S=2;
	}
	//�ӳ�OFF_TIMEʱ������������õ͵�ƽ������2576���
	else if((gOFF_Power_S==2)&&(gTimer>OFF_TIME))
	{
		P6OUT&=~POWER_ON_OFF;
	    gOFF_Power_S=3;
	    gTimer=0;
	}
	else if((gOFF_Power_S==3)&&(gTimer>DELAY_TIME))
	{
	    gTimer=0;
	    gOFF_Power_S=0;
	    if(gGeneral_Flag&OFF_M22_F_1)
	    {
			Send_COM1_Byte('M');
	    }
	    else if(gGeneral_Flag&OFF_GPS_F_1)
	    {
			Send_COM1_Byte('G');
	    }
	    Send_COM1_String((unsigned char *)POWER_OFF,sizeof(POWER_OFF));
	    Reset_Variable();
	    gGeneral_Flag&=~OFF_M22_F_1;				//ȡ������M22�ı�־λ
	    gGeneral_Flag&=~OFF_GPS_F_1;				//ȡ������GPS�ı�־λ
	    gPhone_Status=READY;
	    gStatus1_Flag&=~PHONE_ONLINE_F_1;
	}
}
/********************************************************\
*	��������Do_Reset_GSM_On
	�������ⲿ�ļ�����
*	���ܣ�  ���G20ģ�����ϵ�����������Ĳ���������һЩATָ��ĳ�ʼ��
*	������
*	����ֵ����Ҫ��һЩ��ص�ȫ�ֱ������и�ֵ
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Do_Reset_GSM_On(void)
{
	static unsigned char M22_ReturnER_Count=0;		//ATָ��ش���ļ���
	unsigned int i=0,j=0;
	gVolume=TWO;
	gGeneral_Flag&=~LAND_TCP_F_1;
	Clear_Exter_WatchDog();
	gGet_Driver_Info_Count=0;
	gSet_Distance_Info_Count=0;
	//1,AT
	if(gReset_M22_On_Setp==1)
	{
		if(gTimer_GSM_AT>300)
		{
			Send_COM0_String((unsigned char *)AT,sizeof(AT));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=2;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==2)
	{
		if(Check_GSM()!=0)
		{
			gTimer_GSM_AT=0;
		    if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK))!=0)
		    {
		    	M22_ReturnER_Count=0;
		    	gReset_M22_On_Setp=3;
		    }
		    else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER))!=0)
		    {
		    	M22_ReturnER_Count++;
				if(M22_ReturnER_Count>10)
				{
					M22_ReturnER_Count=0;
					gGeneral_Flag|=OFF_M22_F_1;
					gOFF_Power_S=1;
					gPhone_Status=READY;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
					return;
				}
				else	gReset_M22_On_Setp=1;
		    }
		}
		else if(gTimer_GSM_AT>3000)
		{
	    	M22_ReturnER_Count++;
			if(M22_ReturnER_Count>10)
			{
				M22_ReturnER_Count=0;
				gGeneral_Flag|=OFF_M22_F_1;
				gOFF_Power_S=1;
				gPhone_Status=READY;
				gStatus1_Flag&=~PHONE_ONLINE_F_1;
				return;
			}
			else	gReset_M22_On_Setp=1;
		}
	}
	//2��ATZ
	else if(gReset_M22_On_Setp==3)
	{
		if(gTimer_GSM_AT>300)
		{
			Send_COM0_String((unsigned char *)ATZ,sizeof(ATZ));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=4;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==4)
	{
		Wait_Return_M22(5);
		/*
		if(Check_GSM()!=0)
		{
			gTimer_GSM_AT=0;
		    if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK))!=0)
		    {
		    	gReset_M22_On_Setp=5;
		    }
		    else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER))!=0)
		    {
		    	gReset_M22_On_Setp=1;
		    }
		}
		else if(gTimer_GSM_AT>3000)
		{
			gReset_M22_On_Setp=1;
		}
		*/
	}
	//3,AT+CGMM
	else if(gReset_M22_On_Setp==5)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_CGMM,sizeof(AT_CGMM));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=6;
		}
		else	Check_GSM();
	}
	//4,AT+CGMR
	else if(gReset_M22_On_Setp==6)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_CGMR,sizeof(AT_CGMR));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=7;
		}
		else	Check_GSM();
	}
	//5,AT+CGSN
	else if(gReset_M22_On_Setp==7)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_CGSN,sizeof(AT_CGSN));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=8;
		}
		else	Check_GSM();
	}
	//6,AT+CMGF=0
	else if(gReset_M22_On_Setp==8)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_CMGF0,sizeof(AT_CMGF0));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=9;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==9)
	{
		Wait_Return_M22(10);
		/*
		if(Check_GSM()!=0)
		{
			gTimer_GSM_AT=0;
		    if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK))!=0)
		    {
		    	gReset_M22_On_Setp=10;
		    }
		    else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER))!=0)
		    {
		    	gReset_M22_On_Setp=1;
		    }
		}
		else if(gTimer_GSM_AT>6000)
		{
			gReset_M22_On_Setp=1;
		}
		*/
	}
	//6-1��AT+CREG?
	else if(gReset_M22_On_Setp==10)
	{
		if(gTimer_GSM_AT>2000)
		{
			Send_COM0_String((unsigned char *)AT_CREG,sizeof(AT_CREG));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=11;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==11)
	{
		if(Check_GSM()!=0)
		{
			gTimer_GSM_AT=0;
		    if(Compare_String(gGeneral_Buffer,(unsigned char *)String_CREG,sizeof(String_CREG))!=0)
		    {
		    	if( (gGeneral_Buffer[8]==',')&&(gGeneral_Buffer[10]=='1') )
		    	{
		    		gReset_M22_On_Setp=12;
		    		gM22_ReturnER_SIM_Count=0;
		    	}
		    	else
		    	{
		    		gM22_ReturnER_SIM_Count++;
		    		if(gM22_ReturnER_SIM_Count>60)
		    		{
						gGeneral_Flag|=OFF_M22_F_1;
						gOFF_Power_S=1;
						gPhone_Status=READY;
						gM22_ReturnER_SIM_Count=0;
						return;
		    		}
		    		else
		    		{
		    			gReset_M22_On_Setp=10;
		    			gTimer_GSM_AT=0;
		    		}
		    	}
		    }
		}
		else if(gTimer_GSM_AT>6000)
		{
			gReset_M22_On_Setp=1;
		}
	}
	//7,AT+COPS?
	else if(gReset_M22_On_Setp==12)
	{
		if(gTimer_GSM_AT>2000)
		{
			Send_COM0_String((unsigned char *)AT_COPS_C,sizeof(AT_COPS_C));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=13;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==13)
	{
		if(Check_GSM()!=0)
		{
			gTimer_GSM_AT=0;
		    if(Compare_String(gGeneral_Buffer,(unsigned char *)String_COPS,sizeof(String_COPS))!=0)
		    {
		    	if( (gGeneral_Buffer[8]==',')&&(gGeneral_Buffer[9]=='0') )
		    	{
		    		//��ʾ�Ѿ��ҵ�SIM��������
		    		gReset_M22_On_Setp=14;
		    		gM22_ReturnER_SIM_Count=0;
		    	}
		    	else
		    	{
	    			gM22_ReturnER_SIM_Count++;
	    			if(gM22_ReturnER_SIM_Count>60)
	    			{
						gGeneral_Flag|=OFF_M22_F_1;
						gOFF_Power_S=1;
						gPhone_Status=READY;
						gM22_ReturnER_SIM_Count=0;
						gStatus1_Flag&=~PHONE_ONLINE_F_1;
						return;
	    			}
	    			else
	    			{
	    				gReset_M22_On_Setp=12;
	    			}
		    	}
		    }
		}
		else if(gTimer_GSM_AT>6000)
		{
			gReset_M22_On_Setp=1;
		}
	}
	//8,AT+CIMI
	else if(gReset_M22_On_Setp==14)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_CIMI,sizeof(AT_CIMI));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=15;
		}
		else	Check_GSM();
	}
	//9,AT+CSCA?
	else if(gReset_M22_On_Setp==15)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_CSCA_C,sizeof(AT_CSCA_C));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=16;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==16)
	{
		if(Check_GSM()!=0)
		{
			gTimer_GSM_AT=0;
		    if(Compare_String(gGeneral_Buffer,(unsigned char *)String_CSCA,sizeof(String_CSCA))!=0)
		    {
		    	//���彫�����Ķ���Ϣ���ĺ���洢��buffer1��Ӧ�������У�ֱ����ASCII��ʽ�洢��
		    	if(gGeneral_Buffer[7]=='"')
		    	{
		    		i=8;
					while(1)
					{
						if(gGeneral_Buffer[i]=='"') break;
						i++;if(i>8+20) {gReset_M22_On_Setp=1;return;}		//�Ƿ����ȣ����˳�
					}
					if(i>8+1)
					{
						Load_SMS_CenterNum(i);
						gInternal_Flag|=CENTER_NUM_VAILD_F_1;
						gInternal_Flag&=~SET_CSCA_F_1;
					}
					else
					{
						gInternal_Flag&=~CENTER_NUM_VAILD_F_1;
						gInternal_Flag|=SET_CSCA_F_1;
					}
					gReset_M22_On_Setp=17;
				}
		    	else	gReset_M22_On_Setp=1;
		    }
		    else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER))!=0)
		    {
		    	gReset_M22_On_Setp=1;
		    }
		}
	}
	//10,AT$NOSLEEP=1
	else if(gReset_M22_On_Setp==17)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_NOSLEEP1,sizeof(AT_NOSLEEP1));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=18;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==18)
	{
		Wait_Return_M22(19);
	}
	//11,AT+COLP=1
	else if(gReset_M22_On_Setp==19)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_COLP1,sizeof(AT_COLP1));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=20;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==20)
	{
		Wait_Return_M22(21);
	}
	//12,AT+CLIP=1
	else if(gReset_M22_On_Setp==21)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_CLIP1,sizeof(AT_CLIP1));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=22;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==22)
	{
		Wait_Return_M22(23);
	}
	//13,AT$AUPATH=3,1,ѡ���ֵ�����ͨ��
	else if(gReset_M22_On_Setp==23)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_AUPATH,sizeof(AT_AUPATH));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=24;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==24)
	{
		Wait_Return_M22(25);
	}
	//14,AT$AUGAIN=(0-2),(1,9)
	else if(gReset_M22_On_Setp==25)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_AUGAIN,sizeof(AT_AUGAIN));
			Send_COM0_Byte('1');Send_COM0_Byte(',');
			Send_COM0_Byte('5');Send_COM0_Byte(0x0d);
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=26;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==26)
	{
		Wait_Return_M22(27);
	}
	else if(gReset_M22_On_Setp==27)
	{
		if(gTimer_GSM_AT>1000)
		{
			//2,�̶�����SPEKER�Ľ�������
			Send_COM0_String((unsigned char *)AT_AUGAIN,sizeof(AT_AUGAIN));
			Send_COM0_Byte('2');Send_COM0_Byte(',');Send_COM0_Byte(Handle_VOL[TWO]);Send_COM0_Byte(0x0d);
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=28;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==28)
	{
		Wait_Return_M22(29);
	}
	else if(gReset_M22_On_Setp==29)
	{
		if(gTimer_GSM_AT>1000)
		{
			//3,����AT$AUVOL�Ľ�������
			Send_COM0_String((unsigned char *)AT_AUVOL,sizeof(AT_AUVOL));
			Send_COM0_Byte('5');Send_COM0_Byte(0x0d);
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=30;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==30)
	{
		Wait_Return_M22(31);
	}
	//15,AT+CNMI=2,2,0,0,0
	else if(gReset_M22_On_Setp==31)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_CNMI,sizeof(AT_CNMI));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=32;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==32)
	{
		Wait_Return_M22(33);
	}
	//16,AT$TIMEOUT=XX(XX��ʾ100--5000����)
	else if(gReset_M22_On_Setp==33)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_TIMEOUT,sizeof(AT_TIMEOUT));
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=34;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==34)
	{
		Wait_Return_M22(35);
	}
	//17,AT+CMGD=1,4
	else if(gReset_M22_On_Setp==35)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_CMGD,sizeof(AT_CMGD));
			Send_COM0_Byte('1');
			Send_COM0_Byte(',');
			Send_COM0_Byte('4');
			Send_COM0_Byte(0x0d);
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=36;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==36)
	{
		Wait_Return_M22(37);
	}
	//18,AT+CMGD=1,4
	else if(gReset_M22_On_Setp==37)
	{
		if(gTimer_GSM_AT>1000)
		{
			Send_COM0_String((unsigned char *)AT_CMGD,sizeof(AT_CMGD));
			Send_COM0_Byte('1');
			Send_COM0_Byte(',');
			Send_COM0_Byte('4');
			Send_COM0_Byte(0x0d);
			gTimer_GSM_AT=0;
			gReset_M22_On_Setp=38;
		}
		else	Check_GSM();
	}
	else if(gReset_M22_On_Setp==38)
	{
		Wait_Return_M22(39);
	}
	//19,��ʾ�����ʱ�������
	else if(gReset_M22_On_Setp==39)
	{
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		//�����̣�������̩ɭ�������޹�˾
		Send_COM1_String( (unsigned char *)MADE,sizeof(MADE) );
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		//�����ţ�
		Send_COM1_String( (unsigned char *)AUTHOR,sizeof(AUTHOR) );
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		//��Ʒ�ͺ�:
		Send_COM1_String( (unsigned char *)MODEL,sizeof(MODEL) );
		Send_COM1_Byte(':');
		Send_COM1_String( (unsigned char *)MODEL_TYPE2,sizeof(MODEL_TYPE2) );
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		//��������
		Send_COM1_String( (unsigned char *)DESIGN_DATA,sizeof(DESIGN_DATA) );
		Send_COM1_String( (unsigned char *)EDIT_DATE,sizeof(EDIT_DATE) );
		//�����汾
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		Send_COM1_String( (unsigned char *)SOFT_EDITION,sizeof(SOFT_EDITION) );
		Send_COM1_String( (unsigned char *)GPS_2005A,sizeof(GPS_2005A) );
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		Send_COM1_String( (unsigned char *)UPDATE_TIME,sizeof(UPDATE_TIME) );
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);

		gReset_M22_On_Setp=40;
	}
	//20,M22��ʼ�����
	else if(gReset_M22_On_Setp==40)
	{
		gReset_M22_On_Setp=0;
		gGeneral_Flag&=~RESET_GSM_ON_F_1;
		gM22_Status=GSM_WORK;
		gGeneral_Flag&=~LAND_TCP_SUCCESS_F_1;
		gGSM_Oper_Type=GSM_PHONE;
		gInternal_Flag&=~GSM_ONEAT_SUCCESS_1;
		gPhone_Step=0;gTimer_GSM_AT=0;

		Send_COM1_String( (unsigned char *)M22_RESET_ON_OK,sizeof(M22_RESET_ON_OK) );

		gReceive1_Point=SRAM_RECORD_S;

		SRAM_Init();
		SRAM_Write(PHONE_AT_2+0,NULL_2);
		for(i=PHONE_AT_2+1;i<ECHO_SMS_2;i++)
		{
			SRAM_Write(i,0);
		}
		gInternal_Flag|=DISP_ON_F_1;
		gStatus1_Flag|=DISPLAY_ON_F_1;
		gTimer=0;
		while(gTimer<1000){Check_GSM();Clear_Exter_WatchDog();}
		gRecord_Data_Timer=0;
		gTimer_CSQ=GSM_CHECKCSQ_TIME+1;
		SRAM_Init();
		SRAM_Write(SHANDLE_DATA_VAILD,NULL_2);
		gCheck_Par_Timer=CHECK_SYS_PAR_TIME+1;
	}
}
/********************************************************\
*	��������Ask_SMS_Send
	�������ⲿ�ļ�����
*	���ܣ�  �ڽ���SMS�ķ��Ͳ���AT+CMGS=L(����)
*	������  Input
*	����ֵ: ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Ask_SMS_Send(unsigned char nInput)
{
	Send_COM0_String((unsigned char *)AT_CMGS,sizeof(AT_CMGS));
	Send_COM0_Byte(ASCII(nInput/100));
	Send_COM0_Byte(ASCII((nInput%100)/10));
	Send_COM0_Byte(ASCII(nInput%10));
	Send_COM0_Byte(0x0d);
}

/********************************************************\
*	��������Send_SMS_Head
	�������ⲿ�ļ�����
*	���ܣ�  ����SMS�ı�׼��ʽ�������ǽ�����Ϣ���ĺ��룬Ŀ��绰���룬Э���ʽ
			���͵�GSMģ��
*	������  I
*	����ֵ: ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
	����SMS��ͷ����������Ϣ���ĺ������ڵĹ̶�����
	ע�������ͷ����G18�ﲻһ�������뽫����Ϣ���ĺ��������ݰ�ͷ��

	AT+CMGS=27
	>
	0891683108705505F011000D91683167108230F70008000CFF4FB008000101000102090D

    �������ģ���з��͵�������Ϊ0891683108705505F011000D91683167108230F7000800
    �������ֻ���з���SMS���ݵĹ������֡��������Ŀ�ļ��У�ֻҪ����Ϣ���ĺ���
    ��Ŀ��绰���벻�䣬���ⲿ�ֵķ�������һ����
    0CFF4FB008000101000102090D �ⲿ�����������������ɺ���ĳ�������

	����27�ĳ�����ָ11000D91683167108230F70008000CFF4FB008000101000102090D

	AT+CSCA?
	+CSCA: "+8613800755500",145

	OK

\********************************************************/
void Send_SMS_Head(void)
{
	unsigned int i,j,k,x;
	//������Ϣ���ĺ���ĳ���+8613800755500��11λ��
	SRAM_Init();
	j=SRAM_Read(CENTER_NUM_2);
	//�ж��Ƿ���+��
	i=SRAM_Read(CENTER_NUM_2+1);
	if(i=='+')
	{
		k=(j+1)/2+1;gInternal_Flag&=~SMS_CENTER_CLASS_1;	//��91
	}
	else
	{
		k=(j+1)/2+2;gInternal_Flag|=SMS_CENTER_CLASS_1;		//��A1
	}
	//���ͺ���ĳ���
	Send_COM0_Byte(ASCII(k/16));
	Send_COM0_Byte(ASCII(k%16));
	if(gInternal_Flag&SMS_CENTER_CLASS_1)	Send_COM0_Byte('A');
	else									Send_COM0_Byte('9');
	Send_COM0_Byte('1');
	//��A1,����+����ǰ���iҲ����������Ч��������
	if(gInternal_Flag&SMS_CENTER_CLASS_1)
	{
		k=i;
		x=SRAM_Read(CENTER_NUM_2+2);
		Send_COM0_Byte(x);
		Send_COM0_Byte(k);
		i=3;
	}
	//��91��ǰ���+
	else
	{
		k=SRAM_Read(CENTER_NUM_2+2);
		x=SRAM_Read(CENTER_NUM_2+3);
		Send_COM0_Byte(x);
		Send_COM0_Byte(k);
		i=4;
	}
	while(1)
	{
		if(i<j)//����������j=11
		{
			k=SRAM_Read(CENTER_NUM_2+i);
			x=SRAM_Read(CENTER_NUM_2+i+1);
			Send_COM0_Byte(x);
			Send_COM0_Byte(k);
			i++;
			i++;
		}
		//���Ͷ���Ϣ���ĺ�������һ���ֽ�0
		else if(i==j)
		{
			k=SRAM_Read(CENTER_NUM_2+i);
			Send_COM0_Byte('F');
			Send_COM0_Byte(k);
			break;
		}
		else break;
	}
	// ����1100 (��������)����������е�Э���о�һ��
	for(i=0;i<sizeof(SMS_Head_1);i++)
	{
		Send_COM0_Byte(SMS_Head_1[i]);
	}
	//++++++++++++++++++++++++++++++++++++++++++++++
	//����Ŀ��绰����
	//����������洢Ϊ 14,'+','8','6','1','3','6','7','0','1','5','5','7','0','4'
	//���ߴ洢Ϊ		13,'8','6','1','3','6','7','0','1','5','5','7','0','4'
	j=SRAM_Read(TARGET_NUMBER1_2);
	i=SRAM_Read(TARGET_NUMBER1_2+1);
	if(i=='+')
	{
		k=j-1;gInternal_Flag&=~SMS_TARGET_CLASS_1;
	}
	else
	{
		k=j;gInternal_Flag|=SMS_TARGET_CLASS_1;
	}
	Send_COM0_Byte(ASCII(k/16));
	Send_COM0_Byte(ASCII(k%16));

	if(gInternal_Flag&SMS_TARGET_CLASS_1)	Send_COM0_Byte('A');
	else									Send_COM0_Byte('9');
	Send_COM0_Byte('1');
	//��A1,����+����ǰ���iҲ����������Ч��������
	if(gInternal_Flag&SMS_TARGET_CLASS_1)
	{
		k=i;
		x=SRAM_Read(TARGET_NUMBER1_2+2);
		Send_COM0_Byte(x);
		Send_COM0_Byte(k);
		i=3;
	}
	else									//��91��ǰ���+
	{
		k=SRAM_Read(TARGET_NUMBER1_2+2);
		x=SRAM_Read(TARGET_NUMBER1_2+3);
		Send_COM0_Byte(x);
		Send_COM0_Byte(k);
		i=4;
	}
	while(1)
	{
		if(i<j)
		{
			k=SRAM_Read(TARGET_NUMBER1_2+i);
			x=SRAM_Read(TARGET_NUMBER1_2+i+1);
			Send_COM0_Byte(x);
			Send_COM0_Byte(k);
			i++;
			i++;
		}
		//Ŀ��绰��������һ���ֽ�
		else if(i==j)
		{
			k=SRAM_Read(TARGET_NUMBER1_2+i);
			Send_COM0_Byte('F');
			Send_COM0_Byte(k);
			OperateSPIEnd();
			break;
		}
		else break;
	}
	OperateSPIEnd();
	// ���Ͷ���Ϣ��Э�鷽ʽ(��������)����������е�Э���о�һ��
	for( i=0;i<6;i++ )
	{
		Send_COM0_Byte(SMS_Head_2[i]);
	}
}

/********************************************************\
*	��������Send_Echo_GSM_Head
	�������ⲿ�ļ�����
*	���ܣ�

*	������  nInput 			�������SMS�ķ��ͣ������������ʾ�������ݰ��ĳ���
 			nCommand		���ݰ��е�������
 			nACK			ACK��Ӧ������������������͵��������ݰ�����ACK=0
 			nLength			ָ���������ݰ��д���Ϣ�ŵ�CHK��ǰ�����ݵ��ֽ���

*	����ֵ: iCheckOut  		�ⲿ�ַ������ݵĺͣ���������У��ˣ�
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������

	����˵����������buffer1�д洢��2��SMS��������ʽ���£�0x7B----0xB6����������������С��
			  0x7B	�洢��ʾ��������SMS�Ƿ�����Ϊ1���ʾ���滹��δ������SMS��
			  		Ϊ0���ʾ�Ѿ���������SMS
			  0x7C	�洢���ݰ�����������������ΪnCommand
			  0x7D	�洢���ݰ���ACK��ֵACK
			  0x7E	�洢�������ݰ��ĳ��ȣ���ע��û�д洢CHK(MSB),CHK(LSB),EOT,�������ֽڣ�
			  0x7F  ���⿪ʼ�洢���ݰ�

	AT+CMGS=27
	>
	0891683108705505F011000D91683167108230F70008000CFF4FB008000101000102090D

	����27�ĳ�����ָ���Ǵ�1100 0D 91 68 3167108230F7 000800 0C FF4FB008000101000102090D
										13760128037
	����Send_Echo_Head_1��ģ��ļ��������ļ��㷽ʽ���£�
	nInput�ļ��㷽ʽ��	nInput=12+(0x7d)
	nCommand�ĸ�ֵ��ʽ��nCommand=(0x7C)
	nACK�ĸ�ֵ��ʽ��	nACK=(0x7E)
	nLength�ĸ�ֵ��ʽ��	nLength=nInput-4

	����������������ڷ��͵ڶ���SMS��ʱ��,AT+CMGS=����������ݵĳ��ȵļ��㷽ʽ��Ϊ

	(Ŀ����볤��+1)/2+8+12+(0x7E)
	�������������ӣ�Ŀ�����Ϊ13760128037Ϊ11λ.  8 ��ָ��1100 0D 91 68 000800�ĳ���
					  12ָ����0C�ĳ��ȣ���Ϊ�������ڵ�һ���SMS�ĸ�ʽ�����ݳ���Ϊ0��
					  �����ڵڶ����SMS�л���Ҫ�����������ĳ��ȣ�������洢��buffer1�е�0x7e����

\********************************************************/
unsigned int Send_Echo_GSM_Head(unsigned char nInput,unsigned char nCommand,unsigned char nACK,unsigned char nLength)
{
	unsigned int iCheckOut=0;
	Send_COM0_Byte(ASCII(nInput/0x10));
	Send_COM0_Byte(ASCII(nInput%0x10));
	iCheckOut=0;
	Send_COM0_Byte(ASCII(SOH/0x10));
	Send_COM0_Byte(ASCII(SOH%0x10));
	iCheckOut+=SOH;
	Send_COM0_Byte(ASCII((char)nCommand/0x10));
	Send_COM0_Byte(ASCII((char)nCommand%0x10));
	iCheckOut+=(char)nCommand;
	Send_COM0_Byte(ASCII((char)(~nCommand)/0x10));
	Send_COM0_Byte(ASCII((char)(~nCommand)%0x10));
	iCheckOut+=(char)(~nCommand);
	Send_COM0_Byte(ASCII((nLength)/0x10));
	Send_COM0_Byte(ASCII((nLength)%0x10));
	iCheckOut+=(nInput-4);
	Send_COM0_Byte(ASCII(nACK/0x10));		//ACK��Ӧ�ַ�
	Send_COM0_Byte(ASCII(nACK%0x10));
	iCheckOut+=nACK;
	Send_COM0_Byte(ASCII(gKind/0x10));
	Send_COM0_Byte(ASCII(gKind%0x10));
	iCheckOut+=gKind;
	Send_COM0_Byte(ASCII(gGroup/0x10));
	Send_COM0_Byte(ASCII(gGroup%0x10));
	iCheckOut+=gGroup;
	Send_COM0_Byte(ASCII(gVID/0x1000));
	Send_COM0_Byte(ASCII(gVID%0x1000/0x100));
	iCheckOut+=gVID/0x100;
	Send_COM0_Byte(ASCII(gVID%0x100/0x10));
	Send_COM0_Byte(ASCII(gVID%0x10));
	iCheckOut+=gVID%0x100;
	return(iCheckOut);
	//iCheckOutΪ����ǰ�淢�Ͳ��ֵ����ݵ�У��͡�������滹�и��������
	//�ڼ�������У��˵�ʱ����Ҫ�����������һ���������
}
/********************************************************\
*	��������Send_Echo_TCP_Head
	�������ⲿ�ļ�����
*	���ܣ�
*	������ iCheckIn	ǰ������У���
		   nAddress ��Ҫ����������buffer1�е��׵�ַ
		   nLength	��Ҫ���͵�������buffer1�еĳ���

*	����ֵ: iCheckOut  �ⲿ�ַ������ݵĺͣ���������У��ˣ�
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������

\********************************************************/
unsigned int Send_Echo_TCP_Head(unsigned char nInput,unsigned char nCommand,unsigned char nACK,unsigned char nLength)
{
	unsigned int iCheckOut=0;
	iCheckOut=nInput;
	iCheckOut=0;
	Send_COM0_Byte(SOH);
	iCheckOut+=SOH;
	Send_COM0_Byte( nCommand );
	iCheckOut+= nCommand;
	Send_COM0_Byte( (char)(~nCommand) );
	iCheckOut+=(char)(~nCommand);
	Send_COM0_Byte( nLength );
	iCheckOut+=nLength;
	Send_COM0_Byte( nACK );
	iCheckOut+=nACK;
	Send_COM0_Byte( gKind );
	iCheckOut+=gKind;
	Send_COM0_Byte( gGroup );
	iCheckOut+=gGroup;
	Send_COM0_Byte( gVID/0x100 );
	iCheckOut+=gVID/0x100;
	Send_COM0_Byte( gVID%0x100 );
	iCheckOut+=gVID%0x100;
	return(iCheckOut);
	//iCheckOutΪ����ǰ�淢�Ͳ��ֵ����ݵ�У��͡�������滹�и��������
	//�ڼ�������У��˵�ʱ����Ҫ�����������һ���������
}
/********************************************************\
*	��������Send_Buffer1_GSM_Data
	�������ⲿ�ļ�����
*	���ܣ� ������ʹ洢��buffer1�е������������ݡ�������У���
*	������ iCheckIn	ǰ������У���
		   nAddress ��Ҫ����������buffer1�е��׵�ַ
		   nLength	��Ҫ���͵�������buffer1�еĳ���

*	����ֵ: iCheckOut  �ⲿ�ַ������ݵĺͣ���������У��ˣ�
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������

\********************************************************/
unsigned int Send_Buffer1_GSM_Data(unsigned int iCheckIn,unsigned char nAddress,unsigned char nLength)
{
	unsigned int iCheckOut,i;
	unsigned char Data_Temp;
	OperateSPIEnd();
	iCheckOut=iCheckIn;
	if(nLength>0)
	{
		Data_Temp=SRAM_Read(nAddress);
		iCheckOut += Data_Temp;
		Send_COM0_Byte(ASCII(Data_Temp/0x10));
		Send_COM0_Byte(ASCII(Data_Temp%0x10));
		for(i=1;i<nLength;i++)
		{
			Data_Temp=SRAM_Read(nAddress+i);
			iCheckOut += Data_Temp;
			Send_COM0_Byte(ASCII(Data_Temp/0x10));
			Send_COM0_Byte(ASCII(Data_Temp%0x10));
		}
	}
	OperateSPIEnd();
	return(iCheckOut);
}
/********************************************************\
*	��������Send_Buffer1_TCP_Data
	�������ⲿ�ļ�����
*	���ܣ� ������ʹ洢��buffer1�е������������ݡ�������У���
*	������ iCheckIn	ǰ������У���
		   nAddress ��Ҫ����������buffer1�е��׵�ַ
		   nLength	��Ҫ���͵�������buffer1�еĳ���

*	����ֵ: iCheckOut  �ⲿ�ַ������ݵĺͣ���������У��ˣ�
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
unsigned int Send_Buffer1_TCP_Data(unsigned int iCheckIn,unsigned char nAddress,unsigned char nLength)
{
	unsigned int iCheckOut,i;
	unsigned char Data_Temp;
	OperateSPIEnd();
	iCheckOut=iCheckIn;
	if(nLength>0)
	{
		Data_Temp=SRAM_Read(nAddress);
		iCheckOut += Data_Temp;
		Send_COM0_Byte( Data_Temp );
		for(i=1;i<nLength;i++)
		{
			Data_Temp=SRAM_Read(nAddress+i);
			iCheckOut += Data_Temp;
			Send_COM0_Byte( Data_Temp );
		}
	}
	OperateSPIEnd();
	return(iCheckOut);
}
/********************************************************\
*	��������Send_Echo_TCP_End
	�������ⲿ�ļ�����
*	���ܣ�  ������󲿷ֵ����ݷ��ͣ��ⲿ�ֿ�����GPRS�����ݷ��͹���
			�����з���SMS,��GPRS���ݾ��ɵ��õ��ӳ���
*	������  Input  ǰ������У��ˣ�������Ҫ���ͳ�ȥ
*	����ֵ:
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Send_Echo_TCP_End(unsigned int iInput)
{
	Send_COM0_Byte( iInput/0x100 );
	Send_COM0_Byte( iInput%0x100 );
	Send_COM0_Byte( EOT );
}
/********************************************************\
*	��������Send_Echo_GSM_End
	�������ⲿ�ļ�����
*	���ܣ�  ������󲿷ֵ����ݷ��ͣ��ⲿ�ֿ�����GPRS�����ݷ��͹���
			�����з���SMS,��GPRS���ݾ��ɵ��õ��ӳ���
*	������  Input  ǰ������У��ˣ�������Ҫ���ͳ�ȥ
*	����ֵ:
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Send_Echo_GSM_End(unsigned int iInput)
{
	Send_COM0_Byte(ASCII(iInput/0x1000));
	Send_COM0_Byte(ASCII(iInput%0x1000/0x100));
	Send_COM0_Byte(ASCII(iInput%0x100/0x10));
	Send_COM0_Byte(ASCII(iInput%0x10));
	Send_COM0_Byte(ASCII(EOT/0x10));
	Send_COM0_Byte(ASCII(EOT%0x10));
}

/********************************************************\
*	��������Send_Echo_GSM_End
	�������ⲿ�ļ�����
*	���ܣ�  ������󲿷ֵ����ݷ��ͣ��ⲿ�ֿ�����GPRS�����ݷ��͹���
			�����з���SMS,��GPRS���ݾ��ɵ��õ��ӳ���
*	������  Input  ǰ������У��ˣ�������Ҫ���ͳ�ȥ
*	����ֵ:
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Wait_Return_M22(unsigned char Next_Step)
{
	if(Check_GSM()!=0)
	{
		gTimer_GSM_AT=0;
	    if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK))!=0)
	    {
	    	gReset_M22_On_Setp=Next_Step;
	    }
	    else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER))!=0)
	    {
	    	gReset_M22_On_Setp=1;
	    }
	}
	else if(gTimer_GSM_AT>3000)
	{
		gReset_M22_On_Setp=1;
	}
}
