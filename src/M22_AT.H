/********************************************************\
*	文件名：  M22_AT.C
*	创建时间：2004年12月7日
*	创建人：  
*	版本号：  1.00
*	功能：	  有关FLASH，Buffer1中相关的一些位置定义
*	文件属性：公共文件
*	修改历史：（每条详述）
	2004年12月14日：修改
\********************************************************/

#ifndef __M22_AT_H
#define __M22_AT_H

extern const unsigned char  AT[3];								//Communication
extern const unsigned char  ATZ[4];						//恢复出厂时候的相关设置
extern const unsigned char  ATE1[5]; 					//默认，the echo should be enabled
extern const unsigned char  ATE0[5];
extern const unsigned char  AT_CGMM[8];	//显示表示是M22模块
extern const unsigned char  AT_CGMR[8];	//显示M22的版本号
extern const unsigned char  AT_CGSN[8];	//显示M22产品序列号
extern const unsigned char  AT_CMGF0[10];	//设置PDU模式
extern const unsigned char  AT_CREG[9];		//查询信号附着
extern const unsigned char  AT_COPS_C[9];		//查询是SIM卡的网络状态
extern const unsigned char  AT_CIMI[8];			//显示SIM卡的标识
extern const unsigned char  AT_CSCA_C[9];		//显示菜单的参数设置，具体见PDF
extern const unsigned char  AT_CSCA[8];
extern const unsigned char  AT_NOSLEEP1[13];	//设置模块不用处于休眠状态
extern const unsigned char  AT_COLP1[10];
extern const unsigned char  AT_CLIP1[10];
extern const unsigned char  AT_AUPATH[14];
extern const unsigned char  AT_AUGAIN[10];
extern const unsigned char  AT_CNMI[18];
extern const unsigned char  AT_TIMEOUT[16];
extern const unsigned char  AT_CMGD[12];

//与电话有关的AT指令
extern const unsigned char  ATD[3];				//拨号处理
extern const unsigned char  ATH[4];		//挂机AT指令
extern const unsigned char  ATA[4];		//接听AT指令

extern const unsigned char  AT_AUVOL[9];	//音量的设置,具体见PDF文档
extern const unsigned char  AT_CSQ[7];
extern const unsigned char  AT_CLCC[8];			//查询Call Status Message
extern const unsigned char  AT_VTS[7];					//语音方面的DTMF的处理
//与登陆TCP相关的指令
extern const unsigned char  AT_CGDCONT[11];
extern const unsigned char  APN_DEFAULT[14];
extern const unsigned char  AT_CGPCO[21];
extern const unsigned char  AT_DESTINFO[12];
extern const unsigned char  ATD_97[8];
extern const unsigned char  HANG_TCP[3];
extern const unsigned char  ATO[4];
//与发送SMS相关的AT指令
extern const unsigned char  AT_CMGS[8];

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						Motorola G20---AT指令返回常用字符说明
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
//GPS模块返回的字符串
extern const unsigned char String_GPRMC[6];			//GPS返回数据的帧头

//M22返回的常用的字符串
extern const unsigned char String_OK[2];
extern const unsigned char String_ER[5];
extern const unsigned char String_Input_Label[2];
extern const unsigned char String_CLCC[6];
extern const unsigned char String_CMGS[6];
extern const unsigned char String_CSQ[5];
extern const unsigned char String_CSCA[5];
extern const unsigned char String_CREG[6];
extern const unsigned char String_RING[4];
extern const unsigned char String_CLIP[6];
extern const unsigned char String_COLP[6];
extern const unsigned char String_CMT[5];
extern const unsigned char String_NOCARRIER[10];
extern const unsigned char String_L_TCP[16];
extern const unsigned char String_DESTINFO[10];
extern const unsigned char String_COPS[6];
extern const unsigned char String_CONNECT[7];
extern const unsigned char String_BUSY[4];
extern const unsigned char String_NOANSWER[9];
extern const unsigned char String_AT[2];
extern const unsigned char String_END[2];


//程序中其他显示的字符串
extern const unsigned char M22_RESET_ON_OK[21];

//供应商：哈工大泰森电子有限公司
extern const unsigned char MADE[32];
//产品型号
extern const unsigned char MODEL[8];
//开发日期
extern const unsigned char DESIGN_DATA[8];

