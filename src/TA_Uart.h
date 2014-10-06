/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
˵����MSP430C149
��չI/O��Ϊģ��RS232���ڣ�P1�ڣ�
����2��P1.1----RX ;P1.5-----TX
����3��P1.2----RX ;P1.6-----TX
����4��P1.3----RX ;P1.7-----TX

���������˵����SBUFIN2,SBUFIN3,SBUFIN4�ֱ�Ϊģ�⴮�ڣ�2��3��4�������ݻ���
                SBUFOUT2,SBUFOUT3,SBUFOUT4�ֱ�Ϊģ�⴮�ڣ�2��3��4�ķ������ݻ���
                BitCnt2,BitCnt3,BitCnt4�ֱ�Ϊ����һ���ֽڵ�λ�������������ʼλ����ֹͣλ��10λ
                RTI2,RTI3,RTI4�ֱ�Ϊ�洢����ģ�⴮�ڵķ��ͺͽ��ձ�־�ı�����һ��ģ���Ӧһ������
                	ÿһ������ռ������BIT��һ��Ϊ���ͱ�־��һ��Ϊ���ձ�־



�ӳ���˵����
void TX2_Byte(void);      	Ϊ����һ���ֽڵķ����ӳ��򣬷���ÿһ��BITλ��������ж��ӳ���
                          	֪�����е�λ������ɣ����ж��ӳ����У�ÿ������һ���ֽ���������Ӧ�ķ�����ɱ�־λ��������������е��ֽ�
                          	�����������йر��ж�����λ�����͵��ֽڴ洢��SBUFOUT2������

void TX3_Byte(void);
void TX4_Byte(void);      	ģ�⴮��3��4��ͬһ���ж��ӳ����д���ģ�����Ȼ����������ģ�飩����ģ�⴮�ڵ�3��4�ķ��;�����ͬʱ����
							ģ�⴮��ÿ�������һ���ֽڣ���������Ӧ�ķ�����ɵı�־λ�����������У�������������е��ֽڣ���ر�
							�ж��������͵��ֽڴ洢��SBUFOUT3��SBUFOUT4������
**************************************************************************************
void RX2_Ready(void);		Ϊ׼���������ݵ�׼�����Ĵ����Ľ��ճ�ʼ�����ã�����Ӧ����ģ����ж�������ÿ������һ���ֽڣ����ܹر���Ӧ����ģ��
							���ж�����λ������Ӱ����һ���ֽڵĽ��գ���������Ľ��մ����֣����Բ��ó����೤ʱ��û���յ��ֽ��ˣ�����Թر�
							�˲���ģ����ж�����λ���ر�
							�ر�ע����ǣ�RTI2�е�RECEIVE_FLAGֻ������Ҫ�����������ݵ�ʱ�����0�������Ͳ�������
										  RTI2�е���SEND_FLAGֻ���ڷ���һ���ֽڵ�ʱ����0���ڷ�����һ���ֽں�����1��

void RX3_Ready(void);
void RX4_Ready(void);

ģ�⴮��2�ķ��ͺͽ��մ�����һ���ж��ӳ����У�ģ��2�Ľ��մ�����������ķ�ʽ��һ���н��յ�һ���µ��ֽ�����
							�����ñȽ���A���жϱ�־���������򲿷������Ƚ���A���жϣ��ڱȽ���A���ж��ӳ�����
							�������ģ�⴮��2�Ľ������ݡ�
ģ�⴮��3��4�ķ��ͺͽ��մ���������һ���ж��ӳ����У���ÿ��ģ�⴮�ڶ�Ӧ��ͬ�Ĳ���ģ�顣
							ģ�⴮��3��4�����ڽ���һ���ֽ����ݺ������ý����ж������ý������ݱ�־�����崦��������������
							����ش���

void OFF_RXTXCOM2(void);	�رչ���ģ�⴮��2�����Ĳ���ģ����ж�����
void OFF_RXTXCOM3(void);	�رչ���ģ�⴮��3�����Ĳ���ģ����ж�����
void OFF_RXTXCOM4(void);	�رչ���ģ�⴮��4�����Ĳ���ģ����ж�����
		                    ����ر���Ҫ����ʵ���������������Ӧ��
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#ifndef __TA_Uart_H
#define __TA_Uart_H

#define TXD2   			0x20      //100000    TXD2 on P1.5������P1.5ΪTXD2
#define RXD2   			0x02      //10        RXD2 on P1.1������P1.1ΪRXD2

#define TXD3			0x40      //1000000	  TXD3 on P1.6������P1.6ΪTXD3
#define RXD3			0x04      //100       RXD3 on P1.2������P1.2ΪRXD3

#define TXD4			0x80	  //10000000  TXD4 on P1.7������P1.7ΪTXD4
#define RXD4			0x08      //1000      RXD4 on P1.3������P1.3ΪRXD4



