
/********************************************************\
*	文件名：  Sub_C.C
*	创建时间：2004年12月7日
*	创建人：  
*	版本号：  1.00
*	功能：	  包括一些公共能调用的子程序部分
*	文件属性：公共文件
*	修改历史：（每条详述）

\********************************************************/
#include <msp430x14x.h>
#include "Sub_C.h"
#include "General.h"
#include "Define_Bit.h"
#include "SPI45DB041.h"
#include "D_Buffer.h"
#include "Other_Define.h"
#include "M22_AT.h"
#include "Check_GSM.h"
#include "W_Protocol.h"
#include "Do_Other.h"
#include "TA_Uart.h"
#include "Handle_Protocol.h"
#include "Uart01.h"
#include "Do_SRAM.h"
#include "Msp430_Flash.h"
/*
#include "Do_GSM.h"
#include "Main_Init.h"
#include "Do_Reset.h"
#include "Do_Handle.h"
#include "Do_GPS.h"
#include "Do_Disp.h"
#include "Msp430_Flash.h"
*/
void Check_Flash(unsigned int Add_Sector);
void Write_Phone_Buffer2(unsigned char Add_Flag);
//#define Debug_GSM_COM1

/********************************************************\
*	函数名：ASCII
	作用域：外部文件调用
*	功能：  将一个数转换ASCII数值比如数值1，转换完后的结果为0x31
*	参数：  Input
*	返回值：char  一个ASCII字符的数值
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned char ASCII( unsigned char Input )
{
	if( Input>9 ) return( Input+0x37 );
	else return( Input+0x30 );
}
/********************************************************\
*	函数名：ASCIITOHEX
	作用域：外部文件调用
*	功能：  将ASCII字符转换为16进制的数比如0x31,0x32，转换完后返回的结果为12
*	参数：  Input_H，Input_L
*	返回值：char 一个16进制的数
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned char ASCIITOHEX( unsigned char Input_H,unsigned char Input_L )
{
    if( Input_H>0x39 ) Input_H=Input_H-0x37;
    else Input_H=Input_H-0x30;
    if( Input_L>0x39 ) Input_L=Input_L-0x37;
    else Input_L=Input_L-0x30;
    return( Input_H*16+Input_L );
}
/********************************************************\
*	函数名：Compare_String
	作用域：外部文件调用
*	功能：  比较两个字符串是否相同
*	参数：  *Source 需要比较的字符串的的头指针
            *Target 用来做比较标准的字符串头指针
             Count  需要比较的长

*	返回值：0	表示比较结果不相同
            1   表示比较结果相同

*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned char Compare_String(unsigned char *Source,unsigned char *Target,unsigned int Count )
{
    while(Count>0)
    {
        if((*Source)!=(*Target)) return(0);
        Source++;
        Target++;
        Count--;
    }
    return(1);
}
/********************************************************\
*	函数名：Reset_Variable
	作用域：外部文件调用
*	功能：  设置能进入复位G20的一些参数变量
*	参数：

*	返回值：

*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Reset_Variable(void)
{
	unsigned int i=0;
	gGeneral_Flag|=RESET_GSM_ON_F_1;
	gReset_M22_On_Setp=1;
	gGeneral_Flag&=~LAND_TCP_F_1;
	gM22_Status=GSM_WORK;
	gGeneral_Flag&=~LAND_TCP_SUCCESS_F_1;
	gOther_Flag&=~QUIT_TCP_F_1;
	gM22_GSM_ECHO_Timer=0;
	gLand_Interval_Timer=0;
	gTCP_FF0D_Send_Timer=0;
	gGPS_No_Output_Timer=0;
	gTimer_GSM_AT=0;
	gTimer_CSQ=0;
	gGPS_Receive_Over=0;

    gPhone_Status=READY;
	gStatus1_Flag&=~PHONE_ONLINE_F_1;
	gGeneral_Flag&=~DAILING_UP_F_1;
	gGeneral_Flag&=~MONITOR_ON_F_1;
	gGeneral_Flag&=~TCP_PHONE_F_1;
	gGeneral_Flag&=~RING_F_1;
	gGSM_Oper_Type=0;
	gSMS_IN_Step=0;
	gSMS_OUT_Step=0;
	gLand_GPRS_Step=0;
	gGPRS_Data_Step=0;
	gGSM_OTHER_Step=0;
	for(i=0;i<sizeof(gReceive0_GSM_Buffer);i++)
	{
		gReceive0_GSM_Buffer[i]=0;
	}
	gReceive0_GSM_Buffer_End=sizeof(gReceive0_GSM_Buffer)-1;
	gReceive0_GSM_Buffer_Point=0;
	gReceive0_GSM_Buffer_Move=0;

	gReceive1_Point=SRAM_RECORD_S;

	SRAM_Init();
	SRAM_Write(PHONE_AT_2+0,NULL_2);
	for(i=PHONE_AT_2+1;i<END_SRAM_2;i++)
	{
		SRAM_Write(i,0);
	}
}
/********************************************************\
*	函数名：Clear_Exter_WatchDog
	作用域：外部文件调用
*	功能：  喂狗操作，(外部硬件看门狗的操作)翻转WDI管脚的电平
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Clear_Exter_WatchDog(void)
{
	if( (gOther_Flag&RESET_PROGRAME_F_1)==0 )
	{
		if(P6IN&WDI)
		{
			P6OUT&=~WDI;		//如果是高电位，则翻转为低电位
		}
		else
		{
			P6OUT|=WDI;			//如果是低电位，则翻转为高电位
		}
	}
}
/********************************************************\
*	函数名：Wait_OK
	作用域：本文件调用
*	功能：  判断大多的AT指令是否返回OK
*	参数：  无
*	返回值: 无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Wait_OK(void)
{
	gTimer=0;
	while(gTimer<5000)
	{
		Clear_Exter_WatchDog();
		if(Check_GSM()!=0)
		{
		    if(Compare_String(gGeneral_Buffer,(unsigned char *)String_OK,sizeof(String_OK))!=0)
		    {
		    	gInternal_Flag|=M22_RETURN_OK_1;
		    	break;
		    }
		    else if(Compare_String(gGeneral_Buffer,(unsigned char *)String_ER,sizeof(String_ER))!=0)
		    {
		    	break;
		    }
		}
	}
}
/********************************************************\
*	函数名：Copy_String
	作用域：外部文件调用
*	功能：  移动
*	参数：  无
*	返回值: 无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Copy_String( unsigned char *PS,unsigned char *PD,unsigned int COUNT )
{
    unsigned int i;
    for( i=0;i<COUNT;i++ )
    {
        *(PD+i)=*(PS+i);
    }
}

/********************************************************\
*	函数名：Write_PartData_Buffer1()
	作用域：外部文件调用
*	功能：
*	参数：  无
*	返回值: 无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/********************************************************\
*	函数名：Write_PartData_Sram()
	作用域：外部文件调用
*	功能：
*	参数：  无
*	返回值: 无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Write_PartData_Sram(unsigned int S_Address)
{
	unsigned char nFlag;
	nFlag=gAlarm_Type;
	//4个字节的经度
	SRAM_Write(S_Address+1,gLongitude/0x1000000);
	SRAM_Write(S_Address+2,gLongitude%0x1000000/0x10000);
	SRAM_Write(S_Address+3,gLongitude%0x10000/0x100);
	SRAM_Write(S_Address+4,gLongitude%0x100);
	//4个字节的纬度
	SRAM_Write(S_Address+5,gLatitude/0x1000000);
	SRAM_Write(S_Address+6,gLatitude%0x1000000/0x10000);
	SRAM_Write(S_Address+7,gLatitude%0x10000/0x100);
	SRAM_Write(S_Address+8,gLatitude%0x100);
	//速度，角度
	SRAM_Write(S_Address+9,gSpeed);
	SRAM_Write(S_Address+10,gAzimuth);

	//Flag标志位(判断是否GPS有效果)
	if(gGeneral_Flag&GPS_VALID_F_1)	nFlag|=FLAG_GPSVAILD_1;
	else							nFlag&=~FLAG_GPSVAILD_1;
	SRAM_Write(S_Address+11,nFlag);
	//时间，年，月，日，时，分，秒
	SRAM_Write(S_Address+12,gYear);
	SRAM_Write(S_Address+13,gMonth);
	SRAM_Write(S_Address+14,gDate);
	SRAM_Write(S_Address+15,gHour);
	SRAM_Write(S_Address+16,gMinute);
	SRAM_Write(S_Address+17,gSecond);
}

/********************************************************\
*	函数名：Write_PartData_Buffer1()
	作用域：外部文件调用
*	功能：
*	参数：  无
*	返回值: 无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Page_R_Write_Check(unsigned int Page)
{
	unsigned int i=0,j=0;
	OperateSPIEnd();
	j=ReadByteFromBuffer2(0);
	for(i=1;i<254;i++)	j+=ReadNextFromBuffer2();
	OperateSPIEnd();
	WriteOneByteToBuffer2(254,j/256);
	WriteNextByteToBuffer2(j%256);
	OperateSPIEnd();
	//将重新计算过的buffer2数据写入扇区0
	P6OUT|=SPI45DB041_WP;
	WriteBuffer2ToPage(Page);
	P6OUT&=~SPI45DB041_WP;
	OperateSPIEnd();
}

/********************************************************\
*	函数名：Send_STOP_DealWay()
	作用域：外部文件调用
*	功能：
*	参数：  无
*	返回值: 无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Send_STOP_DealWay(unsigned char nCommand,unsigned char nACK)
{
	unsigned char VCommand=0;
	VCommand=nCommand;
	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,VCommand);
	SRAM_Write(ECHO_SMS_2+2,nACK);
	SRAM_Write(ECHO_SMS_2+3,28);
	SRAM_Write(ECHO_SMS_2+4,0);

	SRAM_Write(ECHO_SMS_2+5,gYear);
	SRAM_Write(ECHO_SMS_2+6,gMonth);
	SRAM_Write(ECHO_SMS_2+7,gDate);
	SRAM_Write(ECHO_SMS_2+8,gHour);
	SRAM_Write(ECHO_SMS_2+9,gMinute);
	SRAM_Write(ECHO_SMS_2+10,gSecond);
	//经度，纬度，速度，角度，报警标志，当前时间
	Write_PartData_Sram(ECHO_SMS_2+10);
	SRAM_Write(ECHO_SMS_2+10+18,0);
	SRAM_Write(ECHO_SMS_2+10+19,gStatus3_Flag);
	SRAM_Write(ECHO_SMS_2+10+20,gStatus2_Flag);
	SRAM_Write(ECHO_SMS_2+10+21,gStatus1_Flag);

	Judge_SMS_Way();
}

/********************************************************\
*	函数名：Disp_Handle_Steal_Alarm_Echo()
	作用域：本地文件调用
*	功能：	接收到来自显示终端主动改变密码的信令
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Disp_Handle_Steal_Alarm_Echo(void)
{
	//密码连续超过3次错误，则产生被盗报警
	//将被盗报警信息存储在Flash中
	gAlarm_Type=STEAL_ALARM;
	gSMS_ACK[ALARM_ACK]=0;
	OperateSPIEnd();
	ReadOnePageToBuffer2(FLASH_INFO2_ONE_2);
	OperateSPIEnd();
	WriteOneByteToBuffer2(ALARM_F_2,STEAL_ALARM);
	WriteNextByteToBuffer2( (char)(~STEAL_ALARM) );
	OperateSPIEnd();
	gCycle_Alarm=ALARM_CIRCLE_TIME;
	RFlash_Sector(FLASH_INFO2_ONE_2);
	gCycle_Send_Status=ALARM_DATA;
	gCircle_Timer=0;
	gSMS_ACK[ALARM_ACK]=0;
	Circle_Data_TCP(ALARM_UP,gSMS_ACK[ALARM_ACK]);
	gInternal_Flag|=SEND_CIRCLE_F_1;
	Judge_SMS_Way();
}
/********************************************************\
*	函数名：Load_Flash_Par_Buffer1()
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void  Load_Flash_Par_Buffer1(void)
{
	unsigned int i=0;
	unsigned char Check_Data=0;
	//车型，车类，车组，车号的处理
	OperateSPIEnd();
	ReadOnePageToBuffer2(FLASH_INFO1_ONE_2);
	OperateSPIEnd();
	gType=ReadByteFromBuffer2(VEH_TYPE_2);
	gKind=ReadNextFromBuffer2();
	gGroup=ReadNextFromBuffer2();
	gVID = ReadNextFromBuffer2();
	gVID<<=8;
	gVID += ReadNextFromBuffer2();
	OperateSPIEnd();

	//1,目标号码1的处理
	gGeneral_Buffer[TARGET_NUMBER1_2]=ReadByteFromBuffer2(TARGET_NUMBER1_F_2);	//读目标电话号码的长度
	Check_Data=0;
	if( (gGeneral_Buffer[TARGET_NUMBER1_2]>=1)&&(gGeneral_Buffer[TARGET_NUMBER1_2]<=16) )
	{
		gGeneral_Flag|=NUM1_VAILD_F_1;
		for(i=1;i<=gGeneral_Buffer[TARGET_NUMBER1_2];i++)						//不包括长度本身整个目标电话号码的长度
		{
			gGeneral_Buffer[i+TARGET_NUMBER1_2]=ReadNextFromBuffer2();			//目标电话号码的第一个字节
			Check_Data += gGeneral_Buffer[i+TARGET_NUMBER1_2];
		}
		gGeneral_Buffer[TARGET_NUMBER1_CHECK_2]=Check_Data;
		OperateSPIEnd();
		if( (gGeneral_Buffer[TARGET_NUMBER1_2+1]>'9')||(gGeneral_Buffer[TARGET_NUMBER1_2+1]<'0') )
		{
			gGeneral_Flag&=~NUM1_VAILD_F_1;
		}
	}
	else
	{
		gGeneral_Flag&=~NUM1_VAILD_F_1;
	}
	OperateSPIEnd();

	//2,TCP服务器地址的处理
	gGeneral_Buffer[GPRS_IP_ADDRESS1_2]=ReadByteFromBuffer2(GPRS_IP_ADDRESS1_F_2);
	if(	(gGeneral_Buffer[GPRS_IP_ADDRESS1_2]>6)&&(gGeneral_Buffer[GPRS_IP_ADDRESS1_2]<16) )
	{
		gGeneral_Flag|=TCP_IP_VALID_1;
		gStatus2_Flag|=IP_VAILD_F_1;
		Check_Data=0;
		for(i=1;i<=gGeneral_Buffer[GPRS_IP_ADDRESS1_2];i++)
		{
			gGeneral_Buffer[GPRS_IP_ADDRESS1_2+i]=ReadNextFromBuffer2();
			Check_Data += gGeneral_Buffer[GPRS_IP_ADDRESS1_2+i];
		}
		gGeneral_Buffer[GPRS_IP_ADDRESS1_CHECK_2]=Check_Data;
	}
	else
	{
		gGeneral_Flag&=~TCP_IP_VALID_1;
		gStatus2_Flag&=~IP_VAILD_F_1;
	}
	OperateSPIEnd();
	Send_COM1_Byte('I');Send_COM1_Byte('P');Send_COM1_Byte(':');
	if(gGeneral_Flag&TCP_IP_VALID_1)
	{
		Send_COM1_Byte('O');Send_COM1_Byte('k');
	}
	else
	{
		Send_COM1_Byte('E');Send_COM1_Byte('r');Send_COM1_Byte('r');Send_COM1_Byte('o');Send_COM1_Byte('r');
	}
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);

	//3，TCP服务器的登陆端口的处理
	OperateSPIEnd();
	Check_Data=0;
	gGeneral_Buffer[GPRS_SOCKET1_2]=ReadByteFromBuffer2(GPRS_SOCKET1_F_2);
	Check_Data = gGeneral_Buffer[GPRS_SOCKET1_2];
	for(i=1;i<5;i++)
	{
		gGeneral_Buffer[GPRS_SOCKET1_2+i]=ReadNextFromBuffer2();
		Check_Data += gGeneral_Buffer[GPRS_SOCKET1_2+i];
	}
	gGeneral_Buffer[GPRS_SOCKET1_END_2]=Check_Data;
	OperateSPIEnd();

	//4,TCP登陆的APN的处理
	OperateSPIEnd();
	Check_Data=0;
	gGeneral_Buffer[GPRS_APN_ADDRESS2_2]=ReadByteFromBuffer2(GPRS_APN_DATA_F_2);
	if(  (gGeneral_Buffer[GPRS_APN_ADDRESS2_2]>5)&&(gGeneral_Buffer[GPRS_APN_ADDRESS2_2]<25 ) )
	{
		for(i=1;i<=gGeneral_Buffer[GPRS_APN_ADDRESS2_2];i++)
		{
			gGeneral_Buffer[GPRS_APN_ADDRESS2_2+i]=ReadNextFromBuffer2();
			Check_Data += gGeneral_Buffer[GPRS_APN_ADDRESS2_2+i];
		}
		gGeneral_Buffer[GPRS_APN_END_2]=Check_Data;
	}
	//----如果写入的APN不正确，则采取默认的APN格式
	else
	{
		Check_Data=0;
		gGeneral_Buffer[GPRS_APN_ADDRESS2_2]=sizeof(APN_DEFAULT);
		for(i=0;i<sizeof(APN_DEFAULT);i++)
		{
			gGeneral_Buffer[GPRS_APN_ADDRESS2_2+i+1]=APN_DEFAULT[i];
			Check_Data += gGeneral_Buffer[GPRS_APN_ADDRESS2_2+i+1];
		}
		gGeneral_Buffer[GPRS_APN_END_2]=Check_Data;
	}
	//5，将上面的处理的相关数据存储在buffer1的相关区域中
	SRAM_Init();
	SRAM_Write(TARGET_NUMBER1_2+0,gGeneral_Buffer[TARGET_NUMBER1_2]);
	for(i=TARGET_NUMBER1_2+1;i<PHONE_AT_2;i++)
	{
		SRAM_Write(i,gGeneral_Buffer[i]);
	}
}

/********************************************************\
*	函数名：Load_Flash_Par_Buffer1()
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Load_Buffer1_Echo(unsigned char nCommand,unsigned char nACK)
{
	SRAM_Init();
	SRAM_Write(ECHO_SMS_2+0,VAILD_2);
	SRAM_Write(ECHO_SMS_2+1,nCommand);
	SRAM_Write(ECHO_SMS_2+2,nACK);
	SRAM_Write(ECHO_SMS_2+3,0);
	Judge_SMS_Way();
}
/********************************************************\
*	函数名：Send_Type1_GSM_Data_One
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned int Check_Allow_Phone(unsigned char nPhone_Length)
{
	unsigned int i=0,j=0,k=0;
	unsigned char Check_Temp=0;

	//1，读电话号码本的第一个扇区到buffer2中(比较第一个扇区的第一个帧5个电话号码)
	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_PHONE_INFO_ONE_2);OperateSPIEnd();
	//2，比较gHandle_Buffer[]中的号码是否与前5个电话号码是否匹配中
	//	gGeneral_Buffer[0]---gGeneral_Buffer[nPhone_Length]中存储的是需要检验的电话号码
	for(k=0;k<5;k++)
	{
		j=0;
		OperateSPIEnd();
		Check_Temp=ReadByteFromBuffer2( (ONE_1_FRAME_2+8)+21*k );
		if( gGeneral_Buffer[0] != Check_Temp )
		{
			j=1;
		}
		for(i=1;i<nPhone_Length;i++)
		{
			if( gGeneral_Buffer[i] != ReadNextFromBuffer2() )
			{
				j=1;break;
			}
		}
		OperateSPIEnd();
		//如果返回0，则说明当前号码与比较号码相匹配
		if(j==0)
		{
			return(0);
		}
	}
	//如果返回1，则说明5个号码与比较号码没一个匹配
	return(1);
}
/********************************************************\
*	函数名：Send_Handle_Signal()
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Send_Handle_Signal(unsigned char nSignal)
{
	SRAM_Init();
	SRAM_Write(SHANDLE_DATA_VAILD+0,VAILD_2);
	SRAM_Write(SHANDLE_DATA_VAILD+1,0x24);
	SRAM_Write(SHANDLE_DATA_VAILD+2,0x09);
	SRAM_Write(SHANDLE_DATA_VAILD+3,MAIN_STATUS);
	SRAM_Write(SHANDLE_DATA_VAILD+4,nSignal);
	SRAM_Write(SHANDLE_DATA_VAILD+5,gVolume);
	if(gInternal_Flag&VEHICLE_DEFENCE_F_1)	SRAM_Write(SHANDLE_DATA_VAILD+6,1);
	else									SRAM_Write(SHANDLE_DATA_VAILD+6,0);
	//是否GPS处于有效还是无效状态
	if(gGeneral_Flag&GPS_VALID_F_1)			SRAM_Write(SHANDLE_DATA_VAILD+7,2);
	else									SRAM_Write(SHANDLE_DATA_VAILD+7,1);
	SRAM_Write(SHANDLE_DATA_VAILD+8,1);
	SRAM_Write(SHANDLE_DATA_VAILD+9,0x0d);
}
/********************************************************\
*	函数名：ReloadData_FlashToSram
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
void LoadData_FlashToSram(unsigned int Flash_Srctor,unsigned int Flash_Adds,unsigned int Sram_Adds,unsigned int nLength)
{
	unsigned int i;
	unsigned char Temp;
	OperateSPIEnd();ReadOnePageToBuffer2(Flash_Srctor);OperateSPIEnd();
	Temp=ReadByteFromBuffer2(Flash_Adds);
	SRAM_Init();
	SRAM_Write( Sram_Adds,Temp);
	for(i=1;i<nLength;i++)
	{
		Temp=ReadNextFromBuffer2();
		SRAM_Write( (Sram_Adds+i),Temp);
	}
	OperateSPIEnd();
}
*/
/********************************************************\
*	函数名：Load_SMS_CenterNum()
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void  Load_SMS_CenterNum(unsigned int nLength)
{
	unsigned int i=0;
	unsigned char Check_Data=0;
	SRAM_Init();
	//在buffer1的相应区域写入短消息中心号码的长度(不包括长度本身的这个字节)
	Check_Data=0;
	SRAM_Write(CENTER_NUM_2+0,(nLength-8));
	for(i=8;i<nLength;i++)
	{
		//依次写入短消息中心号码
		Check_Data += gGeneral_Buffer[i];
		SRAM_Write(CENTER_NUM_2+i-8+1,gGeneral_Buffer[i]);
	}
	//即使短消息中心号码前带有+号，则一样写入到buffer1中
	//写入buffer1相应的中心号码校验字的位置
	SRAM_Write(CENTER_NUM_CHECK_2,Check_Data);
}
/********************************************************\
*	函数名：Check_TargetNum_Vaild()
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	可以用来检查目标号码，短消息中心号码，IP地址的有效性
\********************************************************/
unsigned int Check_Vaild(unsigned int Address_S,unsigned int Address_E)
{
	unsigned int i=0;
	unsigned char nLength;
	unsigned char Result=0;
	unsigned char Check_Data=0;
	//检查目标号码的有效性
	SRAM_Init();
	Check_Data=0;
	nLength = SRAM_Read(Address_S);
	for(i=0;i<nLength;i++)
	{
		Check_Data += SRAM_Read(Address_S+1+i);
	}
	Result=SRAM_Read(Address_E);
	if(Check_Data==Result)		return(0);
	else						return(1);
}
/********************************************************\
*	函数名：Check_Port()
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	可以用来检查目标号码，短消息中心号码，IP地址的有效性
\********************************************************/
unsigned int Check_Port(void)
{
	unsigned int i=0;
	unsigned char Check_Data=0;
	unsigned char Result=0;
	SRAM_Init();
	Check_Data = SRAM_Read(GPRS_SOCKET1_2);
	for(i=1;i<5;i++)
	{
		Check_Data += SRAM_Read(GPRS_SOCKET1_2+i);
	}
	Result=SRAM_Read(GPRS_SOCKET1_END_2);
	if(Check_Data==Result)		return(0);
	else						return(1);
}

