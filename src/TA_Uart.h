/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
说明：MSP430C149
扩展I/O作为模拟RS232串口（P1口）
串口2：P1.1----RX ;P1.5-----TX
串口3：P1.2----RX ;P1.6-----TX
串口4：P1.3----RX ;P1.7-----TX

程序变量的说明：SBUFIN2,SBUFIN3,SBUFIN4分别为模拟串口，2，3，4接收数据缓冲
                SBUFOUT2,SBUFOUT3,SBUFOUT4分别为模拟串口，2，3，4的发送数据缓冲
                BitCnt2,BitCnt3,BitCnt4分别为发送一个字节的位数，这里加上起始位，和停止位共10位
                RTI2,RTI3,RTI4分别为存储各个模拟串口的发送和接收标志的变量，一个模块对应一个变量
                	每一个变量占用两个BIT，一个为发送标志，一个为接收标志



子程序说明：
void TX2_Byte(void);      	为发送一个字节的发送子程序，发送每一个BIT位都会进入中断子程序，
                          	知道所有的位发送完成，在中断子程序中，每发送完一个字节则设置相应的发送完成标志位，如果发送完所有的字节
                          	则在主程序中关闭中断允许位。发送的字节存储在SBUFOUT2变量中

void TX3_Byte(void);
void TX4_Byte(void);      	模拟串口3，4在同一个中断子程序中处理的（但仍然是两个捕获模块），即模拟串口的3，4的发送均不能同时进行
							模拟串口每发送完成一个字节，则设置相应的发送完成的标志位。在主程序中，如果发送完所有的字节，则关闭
							中断允许。发送的字节存储在SBUFOUT3，SBUFOUT4变量中
**************************************************************************************
void RX2_Ready(void);		为准备接收数据的准备（寄存器的接收初始化设置，开相应捕获模块的中断允许），每接收完一个字节，不能关闭相应捕获模块
							的中断允许位，否则影响下一个字节的接收，在主程序的接收处理部分，可以采用超过多长时间没接收到字节了，则可以关闭
							此捕获模块的中断允许位则会关闭
							特别注意的是，RTI2中的RECEIVE_FLAG只有在需要贮备接收数据的时候才清0，否则发送不了数据
										  RTI2中的在SEND_FLAG只有在发送一个字节的时候情0，在发送完一个字节后，则置1。

void RX3_Ready(void);
void RX4_Ready(void);

模拟串口2的发送和接收处理在一个中断子程序中，模块2的接收处理采用这样的方式，一旦有接收到一个新的字节数据
							则设置比较器A的中断标志，在主程序部分则进入比较器A的中断，在比较器A的中断子程序中
							处理接收模拟串口2的接收数据。
模拟串口3，4的发送和接收处理在另外一个中断子程序中，但每个模拟串口对应不同的捕获模块。
							模拟串口3，4，则在接收一个字节数据后，则设置接收中断中设置接收数据标志，具体处理则在主程序里
							做相关处理。

void OFF_RXTXCOM2(void);	关闭关于模拟串口2操作的捕获模块的中断允许
void OFF_RXTXCOM3(void);	关闭关于模拟串口3操作的捕获模块的中断允许
void OFF_RXTXCOM4(void);	关闭关于模拟串口4操作的捕获模块的中断允许
		                    这里关闭需要根据实际情况在主程序中应用
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#ifndef __TA_Uart_H
#define __TA_Uart_H

#define TXD2   			0x20      //100000    TXD2 on P1.5，定义P1.5为TXD2
#define RXD2   			0x02      //10        RXD2 on P1.1，定义P1.1为RXD2

#define TXD3			0x40      //1000000	  TXD3 on P1.6，定义P1.6为TXD3
#define RXD3			0x04      //100       RXD3 on P1.2，定义P1.2为RXD3

#define TXD4			0x80	  //10000000  TXD4 on P1.7，定义P1.7为TXD4
#define RXD4			0x08      //1000      RXD4 on P1.3，定义P1.3为RXD4



/*
//三个模拟串口的为2400波特率
#define Bitime0_5  0x683   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime0    0xD05   	//
                           	//Bitime的计算方式=8000000/2400=3333.333=3333=0xD05

#define Bitime1_5  0x683   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime1    0xD05   	//
                           	//Bitime的计算方式=8000000/2400=3333.333=3333=0xD05

#define Bitime2_5  0x683   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime2    0xD05   	//
						   	//BBitime的计算方式=8000000/2400=3333.333=3333=0xD05
*/
/*
//三个模拟串口的为4800波特率
#define Bitime0_5  0x341   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime0    0x683   	//
                           	//Bitime的计算方式=8000000/4800=1666.666=1667=0x683

#define Bitime1_5  0x341   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime1    0x683   	//
                           	//Bitime的计算方式=8000000/4800=1666.666=1667=0x683

#define Bitime2_5  0x341   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime2    0x683   	//
						   	//Bitime的计算方式=8000000/4800=1666.666=1667=0x683
*/

//三个模拟串口的为9600波特率
#define Bitime0_5  0x1A0   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime0    0x341   	// 104 us ~ 9596 baud
                           	//Bitime的计算方式=8000000/9600=833.333=833=0x341

#define Bitime1_5  0x1A0   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime1    0x341   	// 104 us ~ 9596 baud
                           	//Bitime的计算方式=8000000/9600=833.333=833=0x341

#define Bitime2_5  0x1A0   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime2    0x341   	// 104 us ~ 9596 baud
						   	//Bitime的计算方式=8000000/9600=833.333=833=0x341
/*
//三个模拟串口的为14400波特率
#define Bitime0_5  0x115   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime0    0x22B   	//
                           	//Bitime的计算方式=8000000/14400=555.555=556=0x22B

#define Bitime1_5  0x115   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime1    0x22B   	//
                           	//Bitime的计算方式=8000000/14400=555.555=556=0x22B

#define Bitime1_5  0x115   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime1    0x22B   	//
                           	//Bitime的计算方式=8000000/14400=555.555=556=0x22B
*/
/*
//三个模拟串口的为14400波特率
#define Bitime0_5  0x115   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime0    0x22B   	//
                           	//Bitime的计算方式=8000000/14400=555.555=556=0x22B

#define Bitime1_5  0x115   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime1    0x22B   	//
                           	//Bitime的计算方式=8000000/14400=555.555=556=0x22B

#define Bitime2_5  0x115   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime2    0x22B   	//
                           	//Bitime的计算方式=8000000/14400=555.555=556=0x22B
*/

/*
//三个模拟串口的为19200波特率
#define Bitime0_5  0x0D0   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime0    0x1A0   	//
                           	//Bitime的计算方式=8000000/19200=416.555=417=0x1A0

#define Bitime1_5  0x0D0   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime1    0x1A0   	//
                           	//Bitime的计算方式=8000000/19200=416.555=417=0x1A0

#define Bitime2_5  0x0D0   	// ~ 0.5 bit length 0.5数据位的长度
#define Bitime2    0x1A0   	//
                           	//Bitime的计算方式=8000000/19200=416.555=417=0x1A0
*/





#define SEND_FLAG                (0x0001)		//各个模拟串口发送标志的BIT位
#define RECEIVE_FLAG             (0x0002)		//各个模拟串口接收标志的BIT位


#define TAUART_VECTOR 11*2       //比较器A的中断向量
#define TAUARTIFG     0x01       //比较器A的中断标志
#define TAUARTIE      0x02       //比较器A的中断允许位
sfrb    TAUARTCTL = 0x0059;      //比较器A的控制寄存器1

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
