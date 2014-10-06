/********************************************************\
*	文件名：  Do_Reset.h
*	创建时间：2004年10月20日
*	创建人：  
*	版本号：  1.00
*	功能：
*
*	修改历史：（每条详述）
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
*	函数名：Do_OFF_Power
	作用域：外部文件调用
*	功能：  进行GPS和GSM模块掉电重启的操作。
*	参数：
*	返回值：需要对一些相关的全局变量进行附值
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Do_OFF_Power(void)
{
	//设置高电平，以关断2576输出
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
	//延长OFF_TIME时间后，则重新设置低电平，开启2576输出
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
	    gGeneral_Flag&=~OFF_M22_F_1;				//取消掉电M22的标志位
	    gGeneral_Flag&=~OFF_GPS_F_1;				//取消掉电GPS的标志位
	    gPhone_Status=READY;
	    gStatus1_Flag&=~PHONE_ONLINE_F_1;
	}
}
/********************************************************\
*	函数名：Do_Reset_GSM_On
	作用域：外部文件调用
*	功能：  针对G20模块在上电后的启动步骤的操作，进行一些AT指令的初始化
*	参数：
*	返回值：需要对一些相关的全局变量进行附值
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Do_Reset_GSM_On(void)
{
	static unsigned char M22_ReturnER_Count=0;		//AT指令返回错误的记数
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
	//2，ATZ
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
	//6-1，AT+CREG?
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
		    		//表示已经找到SIM卡的网络
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
		    	//具体将读出的短消息中心号码存储在buffer1相应的区域中（直接以ASCII形式存储）
		    	if(gGeneral_Buffer[7]=='"')
		    	{
		    		i=8;
					while(1)
					{
						if(gGeneral_Buffer[i]=='"') break;
						i++;if(i>8+20) {gReset_M22_On_Setp=1;return;}		//非法长度，则退出
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
	//13,AT$AUPATH=3,1,选择差分的语音通道
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
			//2,固定调节SPEKER的接收增益
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
			//3,调节AT$AUVOL的接收增益
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
	//16,AT$TIMEOUT=XX(XX表示100--5000毫秒)
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
	//19,显示程序的时间和作者
	else if(gReset_M22_On_Setp==39)
	{
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		//开发商：哈工大泰森电子有限公司
		Send_COM1_String( (unsigned char *)MADE,sizeof(MADE) );
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		//开发着：
		Send_COM1_String( (unsigned char *)AUTHOR,sizeof(AUTHOR) );
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		//产品型号:
		Send_COM1_String( (unsigned char *)MODEL,sizeof(MODEL) );
		Send_COM1_Byte(':');
		Send_COM1_String( (unsigned char *)MODEL_TYPE2,sizeof(MODEL_TYPE2) );
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		//开发日期
		Send_COM1_String( (unsigned char *)DESIGN_DATA,sizeof(DESIGN_DATA) );
		Send_COM1_String( (unsigned char *)EDIT_DATE,sizeof(EDIT_DATE) );
		//软件版本
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		Send_COM1_String( (unsigned char *)SOFT_EDITION,sizeof(SOFT_EDITION) );
		Send_COM1_String( (unsigned char *)GPS_2005A,sizeof(GPS_2005A) );
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);
		Send_COM1_String( (unsigned char *)UPDATE_TIME,sizeof(UPDATE_TIME) );
		Send_COM1_Byte(0x0d);Send_COM1_Byte(0x0a);

		gReset_M22_On_Setp=40;
	}
	//20,M22初始化完成
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
*	函数名：Ask_SMS_Send
	作用域：外部文件调用
*	功能：  在进行SMS的发送操作AT+CMGS=L(长度)
*	参数：  Input
*	返回值: 无
*	创建人：
*
*	修改历史：（每条详述）
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
*	函数名：Send_SMS_Head
	作用域：外部文件调用
*	功能：  根据SMS的标准格式，这里是将短消息中心号码，目标电话号码，协议格式
			发送到GSM模块
*	参数：  I
*	返回值: 无
*	创建人：
*
*	修改历史：（每条详述）
	发送SMS的头，包括短消息中心号码在内的固定长度
	注意这里的头与在G18里不一样，必须将短消息中心号码在数据包头中

	AT+CMGS=27
	>
	0891683108705505F011000D91683167108230F70008000CFF4FB008000101000102090D

    在这个子模块中发送的数据则为0891683108705505F011000D91683167108230F7000800
    这个部分只所有发送SMS数据的公共部分。在这个项目文件中，只要短消息中心号码
    和目标电话号码不变，则这部分的发送数据一样。
    0CFF4FB008000101000102090D 这部分属于数据区。会由后面的程序处理。

	这里27的长度是指11000D91683167108230F70008000CFF4FB008000101000102090D

	AT+CSCA?
	+CSCA: "+8613800755500",145

	OK

\********************************************************/
void Send_SMS_Head(void)
{
	unsigned int i,j,k,x;
	//读短消息中心号码的长度+8613800755500（11位）
	SRAM_Init();
	j=SRAM_Read(CENTER_NUM_2);
	//判断是否有+号
	i=SRAM_Read(CENTER_NUM_2+1);
	if(i=='+')
	{
		k=(j+1)/2+1;gInternal_Flag&=~SMS_CENTER_CLASS_1;	//送91
	}
	else
	{
		k=(j+1)/2+2;gInternal_Flag|=SMS_CENTER_CLASS_1;		//送A1
	}
	//发送号码的长度
	Send_COM0_Byte(ASCII(k/16));
	Send_COM0_Byte(ASCII(k%16));
	if(gInternal_Flag&SMS_CENTER_CLASS_1)	Send_COM0_Byte('A');
	else									Send_COM0_Byte('9');
	Send_COM0_Byte('1');
	//送A1,不带+，则前面的i也是正常的有效发送数据
	if(gInternal_Flag&SMS_CENTER_CLASS_1)
	{
		k=i;
		x=SRAM_Read(CENTER_NUM_2+2);
		Send_COM0_Byte(x);
		Send_COM0_Byte(k);
		i=3;
	}
	//送91，前面带+
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
		if(i<j)//如上例这里j=11
		{
			k=SRAM_Read(CENTER_NUM_2+i);
			x=SRAM_Read(CENTER_NUM_2+i+1);
			Send_COM0_Byte(x);
			Send_COM0_Byte(k);
			i++;
			i++;
		}
		//发送短消息中心号码的最后一个字节0
		else if(i==j)
		{
			k=SRAM_Read(CENTER_NUM_2+i);
			Send_COM0_Byte('F');
			Send_COM0_Byte(k);
			break;
		}
		else break;
	}
	// 发送1100 (如上例子)，这个在所有的协议中均一致
	for(i=0;i<sizeof(SMS_Head_1);i++)
	{
		Send_COM0_Byte(SMS_Head_1[i]);
	}
	//++++++++++++++++++++++++++++++++++++++++++++++
	//发送目标电话号码
	//分析：如果存储为 14,'+','8','6','1','3','6','7','0','1','5','5','7','0','4'
	//或者存储为		13,'8','6','1','3','6','7','0','1','5','5','7','0','4'
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
	//送A1,不带+，则前面的i也是正常的有效发送数据
	if(gInternal_Flag&SMS_TARGET_CLASS_1)
	{
		k=i;
		x=SRAM_Read(TARGET_NUMBER1_2+2);
		Send_COM0_Byte(x);
		Send_COM0_Byte(k);
		i=3;
	}
	else									//送91，前面带+
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
		//目标电话号码的最后一个字节
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
	// 发送短消息的协议方式(如上例子)，这个在所有的协议中均一致
	for( i=0;i<6;i++ )
	{
		Send_COM0_Byte(SMS_Head_2[i]);
	}
}