/********************************************************\
*	函数名：Judge_SMS_Way()
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	补充说明：
	运行此子程序，则是需要有数据发送时候TCP和GSM数据的选择
\********************************************************/
void Judge_SMS_Way(void)
{
	//1，如果TCP的开关量或者IP地址无效的情况下，以短消息的形式传送数据
    if(   ((gGeneral_Flag&TCP_IP_VALID_1)==0)
    	||(gON_OFF_Temp0&TCP_LAND_ON_OFF_1)	   )
    {
    	gInternal_Flag|=ALLOW_DATA_F_1;
    }
    //2，如果TCP的开关量开启，并且IP地址有效
    else
    {
    	//2.1如果处于GSM的工作状态下
    	if(gM22_Status==GSM_WORK)
    	{
    		//2.1.1如果处于禁止登陆TCP的状态
    		if(gGeneral_Flag&LAND_TCP_ERROR_F_1)
    		{
    			gInternal_Flag|=ALLOW_DATA_F_1;
    		}
    		//2.1.2如果处于允许登陆TCP的状态，则设置标志进行TCP登陆
    		else
    		{
    			gOther_Flag|=NEED_LADN_TCP_F_1;
    			gInternal_Flag&=~ALLOW_DATA_F_1;
    		}
    	}
    }
}
/********************************************************\
*	函数名：Load_Info_One()
	作用域：外地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	可以用来检查目标号码，短消息中心号码，IP地址的有效性
\********************************************************/
void Load_Info_One(void)
{
	unsigned int i=0,j=0,k=0;

	Check_Flash(FLASH_INFO1_ONE_2);

	for(i=0;i<=sizeof(gGeneral_Buffer);i++)	gGeneral_Buffer[i]=0;
	Load_Flash_Par_Buffer1();
	//================================================
	//调试读出Flash中的参数是否正确
	OperateSPIEnd();
	ReadOnePageToBuffer2(FLASH_INFO1_ONE_2);
	OperateSPIEnd();
	//1,显示车型，车类，车组，车号
	Send_COM1_String( (unsigned char *)VTYPE,sizeof(VTYPE) );
	j=ReadByteFromBuffer2(VEH_TYPE_2);
	Send_COM1_Byte(ASCII(j/0x10));
	Send_COM1_Byte(ASCII(j%0x10));
	Send_COM1_Byte(':');
	Send_COM1_String( (unsigned char *)VKIND,sizeof(VKIND) );
	j=ReadNextFromBuffer2();
	Send_COM1_Byte(ASCII(j/0x10));
	Send_COM1_Byte(ASCII(j%0x10));
	Send_COM1_Byte(':');
	Send_COM1_String( (unsigned char *)VGROUP,sizeof(VGROUP) );
	j=ReadNextFromBuffer2();
	Send_COM1_Byte(ASCII(j/0x10));
	Send_COM1_Byte(ASCII(j%0x10));
	Send_COM1_Byte(':');
	Send_COM1_String( (unsigned char *)VID,sizeof(VID) );
    Send_COM1_Byte(ASCII(gVID/0x1000));
    Send_COM1_Byte(ASCII((gVID%0x1000)/0x100));
    Send_COM1_Byte(ASCII(((gVID%0x1000)%0x100)/0x10));
	Send_COM1_Byte(ASCII(gVID%0x10));
	OperateSPIEnd();
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);

	//2，显示目标号码1
	Send_COM1_String( (unsigned char *)NUM1,sizeof(NUM1) );
	j=ReadByteFromBuffer2(TARGET_NUMBER1_F_2);
	if(j>16) j=0;
	Send_COM1_Byte(ASCII(j/0x10));
	Send_COM1_Byte(ASCII(j%0x10));
	Send_COM1_Byte('[');
	for(i=0;i<j;i++)
	{
		k=ReadNextFromBuffer2();
		Send_COM1_Byte(k);
	}
	Send_COM1_Byte(']');
	OperateSPIEnd();
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	//3，显示TCP服务器的IP地址
	Send_COM1_String( (unsigned char *)IP_ADDRESS,sizeof(IP_ADDRESS) );
	j=ReadByteFromBuffer2(GPRS_IP_ADDRESS1_F_2);
	if(j>16) j=0;
	Send_COM1_Byte(ASCII(j/0x10));
	Send_COM1_Byte(ASCII(j%0x10));
	Send_COM1_Byte('[');
	for(i=0;i<j;i++)
	{
		k=ReadNextFromBuffer2();
		Send_COM1_Byte(k);
	}
	OperateSPIEnd();
	Send_COM1_Byte(']');
	Send_COM1_Byte(':');
	//4，显示TCP服务器的PORT号
	Send_COM1_String( (unsigned char *)PORT,sizeof(PORT) );
	j=ReadByteFromBuffer2(GPRS_SOCKET1_F_2);
	Send_COM1_Byte('[');
	Send_COM1_Byte(j);
	for(i=0;i<4;i++)
	{
		k=ReadNextFromBuffer2();
		Send_COM1_Byte(k);
	}
	Send_COM1_Byte(']');
	//6，显示APN
	OperateSPIEnd();
	Send_COM1_String( (unsigned char *)TCP_APN,sizeof(TCP_APN) );
	j=ReadByteFromBuffer2(GPRS_APN_DATA_F_2);
	if(j>20) j=0;
	Send_COM1_Byte(ASCII(j/0x10));
	Send_COM1_Byte(ASCII(j%0x10));
	Send_COM1_Byte('[');
	for(i=0;i<j;i++)
	{
		k=ReadNextFromBuffer2();
		Send_COM1_Byte(k);
	}
	Send_COM1_Byte(']');
	OperateSPIEnd();
	Send_COM1_Byte(0x0d);
	Send_COM1_Byte(0x0a);

	//================================================
	//将buffer1中其余的数据填为0
	gTimer=0;
	while(gTimer<10){}

	SRAM_Init();
	SRAM_Write(PHONE_AT_2+0,0);
	for(i=PHONE_AT_2+1;i<END_SRAM_2;i++)
	{
		SRAM_Write(i,0);
	}
}

