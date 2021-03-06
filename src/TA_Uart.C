
/********************************************************\
*	文件名：  TA_Uart.C
*	创建时间：2004年12月7日
*	创建人：  
*	版本号：  1.00
*	功能：	  根据MSP430的特点，这部分实现MSP430三个模拟串口的功能
			  包括三个模拟串口的发送和接收
*	文件属性：公共文件
*	修改历史：（每条详述）

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
*	函数名：TX2_Byte
	作用域：本文件调用
*	功能：  模拟串口2发送一个字节子程序
*	参数：  SBUFOUT2
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void TX2_Byte (void)
{
	RXTXData2=(int)SBUFOUT2;
	BitCnt2=0x0A;    					// Load Bit counter, 8data + ST/SP，装10个BIT位
	RTI2 |= ~SEND_FLAG;					//清除相应的发送标志位
	TACCR0 = TAR;       				// Current state of TA counter，清空记数器
	TACCR0 += Bitime0;        			// Some time till first bit
	RXTXData2 |= 0x100;        			// Add mark stop bit to RXTXData 1为停止位
	RXTXData2 = RXTXData2 << 1;			// Add space start bit  0为开始位（位左移1位）
	TACCTL0 = OUTMOD0+CCIE; 			// TXD先设置1，中断允许
	//while ( TACCTL0 & CCIE ); 		// Wait for TX completion，等待所有的位都发送完成
	while ( (~RTI2)&SEND_FLAG );
	gDelay_Com_Timer=0;
	while(gDelay_Com_Timer<SEND_DELAY_TIME);
}

/********************************************************\
*	函数名：RX2_Ready
	作用域：外部文件调用
*	功能：  模拟串口2准备接收数据子程序（捕获模式0）
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void RX2_Ready (void)
{
	TA_Init();
	RTI2 &= ~RECEIVE_FLAG;
	BitCnt2=0x08;                     	//Load Bit counter，装8个BIT位
	TACCTL0=SCS+OUTMOD0+CM1+CAP+CCIE; 	//同步捕获Sync
					                 	//CCIS0=0,在捕获模式中，定义捕获事件的输入源为CCI0A
	                               		//OUTMOD0=1，输出模式为置位
	                               		//CM1=1，设置捕获模式1，下降沿捕获
                                   		//CAP=1，设置为捕获模式
                                  		//CCIE=1，中断允许
}										//SCS同步捕获，（CCIS0=1，CCIS1=0）定义提供捕获事件的输入源

/********************************************************\
*	函数名：TX3_Byte
	作用域：本文件调用
*	功能：  模拟串口2发送一个字节子程序
*	参数：  SBUFOUT3
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void TX3_Byte (void)
{
	TACCTL0 &=~ CCIE;
	TBCCTL0 &=~ CCIE;
	IE2&=~URXIE1;
	RXTXData3=(int)SBUFOUT3;
	BitCnt3 = 0xA;            			// Load Bit counter, 8data + ST/SP，装10个BIT位
	RTI3 |= ~SEND_FLAG;					//清除相应的发送标志位
	TACCR1 = TAR;             			// Current state of TA counter，清空记数器
	TACCR1 += Bitime1;          		// Some time till first bit
	RXTXData3 |= 0x100;         		// Add mark stop bit to RXTXData 1为停止位
	RXTXData3 = RXTXData3 << 1;  		// Add space start bit    0为开始位（位左移1位）
	TACCTL1 = OUTMOD0+CCIE;      		// TXD先设置1，中断允许,设置CCIFG能进入中断
	//while (TACCTL1 |= CCIE);      	// Wait for TX completion，等待所有的位都发送完成
	while( (~RTI3)&SEND_FLAG  );
	gDelay_Com_Timer=0;
	while(gDelay_Com_Timer<SEND_DELAY_TIME);
}
/********************************************************\
*	函数名：RX3_Ready
	作用域：外部文件调用
*	功能：  模拟串口3准备接收数据子程序（捕获模式1）
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void RX3_Ready (void)
{
	TA_Init();
	RTI3 &= ~RECEIVE_FLAG;
	BitCnt3=0x8;                    	//Load Bit counter，装8个BIT位
	TACCTL1=SCS+OUTMOD0+CM1+CAP+CCIE;  	//同步捕获Sync, �(
	                                    //CCIS0=0,在捕获模式中，定义捕获事件的输入源为CCI0A
	                                    //OUTMOD0=1，输出模式为置位
	                                    //CM1=1，设置捕获模式1，下降沿捕获
	                                    //CAP=1，设置为捕获模式
	                                 	//CCIE=1，中断允许
										//SCS同步捕获，（CCIS0=1，CCIS1=0）定义提供捕获事件的输入源
	gCommon_Flag|=ALLOW_R232_F_1;
}



/********************************************************\
*	函数名：TX4_Byte
	作用域：本文件调用
*	功能：  模拟串口4发送一个字节子程序
	(根据这个串口是针对行使记录仪发送，则需要在发送数据前，需要有增加奇校验)

*	参数：  SBUFOUT4
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
补充说明：
Odd Parity 奇 校 验
数 据 传 输 正 确 性 的 一 种 校 验 方 法。
在 数 据 传 输 前 附 加 一 位 奇 校 验 位，
用 来 表 示 传 输 的 数 据 中"1"的 个 数 是 奇 数 还 是 偶 数，
为 奇 数 时， 校 验 位 置 为"0"， 否 则 置 为"1"，
用 以 保 持 数 据 的 奇 偶 性 不 变。 例 如，
需 要 传 输"11001110"， 数 据 中 含5个"1"，
所 以 其 奇 校 验 位 为"0"， 同 时 把"110011100"传 输 给 接 收 方，
接 收 方 收 到 数 据 后 再 一 次 计 算 奇 偶 性，"110011100"中 仍 然 含 有5个"1"，
所 以 接 收 方 计 算 出 的 奇 校 验 位 还 是"0"， 与 发 送 方 一 致，
表 示 在 此 次 传 输 过 程 中 未 发 生 错 误。

\********************************************************/
void TX4_Byte (void)
{
	TACCTL0 &=~ CCIE;
	TBCCTL0 &=~ CCIE;
	IE2&=~URXIE1;
	RXTXData4=(int)SBUFOUT4;
	BitCnt4 = 0xA;         		    	// Load Bit counter, 8data + ST/SP，装11个BIT位,(还增加一个奇校验位)
	RTI4 |= ~SEND_FLAG;			    	// 清除相应的发送标志位
	TACCR2 = TAR;           	    	// Current state of TA counter，清空记数器
	TACCR2 += Bitime2;        	    	// Some time till first bit
	RXTXData4 |= 0x100;         	    // Add mark stop bit to RXTXData 1为停止位
	RXTXData4 = RXTXData4 << 1;   	    // Add space start bit    0为开始位（位左移1位）
	TACCTL2 = OUTMOD0+CCIE;       	    // TXD先设置1，中断允许,设置CCIFG能进入中断
	//while (TACCTL2 |= CCIE);          // Wait for TX completion，等待所有的位都发送完成
	while ( (~RTI4)&SEND_FLAG );		//循环等待发送完成（在中断子程序中，发送完成则会设置相应的发送完成标志位）
	gDelay_Com_Timer=0;
	while(gDelay_Com_Timer<SEND_DELAY_TIME);
}
/*
void TX4_Byte (void)
{
	RXTXData4=(int)SBUFOUT4;
	BitCnt4 = 0xB;         		    	// Load Bit counter, 8data + ST/SP，装11个BIT位,(还增加一个奇校验位)
	RTI4 |= ~SEND_FLAG;			    	// 清除相应的发送标志位
	TACCR2 = TAR;           	    	// Current state of TA counter，清空记数器
	TACCR2 += Bitime2;        	    	// Some time till first bit
	RXTXData4 |= 0x200;         	    // Add mark stop bit to RXTXData 1为停止位
	//处理增加校验位的部分：
	//1，先移动一位，将校验位默认为0

	//2，计算校验位，如果数据的1的个数为偶数，则将校验位设置为1
	for(i=0;i<8;i++)
	{
		if( (SBUFOUT4&0x01)==0x01 )	j=j^0x01;
		SBUFOUT4 = SBUFOUT4 >> 1;
	}
	//3，修正校验位
	if(j==1)	RXTXData4 |=0x100;
	//4，移动一位，设置开始位
	RXTXData4 = RXTXData4 << 1;   	    // Add space start bit    0为开始位（位左移1位）
	TACCTL2 = OUTMOD0+CCIE;       	    // TXD先设置1，中断允许,设置CCIFG能进入中断
	//while (TACCTL2 |= CCIE);          // Wait for TX completion，等待所有的位都发送完成
	while ( (~RTI4)&SEND_FLAG );		//循环等待发送完成（在中断子程序中，发送完成则会设置相应的发送完成标志位）
	gDelay_Com_Timer=0;
	while(gDelay_Com_Timer<SEND_DELAY_TIME);
}
*/
/********************************************************\
*	函数名：RX4_Ready
	作用域：外部文件调用
*	功能：  模拟串口4准备接收数据子程序（捕获模式1）
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void RX4_Ready (void)
{
	TA_Init();
	RTI4 &= ~RECEIVE_FLAG;
	BitCnt4=0x08;                    	//Load Bit counter，装8个BIT位
	TACCTL2=SCS+OUTMOD0+CM1+CAP+CCIE;  	//同步捕获Sync,
	                                   	//CCIS0=0,在捕获模式中，定义捕获事件的输入源为CCI0A
	                                	//OUTMOD0=1，输出模式为置位
	                               		//CM1=1，设置捕获模式1，下降沿捕获
	                               		//CAP=1，设置为捕获模式
	                              		//CCIE=1，中断允许
										//SCS同步捕获，（CCIS0=1，CCIS1=0）定义提供捕获事件的输入源
	gCommon_Flag|=ALLOW_R232_F_1;
}

/********************************************************\
*	函数名：OFF_RXTXCOM2
	作用域：此项目文件未调用
*	功能：  关闭模拟串口2发送和接收允许
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
void OFF_RXTXCOM2(void)
{
	TACCTL0 &= ~ CCIE;
}
*/
/********************************************************\
*	函数名：OFF_RXTXCOM3
	作用域：此项目文件未调用
*	功能：  关闭模拟串口3发送和接收允许
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
void OFF_RXTXCOM3(void)
{
	TACCTL1 &= ~ CCIE;
}
*/
/********************************************************\
*	函数名：OFF_RXTXCOM4
	作用域：此项目文件未调用
*	功能：  关闭模拟串口4发送和接收允许
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
void OFF_RXTXCOM4(void)
{
	TACCTL2 &= ~ CCIE;
}
*/
/********************************************************\
*	函数名：Send_COM2_Byte
	作用域：外部文件调用
*	功能：  通过模拟串口2发送一个字节
*	参数：  Input
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Send_COM2_Byte(unsigned char Input)
{
	SBUFOUT2=Input;
	TX2_Byte();
}

/********************************************************\
*	函数名：Send_COM2_String
	作用域：外部文件调用
*	功能：  通过模拟串口2发送一个字符串
*	参数：  *Point 字符串指针
			 Count 字符串长度
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
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
*	函数名：Send_COM3_Byte
	作用域：外部文件调用
*	功能：  通过模拟串口3发送一个字节
*	参数：  Input
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Send_COM3_Byte(unsigned char Input)
{
	SBUFOUT3=Input;
	TX3_Byte();
}
/********************************************************\
*	函数名：Send_COM3_String
	作用域：外部文件调用
*	功能：  通过模拟串口3发送一个字符串
*	参数：  *Point 字符串指针
			 Count 字符串长度
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
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
*	函数名：Send_COM4_Byte
	作用域：外部文件调用
*	功能：  通过模拟串口4发送一个字节
*	参数：  Input
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Send_COM4_Byte(unsigned char Input)
{
	SBUFOUT4=Input;
	TX4_Byte();
}
/********************************************************\
*	函数名：Send_COM4_String
	作用域：外部文件调用
*	功能：  通过模拟串口4发送一个字符串
*	参数：  *Point 字符串指针
			 Count 字符串长度
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
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
*	函数名：Timer_A0（中断子程序）
	作用域：外部文件调用
*	功能：  定时器TA的中断Timer A CC0 (用做模拟串口2的发送和接收中断处理)
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
interrupt[TIMERA0_VECTOR] void Timer_A0 (void)
{
	TACCR0 += Bitime0;         				//定时到，则CCR0的值增加，以继续TA记时
        //RX，接收一个字节的程序处理
  	if ( (~RTI2)&RECEIVE_FLAG )     	    //判断是否提供捕获事件的捕获源有捕获事件发生
  	{
    	if( TACCTL0 & CAP )     			//判断是否还处于捕获模式状态
    										//如果为捕获模式（表示接收到了开始位）
    	{
      		TACCTL0 &= ~ CAP;     			//是捕获模式 则将捕获模式变成比较模式，准备开始接收数据位                                                              		//则将捕获模式更改为比较模式
      		TACCR0 += Bitime0_5;			//增加计数的值,则共1.5个数据位的时间长度
    	}
    	else	                			//判断处于比较模式状态，则继续接收未接收完的数据位
    										//为比较模式，则继续接收数据位
    	{
      		RXTXData2 = RXTXData2 >> 1; 	//数据右移1位（先接收数据低位）
      		if (TACCTL0 & SCCI)       		// 从锁存器中取出输入信号，如果为1，则继续，否则为0
      		{
        		RXTXData2 |= 0x80;       	//接收信号为1
      		}
      		BitCnt2 -- ;
      		if ( BitCnt2 == 0 )         	//判断数据是否接收完？
      		{
        		//TACCTL0 &= ~CCIE;       	//所有的位接收完毕，则禁止中断
        		SBUFIN2=(char)RXTXData2;
        		TAUARTCTL|=TAUARTIFG;		//进入比较器A的中断向量的标志位，则进入比较器A的中断
        		RTI2|=RECEIVE_FLAG;			//设置模拟串口2的接收标志位
      		}
    	}
	}
	// TX，发送一个字节的程序处理
  	else
  	{
    	if ( BitCnt2 == 0 )           		//判断是否所有数据发送完成
    	{
    		RTI2 |= SEND_FLAG;
          	//TACCTL0 &= ~ CCIE;          	//所有的位都发送完成，则禁止中断发
        }
    	else                        		//位未发送完，则继续发送
    	{
      		TACCTL0|=OUTMOD2+OUTMOD0;   	// TX Space置低
      		if (RXTXData2 & 0x01)        	//如果发送位为1
      		{
        		TACCTL0 &= ~ OUTMOD2;     	// TX Mark 置高
      		}
      		RXTXData2 = RXTXData2 >> 1;
      		BitCnt2 --;
		}
  	}
}

/*
定时器TA的中断
Timer A CC1-2
*/
/********************************************************\
*	函数名：Timer_A1（中断子程序）
	作用域：外部文件调用
*	功能：  定时器TA的中断Timer A CC1-2 (用做模拟串口3，4的发送和接收中断处理，
			两个模拟串口发送数据，接收数据的处理均在这个中断里完成)
*	参数：  无
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
interrupt[TIMERA1_VECTOR] void Timer_A1 (void)
{
	if(TACCTL1&CCIFG)						//TACCIFG1设置位置位了,访问TAIV，则相应的CCIFG复位
   	{
		TACCTL1&=0xFFFE;
		TACCR1 += Bitime1;         			// 定时到，则TACCR1的值增加，以继续TA记时
	 	if ( (~RTI3)&RECEIVE_FLAG )    		//判断是否提供捕获事件的捕获源有捕获事件发生？
	 	{
	   		if( TACCTL1 & CAP )     		// 判断是否还处于捕获模式状态	   																									//如果为捕获模式（表示接收到了开始位）
	   		{
	     		TACCTL1 &= ~ CAP;     		//是捕获模式 则将捕获模式变成比较模式，准备开始接收数据位                                                             		//则将捕获模式更改为比较模式
	     		TACCR1 += Bitime1_5;		//增加计数的值,则共1.5个数据位的时间长度
	     		//停止模拟串口2的中断
    			TACCTL0 &=~ CCIE;
    			TBCCTL0 &=~ CCIE;
    			IE2&=~URXIE1;
	   		}
	   		else	                		//判断处于比较模式状态，则继续接收未接收完的数据位																								//为比较模式，则继续接收数据位
	   		{
	     		RXTXData3 = RXTXData3 >> 1; //数据右移1位（先接收数据低位）
	     		if (TACCTL1 & SCCI)       	// 从锁存器中取出输入信号，如果为1，则继续，否则为0
	     		{
	       			RXTXData3 |= 0x80;  	//接收信号为1
	     		}
	     		BitCnt3 --;
	     		if ( BitCnt3 == 0)			//判断数据是否接收完？
	     		{
	       			SBUFIN3 = (char)RXTXData3;
	       			RTI3 |= RECEIVE_FLAG;
	       			//处理接收手柄的数据的部分
					gHandle_Buffer[gHandle_Buffer_Point]=SBUFIN3;
					gHandle_Buffer_Point++;
					if(gHandle_Buffer_Point>=sizeof(gHandle_Buffer)) gHandle_Buffer_Point=0;
					gHandle_Receive_Over=0;
					RTI3 &= ~RECEIVE_FLAG;
					BitCnt3=0x8;            //Load Bit counter，装8个BIT位
					TACCTL1=SCS+OUTMOD0+CM1+CAP+CCIE;
					TACCTL0 |= CCIE;
					TBCCTL0 |= CCIE;
					IE2|=URXIE1;
	     		}
	   		}
	 	}
	 	else                   				//为发送中断
	 	{
	    	if ( BitCnt3 == 0)           	//判断是否所有数据发送完成
	    	{
	    		RTI3 |= SEND_FLAG;			//设置相应的发送成功的标志位
				TACCTL1 &= ~ CCIE;
				TACCTL0 |= CCIE;
				TBCCTL0 |= CCIE;
				IE2|=URXIE1;
			}
	    	else                        	//位未发送完，则继续发送
	    	{
	      		TACCTL1|=OUTMOD2+OUTMOD0; 	// TX Space置低
	      		if (RXTXData3 & 0x01)      	//如果发送位为1
	      		{
	        		TACCTL1 &= ~ OUTMOD2;	// TX Mark 置高
	      		}
	      		RXTXData3= RXTXData3 >> 1;
	      		BitCnt3 --;
	    	}
	 	}
	}

   	if(TACCTL2&CCIFG)						//TACCIFG2设置位置位了
   	{
   	 	TACCTL2&=0xFFFE;
	 	TACCR2 += Bitime2;         			// 定时到，则TACCR2的值增加，以继续TA记时
	 	if ((~RTI4)&RECEIVE_FLAG)     	 	//判断是否提供捕获事件的捕获源有捕获事件发生？
	 	{
	   		if( TACCTL2 & CAP )     		// 判断是否还处于捕获模式状态	   																									//如果为捕获模式（表示接收到了开始位）
	   		{
		     	TACCTL2 &= ~ CAP;     		//是捕获模式 则将捕获模式变成比较模式，准备开始接收数据位                                                             		//则将捕获模式更改为比较模式
		     	TACCR2 += Bitime2_5;		//增加计数的值,则共1.5个数据位的时间长度
	     		//停止模拟串口2和TB的中断
				TACCTL0 &=~ CCIE;
				TBCCTL0 &=~ CCIE;
				IE2&=~URXIE1;
	   		}
	   		else	                		//判断处于比较模式状态，则继续接收未接收完的数据位 																									//为比较模式，则继续接收数据位
	   		{
	     		RXTXData4 = RXTXData4 >> 1; //数据右移1位（先接收数据低位）
	     		if (TACCTL2 & SCCI)       	// 从锁存器中取出输入信号，如果为1，则继续，否则为0
	     		{
	      			RXTXData4 |= 0x80;      //接收信号为1(接收9个bit，因为还有一个奇校验位)
	     		}
	     		BitCnt4 --;
	     		if ( BitCnt4 == 0)         	//判断数据是否接收完？
	     		{
	       			SBUFIN4 = (char)RXTXData4;
	       			RTI4 |= RECEIVE_FLAG;
					gDisp_Buffer_R[gDisp_Buffer_R_Point]=SBUFIN4;
					gDisp_Buffer_R_Point++;
					if(gDisp_Buffer_R_Point>=sizeof(gDisp_Buffer_R)) gDisp_Buffer_R_Point=0;
					gDisp_Receive_Over=0;
					RTI4 &= ~RECEIVE_FLAG;
					BitCnt4=0x08;                   //Load Bit counter，装8个BIT位
					TACCTL2=SCS+OUTMOD0+CM1+CAP+CCIE;
					TACCTL0 |= CCIE;
					TBCCTL0 |= CCIE;
					IE2|=URXIE1;
	     		}
	   		}
		}
		else                       			//为发送中断
		{
			if ( BitCnt4 == 0)				//判断是否所有数据发送完成？
			{
				RTI4 |= SEND_FLAG;
				TACCTL2 &= ~ CCIE;
				TACCTL0 |= CCIE;
				TBCCTL0 |= CCIE;
				IE2|=URXIE1;
			}
			else                     		//位未发送完，则继续发送
			{
				TACCTL2|=OUTMOD2+OUTMOD0; 	// TX Space置低
				if (RXTXData4 & 0x01)     	//如果发送位为1
				{
					TACCTL2 &= ~ OUTMOD2;  	// TX Mark 置高
				}
				RXTXData4= RXTXData4 >> 1;
				BitCnt4 --;
			}
		}
	}
}
