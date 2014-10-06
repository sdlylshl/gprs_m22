
/********************************************************\
*	�ļ�����  TA_Uart.C
*	����ʱ�䣺2004��12��7��
*	�����ˣ�  
*	�汾�ţ�  1.00
*	���ܣ�	  ����MSP430���ص㣬�ⲿ��ʵ��MSP430����ģ�⴮�ڵĹ���
			  ��������ģ�⴮�ڵķ��ͺͽ���
*	�ļ����ԣ������ļ�
*	�޸���ʷ����ÿ��������

\********************************************************/

#include <msp430x14x.h>
#include "TA_Uart.h"
#include "General.h"
#include "Sub_C.h"
#include "Define_Bit.h"

#define SEND_DELAY_TIME 	3

unsigned char SBUFIN2,SBUFIN3,SBUFIN4;
unsigned char SBUFOUT2,SBUFOUT3,SBUFOUT4;
unsigned char BitCnt2,BitCnt3,BitCnt4;
unsigned char RTI2,RTI3,RTI4;
unsigned char RIE;
unsigned int  RXTXData2,RXTXData3,RXTXData4;

void TX2_Byte(void);
void TX3_Byte(void);
void TX4_Byte(void);

/********************************************************\
*	��������TX2_Byte
	�����򣺱��ļ�����
*	���ܣ�  ģ�⴮��2����һ���ֽ��ӳ���
*	������  SBUFOUT2
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void TX2_Byte (void)
{
	RXTXData2=(int)SBUFOUT2;
	BitCnt2=0x0A;    					// Load Bit counter, 8data + ST/SP��װ10��BITλ
	RTI2 |= ~SEND_FLAG;					//�����Ӧ�ķ��ͱ�־λ
	TACCR0 = TAR;       				// Current state of TA counter����ռ�����
	TACCR0 += Bitime0;        			// Some time till first bit
	RXTXData2 |= 0x100;        			// Add mark stop bit to RXTXData 1Ϊֹͣλ
	RXTXData2 = RXTXData2 << 1;			// Add space start bit  0Ϊ��ʼλ��λ����1λ��
	TACCTL0 = OUTMOD0+CCIE; 			// TXD������1���ж�����
	//while ( TACCTL0 & CCIE ); 		// Wait for TX completion���ȴ����е�λ���������
	while ( (~RTI2)&SEND_FLAG );
	gDelay_Com_Timer=0;
	while(gDelay_Com_Timer<SEND_DELAY_TIME);
}

/********************************************************\
*	��������RX2_Ready
	�������ⲿ�ļ�����
*	���ܣ�  ģ�⴮��2׼�����������ӳ��򣨲���ģʽ0��
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void RX2_Ready (void)
{
	TA_Init();
	RTI2 &= ~RECEIVE_FLAG;
	BitCnt2=0x08;                     	//Load Bit counter��װ8��BITλ
	TACCTL0=SCS+OUTMOD0+CM1+CAP+CCIE; 	//ͬ������Sync
					                 	//CCIS0=0,�ڲ���ģʽ�У����岶���¼�������ԴΪCCI0A
	                               		//OUTMOD0=1�����ģʽΪ��λ
	                               		//CM1=1�����ò���ģʽ1���½��ز���
                                   		//CAP=1������Ϊ����ģʽ
                                  		//CCIE=1���ж�����
}										//SCSͬ�����񣬣�CCIS0=1��CCIS1=0�������ṩ�����¼�������Դ

/********************************************************\
*	��������TX3_Byte
	�����򣺱��ļ�����
*	���ܣ�  ģ�⴮��2����һ���ֽ��ӳ���
*	������  SBUFOUT3
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void TX3_Byte (void)
{
	TACCTL0 &=~ CCIE;
	TBCCTL0 &=~ CCIE;
	IE2&=~URXIE1;
	RXTXData3=(int)SBUFOUT3;
	BitCnt3 = 0xA;            			// Load Bit counter, 8data + ST/SP��װ10��BITλ
	RTI3 |= ~SEND_FLAG;					//�����Ӧ�ķ��ͱ�־λ
	TACCR1 = TAR;             			// Current state of TA counter����ռ�����
	TACCR1 += Bitime1;          		// Some time till first bit
	RXTXData3 |= 0x100;         		// Add mark stop bit to RXTXData 1Ϊֹͣλ
	RXTXData3 = RXTXData3 << 1;  		// Add space start bit    0Ϊ��ʼλ��λ����1λ��
	TACCTL1 = OUTMOD0+CCIE;      		// TXD������1���ж�����,����CCIFG�ܽ����ж�
	//while (TACCTL1 |= CCIE);      	// Wait for TX completion���ȴ����е�λ���������
	while( (~RTI3)&SEND_FLAG  );
	gDelay_Com_Timer=0;
	while(gDelay_Com_Timer<SEND_DELAY_TIME);
}
/********************************************************\
*	��������RX3_Ready
	�������ⲿ�ļ�����
*	���ܣ�  ģ�⴮��3׼�����������ӳ��򣨲���ģʽ1��
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void RX3_Ready (void)
{
	TA_Init();
	RTI3 &= ~RECEIVE_FLAG;
	BitCnt3=0x8;                    	//Load Bit counter��װ8��BITλ
	TACCTL1=SCS+OUTMOD0+CM1+CAP+CCIE;  	//ͬ������Sync, �(
	                                    //CCIS0=0,�ڲ���ģʽ�У����岶���¼�������ԴΪCCI0A
	                                    //OUTMOD0=1�����ģʽΪ��λ
	                                    //CM1=1�����ò���ģʽ1���½��ز���
	                                    //CAP=1������Ϊ����ģʽ
	                                 	//CCIE=1���ж�����
										//SCSͬ�����񣬣�CCIS0=1��CCIS1=0�������ṩ�����¼�������Դ
	gCommon_Flag|=ALLOW_R232_F_1;
}



/********************************************************\
*	��������TX4_Byte
	�����򣺱��ļ�����
*	���ܣ�  ģ�⴮��4����һ���ֽ��ӳ���
	(������������������ʹ��¼�Ƿ��ͣ�����Ҫ�ڷ�������ǰ����Ҫ��������У��)

*	������  SBUFOUT4
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
����˵����
Odd Parity �� У ��
�� �� �� �� �� ȷ �� �� һ �� У �� �� ����
�� �� �� �� �� ǰ �� �� һ λ �� У �� λ��
�� �� �� ʾ �� �� �� �� �� ��"1"�� �� �� �� �� �� �� �� ż ����
Ϊ �� �� ʱ�� У �� λ �� Ϊ"0"�� �� �� �� Ϊ"1"��
�� �� �� �� �� �� �� �� ż �� �� �䡣 �� �磬
�� Ҫ �� ��"11001110"�� �� �� �� ��5��"1"��
�� �� �� �� У �� λ Ϊ"0"�� ͬ ʱ ��"110011100"�� �� �� �� �� ����
�� �� �� �� �� �� �� �� �� һ �� �� �� �� ż �ԣ�"110011100"�� �� Ȼ �� ��5��"1"��
�� �� �� �� �� �� �� �� �� �� У �� λ �� ��"0"�� �� �� �� �� һ �£�
�� ʾ �� �� �� �� �� �� �� �� δ �� �� �� ��

\********************************************************/
void TX4_Byte (void)
{
	TACCTL0 &=~ CCIE;
	TBCCTL0 &=~ CCIE;
	IE2&=~URXIE1;
	RXTXData4=(int)SBUFOUT4;
	BitCnt4 = 0xA;         		    	// Load Bit counter, 8data + ST/SP��װ11��BITλ,(������һ����У��λ)
	RTI4 |= ~SEND_FLAG;			    	// �����Ӧ�ķ��ͱ�־λ
	TACCR2 = TAR;           	    	// Current state of TA counter����ռ�����
	TACCR2 += Bitime2;        	    	// Some time till first bit
	RXTXData4 |= 0x100;         	    // Add mark stop bit to RXTXData 1Ϊֹͣλ
	RXTXData4 = RXTXData4 << 1;   	    // Add space start bit    0Ϊ��ʼλ��λ����1λ��
	TACCTL2 = OUTMOD0+CCIE;       	    // TXD������1���ж�����,����CCIFG�ܽ����ж�
	//while (TACCTL2 |= CCIE);          // Wait for TX completion���ȴ����е�λ���������
	while ( (~RTI4)&SEND_FLAG );		//ѭ���ȴ�������ɣ����ж��ӳ����У�����������������Ӧ�ķ�����ɱ�־λ��
	gDelay_Com_Timer=0;
	while(gDelay_Com_Timer<SEND_DELAY_TIME);
}
/*
void TX4_Byte (void)
{
	RXTXData4=(int)SBUFOUT4;
	BitCnt4 = 0xB;         		    	// Load Bit counter, 8data + ST/SP��װ11��BITλ,(������һ����У��λ)
	RTI4 |= ~SEND_FLAG;			    	// �����Ӧ�ķ��ͱ�־λ
	TACCR2 = TAR;           	    	// Current state of TA counter����ռ�����
	TACCR2 += Bitime2;        	    	// Some time till first bit
	RXTXData4 |= 0x200;         	    // Add mark stop bit to RXTXData 1Ϊֹͣλ
	//��������У��λ�Ĳ��֣�
	//1�����ƶ�һλ����У��λĬ��Ϊ0

	//2������У��λ��������ݵ�1�ĸ���Ϊż������У��λ����Ϊ1
	for(i=0;i<8;i++)
	{
		if( (SBUFOUT4&0x01)==0x01 )	j=j^0x01;
		SBUFOUT4 = SBUFOUT4 >> 1;
	}
	//3������У��λ
	if(j==1)	RXTXData4 |=0x100;
	//4���ƶ�һλ�����ÿ�ʼλ
	RXTXData4 = RXTXData4 << 1;   	    // Add space start bit    0Ϊ��ʼλ��λ����1λ��
	TACCTL2 = OUTMOD0+CCIE;       	    // TXD������1���ж�����,����CCIFG�ܽ����ж�
	//while (TACCTL2 |= CCIE);          // Wait for TX completion���ȴ����е�λ���������
	while ( (~RTI4)&SEND_FLAG );		//ѭ���ȴ�������ɣ����ж��ӳ����У�����������������Ӧ�ķ�����ɱ�־λ��
	gDelay_Com_Timer=0;
	while(gDelay_Com_Timer<SEND_DELAY_TIME);
}
*/
/********************************************************\
*	��������RX4_Ready
	�������ⲿ�ļ�����
*	���ܣ�  ģ�⴮��4׼�����������ӳ��򣨲���ģʽ1��
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void RX4_Ready (void)
{
	TA_Init();
	RTI4 &= ~RECEIVE_FLAG;
	BitCnt4=0x08;                    	//Load Bit counter��װ8��BITλ
	TACCTL2=SCS+OUTMOD0+CM1+CAP+CCIE;  	//ͬ������Sync,
	                                   	//CCIS0=0,�ڲ���ģʽ�У����岶���¼�������ԴΪCCI0A
	                                	//OUTMOD0=1�����ģʽΪ��λ
	                               		//CM1=1�����ò���ģʽ1���½��ز���
	                               		//CAP=1������Ϊ����ģʽ
	                              		//CCIE=1���ж�����
										//SCSͬ�����񣬣�CCIS0=1��CCIS1=0�������ṩ�����¼�������Դ
	gCommon_Flag|=ALLOW_R232_F_1;
}

/********************************************************\
*	��������OFF_RXTXCOM2
	�����򣺴���Ŀ�ļ�δ����
*	���ܣ�  �ر�ģ�⴮��2���ͺͽ�������
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
/*
void OFF_RXTXCOM2(void)
{
	TACCTL0 &= ~ CCIE;
}
*/
/********************************************************\
*	��������OFF_RXTXCOM3
	�����򣺴���Ŀ�ļ�δ����
*	���ܣ�  �ر�ģ�⴮��3���ͺͽ�������
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
/*
void OFF_RXTXCOM3(void)
{
	TACCTL1 &= ~ CCIE;
}
*/
/********************************************************\
*	��������OFF_RXTXCOM4
	�����򣺴���Ŀ�ļ�δ����
*	���ܣ�  �ر�ģ�⴮��4���ͺͽ�������
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
/*
void OFF_RXTXCOM4(void)
{
	TACCTL2 &= ~ CCIE;
}
*/
/********************************************************\
*	��������Send_COM2_Byte
	�������ⲿ�ļ�����
*	���ܣ�  ͨ��ģ�⴮��2����һ���ֽ�
*	������  Input
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Send_COM2_Byte(unsigned char Input)
{
	SBUFOUT2=Input;
	TX2_Byte();
}

/********************************************************\
*	��������Send_COM2_String
	�������ⲿ�ļ�����
*	���ܣ�  ͨ��ģ�⴮��2����һ���ַ���
*	������  *Point �ַ���ָ��
			 Count �ַ�������
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Send_COM2_String(unsigned char *Point,unsigned int Count)
{
	while(Count>0)
	{
		SBUFOUT2=*Point;
		TX2_Byte();
		Point++;
		Count--;
	}
}
/********************************************************\
*	��������Send_COM3_Byte
	�������ⲿ�ļ�����
*	���ܣ�  ͨ��ģ�⴮��3����һ���ֽ�
*	������  Input
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Send_COM3_Byte(unsigned char Input)
{
	SBUFOUT3=Input;
	TX3_Byte();
}
/********************************************************\
*	��������Send_COM3_String
	�������ⲿ�ļ�����
*	���ܣ�  ͨ��ģ�⴮��3����һ���ַ���
*	������  *Point �ַ���ָ��
			 Count �ַ�������
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Send_COM3_String(unsigned char *Point,unsigned int Count)
{
	while(Count>0)
	{
		SBUFOUT3=*Point;
		TX3_Byte();
		Point++;
		Count--;
	}
	RX3_Ready();
}
/********************************************************\
*	��������Send_COM4_Byte
	�������ⲿ�ļ�����
*	���ܣ�  ͨ��ģ�⴮��4����һ���ֽ�
*	������  Input
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Send_COM4_Byte(unsigned char Input)
{
	SBUFOUT4=Input;
	TX4_Byte();
}
/********************************************************\
*	��������Send_COM4_String
	�������ⲿ�ļ�����
*	���ܣ�  ͨ��ģ�⴮��4����һ���ַ���
*	������  *Point �ַ���ָ��
			 Count �ַ�������
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void Send_COM4_String(unsigned char *Point,unsigned int Count)
{
	while(Count>0)
	{
		SBUFOUT4=*Point;
		TX4_Byte();
		Point++;
		Count--;
	}
}
/********************************************************\
*	��������Timer_A0���ж��ӳ���
	�������ⲿ�ļ�����
*	���ܣ�  ��ʱ��TA���ж�Timer A CC0 (����ģ�⴮��2�ķ��ͺͽ����жϴ���)
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
interrupt[TIMERA0_VECTOR] void Timer_A0 (void)
{
	TACCR0 += Bitime0;         				//��ʱ������CCR0��ֵ���ӣ��Լ���TA��ʱ
        //RX������һ���ֽڵĳ�����
  	if ( (~RTI2)&RECEIVE_FLAG )     	    //�ж��Ƿ��ṩ�����¼��Ĳ���Դ�в����¼�����
  	{
    	if( TACCTL0 & CAP )     			//�ж��Ƿ񻹴��ڲ���ģʽ״̬
    										//���Ϊ����ģʽ����ʾ���յ��˿�ʼλ��
    	{
      		TACCTL0 &= ~ CAP;     			//�ǲ���ģʽ �򽫲���ģʽ��ɱȽ�ģʽ��׼����ʼ��������λ                                                              		//�򽫲���ģʽ����Ϊ�Ƚ�ģʽ
      		TACCR0 += Bitime0_5;			//���Ӽ�����ֵ,��1.5������λ��ʱ�䳤��
    	}
    	else	                			//�жϴ��ڱȽ�ģʽ״̬�����������δ�����������λ
    										//Ϊ�Ƚ�ģʽ���������������λ
    	{
      		RXTXData2 = RXTXData2 >> 1; 	//��������1λ���Ƚ������ݵ�λ��
      		if (TACCTL0 & SCCI)       		// ����������ȡ�������źţ����Ϊ1�������������Ϊ0
      		{
        		RXTXData2 |= 0x80;       	//�����ź�Ϊ1
      		}
      		BitCnt2 -- ;
      		if ( BitCnt2 == 0 )         	//�ж������Ƿ�����ꣿ
      		{
        		//TACCTL0 &= ~CCIE;       	//���е�λ������ϣ����ֹ�ж�
        		SBUFIN2=(char)RXTXData2;
        		TAUARTCTL|=TAUARTIFG;		//����Ƚ���A���ж������ı�־λ�������Ƚ���A���ж�
        		RTI2|=RECEIVE_FLAG;			//����ģ�⴮��2�Ľ��ձ�־λ
      		}
    	}
	}
	// TX������һ���ֽڵĳ�����
  	else
  	{
    	if ( BitCnt2 == 0 )           		//�ж��Ƿ��������ݷ������
    	{
    		RTI2 |= SEND_FLAG;
          	//TACCTL0 &= ~ CCIE;          	//���е�λ��������ɣ����ֹ�жϷ�
        }
    	else                        		//λδ�����꣬���������
    	{
      		TACCTL0|=OUTMOD2+OUTMOD0;   	// TX Space�õ�
      		if (RXTXData2 & 0x01)        	//�������λΪ1
      		{
        		TACCTL0 &= ~ OUTMOD2;     	// TX Mark �ø�
      		}
      		RXTXData2 = RXTXData2 >> 1;
      		BitCnt2 --;
		}
  	}
}

/*
��ʱ��TA���ж�
Timer A CC1-2
*/
/********************************************************\
*	��������Timer_A1���ж��ӳ���
	�������ⲿ�ļ�����
*	���ܣ�  ��ʱ��TA���ж�Timer A CC1-2 (����ģ�⴮��3��4�ķ��ͺͽ����жϴ�����
			����ģ�⴮�ڷ������ݣ��������ݵĴ�����������ж������)
*	������  ��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
interrupt[TIMERA1_VECTOR] void Timer_A1 (void)
{
	if(TACCTL1&CCIFG)						//TACCIFG1����λ��λ��,����TAIV������Ӧ��CCIFG��λ
   	{
		TACCTL1&=0xFFFE;
		TACCR1 += Bitime1;         			// ��ʱ������TACCR1��ֵ���ӣ��Լ���TA��ʱ
	 	if ( (~RTI3)&RECEIVE_FLAG )    		//�ж��Ƿ��ṩ�����¼��Ĳ���Դ�в����¼�������
	 	{
	   		if( TACCTL1 & CAP )     		// �ж��Ƿ񻹴��ڲ���ģʽ״̬	   																									//���Ϊ����ģʽ����ʾ���յ��˿�ʼλ��
	   		{
	     		TACCTL1 &= ~ CAP;     		//�ǲ���ģʽ �򽫲���ģʽ��ɱȽ�ģʽ��׼����ʼ��������λ                                                             		//�򽫲���ģʽ����Ϊ�Ƚ�ģʽ
	     		TACCR1 += Bitime1_5;		//���Ӽ�����ֵ,��1.5������λ��ʱ�䳤��
	     		//ֹͣģ�⴮��2���ж�
    			TACCTL0 &=~ CCIE;
    			TBCCTL0 &=~ CCIE;
    			IE2&=~URXIE1;
	   		}
	   		else	                		//�жϴ��ڱȽ�ģʽ״̬�����������δ�����������λ																								//Ϊ�Ƚ�ģʽ���������������λ
	   		{
	     		RXTXData3 = RXTXData3 >> 1; //��������1λ���Ƚ������ݵ�λ��
	     		if (TACCTL1 & SCCI)       	// ����������ȡ�������źţ����Ϊ1�������������Ϊ0
	     		{
	       			RXTXData3 |= 0x80;  	//�����ź�Ϊ1
	     		}
	     		BitCnt3 --;
	     		if ( BitCnt3 == 0)			//�ж������Ƿ�����ꣿ
	     		{
	       			SBUFIN3 = (char)RXTXData3;
	       			RTI3 |= RECEIVE_FLAG;
	       			//���������ֱ������ݵĲ���
					gHandle_Buffer[gHandle_Buffer_Point]=SBUFIN3;
					gHandle_Buffer_Point++;
					if(gHandle_Buffer_Point>=sizeof(gHandle_Buffer)) gHandle_Buffer_Point=0;
					gHandle_Receive_Over=0;
					RTI3 &= ~RECEIVE_FLAG;
					BitCnt3=0x8;            //Load Bit counter��װ8��BITλ
					TACCTL1=SCS+OUTMOD0+CM1+CAP+CCIE;
					TACCTL0 |= CCIE;
					TBCCTL0 |= CCIE;
					IE2|=URXIE1;
	     		}
	   		}
	 	}
	 	else                   				//Ϊ�����ж�
	 	{
	    	if ( BitCnt3 == 0)           	//�ж��Ƿ��������ݷ������
	    	{
	    		RTI3 |= SEND_FLAG;			//������Ӧ�ķ��ͳɹ��ı�־λ
				TACCTL1 &= ~ CCIE;
				TACCTL0 |= CCIE;
				TBCCTL0 |= CCIE;
				IE2|=URXIE1;
			}
	    	else                        	//λδ�����꣬���������
	    	{
	      		TACCTL1|=OUTMOD2+OUTMOD0; 	// TX Space�õ�
	      		if (RXTXData3 & 0x01)      	//�������λΪ1
	      		{
	        		TACCTL1 &= ~ OUTMOD2;	// TX Mark �ø�
	      		}
	      		RXTXData3= RXTXData3 >> 1;
	      		BitCnt3 --;
	    	}
	 	}
	}

   	if(TACCTL2&CCIFG)						//TACCIFG2����λ��λ��
   	{
   	 	TACCTL2&=0xFFFE;
	 	TACCR2 += Bitime2;         			// ��ʱ������TACCR2��ֵ���ӣ��Լ���TA��ʱ
	 	if ((~RTI4)&RECEIVE_FLAG)     	 	//�ж��Ƿ��ṩ�����¼��Ĳ���Դ�в����¼�������
	 	{
	   		if( TACCTL2 & CAP )     		// �ж��Ƿ񻹴��ڲ���ģʽ״̬	   																									//���Ϊ����ģʽ����ʾ���յ��˿�ʼλ��
	   		{
		     	TACCTL2 &= ~ CAP;     		//�ǲ���ģʽ �򽫲���ģʽ��ɱȽ�ģʽ��׼����ʼ��������λ                                                             		//�򽫲���ģʽ����Ϊ�Ƚ�ģʽ
		     	TACCR2 += Bitime2_5;		//���Ӽ�����ֵ,��1.5������λ��ʱ�䳤��
	     		//ֹͣģ�⴮��2��TB���ж�
				TACCTL0 &=~ CCIE;
				TBCCTL0 &=~ CCIE;
				IE2&=~URXIE1;
	   		}
	   		else	                		//�жϴ��ڱȽ�ģʽ״̬�����������δ�����������λ 																									//Ϊ�Ƚ�ģʽ���������������λ
	   		{
	     		RXTXData4 = RXTXData4 >> 1; //��������1λ���Ƚ������ݵ�λ��
	     		if (TACCTL2 & SCCI)       	// ����������ȡ�������źţ����Ϊ1�������������Ϊ0
	     		{
	      			RXTXData4 |= 0x80;      //�����ź�Ϊ1(����9��bit����Ϊ����һ����У��λ)
	     		}
	     		BitCnt4 --;
	     		if ( BitCnt4 == 0)         	//�ж������Ƿ�����ꣿ
	     		{
	       			SBUFIN4 = (char)RXTXData4;
	       			RTI4 |= RECEIVE_FLAG;
					gDisp_Buffer_R[gDisp_Buffer_R_Point]=SBUFIN4;
					gDisp_Buffer_R_Point++;
					if(gDisp_Buffer_R_Point>=sizeof(gDisp_Buffer_R)) gDisp_Buffer_R_Point=0;
					gDisp_Receive_Over=0;
					RTI4 &= ~RECEIVE_FLAG;
					BitCnt4=0x08;                   //Load Bit counter��װ8��BITλ
					TACCTL2=SCS+OUTMOD0+CM1+CAP+CCIE;
					TACCTL0 |= CCIE;
					TBCCTL0 |= CCIE;
					IE2|=URXIE1;
	     		}
	   		}
		}
		else                       			//Ϊ�����ж�
		{
			if ( BitCnt4 == 0)				//�ж��Ƿ��������ݷ�����ɣ�
			{
				RTI4 |= SEND_FLAG;
				TACCTL2 &= ~ CCIE;
				TACCTL0 |= CCIE;
				TBCCTL0 |= CCIE;
				IE2|=URXIE1;
			}
			else                     		//λδ�����꣬���������
			{
				TACCTL2|=OUTMOD2+OUTMOD0; 	// TX Space�õ�
				if (RXTXData4 & 0x01)     	//�������λΪ1
				{
					TACCTL2 &= ~ OUTMOD2;  	// TX Mark �ø�
				}
				RXTXData4= RXTXData4 >> 1;
				BitCnt4 --;
			}
		}
	}
}