/********************************************************\
*	函数名：Load_Info_Two()
	作用域：外地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	可以用来检查目标号码，短消息中心号码，IP地址的有效性
\********************************************************/
void Load_Info_Two(void)
{
    unsigned int i=0,j=0,k=0;
	unsigned char Buffer2_Temp;

	Check_Flash(FLASH_INFO2_ONE_2);

	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO2_ONE_2);OperateSPIEnd();
	//处理开关量(出厂时候的默认值为屏蔽,相应的位置置1，则表示此开关量处于关闭状态)
	gON_OFF_Temp0=ReadByteFromBuffer2(ON_OFF_V1_2);
	gON_OFF_Temp1=ReadNextFromBuffer2();
	gON_OFF_Temp2=ReadNextFromBuffer2();
	//报警信息的处理(判断属于那一类型的报警，报警类型有1--126,gFlag的低7位作为报警类型，最高位在系统中作为GPS数据有效和无效的标志)
	gAlarm_Type=ReadNextFromBuffer2();
	if( gAlarm_Type ==(char)(~ReadNextFromBuffer2()))
	{
		gCycle_Send_Status=ALARM_DATA;
		gCycle_Alarm=ALARM_CIRCLE_TIME;
	}
	else
	{
		gAlarm_Type=0;
		gCycle_Alarm=0;
		gCycle_Send_Status=NOP_DATA;
	}
	gAlarm_Type=0;
	gCycle_Alarm=0;
	gCycle_Send_Status=NOP_DATA;
	//1,判断监控信息的处理
	ReadNextFromBuffer2();
	OperateSPIEnd();
	/*
	if( (gON_OFF_Temp0&PHONE_OUT_ON)==0 )		gStatus2_Flag|=HANDLE_PHONE_OUT_LIMIT;
	else										gStatus2_Flag&=~HANDLE_PHONE_OUT_LIMIT;

	if( (gON_OFF_Temp0&PHONE_IN_ON)==0 )		gStatus2_Flag|=HANDLE_PHONE_IN_LIMIT;
	else										gStatus2_Flag&=~HANDLE_PHONE_IN_LIMIT;

	if( (gON_OFF_Temp0&TCP_LAND_ON_OFF_1)==0 )	gStatus2_Flag|=TCP_ON_OFF_F_1;
	else										gStatus2_Flag&=~TCP_ON_OFF_F_1;
	*/
	//默认电话全开
	gStatus2_Flag&=~HANDLE_PHONE_OUT_LIMIT;
	gStatus2_Flag&=~HANDLE_PHONE_IN_LIMIT;
	gStatus2_Flag&=~TCP_ON_OFF_F_1;
	gON_OFF_Temp0|=PHONE_OUT_ON;
	gON_OFF_Temp0|=PHONE_IN_ON;
	gON_OFF_Temp0|=TCP_LAND_ON_OFF_1;

	if((gON_OFF_Temp1&DISTANCE_TCP_ON_1)==0)	gStatus3_Flag |= DISTANCE_ON_F_1;
	else										gStatus3_Flag &=~ DISTANCE_ON_F_1;

	//2,判断是否设防标志
	OperateSPIEnd();
	Buffer2_Temp=ReadByteFromBuffer2(DEFENCE_F_2);
	if(Buffer2_Temp==DEFENCE_YES)
	{
		gInternal_Flag|=VEHICLE_DEFENCE_F_1;
		gStatus1_Flag|=DEFENCE_STATUS_F_1;
	}
	else
	{
		gInternal_Flag&=~VEHICLE_DEFENCE_F_1;
		gStatus1_Flag&=~DEFENCE_STATUS_F_1;
	}

	//TCP工作方式下的行车诡计的时间间隔
	gCycle_TCP = ReadNextFromBuffer2();
	gCycle_TCP<<=8;
	gCycle_TCP += ReadNextFromBuffer2();
	//GSM工作方式下的行车轨迹的时间间隔
	gCycle_GSM = ReadNextFromBuffer2();
	gCycle_GSM<<=8;
	gCycle_GSM += ReadNextFromBuffer2();
	//间隔距离传输的距离值
	gCycle_Distance = ReadNextFromBuffer2();
	gCycle_Distance<<=8;
	gCycle_Distance += ReadNextFromBuffer2();

	if( gCycle_GSM>3600 )		gCycle_GSM=1800;
	if( gCycle_TCP>3600 )		gCycle_TCP=1800;

	if( (gCycle_TCP>0)&&(gCycle_TCP<5) )	gCycle_TCP=5;
	if( (gCycle_GSM>0)&&(gCycle_GSM<5) )	gCycle_GSM=5;
	//处理传输方式类型
	if(gCycle_TCP>5)
	{
		gCycle_Distance=0;
	}
	else if(gCycle_Distance==0xFFFF)
	{
		gCycle_Distance=0;
	}
	//历史轨迹存储的时间间隔
	ReadNextFromBuffer2();
	//行车最大速度
	gMAX_Speed=ReadNextFromBuffer2();

	//如果有线路报警时刻的最小限制距离
	gMax_Limit_Distance = ReadNextFromBuffer2();
	gMax_Limit_Distance<<=8;
	gMax_Limit_Distance += ReadNextFromBuffer2();
	if(  (gMax_Limit_Distance>20)&&(gMax_Limit_Distance<0x0FFF) )
	{
		gCommon_Flag|=ALLOW_LINE_DIS_F_1;
	}
	else
	{
		gCommon_Flag&=~ALLOW_LINE_DIS_F_1;
	}
	//显示出读出的数据
