/*	主程序修改说明记录
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
2005年6月20日：
程序文件说明：
	根据新制作的PCB图(GPS-M22：V2.4)版本的原理图，此程序为对应的系统文件

程序版本号：V2.32

程序功能说明：
	1，增加外部的SRAM，用于接收外接部件的大量数据的传输（增加W24L011AJ(128K)，实际使用64K），锁存器74HC573。
	2，串口的分配：	串口0		M22
				 	串口1		外接部件（调试串口）
				 	串口2		GPS
				 	串口3		车载电话
				 	串口4		显示终端
	3，主要实现功能：
		(1),与行使记录仪之间的交互通讯
		(2),
		(3),
		(4),
		(5),

详细说明与行使记录仪之间的通讯流程：

主控向记录仪发送的数据有：	1，每一秒种发送8个字节的经，纬度,(用于记录仪存储和运算)（记录仪不用给出ECHO）
							2，每隔1个小时发送一个查询当前记录仪时间，如果判断记录仪的时间和GPS的时间误差超过1秒，此时记录仪显示车辆速度为0，则发送矫正时间参数）
							   记录仪需要给出ECHO
							3，主控接收到中心设置超速的限制值的参数设置（转发给记录仪）（记录仪需要给出ECHO）
							4，主控接收到中心设置间隔距离的限制值的参数设置（转发给记录仪）（记录仪需要给出ECHO）
							5，主控接收到中心设置疲劳驾驶的时间限制值的参数设置（转发给记录仪）（记录仪需要给出ECHO）

记录仪表向主控发送的数据有：1，记录仪接收到主控的查询当前时间命令，则返回当前记录仪的时间和当前速度的值
							2，如果车辆速度超过设置的报警警戒值，则记录仪上发超速报警的信息，每间隔2秒发送一次，直至主控收到后返回响应，则停止发送超速报警信息
							   如果记录仪在超速的状态下，判断不处于超速，则主动发送停止超速的的信息，直至主控收到后返回响应
							3，如果属于疲劳驾驶的状态下，记录仪应该向主控发送一个信息
							4，车辆行使状态下，根据数据上报的类型（按时间上报数据点，按距离上报数据点...）如果属于按距离上报数据点，则如果记录仪表每判断到一个间隔
							   距离，则需要向主控发送一个信息

主控查询大量记录仪数据的通讯形式：中心下发一个查询记录仪大量数据的下行命令，主控转发给记录仪，记录仪则分批读取数据发送给主控
								  主控先负责接收数据，每接收一包数据，先判断是否属于部分数据包，如果是，则将有效数据提取出来，依次存储在外部的扩展SRAM里。然后给出一个
								  接收到此数据包的响应，然后等待接收记录仪的下一个数据包。直至接收到记录仪发送过来的此数据全部转发完成的信息，则主控开始进行外部SRAM的数据
								  转发（TCP方式）]

								  所有的中心的查询命令和设置命令，主控均不做命令字的检验，，主控接收记录仪的所有命令的回应包括设置的响应，或者数据返回

2005年8月30日
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
/********************************************************\
系统程序结构的说明：
1，FLASH的分配
	扇区0-----暂停使用
	扇区1，2---作为存储目标号码，IP地址和端口号（双备份）
	扇区3，4---作为电话号码的存储
	扇区5，6---作为上传数据间隔，开关量等等的存储（双备份）

2，串口的分配
					串口0		M22
				 	串口1		外接部件（调试串口）
				 	串口2		GPS
				 	串口3		车载电话
				 	串口4		显示终端
3，	SRAM的分配
	1，串行存储器的两个264byte的SRAM的分配buffer1,buffer2，
		buffer1的分配：与原来的程序保持一致（不变），具体细节见相关地方
		buffer2的使用用来存储读取的相关页的数据

	2，外部64K的SRAM的分配
	   0-----60K		作为接收外部数据（以行使记录仪为例）大量数据的接收（在串口1的中断子程序中处理接收）
	   61K---64K		作为存储记录仪需要向显示终端发送数据的缓冲存储（暂时这么考虑）

	3，MSP430F149的内部SRAM的分配

4，TCP数据的发送方式和类别
	根据以前的TCP数据的方式发送方式，现在将发送的数据分位如下几类：
	A，	存储在buffer1相关区域的上行数据（包括手柄，和显示终端需要上行的相关信息，和对一些下行命令的响应信息）
	B， 存储在周期数据缓冲区gCircle_Buffer[]中有需要发送的数据（主要存储的是周期发送的报警信息和行车轨迹信息）
	C， TCP在线状态下的握手信息数据的定时发送主要以发FF 0D数据
	D， 发送外接部件的数据（以存储在外部的SRAM的数据区域）

5，显示终端和手柄终端的接收和发送缓冲区的说明
	手柄和显示屏与主控系统通讯均是采用的模拟串口3，和4。属于半双工的形式，故在向终端发送数据的时候需要判断是否在接收数据。

	接收手柄的数据缓冲：gHandle_Buffer[30]
	向手柄的发送数据的缓冲：保持与原来的兼容，是在需要向手柄发送数据的时候才组织数据发送（以节省SRAM的空间）

	接收显示屏数据缓冲：gDisp_Buffer_R[50]
	向显示屏的发送数据的缓冲：gDisp_Buffer[256]

6,与外接部件的数据通讯的接收和发送缓冲说明（以记录仪为例）
	接收外接部件的数据缓冲：外部SRAM（0---64K）
	向外接部件发送的数据缓冲：

修改记录：

2005年8月23日：

		ACC关闭后20分钟，TCP下线，将按30分钟的方式传送数据。1个小时关闭模块
		ACC关闭后20分钟 ，如果ACC再次开启，则重新启动程序

		行车轨迹只可以最少10分钟的SMS的补偿，报警信息可以允许GSM方式传送

		手柄通讯方式的改变


		重点监控制：中心下行监控命令，则改变工作状态为监控方式，则上传数据的不受ACC开和关闭的控制
					并且，不管时间间隔多大，均采用TCP常连线的方式。重点监控的方式是数传方式的改变
					但此状态方式并不存储在FLASH中，同时，即使ACC关闭，在开启的时候，也不允许重新
					启动程序。直到重点监控结束。数传的方式改变。

2005年9月1日：

		增加通过手柄接口设置参数的部分,APN,电话号码本
		调试一些线路报警的算法

2005年9月5日：
		线路偏离的算法正确验证
		增加如果ACC开启状态下，持续12个小时，则会将MCU重新启动一次。以恢复设备状态
		此功能是为了防止在实际接线中，施工时候直接将ACC接在常电上的一个自恢复保护措施


2005年9月6日：
		完善线路偏离报警的算法（但线路数据通过中心并没完成设置）
		通过手柄端口设置电话号码完成
		通过手柄端口参数设置完善（增加了APN的设置，按距离上传的距离值设置，）
2005年9月8日：

		通过手柄端口设置显示终端的短信预置完成

2005年9月14日：
		根据要求修改上报数据的补偿方式（短消息）
		1，被劫报警的上报方式的修改
2005年9月22日：实际跑车进行线路报警的完善

2005年10月9日:
	1,针对电话方面的恢复处理
	2,针对疲劳报警或者预警的误报处理

	根据在现场实际应用中发现的情况,以后需要完善的几点:
	A;因为手柄和显示终端是半双工通讯,并且均为模拟串口的方式,在需要将232的IC改换为3232的同时,
		为了增加通讯的可靠性,则需要针对部分指令的响应处理.
2005年10月16日：
	1，针对模拟串口的发送和接收时候，需要停止某些中断

	2,增加对MSP430内部的两个信息存储器的利用。分别为128个字节，作为存储备份参数利用
	 	信息存储器A，备份扇区1和扇区2的前128字节的数据
	 	信息存储器B，备份扇区3和扇区4的前128字节的数据

	 	如果开机检测扇区1和扇区2，均是校验核不正确的扇区数据，则需要将信息存储器A中的前128个字节读出，并重新计算校验核
	 	存储在扇区1或者扇区2中。

	 	同样道理，信息存储器B是对应的扇区3和扇区4

	3，每次更新或者从新计算扇区1和2的时候需要及时的更新信息存储器A
		每次更新或者重新计算扇区3和4的时候需要及时的更新信息存储器B:


		暂时未完成测试

2005年10月17日：
OK    **1，为了减少串行FLASH的操作（包括串行FLASH中的SRAM的操作，则将以前存储在buffer1中的数据转移到外部的SRAM中）
		3，根据硬件PCB的改进，增加对外部串行FLASH的WP管脚的控制（预防对FLASH的操作损坏）
		4，针对场强信号的发送，改为每间隔一定时间发送场强信号，而不是在查询CSQ后才发送场强指令OK
		5，如果M22处于电话状态下，则需要每间隔一定时间查询CLCC指令	OK

2005年10月19日：
		1，增加进行测试外部SRAM的指令()完成测试OK
		2，增加主控主动挂机指令的接收手柄的响应，如果2秒内未接收到手柄的挂机指令响应，则重复发送
			重复5次，则丢弃此包（还未调试）
		3,将对手柄需要发送的的数据存储在外部SRAM中。改变原来向手柄发送指令随时发送的结构模式
			取消所有对buffer1的操作
		14，因为每次在发送数据和登陆TCP前，均有个验证中心号码和目标号码等参数的有效性，需要是对外部SRAM进行操作
			频繁的操作可能引起外部FLASH内容的丢失，则这里改为每30分钟进行所有的验证，如果严整正确，则设置
			一个标志，如果失败，则立刻进行数据的重装工作

2005年11月3日
		1，根据在MSP430内部的两个信息扇区，取消对信息扇区数据的校验核的计算，取代的是最后一个字节存储0XAA，
			只表示此信息扇区存储过数据。如果在检验外部FLASH的两个扇区的数据均不正确的情况下
			则需要将两个信息扇区的数据覆盖到外部的FLASH中

2005年11月8日：
		1，增加定位查询的可靠信，如果在接收到一个短消息（属于定位查询），并且，设备工作在
			GSM的工作方式下，并且目标号码属于无效的状态，则需要暂时提取这个短消息号码中的
			号码，然后作为暂时的目标号码存储起来，在发送定位数据的时候则以此目标号码为准。

2005年11月16日：
		1，取消每次系统程序上电后，查询记录仪驾驶员代码的操作

2005年11月19日
		1，精简程序，减少AD转换判断部分，增加对FLASH的可靠操作，尤其是对电话号码存储部分的操作


2005年11月21日
		1，完善参数设置方面的尤其是APN方面的处理，增加对buffer2的处理机制

2005年11月23日
		1，针对山东市场的一体机，默认电话始终处于开启状态

2005年12月1日：
		1，针对在测试过程中，出现的模拟串口失效的问题，则增加了个几个恢复（重新对TA定时器进行初始化）
2005年12月3日：
		1，针对如果外部的晶振如果暂时的失效，则会严重影响三个模拟串口的接收，则采用系统程序重新启来恢复
2005年12月4日：
		1，针对外部晶振XT2的暂时失效，则增加对外部晶振XT2的失效中断处理程序



2005年12月8日：
		在前面版本的一体机的程序结构下，需要进行部分程序结构的修改，使得符合一体机的概念，就是将GPS
		工作部分和记录仪的工作部分能够在任何一个还没工作正常下，双方能互相不干扰和影响，即GSM模块在
		进行初始化或者正在初始化的过程中，并不影响记录仪的工作和显示屏的工作


2005年12月27日：
		根据以前存在接收记录仪数据的方式有可能判断不到正确的数据包，现在将接收记录仪数据的方式进行改进
\********************************************************/


