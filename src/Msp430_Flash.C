/********************************************************\
*	�ļ�����  Msp430_Flash.C
*	����ʱ�䣺2005��10��16��
*	�����ˣ�  
*	�汾�ţ�  1.00
*	���ܣ�
*	�ļ����ԣ������ļ�
*	�޸���ʷ����ÿ��������
\********************************************************/
#include <msp430x14x.h>
#include "Msp430_Flash.h"
#include "General.h"
/*
#include "TA_Uart.h"
#include "SPI45DB041.h"
#include "W_Protocol.h"
#include "Handle_Protocol.h"
#include "Uart01.h"
#include "M22_AT.h"
#include "Define_Bit.h"
#include "D_Buffer.h"
#include "Other_Define.h"

#include "Main_Init.h"
#include "Sub_C.h"
#include "Check_GSM.h"
#include "Do_Reset.h"
#include "Do_M22.h"
#include "Do_Handle.h"
#include "Do_GPS.h"
#include "Do_Disp.h"
#include "Do_Other.h"
#include "Do_SRAM.h"
#include "Do_Record.h"
#include "Record_Protocol.h"
*/
/********************************************************\
*	��������Init_OperFlash()
	�������ⲿ�ļ�����
*	���ܣ�
*	������
*	����ֵ��
*	�����ˣ�
\********************************************************/
void Init_OperFlash(void)
{
	WDTCTL=WDTPW+WDTHOLD;   //�رտ��Ź�WDT  ,WDTPW���Ź��Ŀ���
	FCTL2 =	FWKEY + FSSEL1 + FN0;
}
/********************************************************\
*	��������Write_SegAB()
	�������ⲿ�ļ�����
*	���ܣ�
*	������
*	����ֵ��
*	�����ˣ�
\********************************************************/
void Write_SegAB(unsigned char Address_Flag)
{
	char *Flash_ptr;
	unsigned int i;
	if(Address_Flag==0)
	{
		Flash_ptr = (char *)0x1080;	//��ʼ��ָ��
	}
	else if(Address_Flag==1)
	{
		Flash_ptr = (char *)0x1000;	//��ʼ��ָ��
	}
	else
	{
		return;
	}
	_DINT();							//�ر����������ж�
	FCTL1 = FWKEY + ERASE;				//ʹ�ܲ�������
	FCTL3 = FWKEY;						//FLASH����
	*Flash_ptr = 0;						//��д����������
	FCTL1 = FWKEY + WRT;				//����д����
	for(i=0;i<128;i++)
	{
		*Flash_ptr++=gGeneral_Buffer[i];
	}
	FCTL1 = FWKEY;
	FCTL3 = FWKEY+LOCK;					//д���ˣ�����
	_EINT();							//�������������ж�
}

/********************************************************\
*	��������Erase_SegAB()
	�������ⲿ�ļ�����
*	���ܣ�
*	������
*	����ֵ��
*	�����ˣ�
\********************************************************/
void Read_SegAB(unsigned char Address_Flag)
{
	unsigned char *Flash_ptr;
	unsigned int i;
	if(Address_Flag==0)
	{
		Flash_ptr = (unsigned char *)0x1080;	//��ʼ��ָ��
	}
	else if(Address_Flag==1)
	{
		Flash_ptr = (unsigned char *)0x1000;	//��ʼ��ָ��
	}
	else
	{
		return;
	}
	for(i=0;i<128;i++)
	{
		gGeneral_Buffer[i]=*Flash_ptr++;
	}
}