//	#ifdef Debug_GSM_COM1
	Send_COM1_String( (unsigned char *)ON_OFF0,sizeof(ON_OFF0) );
	Send_COM1_Byte(ASCII(gON_OFF_Temp0/0x10));
	Send_COM1_Byte(ASCII(gON_OFF_Temp0%0x10));
	Send_COM1_Byte(':');
	Send_COM1_String( (unsigned char *)ON_OFF1,sizeof(ON_OFF1) );
	Send_COM1_Byte(ASCII(gON_OFF_Temp1/0x10));
	Send_COM1_Byte(ASCII(gON_OFF_Temp1%0x10));
	Send_COM1_Byte(':');
	Send_COM1_String( (unsigned char *)ON_OFF2,sizeof(ON_OFF2) );
	Send_COM1_Byte(ASCII(gON_OFF_Temp2/0x10));
	Send_COM1_Byte(ASCII(gON_OFF_Temp2%0x10));
	Send_COM1_Byte(':');
	Send_COM1_String( (unsigned char *)FLAG,sizeof(FLAG) );
	Send_COM1_Byte(ASCII(gAlarm_Type/0x10));
	Send_COM1_Byte(ASCII(gAlarm_Type%0x10));
	Send_COM1_Byte(':');
	Send_COM1_String( (unsigned char *)TCP_TIME,sizeof(TCP_TIME) );
    Send_COM1_Byte(ASCII(gCycle_TCP/1000));
    Send_COM1_Byte(ASCII((gCycle_TCP%1000)/100));
    Send_COM1_Byte(ASCII(((gCycle_TCP%1000)%100)/10));
	Send_COM1_Byte(ASCII(gCycle_TCP%10));
	Send_COM1_Byte(':');
	Send_COM1_String( (unsigned char *)GSM_TIME,sizeof(GSM_TIME) );
    Send_COM1_Byte(ASCII(gCycle_GSM/1000));
    Send_COM1_Byte(ASCII((gCycle_GSM%1000)/100));
    Send_COM1_Byte(ASCII(((gCycle_GSM%1000)%100)/10));
	Send_COM1_Byte(ASCII(gCycle_GSM%10));
	Send_COM1_Byte(':');
	Send_COM1_String( (unsigned char *)DIS,sizeof(DIS) );
    Send_COM1_Byte(ASCII(gCycle_Distance/1000));
    Send_COM1_Byte(ASCII((gCycle_Distance%1000)/100));
    Send_COM1_Byte(ASCII(((gCycle_Distance%1000)%100)/10));
	Send_COM1_Byte(ASCII(gCycle_Distance%10));
	Send_COM1_Byte(':');

	Send_COM1_Byte(0x0d);
	Send_COM1_Byte(0x0a);

	Send_COM1_String( (unsigned char *)MAXSPEED,sizeof(MAXSPEED) );

    Send_COM1_Byte(ASCII(gMAX_Speed/100));
    Send_COM1_Byte(ASCII((gMAX_Speed%100)/10));
    Send_COM1_Byte(ASCII(gMAX_Speed%10));
    Send_COM1_Byte(':');
    Send_COM1_String( (unsigned char *)LIMIT_DIS,sizeof(LIMIT_DIS) );
    Send_COM1_Byte('=');
    Send_COM1_Byte(ASCII(gMax_Limit_Distance/0x1000));
    Send_COM1_Byte(ASCII((gMax_Limit_Distance%0x1000)/0x100));
    Send_COM1_Byte(ASCII(((gMax_Limit_Distance%0x1000)%0x100)/0x10));
	Send_COM1_Byte(ASCII(gMax_Limit_Distance%0x10));

	Send_COM1_Byte(0x0d);
	Send_COM1_Byte(0x0a);