/********************************************************\
*	函数名：Send_Echo_GSM_Head
	作用域：外部文件调用
*	功能：

*	参数：  nInput 			如果属于SMS的发送，则这个参数表示整个数据包的长度
 			nCommand		数据包中的命令字
 			nACK			ACK响应，如果是属于主动发送的上行数据包，则ACK=0
 			nLength			指的是在数据包中从消息号到CHK以前的数据的字节数

*	返回值: iCheckOut  		这部分发送数据的和（用来计算校验核）
*	创建人：
*
*	修改历史：（每条详述）

	补充说明：具体在buffer1中存储第2类SMS的数据形式如下：0x7B----0xB6（整个分配的区域大小）
			  0x7B	存储表示这个区域的SMS是否处理，为1则表示后面还有未处理的SMS，
			  		为0则表示已经处理完了SMS
			  0x7C	存储数据包的命令字在这里则为nCommand
			  0x7D	存储数据包的ACK的值ACK
			  0x7E	存储后面数据包的长度（但注意没有存储CHK(MSB),CHK(LSB),EOT,这三个字节）
			  0x7F  从这开始存储数据包

	AT+CMGS=27
	>
	0891683108705505F011000D91683167108230F70008000CFF4FB008000101000102090D

	这里27的长度是指的是从1100 0D 91 68 3167108230F7 000800 0C FF4FB008000101000102090D
										13760128037
	所以Send_Echo_Head_1子模块的几个参数的计算方式如下：
	nInput的计算方式：	nInput=12+(0x7d)
	nCommand的附值方式：nCommand=(0x7C)
	nACK的附值方式：	nACK=(0x7E)
	nLength的附值方式：	nLength=nInput-4

	根据上面的描述，在发送第二累SMS的时候,AT+CMGS=后面跟的数据的长度的计算方式则为

	(目标号码长度+1)/2+8+12+(0x7E)
	则根据上面的例子，目标号码为13760128037为11位.  8 是指的1100 0D 91 68 000800的长度
					  12指的是0C的长度，因为这是属于第一类的SMS的格式，数据长度为0，
					  但在在第二类的SMS中还需要加上数据区的长度，在这里存储在buffer1中的0x7e区域

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
	Send_COM0_Byte(ASCII(nACK/0x10));		//ACK响应字符
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
	//iCheckOut为计算前面发送部分的数据的校验和。如果后面还有更多的数据
	//在计算整个校验核的时候则要继续这个数据一起计算在内
}
/********************************************************\
*	函数名：Send_Echo_TCP_Head
	作用域：外部文件调用
*	功能：
*	参数： iCheckIn	前面计算的校验核
		   nAddress 需要发送数据在buffer1中的首地址
		   nLength	需要发送的数据在buffer1中的长度

*	返回值: iCheckOut  这部分发送数据的和（用来计算校验核）
*	创建人：
*
*	修改历史：（每条详述）

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
	//iCheckOut为计算前面发送部分的数据的校验和。如果后面还有更多的数据
	//在计算整个校验核的时候则要继续这个数据一起计算在内
}
/********************************************************\
*	函数名：Send_Buffer1_GSM_Data
	作用域：外部文件调用
*	功能： 打包发送存储在buffer1中的数据区的数据。并计算校验核
*	参数： iCheckIn	前面计算的校验核
		   nAddress 需要发送数据在buffer1中的首地址
		   nLength	需要发送的数据在buffer1中的长度

*	返回值: iCheckOut  这部分发送数据的和（用来计算校验核）
*	创建人：
*
*	修改历史：（每条详述）

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
*	函数名：Send_Buffer1_TCP_Data
	作用域：外部文件调用
*	功能： 打包发送存储在buffer1中的数据区的数据。并计算校验核
*	参数： iCheckIn	前面计算的校验核
		   nAddress 需要发送数据在buffer1中的首地址
		   nLength	需要发送的数据在buffer1中的长度

*	返回值: iCheckOut  这部分发送数据的和（用来计算校验核）
*	创建人：
*
*	修改历史：（每条详述）
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
*	函数名：Send_Echo_TCP_End
	作用域：外部文件调用
*	功能：  进行最后部分的数据发送，这部分可以与GPRS的数据发送共用
			是所有发送SMS,和GPRS数据均可调用的子程序
*	参数：  Input  前面计算的校验核，在这里要发送出去
*	返回值:
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void Send_Echo_TCP_End(unsigned int iInput)
{
	Send_COM0_Byte( iInput/0x100 );
	Send_COM0_Byte( iInput%0x100 );
	Send_COM0_Byte( EOT );
}
/********************************************************\
*	函数名：Send_Echo_GSM_End
	作用域：外部文件调用
*	功能：  进行最后部分的数据发送，这部分可以与GPRS的数据发送共用
			是所有发送SMS,和GPRS数据均可调用的子程序
*	参数：  Input  前面计算的校验核，在这里要发送出去
*	返回值:
*	创建人：
*
*	修改历史：（每条详述）
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
*	函数名：Send_Echo_GSM_End
	作用域：外部文件调用
*	功能：  进行最后部分的数据发送，这部分可以与GPRS的数据发送共用
			是所有发送SMS,和GPRS数据均可调用的子程序
*	参数：  Input  前面计算的校验核，在这里要发送出去
*	返回值:
*	创建人：
*
*	修改历史：（每条详述）
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

