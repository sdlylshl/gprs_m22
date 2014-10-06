/********************************************************\
*	�ļ�����  Main_Init.C
*	����ʱ�䣺2004��12��7��
*	�����ˣ�  
*	�汾�ţ�  1.00
*	���ܣ�	  ��Գ�����ϵ�׶Σ���ϵͳ��һЩ��ʼ���Ĳ���
*	�ļ����ԣ������ļ�
*	�޸���ʷ����ÿ��������
	2004��12��15��
\********************************************************/
#include <msp430x14x.h>
#include "SPI45DB041.h"
#include "Define_Bit.h"
#include "D_Buffer.h"
#include "Other_Define.h"
#include "General.h"
#include "Uart01.h"
#include "Main_Init.h"
#include "Sub_C.h"
#include "M22_AT.h"
#include "Do_SRAM.h"
#include "Msp430_Flash.h"
/*
#include "SPI45DB041.h"
#include "W_Protocol.h"
#include "Handle_Protocol.h"
#include "Uart01.h"
#include "Define_Bit.h"
#include "D_Buffer.h"
#include "Other_Define.h"
#include "General.h"
#include "Main_Init.h"
#include "Sub_C.h"
#include "Check_GSM.h"
#include "Do_Reset.h"
#include "Do_GSM.h"
#include "Do_Handle.h"
#include "Do_GPS.h"
#include "Do_Disp.h"
#include "Do_Other.h"

*/
//#define Debug_GSM_COM1
/********************************************************\
*	��������Init_MAIN_Module
	�������ⲿ�ļ�����
*	���ܣ�  ���ģ������ϵͳ�ϵ����ϵͳ����ģ��Ŀ��Ƶ�Դ�������ϵ��ʱ����Ҫ����
			���FLASH���в����ĳ�ʼ��ģ�顣����ϸ�ڼ�FLASH����ϸ����
*	������  ��

*	����ֵ����Բ�����ص�ȫ�ֱ������и�ֵ������

*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Init_MAIN_Module(void)
{
    unsigned int i;
	Clear_Exter_WatchDog();
	gCycle_Send_Status=NOP_DATA;
	//��ʾMSP430������Ϣ��������������
	Read_SegAB(0);
	Send_COM1_Byte('A');
	Send_COM1_Byte('[');
	for(i=0;i<128;i++)
	{
		Send_COM1_Byte(ASCII(gGeneral_Buffer[i]/0x10));
		Send_COM1_Byte(ASCII(gGeneral_Buffer[i]%0x10));
	}
	Send_COM1_Byte(']');Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	gTimer=0;
	while(gTimer<1500){Clear_Exter_WatchDog();}
	Read_SegAB(1);
	Send_COM1_Byte('B');
	Send_COM1_Byte('[');
	for(i=0;i<128;i++)
	{
		Send_COM1_Byte(ASCII(gGeneral_Buffer[i]/0x10));
		Send_COM1_Byte(ASCII(gGeneral_Buffer[i]%0x10));
	}
	Send_COM1_Byte(']');Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	/********************************************************\
	һ����1�������͵�2�����������ݵĴ���(����ָ����1��2����)
	\********************************************************/
	gTimer=0;
	while(gTimer<1500){Clear_Exter_WatchDog();}
	Load_Info_One();
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	/********************************************************\
	����
		1���洢�����绰��¼��ҳ
		2���洢ͨ����¼��ҳ
	\********************************************************/
	/********************************************************\
	�����洢ϵͳ��Ϣ2����������(����ָ����5��6����)
	\********************************************************/
	gTimer=0;
	while(gTimer<1500){Clear_Exter_WatchDog();}
	Load_Info_Two();
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	/********************************************************\
	�ģ��������򱨾��Ĳ���
	\********************************************************/
	gTimer=0;
	while(gTimer<1500){Clear_Exter_WatchDog();}
	Load_Info_Three();
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	/********************************************************\
	�壬������·ƫ�뱨���Ĳ���
	\********************************************************/
	gTimer=0;
	while(gTimer<1500){Clear_Exter_WatchDog();}
	Load_Info_Four();
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	//������·ƫ�������
	/********************************************************\
	�������������ͻ���ĳ�ʼ��
	\********************************************************/
	gTimer=0;
	while(gTimer<1500){Clear_Exter_WatchDog();}
	Reset_Variable();
	/********************************************************\
	�ߣ���������ĳ�ʼ��
	\********************************************************/
	gOFF_Power_S=0;
    gGeneral_Flag&=~OFF_M22_F_1;				//ȡ������G20�ı�־λ
    gGeneral_Flag&=~OFF_GPS_F_1;				//ȡ������GPS�ı�־λ
    gPhone_Status=READY;gStatus1_Flag&=~PHONE_ONLINE_F_1;
	gRload_Flash_Par_Timer=0;
	gSet_Distance_Info_Count=0;

	SRAM_Init();
	SRAM_Write(PHONE_AT_2+0,NULL_2);
	for(i=PHONE_AT_2+1;i<END_SRAM_2;i++)
	{
		SRAM_Write(i,0);
	}
	//gStatus1_Flag|=ACC_ON_F_1;
	gON_OFF_Temp0 &=~ TCP_LAND_ON_OFF_1;
	gStatus2_Flag |= TCP_ON_OFF_F_1;
	//Ĭ���ֱ�����
	gStatus1_Flag |= HANDLE_ON_0_F_1;
	gStatus1_Flag &=~ HANDLE_ON_1_F_1;
	//Ĭ����ʾ�ն˿���
	gStatus1_Flag|=DISPLAY_ON_F_1;
}