//	#endif
}
/********************************************************\
*	函数名：Load_Info_Three()
	作用域：外地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Load_Info_Three(void)
{
	unsigned int i=0;
	unsigned char Area_In=0;
	unsigned char Area_Out=0;
	unsigned char Buffer2_Temp=0;
	Check_Flash(FLASH_AREA_ONE_2);
	OperateSPIEnd();
	Send_COM1_String( (unsigned char *)AREA_PAR,sizeof(AREA_PAR) );
	Send_COM1_Byte('[');
	Buffer2_Temp=ReadByteFromBuffer2(FAREA1_LON_1_2);
	SRAM_Init();
	SRAM_Write( SAREA_DATA_FS,0x55);
	SRAM_Write(SAREA1_LON_1_2,Buffer2_Temp);
	Send_COM1_Byte(ASCII(Buffer2_Temp/0x10));
	Send_COM1_Byte(ASCII(Buffer2_Temp%0x10));
	for(i=1;i<17*4;i++)
	{
		Buffer2_Temp=ReadNextFromBuffer2();
		SRAM_Write( (SAREA1_LON_1_2+i),Buffer2_Temp);
		Send_COM1_Byte(ASCII(Buffer2_Temp/0x10));
		Send_COM1_Byte(ASCII(Buffer2_Temp%0x10));
	}
	Send_COM1_Byte(']');
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	OperateSPIEnd();
	SRAM_Write( SAREA_DATA_FE,0xAA);

	gArea_Par=0;
	SRAM_Init();
	Buffer2_Temp=SRAM_Read(SAREA1_PAR_2);
	//0x55表示是禁出标志，0xAA则表示禁入标志
	if( Buffer2_Temp==0x55)		{gArea_Par|=AREA1_ON;Area_Out++;}
	else if(Buffer2_Temp==0xAA)	{gArea_Par|=AREA1_ON;gArea_Par|=AREA1_DIR;Area_In++;}
	Buffer2_Temp=SRAM_Read(SAREA2_PAR_2);
	if( Buffer2_Temp==0x55)		{gArea_Par|=AREA2_ON;Area_Out++;}
	else if(Buffer2_Temp==0xAA)	{gArea_Par|=AREA2_ON;gArea_Par|=AREA2_DIR;Area_In++;}
	Buffer2_Temp=SRAM_Read(SAREA3_PAR_2);
	if( Buffer2_Temp==0x55)		{gArea_Par|=AREA3_ON;Area_Out++;}
	else if(Buffer2_Temp==0xAA)	{gArea_Par|=AREA3_ON;gArea_Par|=AREA3_DIR;Area_In++;}
	Buffer2_Temp=SRAM_Read(SAREA4_PAR_2);
	if( Buffer2_Temp==0x55)		{gArea_Par|=AREA4_ON;Area_Out++;}
	else if(Buffer2_Temp==0xAA)	{gArea_Par|=AREA4_ON;gArea_Par|=AREA4_DIR;Area_In++;}
	SRAM_Init();
	SRAM_Write(SAREA_IN_NUM,Area_In);
	SRAM_Write(SAREA_OUT_NUM,Area_Out);
	if(gArea_Par!=0)	gStatus2_Flag|=AREA_VAILD_F_1;
	else				gStatus2_Flag&=~AREA_VAILD_F_1;
}
/********************************************************\
*	函数名：Load_Info_Four()
	作用域：外地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Load_Info_Four(void)
{
	unsigned int i=0;
	unsigned char Buffer2_Temp=0;
	unsigned char Buffer3_Temp=0;
	//检查第一个扇区的数据点
	Check_Flash(FLASH_LINEDATA1_ONE_2);
	OperateSPIEnd();
	//先判断此扇区数据点的个数
	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_LINEDATA1_ONE_2);OperateSPIEnd();
	Buffer2_Temp=ReadByteFromBuffer2(FLINE_NUM_1);
	Buffer3_Temp=ReadNextFromBuffer2();
	Buffer3_Temp=~Buffer3_Temp;
	OperateSPIEnd();
	if(   ( Buffer2_Temp==Buffer3_Temp)
		&&( Buffer2_Temp>0 )
		&&( Buffer2_Temp<31 )	)

	{
		gLine_Num=Buffer2_Temp;
		gCommon_Flag|=ALLOW_LINE_DATA_F_1;
	}
	else
	{
		gCommon_Flag&=~ALLOW_LINE_DATA_F_1;
		Send_COM1_String((unsigned char *)LOAD_ERROR,sizeof(LOAD_ERROR));
		return;
	}
	//如果扇区点在1-30之间，则认为有效,如果第一个扇区存满30个点，则判断第二个扇区的数据的内容
	if(gLine_Num==30)
	{
		Check_Flash(FLASH_LINEDATA2_ONE_2);
		OperateSPIEnd();
		OperateSPIEnd();ReadOnePageToBuffer2(FLASH_LINEDATA2_ONE_2);OperateSPIEnd();
		Buffer2_Temp=ReadByteFromBuffer2(FLINE_NUM_1);
		Buffer3_Temp=ReadNextFromBuffer2();
		Buffer3_Temp=~Buffer3_Temp;
		OperateSPIEnd();
		if(   (Buffer2_Temp==Buffer3_Temp)
			&&( Buffer2_Temp>0 )
			&&(Buffer2_Temp<=30)	)
		{
			gLine_Num=gLine_Num+Buffer2_Temp;
		}
	}
	//如果最后的点数满足小于30，则只转移第一个扇区的数据到SRAM
	if(gLine_Num<=30)
	{
		SRAM_Init();
		SRAM_Write( SLINE_NUM_1,gLine_Num );
		SRAM_Write( SLINE_NUM_2,(~gLine_Num) );
		OperateSPIEnd();ReadOnePageToBuffer2(FLASH_LINEDATA1_ONE_2);OperateSPIEnd();
		Buffer2_Temp=ReadByteFromBuffer2(FLINE_DATA_1);
		SRAM_Write( SLINE_DATA_1+0,Buffer2_Temp);
		Send_COM1_Byte(ASCII(Buffer2_Temp/0x10));
		Send_COM1_Byte(ASCII(Buffer2_Temp%0x10));
		for(i=1;i<8*gLine_Num;i++)
		{
			Buffer2_Temp=ReadNextFromBuffer2();
			SRAM_Write( (SLINE_DATA_1+i),Buffer2_Temp );
		}
		OperateSPIEnd();
	}
	else if( (gLine_Num>30)&&(gLine_Num<=60) )
	{
		SRAM_Init();
		SRAM_Write( SLINE_NUM_1,gLine_Num );
		SRAM_Write( SLINE_NUM_2,(~gLine_Num) );
		OperateSPIEnd();ReadOnePageToBuffer2(FLASH_LINEDATA1_ONE_2);OperateSPIEnd();
		Buffer2_Temp=ReadByteFromBuffer2(FLINE_DATA_1);
		SRAM_Write( SLINE_DATA_1+0,Buffer2_Temp);
		for(i=1;i<240;i++)
		{
			Buffer2_Temp=ReadNextFromBuffer2();
			SRAM_Write( (SLINE_DATA_1+i),Buffer2_Temp );
		}
		OperateSPIEnd();
		OperateSPIEnd();ReadOnePageToBuffer2(FLASH_LINEDATA2_ONE_2);OperateSPIEnd();
		Buffer2_Temp=ReadByteFromBuffer2(FLINE_DATA_1);
		SRAM_Init();
		SRAM_Write( (SLINE_DATA_1+240),Buffer2_Temp);
		for(i=241;i<8*gLine_Num;i++)
		{
			Buffer2_Temp=ReadNextFromBuffer2();
			SRAM_Write( (SLINE_DATA_1+i),Buffer2_Temp );
		}
		OperateSPIEnd();
	}
	Send_COM1_String( (unsigned char *)LINE_PAR,sizeof(LINE_PAR) );
	Send_COM1_Byte(ASCII(gLine_Num/10));
	Send_COM1_Byte(ASCII(gLine_Num%10));
	SRAM_Init();
	Send_COM1_Byte('[');
	for(i=0;i<8*gLine_Num;i++)
	{
		Buffer2_Temp=SRAM_Read(SLINE_DATA_1+i);
		Send_COM1_Byte(ASCII(Buffer2_Temp/0x10));
		Send_COM1_Byte(ASCII(Buffer2_Temp%0x10));
		Clear_Exter_WatchDog();
	}
	Send_COM1_Byte(']');
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
}

/********************************************************\
*	函数名：Check_Flash()
	作用域：外地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	可以用来检查目标号码，短消息中心号码，IP地址的有效性
	如果数据无效果，则需要将MSP430内部的信息存储区A的数据覆盖
\********************************************************/
void Check_Flash(unsigned int Add_Sector)
{
	unsigned int i=0,j=0,k=0;
	unsigned int iAddress=0;
	unsigned char Sec_Vaild=0;
	unsigned int  nTCP=0;
	unsigned char Secter_One=0,Secter_Two=0;
	Secter_One=0;
	Secter_Two=0;
	//Secter_One=0,说明第一个扇区数据校验核正确。为1，则表示校验核正确，但非正常数据，为2，则表示校验核实不正确
	//Secter_Two=0,说明第二个扇区数据校验核正确。为1，则表示校验核正确，但非正常数据，为2，则表示校验核实不正确
	OperateSPIEnd();ReadOnePageToBuffer2(Add_Sector);OperateSPIEnd();
	j=ReadByteFromBuffer2(0);
	gGeneral_Buffer[0]=j;
	for(i=1;i<254;i++)
	{
		gGeneral_Buffer[i]=ReadNextFromBuffer2();
		j+=gGeneral_Buffer[i];
	}
	gGeneral_Buffer[254]=ReadNextFromBuffer2();
	gGeneral_Buffer[255]=ReadNextFromBuffer2();
	OperateSPIEnd();
	if( ((j/256)==gGeneral_Buffer[254]) && ((j%256)==gGeneral_Buffer[255]) )
	{
		if(Add_Sector==FLASH_INFO1_ONE_2)
		{
			if(   (gGeneral_Buffer[TARGET_NUMBER1_F_2]>16)
				||(gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2]>16)
				||(gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2]<7) )
			{
				Secter_One=1;
			}
			else if(gGeneral_Buffer[TARGET_NUMBER1_F_2]<=16)
			{
				k=0;
				for(i=0;i<gGeneral_Buffer[TARGET_NUMBER1_F_2];i++)
				{
					if(   (gGeneral_Buffer[TARGET_NUMBER1_F_2+i+1]>'9')
						||(gGeneral_Buffer[TARGET_NUMBER1_F_2+i+1]<'0')	)
					{
						k=1;break;
					}
				}
				if(k==1)	Secter_One=1;
				else
				{
					if(   (gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2]>=7)
						&&(gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2]<=16)	)
					{
						k=0;
						for(i=0;i<gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2];i++)
						{
							if(   (gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2+1+i]>'9')
							    ||(gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2+1+i]<'0')  )
							{
								if(gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2+1+i]=='.');
								else
								{
									k=1;break;
								}
							}
						}
						if(k==1)	Secter_One=1;
					}
				}
			}
		}
		else if(Add_Sector==FLASH_INFO2_ONE_2)
		{
			gON_OFF_Temp2 = gGeneral_Buffer[ON_OFF_V3_2];
			if(gON_OFF_Temp2==0xFF);
			else		Secter_Two=1;

			k=	gGeneral_Buffer[WATCH_F_2];
			if(k==0xFF);
			else		Secter_Two=1;

			k=	gGeneral_Buffer[RESET_NUM_2];
			if(k==0xFF);
			else		Secter_Two=1;

			nTCP = gGeneral_Buffer[TCP_TIMER_2];
			nTCP <<= 8;
			nTCP += gGeneral_Buffer[TCP_TIMER_2+1];
			if( (nTCP>0)&&(nTCP<0xFFFF) );
			else		Secter_Two=1;
		}
	}
	else
	{
		Secter_One=2;
	}
	/////////////////////////////////////////////////////////////
	OperateSPIEnd();ReadOnePageToBuffer2(Add_Sector+1);OperateSPIEnd();
	j=ReadByteFromBuffer2(0);
	gGeneral_Buffer[0]=j;
	for(i=1;i<254;i++)
	{
		gGeneral_Buffer[i]=ReadNextFromBuffer2();
		j+=gGeneral_Buffer[i];
	}
	gGeneral_Buffer[254]=ReadNextFromBuffer2();
	gGeneral_Buffer[255]=ReadNextFromBuffer2();
	OperateSPIEnd();
	if( ((j/256)==gGeneral_Buffer[254]) && ((j%256)==gGeneral_Buffer[255]) )
	{
		if(Add_Sector==FLASH_INFO1_ONE_2)
		{
			if(   (gGeneral_Buffer[TARGET_NUMBER1_F_2]>16)
				||(gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2]>16)
				||(gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2]<7) )
			{
				Secter_Two=1;
			}
			else if(gGeneral_Buffer[TARGET_NUMBER1_F_2]<=16)
			{
				k=0;
				for(i=0;i<gGeneral_Buffer[TARGET_NUMBER1_F_2];i++)
				{
					if(   (gGeneral_Buffer[TARGET_NUMBER1_F_2+i+1]>'9')
						||(gGeneral_Buffer[TARGET_NUMBER1_F_2+i+1]<'0')	)
					{
						k=1;break;
					}
				}
				if(k==1)	Secter_Two=1;
				else
				{
					if(   (gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2]>=7)
						&&(gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2]<=16)	)
					{
						k=0;
						for(i=0;i<gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2];i++)
						{
							if(   (gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2+1+i]>'9')
							    ||(gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2+1+i]<'0')  )
							{
								if(gGeneral_Buffer[GPRS_IP_ADDRESS1_F_2+1+i]=='.');
								else
								{
									k=1;break;
								}
							}
						}
						if(k==1)	Secter_Two=1;
					}
				}
			}
		}
		else if(Add_Sector==FLASH_INFO2_ONE_2)
		{
			gON_OFF_Temp2 = gGeneral_Buffer[ON_OFF_V3_2];
			if(gON_OFF_Temp2==0xFF);
			else		Secter_Two=1;

			k=	gGeneral_Buffer[WATCH_F_2];
			if(k==0xFF);
			else		Secter_Two=1;

			k=	gGeneral_Buffer[RESET_NUM_2];
			if(k==0xFF);
			else		Secter_Two=1;

			nTCP = gGeneral_Buffer[TCP_TIMER_2];
			nTCP <<= 8;
			nTCP += gGeneral_Buffer[TCP_TIMER_2+1];
			if( (nTCP>0)&&(nTCP<0xFFFF) );
			else		Secter_Two=1;
		}
	}
	else
	{
		Secter_Two=2;
	}
	///////////////////////////////////////////////////////////
	//显示一下两个数据
	Send_COM1_Byte('[');
	Send_COM1_Byte(ASCII(Secter_One/10));
	Send_COM1_Byte(ASCII(Secter_One%10));
	Send_COM1_Byte(':');
	Send_COM1_Byte(ASCII(Secter_Two/10));
	Send_COM1_Byte(ASCII(Secter_Two%10));
	Send_COM1_Byte(']');

	if(Secter_One==0)
	{
		if( (Secter_Two==1)||(Secter_Two==2) )
		{
			OperateSPIEnd();ReadOnePageToBuffer2(Add_Sector);OperateSPIEnd();
			Clear_Exter_WatchDog();
			P6OUT|=SPI45DB041_WP;
			WriteBuffer2ToPage(Add_Sector+1);
			OperateSPIEnd();
			P6OUT&=~SPI45DB041_WP;
			gTimer=0;while(gTimer<1000){Clear_Exter_WatchDog();}
		}
	}
	else if(Secter_One==2)
	{
		if( (Secter_Two==0) )
		{
			OperateSPIEnd();ReadOnePageToBuffer2(Add_Sector+1);OperateSPIEnd();
			Clear_Exter_WatchDog();
			P6OUT|=SPI45DB041_WP;
			WriteBuffer2ToPage(Add_Sector);
			OperateSPIEnd();
			P6OUT&=~SPI45DB041_WP;
			gTimer=0;while(gTimer<1000){Clear_Exter_WatchDog();}
		}
		else if( (Secter_Two==1)||(Secter_Two==2) )
		{
			if(Add_Sector==FLASH_INFO1_ONE_2)
			{
				Read_SegAB(0);
				if(gGeneral_Buffer[127]==0xAA)
				{
					for(i=128;i<254;i++)	gGeneral_Buffer[i]=0xFF;
					WriteOneByteToBuffer2(0,gGeneral_Buffer[0]);
					for(i=1;i<254;i++)
					{
						WriteNextByteToBuffer2(gGeneral_Buffer[i]);
					}
					OperateSPIEnd();
					RFlash_Sector(FLASH_INFO1_ONE_2);
				}
			}
			else if(Add_Sector==FLASH_INFO2_ONE_2)
			{
				Read_SegAB(1);
				if(gGeneral_Buffer[127]==0xAA)
				{
					for(i=128;i<254;i++)	gGeneral_Buffer[i]=0xFF;
					WriteOneByteToBuffer2(0,gGeneral_Buffer[0]);
					for(i=1;i<254;i++)
					{
						WriteNextByteToBuffer2(gGeneral_Buffer[i]);
					}
					OperateSPIEnd();
					RFlash_Sector(FLASH_INFO2_ONE_2);
				}
			}
		}
	}
	else if(Secter_One==1)
	{
		if(Secter_Two==0)
		{
			OperateSPIEnd();ReadOnePageToBuffer2(Add_Sector+1);OperateSPIEnd();
			Clear_Exter_WatchDog();
			P6OUT|=SPI45DB041_WP;
			WriteBuffer2ToPage(Add_Sector);
			OperateSPIEnd();
			P6OUT&=~SPI45DB041_WP;
			gTimer=0;while(gTimer<1000){Clear_Exter_WatchDog();}
		}
		else if( (Secter_Two==1)||(Secter_Two==2) )
		{
			if(Add_Sector==FLASH_INFO1_ONE_2)
			{
				Read_SegAB(0);
				if(gGeneral_Buffer[127]==0xAA)
				{
					for(i=128;i<254;i++)	gGeneral_Buffer[i]=0xFF;
					WriteOneByteToBuffer2(0,gGeneral_Buffer[0]);
					for(i=1;i<254;i++)
					{
						WriteNextByteToBuffer2(gGeneral_Buffer[i]);
					}
					OperateSPIEnd();
					RFlash_Sector(FLASH_INFO1_ONE_2);
				}
			}
			else if(Add_Sector==FLASH_INFO2_ONE_2)
			{
				Read_SegAB(1);
				if(gGeneral_Buffer[127]==0xAA)
				{
					for(i=128;i<254;i++)	gGeneral_Buffer[i]=0xFF;
					WriteOneByteToBuffer2(0,gGeneral_Buffer[0]);
					for(i=1;i<254;i++)
					{
						WriteNextByteToBuffer2(gGeneral_Buffer[i]);
					}
					OperateSPIEnd();
					RFlash_Sector(FLASH_INFO2_ONE_2);
				}
			}
		}
	}
	if(Secter_One==0)
	{
		iAddress=Add_Sector;
		Sec_Vaild=1;
	}
	else if(Secter_Two==0)
	{
		iAddress=Add_Sector+1;
		Sec_Vaild=1;
	}
	else
	{
		Sec_Vaild=0;
	}
	if(Sec_Vaild==1)
	{
		if(Add_Sector==FLASH_INFO1_ONE_2)
		{
			Read_SegAB(0);
			if(gGeneral_Buffer[127]!=0xAA)
			{
				//OperateSPIEnd();ReadOnePageToBuffer2(iAddress);OperateSPIEnd();
				Init_Buffer2();
				Deal_Sector(iAddress);
				Re430_Flash(0);
				Init_Buffer2();
			}
		}
		else if(Add_Sector==FLASH_INFO2_ONE_2)
		{
			Read_SegAB(1);
			if(gGeneral_Buffer[127]!=0xAA)
			{
				//OperateSPIEnd();ReadOnePageToBuffer2(iAddress);OperateSPIEnd();
				Init_Buffer2();
				Deal_Sector(iAddress);
				Re430_Flash(1);
				Init_Buffer2();
			}
		}
	}
}
/********************************************************\
*	函数名：RFlash_Sector()
	作用域：外地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
	可以用来检查目标号码，短消息中心号码，IP地址的有效性
\********************************************************/
void RFlash_Sector(unsigned int Sector)
{
	Page_R_Write_Check(Sector);
	gTimer=0;
	while(gTimer<1100){Clear_Exter_WatchDog();}
	OperateSPIEnd();
	P6OUT|=SPI45DB041_WP;
	WriteBuffer2ToPage(Sector+1);
	P6OUT&=~SPI45DB041_WP;
	OperateSPIEnd();
	gTimer=0;
	while(gTimer<1000){Clear_Exter_WatchDog();}
}
/********************************************************\
*	函数名：Waiting_Allow_Send()
	作用域：外地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
\********************************************************/
void Waiting_Allow_Send(void)
{
	gTimer=0;
	while(1)
	{
		Clear_Exter_WatchDog();
		if( 	(gHandle_Receive_Over>20)
			  &&(gDisp_Receive_Over>20)
			  &&( (gCommon_Flag&ALLOW_R232_F_1)==0)
			  &&(gGPS_Receive_Over>20)
			  &&(gReceive1_Over_Count>20)  )
			{
			  	break;
			}

		if( gTimer>500 )
		{
			break;
		}
	}
}

