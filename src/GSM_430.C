/*	�������޸�˵����¼
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
2005��6��20�գ�
�����ļ�˵����
	������������PCBͼ(GPS-M22��V2.4)�汾��ԭ��ͼ���˳���Ϊ��Ӧ��ϵͳ�ļ�

����汾�ţ�V2.32

������˵����
	1�������ⲿ��SRAM�����ڽ�����Ӳ����Ĵ������ݵĴ��䣨����W24L011AJ(128K)��ʵ��ʹ��64K����������74HC573��
	2�����ڵķ��䣺	����0		M22
				 	����1		��Ӳ��������Դ��ڣ�
				 	����2		GPS
				 	����3		���ص绰
				 	����4		��ʾ�ն�
	3����Ҫʵ�ֹ��ܣ�
		(1),����ʹ��¼��֮��Ľ���ͨѶ
		(2),
		(3),
		(4),
		(5),

��ϸ˵������ʹ��¼��֮���ͨѶ���̣�

�������¼�Ƿ��͵������У�	1��ÿһ���ַ���8���ֽڵľ���γ��,(���ڼ�¼�Ǵ洢������)����¼�ǲ��ø���ECHO��
							2��ÿ��1��Сʱ����һ����ѯ��ǰ��¼��ʱ�䣬����жϼ�¼�ǵ�ʱ���GPS��ʱ������1�룬��ʱ��¼����ʾ�����ٶ�Ϊ0�����ͽ���ʱ�������
							   ��¼����Ҫ����ECHO
							3�����ؽ��յ��������ó��ٵ�����ֵ�Ĳ������ã�ת������¼�ǣ�����¼����Ҫ����ECHO��
							4�����ؽ��յ��������ü�����������ֵ�Ĳ������ã�ת������¼�ǣ�����¼����Ҫ����ECHO��
							5�����ؽ��յ���������ƣ�ͼ�ʻ��ʱ������ֵ�Ĳ������ã�ת������¼�ǣ�����¼����Ҫ����ECHO��

��¼�Ǳ������ط��͵������У�1����¼�ǽ��յ����صĲ�ѯ��ǰʱ������򷵻ص�ǰ��¼�ǵ�ʱ��͵�ǰ�ٶȵ�ֵ
							2����������ٶȳ������õı�������ֵ�����¼���Ϸ����ٱ�������Ϣ��ÿ���2�뷢��һ�Σ�ֱ�������յ��󷵻���Ӧ����ֹͣ���ͳ��ٱ�����Ϣ
							   �����¼���ڳ��ٵ�״̬�£��жϲ����ڳ��٣�����������ֹͣ���ٵĵ���Ϣ��ֱ�������յ��󷵻���Ӧ
							3���������ƣ�ͼ�ʻ��״̬�£���¼��Ӧ�������ط���һ����Ϣ
							4��������ʹ״̬�£����������ϱ������ͣ���ʱ���ϱ����ݵ㣬�������ϱ����ݵ�...��������ڰ������ϱ����ݵ㣬�������¼�Ǳ�ÿ�жϵ�һ�����
							   ���룬����Ҫ�����ط���һ����Ϣ

���ز�ѯ������¼�����ݵ�ͨѶ��ʽ�������·�һ����ѯ��¼�Ǵ������ݵ������������ת������¼�ǣ���¼���������ȡ���ݷ��͸�����
								  �����ȸ���������ݣ�ÿ����һ�����ݣ����ж��Ƿ����ڲ������ݰ�������ǣ�����Ч������ȡ���������δ洢���ⲿ����չSRAM�Ȼ�����һ��
								  ���յ������ݰ�����Ӧ��Ȼ��ȴ����ռ�¼�ǵ���һ�����ݰ���ֱ�����յ���¼�Ƿ��͹����Ĵ�����ȫ��ת����ɵ���Ϣ�������ؿ�ʼ�����ⲿSRAM������
								  ת����TCP��ʽ��]

								  ���е����ĵĲ�ѯ���������������ؾ����������ֵļ��飬�����ؽ��ռ�¼�ǵ���������Ļ�Ӧ�������õ���Ӧ���������ݷ���

2005��8��30��
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
/********************************************************\
ϵͳ����ṹ��˵����
1��FLASH�ķ���
	����0-----��ͣʹ��
	����1��2---��Ϊ�洢Ŀ����룬IP��ַ�Ͷ˿ںţ�˫���ݣ�
	����3��4---��Ϊ�绰����Ĵ洢
	����5��6---��Ϊ�ϴ����ݼ�����������ȵȵĴ洢��˫���ݣ�

2�����ڵķ���
					����0		M22
				 	����1		��Ӳ��������Դ��ڣ�
				 	����2		GPS
				 	����3		���ص绰
				 	����4		��ʾ�ն�
3��	SRAM�ķ���
	1�����д洢��������264byte��SRAM�ķ���buffer1,buffer2��
		buffer1�ķ��䣺��ԭ���ĳ��򱣳�һ�£����䣩������ϸ�ڼ���صط�
		buffer2��ʹ�������洢��ȡ�����ҳ������

	2���ⲿ64K��SRAM�ķ���
	   0-----60K		��Ϊ�����ⲿ���ݣ�����ʹ��¼��Ϊ�����������ݵĽ��գ��ڴ���1���ж��ӳ����д������գ�
	   61K---64K		��Ϊ�洢��¼����Ҫ����ʾ�ն˷������ݵĻ���洢����ʱ��ô���ǣ�

	3��MSP430F149���ڲ�SRAM�ķ���

4��TCP���ݵķ��ͷ�ʽ�����
	������ǰ��TCP���ݵķ�ʽ���ͷ�ʽ�����ڽ����͵����ݷ�λ���¼��ࣺ
	A��	�洢��buffer1���������������ݣ������ֱ�������ʾ�ն���Ҫ���е������Ϣ���Ͷ�һЩ�����������Ӧ��Ϣ��
	B�� �洢���������ݻ�����gCircle_Buffer[]������Ҫ���͵����ݣ���Ҫ�洢�������ڷ��͵ı�����Ϣ���г��켣��Ϣ��
	C�� TCP����״̬�µ�������Ϣ���ݵĶ�ʱ������Ҫ�Է�FF 0D����
	D�� ������Ӳ��������ݣ��Դ洢���ⲿ��SRAM����������

5����ʾ�ն˺��ֱ��ն˵Ľ��պͷ��ͻ�������˵��
	�ֱ�����ʾ��������ϵͳͨѶ���ǲ��õ�ģ�⴮��3����4�����ڰ�˫������ʽ���������ն˷������ݵ�ʱ����Ҫ�ж��Ƿ��ڽ������ݡ�

	�����ֱ������ݻ��壺gHandle_Buffer[30]
	���ֱ��ķ������ݵĻ��壺������ԭ���ļ��ݣ�������Ҫ���ֱ��������ݵ�ʱ�����֯���ݷ��ͣ��Խ�ʡSRAM�Ŀռ䣩

	������ʾ�����ݻ��壺gDisp_Buffer_R[50]
	����ʾ���ķ������ݵĻ��壺gDisp_Buffer[256]

6,����Ӳ���������ͨѶ�Ľ��պͷ��ͻ���˵�����Լ�¼��Ϊ����
	������Ӳ��������ݻ��壺�ⲿSRAM��0---64K��
	����Ӳ������͵����ݻ��壺

�޸ļ�¼��

2005��8��23�գ�

		ACC�رպ�20���ӣ�TCP���ߣ�����30���ӵķ�ʽ�������ݡ�1��Сʱ�ر�ģ��
		ACC�رպ�20���� �����ACC�ٴο�������������������

		�г��켣ֻ��������10���ӵ�SMS�Ĳ�����������Ϣ��������GSM��ʽ����

		�ֱ�ͨѶ��ʽ�ĸı�


		�ص����ƣ��������м�������ı乤��״̬Ϊ��ط�ʽ�����ϴ����ݵĲ���ACC���͹رյĿ���
					���ң�����ʱ������󣬾�����TCP�����ߵķ�ʽ���ص��صķ�ʽ��������ʽ�ĸı�
					����״̬��ʽ�����洢��FLASH�У�ͬʱ����ʹACC�رգ��ڿ�����ʱ��Ҳ����������
					��������ֱ���ص��ؽ����������ķ�ʽ�ı䡣

2005��9��1�գ�

		����ͨ���ֱ��ӿ����ò����Ĳ���,APN,�绰���뱾
		����һЩ��·�������㷨

2005��9��5�գ�
		��·ƫ����㷨��ȷ��֤
		�������ACC����״̬�£�����12��Сʱ����ὫMCU��������һ�Ρ��Իָ��豸״̬
		�˹�����Ϊ�˷�ֹ��ʵ�ʽ����У�ʩ��ʱ��ֱ�ӽ�ACC���ڳ����ϵ�һ���Իָ�������ʩ


2005��9��6�գ�
		������·ƫ�뱨�����㷨������·����ͨ�����Ĳ�û������ã�
		ͨ���ֱ��˿����õ绰�������
		ͨ���ֱ��˿ڲ����������ƣ�������APN�����ã��������ϴ��ľ���ֵ���ã���
2005��9��8�գ�

		ͨ���ֱ��˿�������ʾ�ն˵Ķ���Ԥ�����

2005��9��14�գ�
		����Ҫ���޸��ϱ����ݵĲ�����ʽ������Ϣ��
		1�����ٱ������ϱ���ʽ���޸�
2005��9��22�գ�ʵ���ܳ�������·����������

2005��10��9��:
	1,��Ե绰����Ļָ�����
	2,���ƣ�ͱ�������Ԥ�����󱨴���

	�������ֳ�ʵ��Ӧ���з��ֵ����,�Ժ���Ҫ���Ƶļ���:
	A;��Ϊ�ֱ�����ʾ�ն��ǰ�˫��ͨѶ,���Ҿ�Ϊģ�⴮�ڵķ�ʽ,����Ҫ��232��IC�Ļ�Ϊ3232��ͬʱ,
		Ϊ������ͨѶ�Ŀɿ���,����Ҫ��Բ���ָ�����Ӧ����.
2005��10��16�գ�
	1�����ģ�⴮�ڵķ��ͺͽ���ʱ����ҪֹͣĳЩ�ж�

	2,���Ӷ�MSP430�ڲ���������Ϣ�洢�������á��ֱ�Ϊ128���ֽڣ���Ϊ�洢���ݲ�������
	 	��Ϣ�洢��A����������1������2��ǰ128�ֽڵ�����
	 	��Ϣ�洢��B����������3������4��ǰ128�ֽڵ�����

	 	��������������1������2������У��˲���ȷ���������ݣ�����Ҫ����Ϣ�洢��A�е�ǰ128���ֽڶ����������¼���У���
	 	�洢������1��������2�С�

	 	ͬ����������Ϣ�洢��B�Ƕ�Ӧ������3������4

	3��ÿ�θ��»��ߴ��¼�������1��2��ʱ����Ҫ��ʱ�ĸ�����Ϣ�洢��A
		ÿ�θ��»������¼�������3��4��ʱ����Ҫ��ʱ�ĸ�����Ϣ�洢��B:


		��ʱδ��ɲ���

2005��10��17�գ�
OK    **1��Ϊ�˼��ٴ���FLASH�Ĳ�������������FLASH�е�SRAM�Ĳ���������ǰ�洢��buffer1�е�����ת�Ƶ��ⲿ��SRAM�У�
		3������Ӳ��PCB�ĸĽ������Ӷ��ⲿ����FLASH��WP�ܽŵĿ��ƣ�Ԥ����FLASH�Ĳ����𻵣�
		4����Գ�ǿ�źŵķ��ͣ���Ϊÿ���һ��ʱ�䷢�ͳ�ǿ�źţ��������ڲ�ѯCSQ��ŷ��ͳ�ǿָ��OK
		5�����M22���ڵ绰״̬�£�����Ҫÿ���һ��ʱ���ѯCLCCָ��	OK

2005��10��19�գ�
		1�����ӽ��в����ⲿSRAM��ָ��()��ɲ���OK
		2���������������һ�ָ��Ľ����ֱ�����Ӧ�����2����δ���յ��ֱ��Ĺһ�ָ����Ӧ�����ظ�����
			�ظ�5�Σ������˰�����δ���ԣ�
		3,�����ֱ���Ҫ���͵ĵ����ݴ洢���ⲿSRAM�С��ı�ԭ�����ֱ�����ָ����ʱ���͵Ľṹģʽ
			ȡ�����ж�buffer1�Ĳ���
		14����Ϊÿ���ڷ������ݺ͵�½TCPǰ�����и���֤���ĺ����Ŀ�����Ȳ�������Ч�ԣ���Ҫ�Ƕ��ⲿSRAM���в���
			Ƶ���Ĳ������������ⲿFLASH���ݵĶ�ʧ���������Ϊÿ30���ӽ������е���֤�����������ȷ��������
			һ����־�����ʧ�ܣ������̽������ݵ���װ����

2005��11��3��
		1��������MSP430�ڲ���������Ϣ������ȡ������Ϣ�������ݵ�У��˵ļ��㣬ȡ���������һ���ֽڴ洢0XAA��
			ֻ��ʾ����Ϣ�����洢�����ݡ�����ڼ����ⲿFLASH���������������ݾ�����ȷ�������
			����Ҫ��������Ϣ���������ݸ��ǵ��ⲿ��FLASH��

2005��11��8�գ�
		1�����Ӷ�λ��ѯ�Ŀɿ��ţ�����ڽ��յ�һ������Ϣ�����ڶ�λ��ѯ�������ң��豸������
			GSM�Ĺ�����ʽ�£�����Ŀ�����������Ч��״̬������Ҫ��ʱ��ȡ�������Ϣ�����е�
			���룬Ȼ����Ϊ��ʱ��Ŀ�����洢�������ڷ��Ͷ�λ���ݵ�ʱ�����Դ�Ŀ�����Ϊ׼��

2005��11��16�գ�
		1��ȡ��ÿ��ϵͳ�����ϵ�󣬲�ѯ��¼�Ǽ�ʻԱ����Ĳ���

2005��11��19��
		1��������򣬼���ADת���жϲ��֣����Ӷ�FLASH�Ŀɿ������������ǶԵ绰����洢���ֵĲ���


2005��11��21��
		1�����Ʋ������÷����������APN����Ĵ��������Ӷ�buffer2�Ĵ�������

2005��11��23��
		1�����ɽ���г���һ�����Ĭ�ϵ绰ʼ�մ��ڿ���״̬

2005��12��1�գ�
		1������ڲ��Թ����У����ֵ�ģ�⴮��ʧЧ�����⣬�������˸������ָ������¶�TA��ʱ�����г�ʼ����
2005��12��3�գ�
		1���������ⲿ�ľ��������ʱ��ʧЧ���������Ӱ������ģ�⴮�ڵĽ��գ������ϵͳ�������������ָ�
2005��12��4�գ�
		1������ⲿ����XT2����ʱʧЧ�������Ӷ��ⲿ����XT2��ʧЧ�жϴ�������



2005��12��8�գ�
		��ǰ��汾��һ����ĳ���ṹ�£���Ҫ���в��ֳ���ṹ���޸ģ�ʹ�÷���һ����ĸ�����ǽ�GPS
		�������ֺͼ�¼�ǵĹ��������ܹ����κ�һ����û���������£�˫���ܻ��಻���ź�Ӱ�죬��GSMģ����
		���г�ʼ���������ڳ�ʼ���Ĺ����У�����Ӱ���¼�ǵĹ�������ʾ���Ĺ���


2005��12��27�գ�
		������ǰ���ڽ��ռ�¼�����ݵķ�ʽ�п����жϲ�����ȷ�����ݰ������ڽ����ռ�¼�����ݵķ�ʽ���иĽ�
\********************************************************/