/*
//����ģ�⴮�ڵ�Ϊ2400������
#define Bitime0_5  0x683   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime0    0xD05   	//
                           	//Bitime�ļ��㷽ʽ=8000000/2400=3333.333=3333=0xD05

#define Bitime1_5  0x683   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime1    0xD05   	//
                           	//Bitime�ļ��㷽ʽ=8000000/2400=3333.333=3333=0xD05

#define Bitime2_5  0x683   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime2    0xD05   	//
						   	//BBitime�ļ��㷽ʽ=8000000/2400=3333.333=3333=0xD05
*/
/*
//����ģ�⴮�ڵ�Ϊ4800������
#define Bitime0_5  0x341   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime0    0x683   	//
                           	//Bitime�ļ��㷽ʽ=8000000/4800=1666.666=1667=0x683

#define Bitime1_5  0x341   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime1    0x683   	//
                           	//Bitime�ļ��㷽ʽ=8000000/4800=1666.666=1667=0x683

#define Bitime2_5  0x341   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime2    0x683   	//
						   	//Bitime�ļ��㷽ʽ=8000000/4800=1666.666=1667=0x683
*/

//����ģ�⴮�ڵ�Ϊ9600������
#define Bitime0_5  0x1A0   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime0    0x341   	// 104 us ~ 9596 baud
                           	//Bitime�ļ��㷽ʽ=8000000/9600=833.333=833=0x341

#define Bitime1_5  0x1A0   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime1    0x341   	// 104 us ~ 9596 baud
                           	//Bitime�ļ��㷽ʽ=8000000/9600=833.333=833=0x341

#define Bitime2_5  0x1A0   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime2    0x341   	// 104 us ~ 9596 baud
						   	//Bitime�ļ��㷽ʽ=8000000/9600=833.333=833=0x341
/*
//����ģ�⴮�ڵ�Ϊ14400������
#define Bitime0_5  0x115   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime0    0x22B   	//
                           	//Bitime�ļ��㷽ʽ=8000000/14400=555.555=556=0x22B

#define Bitime1_5  0x115   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime1    0x22B   	//
                           	//Bitime�ļ��㷽ʽ=8000000/14400=555.555=556=0x22B

#define Bitime1_5  0x115   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime1    0x22B   	//
                           	//Bitime�ļ��㷽ʽ=8000000/14400=555.555=556=0x22B
*/
/*
//����ģ�⴮�ڵ�Ϊ14400������
#define Bitime0_5  0x115   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime0    0x22B   	//
                           	//Bitime�ļ��㷽ʽ=8000000/14400=555.555=556=0x22B

#define Bitime1_5  0x115   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime1    0x22B   	//
                           	//Bitime�ļ��㷽ʽ=8000000/14400=555.555=556=0x22B

#define Bitime2_5  0x115   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime2    0x22B   	//
                           	//Bitime�ļ��㷽ʽ=8000000/14400=555.555=556=0x22B
*/

/*
//����ģ�⴮�ڵ�Ϊ19200������
#define Bitime0_5  0x0D0   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime0    0x1A0   	//
                           	//Bitime�ļ��㷽ʽ=8000000/19200=416.555=417=0x1A0

#define Bitime1_5  0x0D0   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime1    0x1A0   	//
                           	//Bitime�ļ��㷽ʽ=8000000/19200=416.555=417=0x1A0

#define Bitime2_5  0x0D0   	// ~ 0.5 bit length 0.5����λ�ĳ���
#define Bitime2    0x1A0   	//
                           	//Bitime�ļ��㷽ʽ=8000000/19200=416.555=417=0x1A0
*/





#define SEND_FLAG                (0x0001)		//����ģ�⴮�ڷ��ͱ�־��BITλ
#define RECEIVE_FLAG             (0x0002)		//����ģ�⴮�ڽ��ձ�־��BITλ


#define TAUART_VECTOR 11*2       //�Ƚ���A���ж�����
#define TAUARTIFG     0x01       //�Ƚ���A���жϱ�־
#define TAUARTIE      0x02       //�Ƚ���A���ж�����λ
sfrb    TAUARTCTL = 0x0059;      //�Ƚ���A�Ŀ��ƼĴ���1

extern unsigned char SBUFIN2,SBUFIN3,SBUFIN4;
extern unsigned char SBUFOUT2,SBUFOUT3,SBUFOUT4;
extern unsigned char BitCnt2,BitCnt3,BitCnt4;
extern unsigned char RTI2,RTI3,RTI4;
extern unsigned char RIE;

//void TX2_Byte(void);
//void TX3_Byte(void);
//void TX4_Byte(void);

void RX2_Ready(void);
void RX3_Ready(void);
void RX4_Ready(void);

//void OFF_RXTXCOM2(void);
//void OFF_RXTXCOM3(void);
//void OFF_RXTXCOM4(void);

void Send_COM2_Byte(unsigned char Input);
void Send_COM2_String(unsigned char *Point,unsigned int Count);
void Send_COM3_Byte(unsigned char Input);
void Send_COM3_String(unsigned char *Point,unsigned int Count);
void Send_COM4_Byte(unsigned char Input);
void Send_COM4_String(unsigned char *Point,unsigned int Count);

#endif