/********************************************************\
*	函数名：Do_Par()
	作用域：外地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
\********************************************************/
void Do_Par(void)
{
	unsigned int i,j,k;
	unsigned char Check_Data=0;
	unsigned char cNum1,cNum2,cNum3;
	//处理车型，类别，组别，车号
	gType = gGeneral_Buffer[1+9];
	gKind = gGeneral_Buffer[1+10];
	gGroup = gGeneral_Buffer[1+11];
	gVID  = gGeneral_Buffer[1+12];
	gVID <<= 8;
	gVID += gGeneral_Buffer[1+13];
	//处理目标号码1(存储在buffer1相应的缓冲区,第一个字节存储整个目标号码的长度,不包括长度本身这个字节)
	//如果目标号码1带有+，则需要把+号也存储在相应的区域'+'=2B
	i=1+14;
	while(1)
	{
		if(gGeneral_Buffer[i]==0x00)	break;
		i++;
		if(i>1+14+15) return;
		//这里的15表示目标号码1不会超过15个数
	}
	//计算这个目标号码1的长度(包括如果前面有'+'的话，也一样要计算在内)
	cNum1=i-(1+14);
	//如果目标号码1的长度没有超过15，则目标号码有效
	if( (i<=1+14+15)&&(cNum1>0) )
	{
		//计算出此目标号码1的号码长度并写入buffer1中相应的位置
		SRAM_Init();
		SRAM_Write(TARGET_NUMBER1_2+0,cNum1);
		Check_Data=0;
		for(j=1+14;j<i;j++)
		{
			SRAM_Write(TARGET_NUMBER1_2+j-14,gGeneral_Buffer[j]);
			Check_Data += gGeneral_Buffer[j];
		}
		SRAM_Write(TARGET_NUMBER1_CHECK_2,Check_Data);
		gGeneral_Flag|=NUM1_VAILD_F_1;
		if(gGeneral_Buffer[1+14]=='0')
		{
			gGeneral_Flag&=~NUM1_VAILD_F_1;
		}
	}
	OperateSPIEnd();
	//处理目标号码2(存储在buffer1相应的缓冲区，第一个字节存储整个目标号码的长度)
	//程序运行到这里i所表示的意义为gGeneral_Buffer[i]=0x00;
	//如果目标号码1带有+，则需要把+号也存储在相应的区域
	j=i+1;
	while(1)
	{
		if(gGeneral_Buffer[j]==0x00)	break;
		j++;if(j>i+1+15)	return;
	}
	cNum2=j-i;
	if( (j<=i+1+15)&&(cNum2>0) )
	{
	}
	OperateSPIEnd();
	k=j+1;
	while(1)
	{
		if(gGeneral_Buffer[k]==0x00)	break;
		k++;if(k>j+1+15)	return;
	}
	cNum3=k-j;
	if( (k<=j+1+15)&&(cNum3>0) )
	{
	}
	OperateSPIEnd();
	//延长一定时间
	gTimer=0;Clear_Exter_WatchDog();
	while(gTimer<100){}
	Clear_Exter_WatchDog();
	//++++++++++++++++++++++++++++++++++++++++++
	//将两个目标电话号码写入到Flash相应的区域中
	//1,先读出第一个扇区的数据存储在buffer2中。
	ReadOnePageToBuffer2(FLASH_INFO1_ONE_2);
	//先存储车型，类别，车组，车号
	OperateSPIEnd();
	WriteOneByteToBuffer2(VEH_TYPE_2,gType);
	WriteNextByteToBuffer2(gKind);
	WriteNextByteToBuffer2(gGroup);
	WriteNextByteToBuffer2(gGeneral_Buffer[1+12]);
	WriteNextByteToBuffer2(gGeneral_Buffer[1+13]);
	OperateSPIEnd();

	//存储目的电话号码1
	//第一个字节存储号码长度(存储在FLASH中的目标号码如果带有+号，也一样存储在FLASH中，并还要算上长度)
	if(cNum1>0)
	{
		WriteOneByteToBuffer2(TARGET_NUMBER1_F_2,cNum1);
		for(i=0;i<cNum1;i++)
		{
			WriteNextByteToBuffer2(gGeneral_Buffer[1+14+i]);
		}
		OperateSPIEnd();
	}
	//存储目的电话号码2
	if(cNum2>0)
	{
		WriteOneByteToBuffer2(TARGET_NUMBER2_F_2,cNum2);
		for(i=0;i<cNum2;i++)
		{
			WriteNextByteToBuffer2(gGeneral_Buffer[i+1+14+cNum1+1]);
		}
		OperateSPIEnd();
	}
	OperateSPIEnd();
	RFlash_Sector(FLASH_INFO1_ONE_2);
	Re430_Flash(0);
}