/********************************************************\
*	文件名：  GSM_430.C
*	创建时间：2004年12月7日
*	创建人：        
*	版本号：  1.00
*	功能：	  系统主程序框架
*	文件属性：公共文件
*	修改历史：（每条详述）
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
*	函数名：main.c
	作用域：本地文件调用
	        作为主框架程序
*	功能：	项目主函数，系统的循环运行
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void main(void)
{
    //看门狗定时器WDT的设置：
	WDTCTL=WDTPW+WDTHOLD;   //关闭看门狗WDT  ,WDTPW看门狗的口令

    //时钟模块的设置：
    BCSCTL2|=SELS+SELM1;    //SELS表示用XT2CLK作为SMCLK的时钟源
                            //SELM1表示用XT2CLK作为MCLK时钟源
                            //ACLK---辅助时钟，用于软件选择分频因子,也可用软件选做各外围模块的时钟信号
                            //MCLK---主时钟，主要用于CPU和系统,也可软件选择分频因子
                            //SMCLK--子时钟，由软件选择用于外围模块

//	BCSCTL2|=DIVS_3;		//选择SMCLK时钟的分频因子为8
//	BCSCTL2|=DIVM_3;		//选择MCLK时钟的分频因子为8
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
  //定时器TA的设置
    TACTL|=TASSEL1;         //输入时钟选择MCLK
    TACTL|=TACLR;           //定时器A清除位CLR清零
    TACTL|=MC1;             //定时器A选择连续增记数模式
//    TACTL|=TAIE;          //定时器A溢出中断允许

    TAR=0;                  //定时器A的值清零
    TACCTL0|=OUT;           //TXD2 Idle as Mark
	TACCTL1|=OUT;           //TXD3 Idle as Mark
	TACCTL2|=OUT;           //TXD4 Idle as Mark

  //定时器TB的设置
    TBCTL|=TBSSEL1;         //输入时钟源选择MCLK
    TBCTL|=TBCLR;           //定时器B清除位CLR清零
    TBCTL|=MC1;             //模式为连续增记数
//    TBCTL|=TBIE;          //定时器B溢出中断允许
    TBR=0;                  //寄存器B的值清零
    TBCCTL0|=CCIE;
	/*
	=============================================================================
	                             波特率的计算方式
	          8000 000 /19200=416.6667D(十进制)=01A0H+�00.67D(11010101B/8=0XD5/8)
	          8000 000/9600 =833.333D(十进制)=0341H+0.333D(1010100B/8=0X54/8)
	          8000 000/4800 =1666.66666D(十进制)=0682H+0.67(11010101B/8=0XD5/8)
	=============================================================================
	*/
  	//串口0设置(接收是中断形式,发送不产生中断)
  	//将与G20通讯的波特率设置为4800
    UTCTL0|=SSEL1;          //发送选择系统时钟
    URCTL0|=URXEIE;         //所有字符都可使的URXIFG置位
	//波特率9600
    UBR10=0x03;
    UBR00=0x41;
    UMCTL0=0x54;
   	//波特率4800
   	/*
    UBR10=0x06;
    UBR00=0x82;
    UMCTL0=0xD5;
	*/
    UCTL0|=CHAR;            //选择8位字符发送接收
    UCTL0&=(~SWRST);

  	//串口1设置(接收是中断形式，发送不产生中断)
 	 //波特率9600
    UTCTL1|=SSEL1;          //发送选择系统时钟
    URCTL1|=URXEIE;         //所有字符都可使的URXIFG置位

    UBR11=0x03;
    UBR01=0x41;
    UMCTL1=0x54;

    UCTL1|=CHAR;            //选择8位字符发送接收
    UCTL1&=(~SWRST);

    //中断允许1
    IE1|=URXIE0;            //URSART0接受中断允许