extern const unsigned char EDIT_DATE[13];
//软件版本
extern const unsigned char SOFT_EDITION[8];
extern const unsigned char GPS_2005A[16];
extern const unsigned char UPDATE_TIME[21];
//开发者：易爱国
extern const unsigned char AUTHOR[16];
//车载GPS小终端
extern const unsigned char MODEL_TYPE1[13];
//车载GPS	GPRS-M22-2005A
extern const unsigned char MODEL_TYPE2[30];

extern const unsigned char R_OVERTOP[24];
extern const unsigned char JR_OVERTOP[24];
extern const unsigned char CLEAR_SRAM[7];
extern const unsigned char SYS_EDITION[2];
extern const unsigned char SYS_TIME[6];
extern const unsigned char SYS_UPDATATIME[6];
//extern const unsigned char LAND_TCP[8];
//extern const unsigned char PASS_W_PAGE[10];
//extern const unsigned char ALL_DISTANCEP[12];
extern const unsigned char MAXSPEED[12];
extern const unsigned char LIMIT_DIS[14];
//extern const unsigned char PASS_R_PAGE[9];
//extern const unsigned char TIME_START[11];
//extern const unsigned char TIME_END[9];
//extern const unsigned char PASS_SEND_ONE_PAGE[19];
//extern const unsigned char PASS_END[18];
extern const unsigned char MAINSYS[26];
//extern const unsigned char PAGE_S[12];
//extern const unsigned char PAGE_E[10];
extern const unsigned char String_A[60];
//extern const unsigned char CHANGE_PAGE[11];
extern const unsigned char String_TestGPRS[4];
extern const unsigned char SMS_Head_1[4];
extern const unsigned char SMS_Head_2[6];
extern const unsigned char TYPE[7];
extern const unsigned char TCP_QUIT[16];
extern const unsigned char TCP_TEST[13];
extern const unsigned char TCP_COMMAAND[18];
extern const unsigned char TRACK[6];
extern const unsigned char ALARM[6];
extern const unsigned char DISTANCE[8];
extern const unsigned char WATCH[5];
extern const unsigned char POWER_OFF[20];
extern const unsigned char PHONE[17];
extern const unsigned char Handle_VOL[5];
extern const unsigned char PRO_EDITION[18];
extern const unsigned char SYS[9];
extern const unsigned char String_EDIT[60];
extern const unsigned char TCP_LAND_ERROR[16];
extern const unsigned char RECEIVE1[16];
extern const unsigned char RECEIVE2[16];
extern const unsigned char GET_DRIVERINFO_OK[18];
extern const unsigned char SEND_DRIVERINFO_OK[19];
extern const unsigned char CENTER_RELOAD[17];
extern const unsigned char OFF_ACC[12];
extern const unsigned char ASK_DRIVERINFO[17];
extern const unsigned char SET_DISTANCE_INFO[14];
extern const unsigned char RESET_PRO[14];
extern const unsigned char CONNCET_TCP[13];
extern const unsigned char INFO_ERROR[16];
extern const unsigned char INFO_DATA[6];
extern const unsigned char DISTANCE_DATA[18];
extern const unsigned char CHECK_TIME[12];
extern const unsigned char DISP_SHOW_ALARM[17];
extern const unsigned char AREA_PAR[9];
extern const unsigned char LINE_PAR[9];

extern const unsigned char LOAD_ERROR[17];


extern const unsigned char VTYPE[5];
extern const unsigned char VKIND[5];
extern const unsigned char VGROUP[6];
extern const unsigned char VID[4];
extern const unsigned char NUM1[5];
extern const unsigned char IP_ADDRESS[3];
extern const unsigned char PORT[5];
extern const unsigned char TCP_APN[4];

extern const unsigned char ON_OFF0[8];
extern const unsigned char ON_OFF1[8];
extern const unsigned char ON_OFF2[8];

extern const unsigned char FLAG[5];
extern const unsigned char TCP_TIME[4];
extern const unsigned char GSM_TIME[4];
extern const unsigned char DIS[4];


extern const unsigned char	PHONE1[3];
extern const unsigned char	PHONE2[3];
extern const unsigned char	PHONE3[3];
extern const unsigned char	PHONE4[4];
extern const unsigned char	PHONE5[4];
extern const unsigned char	PHONE6[4];
extern const unsigned char	PHONE7[11];

#endif