/********************************************************\
*	函数名：Do_Phone_Note()
	作用域：外地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
\********************************************************/
void Do_Phone_Note(void)
{
	unsigned int i=0,j=0;
	unsigned char nTemp=0;
	nTemp=gGeneral_Buffer[1+10];	//帧数(范围值0，1，2，3)
	j=0;
	if( (nTemp==0)||(nTemp==1) )
	{
		OperateSPIEnd();ReadOnePageToBuffer2(FLASH_PHONE_INFO_ONE_2);OperateSPIEnd();
		Write_Phone_Buffer2(nTemp);
		//将Buffer2中的数据的内容写入相关扇区中
		P6OUT|=SPI45DB041_WP;
		WriteBuffer2ToPage(FLASH_PHONE_INFO_ONE_2);
		P6OUT&=~SPI45DB041_WP;
		j=1;
	}
	else if( (nTemp==2)||(nTemp==3) )
	{
		OperateSPIEnd();ReadOnePageToBuffer2(FLASH_PHONE_INFO_TWO_2);OperateSPIEnd();
		Write_Phone_Buffer2(nTemp-2);
		P6OUT|=SPI45DB041_WP;
		WriteBuffer2ToPage(FLASH_PHONE_INFO_TWO_2);
		P6OUT&=~SPI45DB041_WP;
		j=1;
	}
	else
	{
		return;
	}
	gTimer=0;Clear_Exter_WatchDog();
	while(gTimer<600){}
	Clear_Exter_WatchDog();
	gTimer=0;Clear_Exter_WatchDog();
	while(gTimer<600){}
	Clear_Exter_WatchDog();
}
/********************************************************\
*	函数名：Write_Phone_Buffer2();
	作用域：本地文件调用
*	功能：
*	参数：  无
*	返回值：无
*	创建人：
*	修改历史：（每条详述）

\********************************************************/
void Write_Phone_Buffer2(unsigned char Add_Flag)
{
	unsigned int i=0;
	unsigned char Num_Phone=0;
	OperateSPIEnd();
	//处理第一个帧电话扇区的数据
	if(Add_Flag==0)
	{
		WriteOneByteToBuffer2(ONE_1_FRAME_2,gGeneral_Buffer[1+12]);
	}
	//处理第二个帧电话扇区的数据
	else if(Add_Flag==1)
	{
		WriteOneByteToBuffer2(TWO_1_FRAME_2,gGeneral_Buffer[1+12]);
	}
	for(i=0;i<(5*21-1);i++)
	{
		WriteNextByteToBuffer2(gGeneral_Buffer[1+13+i]);
	}
	OperateSPIEnd();
	Num_Phone = ReadByteFromBuffer2(PHONE_NUM_2);
	OperateSPIEnd();
	//第1个帧的5个号码
	if( Add_Flag==0 )
	{
		Num_Phone = gGeneral_Buffer[1+11];
	}
	//第2个帧的5个号码
	else if( Add_Flag==1 )
	{
		Num_Phone = 5+gGeneral_Buffer[1+11];
	}
	WriteOneByteToBuffer2(PHONE_NUM_2,Num_Phone);
	OperateSPIEnd();
}

/*
void Debug_Line_Data(void)
{
	unsigned int i=0;
	unsigned char nTemp=0;
	//1，有斜率
//	unsigned char LineData[40]={ 0x18,0x73,0x4A,0x48,0x04,0xD6,0x0F,0x70,0x18,0x73,0x62,0x30,0x04,0xD6,0x27,0x58,0x18,0x73,0x85,0x58,0x04,0xD6,0x27,0x58,0x18,0x73,0xA5,0xB0,0x04,0xD6,0x73,0x48,0x18,0x73,0xB7,0xF8,0x04,0xD6,0x88,0x60};
	//2，平行
//	unsigned char LineData[40]={ 0x18,0x72,0xEA,0xA8,0x04,0xD6,0x2E,0x60,0x18,0x73,0x20,0x18,0x04,0xD6,0x2E,0x60,0x18,0x73,0x56,0xF0,0x04,0xD6,0x31,0x30,0x18,0x73,0x83,0xF0,0x04,0xD6,0x31,0x30,0x18,0x73,0xA7,0x18,0x04,0xD6,0x2E,0x60};
	//3，垂直
//	unsigned char LineData[40]={ 0x18,0x73,0x69,0x38,0x04,0xD5,0xFE,0x90,0x18,0x73,0x69,0x38,0x04,0xD6,0x20,0x50,0x18,0x73,0x67,0xD0,0x04,0xD6,0x4A,0x80,0x18,0x73,0x66,0x69,0x04,0xD6,0x74,0xB0,0x18,0x73,0x65,0x00,0x04,0xD6,0x7F,0xF0};

	unsigned char LineData[80]={0x18,0x73,0x67,0xd0,0x04,0xd6,0x57,0x28,0x18,0x73,0x67,0xd0,0x04,0xd6,0x20,0x50,
	0x18,0x73,0x90,0x98,0x04,0xd6,0x0c,0xa0,0x18,0x73,0x7e,0x50,0x04,0xd5,0xec,0x48,
	0x18,0x73,0x6c,0x08,0x04,0xd5,0x47,0xc0,0x18,0x73,0xfa,0x10,0x04,0xd5,0x36,0xe0,
	0x18,0x74,0x97,0x90,0x04,0xd5,0x34,0x10,0x18,0x75,0x6b,0xe8,0x04,0xd5,0x44,0xf0,
	0x18,0x75,0xd5,0x60,0x04,0xd5,0x82,0xd0,0x18,0x75,0xce,0x5b,0x04,0xd5,0xaf,0xd0};

	nTemp=10;
	SRAM_Init();
	SRAM_Write( SLINE_NUM_1,nTemp );
	SRAM_Write( SLINE_NUM_2,(~nTemp) );
	for(i=0;i<80;i++)
	{
		SRAM_Write( (SLINE_DATA_1+i),LineData[i] );
	}
	gLine_Num=10;
	gStatus3_Flag|=LINE_LIMIT_F_1;

	//显示线路数据相应的缓冲区中的数据是否正确
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
	Send_COM1_Byte('[');
	for(i=SLINE_NUM_1;i<SLINE_NUM_1+82;i++)
	{
		nTemp=SRAM_Read(i);
		Send_COM1_Byte(ASCII(nTemp/0x10));
		Send_COM1_Byte(ASCII(nTemp%0x10));
		Clear_Exter_WatchDog();
	}
	Send_COM1_Byte(']');
	Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
}
*/