//    IE1|=UTXE0;           //URSART0发送中断允许
	IE1|=OFIE;				//振荡器故障中断允许
    //中断允许2
    IE2|=URXIE1;            //USART1接收中断允许
//    IE2|=UTXE1;           //USART1发送中断允许



    //模块允许1
    ME1|=URXE0;         	//USART0接收允许（UART模式）
    ME1|=UTXE0;         	//USART0发送允许（UART模式）
    //模块允许2
    ME2|=URXE1;         	//USART1接收允许（UART模式）
    ME2|=UTXE1;         	//USART1发送允许（UART模式）



	/*
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	设置定义三个模拟串口的相关管脚定义
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	*/
	P3SEL=0;
    P3SEL|=BIT4+BIT5+BIT6+BIT7; 	//P3SEL--P3端口选择寄存器，选择UART0，UART1功能

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
	设置定义控制部分相关管脚的定义
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	*/

	P6DIR|=POWER_ON_OFF;
	P6OUT&=~POWER_ON_OFF;						//设置POWER_ON_OFF为低电平

	P1DIR|=MCU_STATUS;

	P6DIR|=STOP_Q2+STOP_Q1;						//设置为输出模式
	P6OUT&=~STOP_Q2;							//设置STOP_Q2为低电平
	P6OUT&=~STOP_Q1;							//设置STOP_Q1为低电平

	P6SEL |= BIT1;								//将A1设置为外围模块功能，即ADC功能
	P6DIR|=WDI;									//设置WDI为输出模式
	P6OUT&=~WDI;								//外部硬件看门狗的设置

	P6DIR|=SEL_MIC;
	P6OUT&=~SEL_MIC;

	Init_OperFlash();
	/*
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	      关于串行存储器AT45DB041B的管脚定义
	      器件默认为工作模式3，则为SCK的上升沿发送和接收数据
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	*/
	//CS,RST,SCK,SI,WP设置为输出模式
    P2DIR|=SPI45DB041_CS+SPI45DB041_RST+SPI45DB041_SCK+SPI45DB041_SI;
    P2OUT|=SPI45DB041_CS;				//置SPI45DB041_CS为高
    P2OUT|=SPI45DB041_RST;        		//置SPI45DB041_RST为高
    P2OUT&=~SPI45DB041_SCK;      		//置SPI45DB041_SCK为低
	P6DIR|=SPI45DB041_WP;
	P6OUT&=~SPI45DB041_WP;				//置SPI45DB041_WP为低
	/*
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	设置定义P6.1,P6.2,P6.3管脚为外围模块功能，即ADC转换功能定义
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	*/
    _EINT();							//开中断
 	//WDTCTL=WDTPW;						//启动看门狗
    TAUARTCTL|=TAUARTIE;        		//比较器A的中断允许
    RX2_Ready();
    RX3_Ready();
    RX4_Ready();
    gTimer=0;
	while(gTimer<2000) {Clear_Exter_WatchDog();}
	Send_COM1_String((unsigned char *)MAINSYS,sizeof(MAINSYS));
	//初始化模块各参数
	gInternal_Flag=0;
	gGeneral_Flag=0;
	gOther_Flag=0;
   	Init_MAIN_Module();
    gGPS_No_Output_Timer=0;
    gNO_Sram_Data_Timer=0;
    Clear_Exter_WatchDog();
    while(1)
    {
    	//调试部分的串口显示程序
    	if( (Debug_Timer>20000)&&((gGeneral_Flag&TCP_PHONE_F_1)==0) )
    	{
			RX2_Ready();RX3_Ready();RX4_Ready();
    		//调试串口1写外部SRAM和读外部SRMA的程序检验部分
    		Debug_Timer=0;
			//显示M22的工作方式状态
			Send_COM1_Byte(ASCII(gM22_Status/10));
			Send_COM1_Byte(ASCII(gM22_Status%10));
			Send_COM1_Byte('=');
			//显示系统正在周期发送数据的类型
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

		//主控系统中的两个模块处于需要断电状态的循环程序
		if( ((gGeneral_Flag&OFF_M22_F_1)||(gGeneral_Flag&OFF_GPS_F_1))
			&& (gPhone_Status==READY)	)
		{
			//处理两个模块的掉电子程序
			Do_OFF_Power();
			Clear_Exter_WatchDog();
			gGPS_No_Output_Timer=0;
			gNO_Sram_Data_Timer=0;
			gM22_GSM_ECHO_Timer=0;
		}
		//主控系统处于正常的运行循环状态
		else
		{
			//表示是重新初始化M22
			if(   (gGeneral_Flag&RESET_GSM_ON_F_1)
				&&(gPhone_Status==READY)	 )
			{
				gM22_GSM_ECHO_Timer=0;
				gGPS_No_Output_Timer=0;
				gNO_Sram_Data_Timer=0;
				Do_Reset_GSM_On();
			}
			//表示从新初始化GPS模块
			//主程序循环
			else
			{
				//如果M22长时间无返回数据
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
*	函数名：TimerB_Interrupt (中断子程序)
	作用域：本地文件调用
*	功能： 定时器TB的中断，产生时间记数
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
interrupt [TIMERB0_VECTOR] void TimerB_Interrupt(void)
{
	TBCCR0+=8000;       		//定时1ms,外部时钟为8M的时钟。
	gTimer++;					//1ms增加一次记数
	gDelay_Com_Timer++;
	gTimer_GSM_AT++;			//处理AT指令之间的间隔时间
	Debug_Timer++;				//作为调试显示数据的间隔
	gGPS_Receive_Over++;		//作为判断GPS多长时间未接收数据字节了
	gHandle_Receive_Over++; 	//此变量作为接收手柄数据百的时间间隔记时，每接收一个手柄数据字节，则此变量置0
	gDisp_Receive_Over++;
	gTest_Alarm_Timer++;		//此变量作为如果有被劫开关按下的延时计时
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
	if(Z_Timer>1000)					//1秒钟的时间计时
	{
		Z_Timer=0;
		gTimer_CSQ++;					//作为间隔查询CSQ的时间记时
		gTimer_Area_Test++;				//作为检测区域的时间间隔
		gCircle_Timer++;				//作为记时需要发送gCircle_Buffer缓冲区数据的时间变量
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
		//每间隔12个小时则需要MCU的看门狗关闭一次，以使得系统程序重头开始执行
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
		//如果电话业务超过30分钟，则主动挂断电话业务
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
		//CPU状态指示灯
		if(P1IN&MCU_STATUS)	P1OUT&=~MCU_STATUS;
		else				P1OUT|=MCU_STATUS;

		//作为判断手柄是否被摘除的时间计时，1秒钟的计时
		gHandle_OnLine_Timer++;
		if( gHandle_OnLine_Timer>HANDLE_OUT_TIME )
		{
			gHandle_OnLine_Timer=0;
			gStatus1_Flag &=~ HANDLE_ON_0_F_1;
			gStatus1_Flag &=~ HANDLE_ON_1_F_1;
		}
		//作为判断显示屏是否关机的时间计时，1秒钟的计时
		gDisp_OnLine_Timer++;
		if(gDisp_OnLine_Timer>DISP_OUT_TIME)
		{
			gDisp_OnLine_Timer=0;
			gInternal_Flag&=~DISP_ON_F_1;
			gStatus1_Flag&=~DISPLAY_ON_F_1;
		}
		//如果车辆登陆数据发送后，一定时间仍然未成功收到下行的登陆响应时，则需要进行间隔的定时间进行登陆数据的发送，直到接收到下行的登陆响应为止
		//如果成功收到下行的登陆响应标志，则需要设置标志LAND_SUCCESS_F_1
		if( ((gGeneral_Flag&LAND_TCP_SUCCESS_F_1)==0)
			&&(gM22_Status==TCP_ONLINE_WORK)	)
		{
			gLand_Interval_Timer++;
		}
		else	gLand_Interval_Timer=0;

		//TCP在线，发送握手数据FF0D的时间计时
		if(gM22_Status==TCP_ONLINE_WORK)	gTCP_FF0D_Send_Timer++;
		else if(gM22_Status==GSM_WORK)		gTCP_FF0D_Send_Timer=0;
		//判断未接收到TCP数据包的时间计时
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
		//判断是否允许重新登陆TCP的操作的标志
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
*	函数名：XT2振动器失效的中断处理程序
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
interrupt[NMI_VECTOR] void XT2_INVAILD (void)
{
	IFG1&=~OFIFG;
	IE1|=OFIE;
	/*
	if(IFG1&OFIFG)
	{
		//1，选择DCO为MCLK
		//BCSCTL2 &=~	SELM0;BCSCTL2 &=~ SELM1;
		//2，用户软件，振荡器失效处理
		//3，复位中断标志
		IFG1&=~OFIFG;
		//4，判断是否振荡器仍然失效
		if( (IFG1&OFIFG)==0 )
		{
			//重新选择失效的XT振荡器
			BCSCTL2|=SELS+SELM1;
		}
	}
	else if(IFG1&ACCVIE)	IFG1&=~ACCVIE;
	else if(IFG1&NMIIE)		IFG1&=~NMIIE;
	//振荡器故障中断允许
	IE1|=OFIE;
	*/
}