/********************************************************\
*	�ļ�����  GSM_430.C
*	����ʱ�䣺2004��12��7��
*	�����ˣ�        
*	�汾�ţ�  1.00
*	���ܣ�	  ϵͳ��������
*	�ļ����ԣ������ļ�
*	�޸���ʷ����ÿ��������
\********************************************************/

#include <msp430x14x.h>
#include <math.h>
#include "TA_Uart.h"
#include "SPI45DB041.h"
#include "W_Protocol.h"
#include "Handle_Protocol.h"
#include "Uart01.h"
#include "M22_AT.h"
#include "Define_Bit.h"
#include "D_Buffer.h"
#include "Other_Define.h"
#include "General.h"
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
#include "Msp430_Flash.h"
/*
#include "Disp_Protocol.h"
*/
//#define Debug_GSM_COM1

unsigned int i;
unsigned int  Z_Timer=0;
unsigned int  Debug_Timer=0;
/********************************************************\
*	��������main.c
	�����򣺱����ļ�����
	        ��Ϊ����ܳ���
*	���ܣ�	��Ŀ��������ϵͳ��ѭ������
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void main(void)
{
    //���Ź���ʱ��WDT�����ã�
	WDTCTL=WDTPW+WDTHOLD;   //�رտ��Ź�WDT  ,WDTPW���Ź��Ŀ���

    //ʱ��ģ������ã�
    BCSCTL2|=SELS+SELM1;    //SELS��ʾ��XT2CLK��ΪSMCLK��ʱ��Դ
                            //SELM1��ʾ��XT2CLK��ΪMCLKʱ��Դ
                            //ACLK---����ʱ�ӣ���������ѡ���Ƶ����,Ҳ��������ѡ������Χģ���ʱ���ź�
                            //MCLK---��ʱ�ӣ���Ҫ����CPU��ϵͳ,Ҳ������ѡ���Ƶ����
                            //SMCLK--��ʱ�ӣ�������ѡ��������Χģ��

//	BCSCTL2|=DIVS_3;		//ѡ��SMCLKʱ�ӵķ�Ƶ����Ϊ8
//	BCSCTL2|=DIVM_3;		//ѡ��MCLKʱ�ӵķ�Ƶ����Ϊ8
//-------------------------------

    do
    {
		IFG1 &= ~OFIFG;            		// Clear OSCFault flag
		for (i = 0xFF; i > 0; i--);		// Time for flag to set
		for (i = 0xFF; i > 0; i--);
    }
    while ( IFG1 & OFIFG );				// OSCFault flag still set?
//--------------------------------
    for (i = 60000; i > 0; i--);
  //��ʱ��TA������
    TACTL|=TASSEL1;         //����ʱ��ѡ��MCLK
    TACTL|=TACLR;           //��ʱ��A���λCLR����
    TACTL|=MC1;             //��ʱ��Aѡ������������ģʽ
//    TACTL|=TAIE;          //��ʱ��A����ж�����

    TAR=0;                  //��ʱ��A��ֵ����
    TACCTL0|=OUT;           //TXD2 Idle as Mark
	TACCTL1|=OUT;           //TXD3 Idle as Mark
	TACCTL2|=OUT;           //TXD4 Idle as Mark

  //��ʱ��TB������
    TBCTL|=TBSSEL1;         //����ʱ��Դѡ��MCLK
    TBCTL|=TBCLR;           //��ʱ��B���λCLR����
    TBCTL|=MC1;             //ģʽΪ����������
//    TBCTL|=TBIE;          //��ʱ��B����ж�����
    TBR=0;                  //�Ĵ���B��ֵ����
    TBCCTL0|=CCIE;
	/*
	=============================================================================
	                             �����ʵļ��㷽ʽ
	          8000 000 /19200=416.6667D(ʮ����)=01A0H+�00.67D(11010101B/8=0XD5/8)
	          8000 000/9600 =833.333D(ʮ����)=0341H+0.333D(1010100B/8=0X54/8)
	          8000 000/4800 =1666.66666D(ʮ����)=0682H+0.67(11010101B/8=0XD5/8)
	=============================================================================
	*/
  	//����0����(�������ж���ʽ,���Ͳ������ж�)
  	//����G20ͨѶ�Ĳ���������Ϊ4800
    UTCTL0|=SSEL1;          //����ѡ��ϵͳʱ��
    URCTL0|=URXEIE;         //�����ַ�����ʹ��URXIFG��λ
	//������9600
    UBR10=0x03;
    UBR00=0x41;
    UMCTL0=0x54;
   	//������4800
   	/*
    UBR10=0x06;
    UBR00=0x82;
    UMCTL0=0xD5;
	*/
    UCTL0|=CHAR;            //ѡ��8λ�ַ����ͽ���
    UCTL0&=(~SWRST);

  	//����1����(�������ж���ʽ�����Ͳ������ж�)
 	 //������9600
    UTCTL1|=SSEL1;          //����ѡ��ϵͳʱ��
    URCTL1|=URXEIE;         //�����ַ�����ʹ��URXIFG��λ

    UBR11=0x03;
    UBR01=0x41;
    UMCTL1=0x54;

    UCTL1|=CHAR;            //ѡ��8λ�ַ����ͽ���
    UCTL1&=(~SWRST);

    //�ж�����1
    IE1|=URXIE0;            //URSART0�����ж�����
//    IE1|=UTXE0;           //URSART0�����ж�����
	IE1|=OFIE;				//���������ж�����
    //�ж�����2
    IE2|=URXIE1;            //USART1�����ж�����
//    IE2|=UTXE1;           //USART1�����ж�����



    //ģ������1
    ME1|=URXE0;         	//USART0����������UARTģʽ��
    ME1|=UTXE0;         	//USART0����������UARTģʽ��
    //ģ������2
    ME2|=URXE1;         	//USART1����������UARTģʽ��
    ME2|=UTXE1;         	//USART1����������UARTģʽ��



	/*
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	���ö�������ģ�⴮�ڵ���عܽŶ���
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	*/
	P3SEL=0;
    P3SEL|=BIT4+BIT5+BIT6+BIT7; 	//P3SEL--P3�˿�ѡ��Ĵ�����ѡ��UART0��UART1����

    P1SEL += TXD2;           		// P1.5/TA0 for TXD function
    P1DIR += TXD2;           		// TXD output on P1
    P1SEL += RXD2;          	 	// P1.1/TA0 as RXD input

    P1SEL += TXD3;           		// P1.6/TA0 for TXD function
    P1DIR += TXD3;           		// TXD output on P1
    P1SEL += RXD3;           		// P1.2/TA0 as RXD input

    P1SEL += TXD4;           		// P1.7/TA0 for TXD function
    P1DIR += TXD4;           		// TXD output on P1
    P1SEL += RXD4;           		// P1.3/TA0 as RXD input
	/*
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	���ö�����Ʋ�����عܽŵĶ���
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	*/

	P6DIR|=POWER_ON_OFF;
	P6OUT&=~POWER_ON_OFF;						//����POWER_ON_OFFΪ�͵�ƽ

	P1DIR|=MCU_STATUS;

	P6DIR|=STOP_Q2+STOP_Q1;						//����Ϊ���ģʽ
	P6OUT&=~STOP_Q2;							//����STOP_Q2Ϊ�͵�ƽ
	P6OUT&=~STOP_Q1;							//����STOP_Q1Ϊ�͵�ƽ

	P6SEL |= BIT1;								//��A1����Ϊ��Χģ�鹦�ܣ���ADC����
	P6DIR|=WDI;									//����WDIΪ���ģʽ
	P6OUT&=~WDI;								//�ⲿӲ�����Ź�������

	P6DIR|=SEL_MIC;
	P6OUT&=~SEL_MIC;

	Init_OperFlash();
	/*
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	      ���ڴ��д洢��AT45DB041B�ĹܽŶ���
	      ����Ĭ��Ϊ����ģʽ3����ΪSCK�������ط��ͺͽ�������
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	*/
	//CS,RST,SCK,SI,WP����Ϊ���ģʽ
    P2DIR|=SPI45DB041_CS+SPI45DB041_RST+SPI45DB041_SCK+SPI45DB041_SI;
    P2OUT|=SPI45DB041_CS;				//��SPI45DB041_CSΪ��
    P2OUT|=SPI45DB041_RST;        		//��SPI45DB041_RSTΪ��
    P2OUT&=~SPI45DB041_SCK;      		//��SPI45DB041_SCKΪ��
	P6DIR|=SPI45DB041_WP;
	P6OUT&=~SPI45DB041_WP;				//��SPI45DB041_WPΪ��
	/*
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	���ö���P6.1,P6.2,P6.3�ܽ�Ϊ��Χģ�鹦�ܣ���ADCת�����ܶ���
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	*/
    _EINT();							//���ж�
 	//WDTCTL=WDTPW;						//�������Ź�
    TAUARTCTL|=TAUARTIE;        		//�Ƚ���A���ж�����
    RX2_Ready();
    RX3_Ready();
    RX4_Ready();
    gTimer=0;
	while(gTimer<2000) {Clear_Exter_WatchDog();}
	Send_COM1_String((unsigned char *)MAINSYS,sizeof(MAINSYS));
	//��ʼ��ģ�������
	gInternal_Flag=0;
	gGeneral_Flag=0;
	gOther_Flag=0;
   	Init_MAIN_Module();
    gGPS_No_Output_Timer=0;
    gNO_Sram_Data_Timer=0;
    Clear_Exter_WatchDog();
    while(1)
    {
    	//���Բ��ֵĴ�����ʾ����
    	if( (Debug_Timer>20000)&&((gGeneral_Flag&TCP_PHONE_F_1)==0) )
    	{
			RX2_Ready();RX3_Ready();RX4_Ready();
    		//���Դ���1д�ⲿSRAM�Ͷ��ⲿSRMA�ĳ�����鲿��
    		Debug_Timer=0;
			//��ʾM22�Ĺ�����ʽ״̬
			Send_COM1_Byte(ASCII(gM22_Status/10));
			Send_COM1_Byte(ASCII(gM22_Status%10));
			Send_COM1_Byte('=');
			//��ʾϵͳ�������ڷ������ݵ�����
			Send_COM1_Byte(ASCII(gCycle_Send_Status/10));
			Send_COM1_Byte(ASCII(gCycle_Send_Status%10));
			Send_COM1_Byte('=');
			Send_COM1_Byte(ASCII(gAlarm_Type/10));
			Send_COM1_Byte(ASCII(gAlarm_Type%10));
			Send_COM1_Byte('[');
			Send_COM1_Byte(ASCII(gStatus1_Flag/0x10));
			Send_COM1_Byte(ASCII(gStatus1_Flag%0x10));
			Send_COM1_Byte(':');
			Send_COM1_Byte(ASCII(gStatus2_Flag/0x10));
			Send_COM1_Byte(ASCII(gStatus2_Flag%0x10));
			Send_COM1_Byte(':');
			Send_COM1_Byte(ASCII(gStatus3_Flag/0x10));
			Send_COM1_Byte(ASCII(gStatus3_Flag%0x10));
			Send_COM1_Byte(']');

			if(gGeneral_Flag&GPS_VALID_F_1)
			{
				Send_COM1_Byte('A');
			}
			else Send_COM1_Byte('V');
			Send_COM1_Byte(':');
			if(gGeneral_Flag&NUM1_VAILD_F_1)
			{
				Send_COM1_Byte('P');
			}
			else Send_COM1_Byte('N');
			Send_COM1_Byte(':');
			if(gInternal_Flag&CENTER_NUM_VAILD_F_1)
			{
				Send_COM1_Byte('C');
			}
			else Send_COM1_Byte('N');
			Send_COM1_Byte(':');
			if(gGeneral_Flag&LAND_TCP_ERROR_F_1)
			{
				Send_COM1_Byte('G');
			}
			else Send_COM1_Byte('N');
			Send_COM1_Byte(':');
			if(gInternal_Flag&SEND_CIRCLE_F_1)
			{
				Send_COM1_Byte('S');
			}
			else Send_COM1_Byte('N');
			Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
    	}
    	Do_GPS_Module();
    	Clear_Exter_WatchDog();
		Do_Disp_Module();
		Clear_Exter_WatchDog();
		Do_Handle_Module();
    	Do_Other_Module();
    	Clear_Exter_WatchDog();
    	Do_Record_Module();
    	Clear_Exter_WatchDog();

		//����ϵͳ�е�����ģ�鴦����Ҫ�ϵ�״̬��ѭ������
		if( ((gGeneral_Flag&OFF_M22_F_1)||(gGeneral_Flag&OFF_GPS_F_1))
			&& (gPhone_Status==READY)	)
		{
			//��������ģ��ĵ����ӳ���
			Do_OFF_Power();
			Clear_Exter_WatchDog();
			gGPS_No_Output_Timer=0;
			gNO_Sram_Data_Timer=0;
			gM22_GSM_ECHO_Timer=0;
		}
		//����ϵͳ��������������ѭ��״̬
		else
		{
			//��ʾ�����³�ʼ��M22
			if(   (gGeneral_Flag&RESET_GSM_ON_F_1)
				&&(gPhone_Status==READY)	 )
			{
				gM22_GSM_ECHO_Timer=0;
				gGPS_No_Output_Timer=0;
				gNO_Sram_Data_Timer=0;
				Do_Reset_GSM_On();
			}
			//��ʾ���³�ʼ��GPSģ��
			//������ѭ��
			else
			{
				//���M22��ʱ���޷�������
				if(gM22_GSM_ECHO_Timer>M22_NULECHO_TIME)
				{
					gGeneral_Flag|=OFF_M22_F_1;
					gOFF_Power_S=1;
					gPhone_Status=READY;
					gM22_ReturnER_SIM_Count=0;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
					return;
				}
				if(gFF0D_Receive_Timer>OVER_TCP_TIME)
				{
					gGeneral_Flag|=OFF_M22_F_1;
					gOFF_Power_S=1;
					gPhone_Status=READY;
					gM22_ReturnER_SIM_Count=0;
					gStatus1_Flag&=~PHONE_ONLINE_F_1;
					return;
				}
				Clear_Exter_WatchDog();
				Do_G20_Out();
			}
		}
    }
}
/********************************************************\
*	��������TimerB_Interrupt (�ж��ӳ���)
	�����򣺱����ļ�����
*	���ܣ� ��ʱ��TB���жϣ�����ʱ�����
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
interrupt [TIMERB0_VECTOR] void TimerB_Interrupt(void)
{
	TBCCR0+=8000;       		//��ʱ1ms,�ⲿʱ��Ϊ8M��ʱ�ӡ�
	gTimer++;					//1ms����һ�μ���
	gDelay_Com_Timer++;
	gTimer_GSM_AT++;			//����ATָ��֮��ļ��ʱ��
	Debug_Timer++;				//��Ϊ������ʾ���ݵļ��
	gGPS_Receive_Over++;		//��Ϊ�ж�GPS�೤ʱ��δ���������ֽ���
	gHandle_Receive_Over++; 	//�˱�����Ϊ�����ֱ����ݰٵ�ʱ������ʱ��ÿ����һ���ֱ������ֽڣ���˱�����0
	gDisp_Receive_Over++;
	gTest_Alarm_Timer++;		//�˱�����Ϊ����б��ٿ��ذ��µ���ʱ��ʱ
	gTest_Status_Timer++;
	gReceive1_Over_Count++;
	gCheck_Stop_Timer++;
	gDisp_Timer++;
	if(gM22_Status==TCP_ONLINE_WORK)	gRing_Test_Timer++;
	else								gRing_Test_Timer=0;

	if(gCommon_Flag&ALLOW_R232_F_1)
	{
		gAllow_R232_Timer++;
		if(gAllow_R232_Timer>200)	gCommon_Flag&=~ALLOW_R232_F_1;
	}
	else
	{
		gAllow_R232_Timer=0;
	}
	if(gM22_Status==TCP_ONLINE_WORK)	gM22_TCP_ECHO_Timer++;

	Z_Timer++;
	if(Z_Timer>1000)					//1���ӵ�ʱ���ʱ
	{
		Z_Timer=0;
		gTimer_CSQ++;					//��Ϊ�����ѯCSQ��ʱ���ʱ
		gTimer_Area_Test++;				//��Ϊ��������ʱ����
		gCircle_Timer++;				//��Ϊ��ʱ��Ҫ����gCircle_Buffer���������ݵ�ʱ�����
		gCheck_Par_Timer++;
		gHandle_Single_Count++;
		gReceive_Record_Info_Timer++;
		gLine_Timer++;

		if(    (gGeneral_Flag&DAILING_UP_F_1)
			 ||(gGeneral_Flag&RING_F_1)
			 ||(gGeneral_Flag&MONITOR_ON_F_1) )
		{
			gHangUp_Timer++;
		}
		else
		{
			gHangUp_Timer=0;
		}

		gDelay_Flash_Timer++;
		if(gCommon_Flag&ALLOW_OUT_DISP_F_1)		gSend_Disp_Timer++;
		else
		{
			gSend_Disp_Timer=0;gDisp_ReSend_Count=0;
		}

		if( (gWatch_Type==WATCH_TYPE_TIME)&&(gWatch_Time>0) )	gWatch_Time--;

		gGps_Null_Count++;
		if(gGps_Null_Count>3)	 gPublic_Flag |= GPS_NULL_F_1;
		gRecord_Null_Count++;
		if(gRecord_Null_Count>3) gPublic_Flag |= RECORD_NULL_F_1;

		gDo_Speed_Count++;

		if(gSys_Handle==0)	gSend_Handle_Timer=0;
		else				gSend_Handle_Timer++;

		//=================================================================
		//ÿ���12��Сʱ����ҪMCU�Ŀ��Ź��ر�һ�Σ���ʹ��ϵͳ������ͷ��ʼִ��
		gRload_Flash_Par_Timer++;
		if(   (gRload_Flash_Par_Timer>RLAOD_FLASH_TIME )
			&&(gStatus1_Flag&ACC_ON_F_1) )
		{
			gRload_Flash_Par_Timer=0;
			gOther_Flag|=RESET_PROGRAME_F_1;
		}
		//=================================================================
		gGet_Driver_Info_Count++;
		if( (gPublic_Flag&SET_DISTANCE_OK_F_1)==0 )		gSet_Distance_Info_Count++;
		else											gSet_Distance_Info_Count=0;

		gRecord_Data_Timer++;

		if(gOther_Flag&RECORD_DATA_F_1)	gSend_RecordData_Timer++;
		else							gSend_RecordData_Timer=0;

		if(gStatus1_Flag&ACC_ON_F_1)	ACC_OFF_Timer=0;
		else							ACC_OFF_Timer++;

		//===========================================================
		//����绰ҵ�񳬹�30���ӣ��������Ҷϵ绰ҵ��
		if( (gGeneral_Flag&TCP_PHONE_F_1)==0 )
		{
			gLast_Phone_Timer=0;
		}
		else
		{
			if(gPhone_Status==ACTIVE_MONITOR);
			else
			{
		   		gLast_Phone_Timer++;
		   		if(gLast_Phone_Timer>ALLOW_PHONE_TIME)
		   		{
		   			gLast_Phone_Timer=0;
		   			gCommon_Flag|=ALLOW_HANGUP_PHONE_F_1;
				}
			}
		}
		//===============================================================
		gGPS_No_Output_Timer++;
		gNO_Sram_Data_Timer++;
		//CPU״ָ̬ʾ��
		if(P1IN&MCU_STATUS)	P1OUT&=~MCU_STATUS;
		else				P1OUT|=MCU_STATUS;

		//��Ϊ�ж��ֱ��Ƿ�ժ����ʱ���ʱ��1���ӵļ�ʱ
		gHandle_OnLine_Timer++;
		if( gHandle_OnLine_Timer>HANDLE_OUT_TIME )
		{
			gHandle_OnLine_Timer=0;
			gStatus1_Flag &=~ HANDLE_ON_0_F_1;
			gStatus1_Flag &=~ HANDLE_ON_1_F_1;
		}
		//��Ϊ�ж���ʾ���Ƿ�ػ���ʱ���ʱ��1���ӵļ�ʱ
		gDisp_OnLine_Timer++;
		if(gDisp_OnLine_Timer>DISP_OUT_TIME)
		{
			gDisp_OnLine_Timer=0;
			gInternal_Flag&=~DISP_ON_F_1;
			gStatus1_Flag&=~DISPLAY_ON_F_1;
		}
		//���������½���ݷ��ͺ�һ��ʱ����Ȼδ�ɹ��յ����еĵ�½��Ӧʱ������Ҫ���м���Ķ�ʱ����е�½���ݵķ��ͣ�ֱ�����յ����еĵ�½��ӦΪֹ
		//����ɹ��յ����еĵ�½��Ӧ��־������Ҫ���ñ�־LAND_SUCCESS_F_1
		if( ((gGeneral_Flag&LAND_TCP_SUCCESS_F_1)==0)
			&&(gM22_Status==TCP_ONLINE_WORK)	)
		{
			gLand_Interval_Timer++;
		}
		else	gLand_Interval_Timer=0;

		//TCP���ߣ�������������FF0D��ʱ���ʱ
		if(gM22_Status==TCP_ONLINE_WORK)	gTCP_FF0D_Send_Timer++;
		else if(gM22_Status==GSM_WORK)		gTCP_FF0D_Send_Timer=0;
		//�ж�δ���յ�TCP���ݰ���ʱ���ʱ
		if(gM22_Status==TCP_ONLINE_WORK)		gFF0D_Receive_Timer++;
		else if(gM22_Status==GSM_WORK)
		{
			gFF0D_Receive_Timer=0;
			gM22_GSM_ECHO_Timer++;
		}
		else
		{
			gM22_GSM_ECHO_Timer++;
		}
		//�ж��Ƿ��������µ�½TCP�Ĳ����ı�־
		if(gGeneral_Flag&LAND_TCP_ERROR_F_1)
		{
			gLand_TCP_Count++;
			if(gLand_TCP_Count>RELAND_TCP_TIME)
			{
				gLand_TCP_Count=0;
				gGeneral_Flag&=~LAND_TCP_ERROR_F_1;
			}
		}
		else	gLand_TCP_Count=0;
	}
}
/********************************************************\
*	��������XT2����ʧЧ���жϴ�������
	�����򣺱����ļ�����
*	���ܣ�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
interrupt[NMI_VECTOR] void XT2_INVAILD (void)
{
	IFG1&=~OFIFG;
	IE1|=OFIE;
	/*
	if(IFG1&OFIFG)
	{
		//1��ѡ��DCOΪMCLK
		//BCSCTL2 &=~	SELM0;BCSCTL2 &=~ SELM1;
		//2���û�����������ʧЧ����
		//3����λ�жϱ�־
		IFG1&=~OFIFG;
		//4���ж��Ƿ�������ȻʧЧ
		if( (IFG1&OFIFG)==0 )
		{
			//����ѡ��ʧЧ��XT����
			BCSCTL2|=SELS+SELM1;
		}
	}
	else if(IFG1&ACCVIE)	IFG1&=~ACCVIE;
	else if(IFG1&NMIIE)		IFG1&=~NMIIE;
	//���������ж�����
	IE1|=OFIE;
	*/
}