void Ask_Par(void)
{
	unsigned int i=0;
	unsigned char nTemp=0;
	unsigned char nLength=0,Num=0,Num1=0,nIP=0;
	unsigned char Dis_On=0;
	unsigned int  iTCP=0,iGSM=0,iDistance=0;
	//读取扇区1，2的相关参数
	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO1_ONE_2);OperateSPIEnd();
	OperateSPIEnd();
	nLength=0;
	gGeneral_Buffer[0]=0;									//有效数据的长度（不包括长度本身）
	gGeneral_Buffer[1]=ReadByteFromBuffer2(VEH_TYPE_2);		//车型
	gGeneral_Buffer[2]=ReadNextFromBuffer2();				//车类
	gGeneral_Buffer[3]=ReadNextFromBuffer2();				//车组
	gGeneral_Buffer[4]=ReadNextFromBuffer2();				//车号高位
	gGeneral_Buffer[5]=ReadNextFromBuffer2();				//车号低位
	nLength=5;
	OperateSPIEnd();
	Num=ReadByteFromBuffer2(TARGET_NUMBER1_F_2);
	if(Num>16)	Num=16;
	for(i=0;i<Num;i++)
	{
		gGeneral_Buffer[6+i]=ReadNextFromBuffer2();			//目标号码1
	}
	OperateSPIEnd();
	gGeneral_Buffer[6+Num]=0x00;
	nLength=6+Num;
	OperateSPIEnd();
	nIP=ReadByteFromBuffer2(GPRS_IP_ADDRESS1_F_2);			//IP地址
	if(nIP>16)	nIP=16;
	for(i=0;i<nIP;i++)
	{
		gGeneral_Buffer[7+Num+i]=ReadNextFromBuffer2();
	}
	OperateSPIEnd();
	gGeneral_Buffer[7+Num+nIP]=0x00;
	nLength=7+Num+nIP;
	OperateSPIEnd();
	gGeneral_Buffer[8+Num+nIP]=ReadByteFromBuffer2(GPRS_SOCKET1_F_2);
	for(i=0;i<4;i++)
	{
		gGeneral_Buffer[9+Num+nIP+i]=ReadNextFromBuffer2();//SOCKET，端口号
	}
	OperateSPIEnd();
	//
	Num1=ReadByteFromBuffer2(GPRS_APN_DATA_F_2);			//APN参数
	if( (Num1>0)&&(Num1<20) )
	{
		for(i=0;i<Num1;i++)
		{
			gGeneral_Buffer[13+Num+nIP+i]=ReadNextFromBuffer2();
		}
		OperateSPIEnd();
	}
	else
	{
		OperateSPIEnd();
		Num1=sizeof(APN_DEFAULT);
		for(i=0;i<Num1;i++)
		{
			gGeneral_Buffer[13+Num+nIP+i]=APN_DEFAULT[i];
		}
	}
	gGeneral_Buffer[13+Num+nIP+Num1]=0x00;
	nLength=13+Num+nIP+Num1;
	//读扇区5，6的信息内容
	gTimer=0;Clear_Exter_WatchDog();
	while(gTimer<50){}
	Clear_Exter_WatchDog();
	OperateSPIEnd();ReadOnePageToBuffer2(FLASH_INFO2_ONE_2);OperateSPIEnd();
	OperateSPIEnd();
	nTemp=ReadByteFromBuffer2(ON_OFF_V1_2);
	/*
	if(nTemp&TCP_LAND_ON_OFF_1)			gGeneral_Buffer[14+Num+nIP+Num1]=0x30;
	else								gGeneral_Buffer[14+Num+nIP+Num1]=0x31;
	*/
	gON_OFF_Temp0 &=~ TCP_LAND_ON_OFF_1;
	gStatus2_Flag |= TCP_ON_OFF_F_1;
	gGeneral_Buffer[14+Num+nIP+Num1]=0x31;
	nLength=14+Num+nIP+Num1;
	nTemp=ReadNextFromBuffer2();
	if(nTemp&DISTANCE_TCP_ON_1)			Dis_On=0x30;
	else								Dis_On=0x31;
	OperateSPIEnd();

	//TCP的间隔参数需要发送6位的ASCII字符数据
	//GSM的间隔一样
	//按距离值一样

	iTCP = ReadByteFromBuffer2(TCP_TIMER_2);
	iTCP <<= 8;
	iTCP += ReadNextFromBuffer2();

	iGSM = ReadNextFromBuffer2();
	iGSM <<= 8;
	iGSM +=	ReadNextFromBuffer2();

	iDistance = ReadNextFromBuffer2();
	iDistance <<= 8;
	iDistance += ReadNextFromBuffer2();
	OperateSPIEnd();

	gGeneral_Buffer[14+Num+nIP+Num1+1] = ASCII(iTCP/10000);
	gGeneral_Buffer[14+Num+nIP+Num1+2] = ASCII((iTCP%10000)/1000);
	gGeneral_Buffer[14+Num+nIP+Num1+3] = ASCII((iTCP%1000)/100);
	gGeneral_Buffer[14+Num+nIP+Num1+4] = ASCII((iTCP%100)/10);
	gGeneral_Buffer[14+Num+nIP+Num1+5] = ASCII(iTCP%10);

	gGeneral_Buffer[14+Num+nIP+Num1+6] = ASCII(iGSM/10000);
	gGeneral_Buffer[14+Num+nIP+Num1+7] = ASCII((iGSM%10000)/1000);
	gGeneral_Buffer[14+Num+nIP+Num1+8] = ASCII((iGSM%1000)/100);
	gGeneral_Buffer[14+Num+nIP+Num1+9] = ASCII((iGSM%100)/10);
	gGeneral_Buffer[14+Num+nIP+Num1+10] = ASCII(iGSM%10);

	gGeneral_Buffer[14+Num+nIP+Num1+11] = ASCII(iDistance/10000);
	gGeneral_Buffer[14+Num+nIP+Num1+12] = ASCII((iDistance%10000)/1000);
	gGeneral_Buffer[14+Num+nIP+Num1+13] = ASCII((iDistance%1000)/100);
	gGeneral_Buffer[14+Num+nIP+Num1+14] = ASCII((iDistance%100)/10);
	gGeneral_Buffer[14+Num+nIP+Num1+15] = ASCII(iDistance%10);

	gGeneral_Buffer[14+Num+nIP+Num1+16]=Dis_On;								//按距离上传时的TCP的工作方式
	OperateSPIEnd();
	nLength=14+Num+nIP+Num1+16;
	gGeneral_Buffer[0]=nLength;		//不包括长度本身这个字节，数据区的长度
}

void TA_Init(void)
{
    TACTL|=TASSEL1;         //输入时钟选择MCLK
    TACTL|=TACLR;           //定时器A清除位CLR清零
    TACTL|=MC1;             //定时器A选择连续增记数模式
//    TACTL|=TAIE;          //定时器A溢出中断允许

    TAR=0;                  //定时器A的值清零
    TACCTL0|=OUT;           //TXD2 Idle as Mark
	TACCTL1|=OUT;           //TXD3 Idle as Mark
	TACCTL2|=OUT;           //TXD4 Idle as Mark
}

void Re430_Flash(unsigned char InFlag)
{
	unsigned int i=0,j=0;

	if(InFlag>1)	return;
	gGeneral_Buffer[127]=0xAA;
	OperateSPIEnd();
	gGeneral_Buffer[0]=ReadByteFromBuffer2(0);
//	gGeneral_Buffer[127]=gGeneral_Buffer[0];
	for(i=1;i<127;i++)
	{
		gGeneral_Buffer[i]=ReadNextFromBuffer2();
		//gGeneral_Buffer[127]+=gGeneral_Buffer[i];
	}
	OperateSPIEnd();
	Write_SegAB(InFlag);
	gTimer=0;
	while(gTimer<1100){Clear_Exter_WatchDog();}
}
/********************************************************\
*	函数名：Check_ParNum()
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned int Check_ParNum(void)
{
	unsigned int x=0,y=0;
	unsigned int i=0;
	x=Check_Vaild(CENTER_NUM_2,CENTER_NUM_CHECK_2);			//短消息中心号码
	y=Check_Vaild(TARGET_NUMBER1_2,TARGET_NUMBER1_CHECK_2); //目标电话号码
	if(x==1)
	{
		gInternal_Flag&=~CENTER_NUM_VAILD_F_1;
		gInternal_Flag|=SET_CSCA_F_1;
		return(1);
	}
	if(y==1)
	{
		gGeneral_Buffer[CENTER_NUM_2]=SRAM_Read(CENTER_NUM_2);
		for(i=CENTER_NUM_2+1;i<=CENTER_NUM_CHECK_2;i++)
		{
			gGeneral_Buffer[i]=SRAM_Read(i);
		}
		OperateSPIEnd();
		Load_Flash_Par_Buffer1();
		return(2);
	}
	return(0);
}
/********************************************************\
*	函数名：Init_Buffer2()
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Init_Buffer2(void)
{
	unsigned int i=0;
	OperateSPIEnd();
	WriteOneByteToBuffer2(0,0xFF);
	for(i=1;i<263;i++)
	{
		WriteNextByteToBuffer2(0xFF);
	}
	WriteNextByteToBuffer2(0xAA);
	OperateSPIEnd();
}
/********************************************************\
*	函数名：Check_Buffer2()
	作用域：本地文件调用
*	功能：
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned char Deal_Sector(unsigned int iSector)
{
	unsigned int i=0;
	unsigned char Data=0;
	for(i=0;i<3;i++)
	{
		Data=0;
		OperateSPIEnd();ReadOnePageToBuffer2(iSector);OperateSPIEnd();
		Data=ReadByteFromBuffer2(263);OperateSPIEnd();
		if(Data==0xAA);
		else
		{
			break;
		}
	}
	if(Data==0xaa)	return(0);
	else			return(1);
}
