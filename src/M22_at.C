/********************************************************\
*	文件名：  M22_AT.C
*	创建时间：2005年3月22日
*	创建人：  
*	版本号：  V2.10
*	功能：	  定义一些与M22的AT指令相关的一些定义
*	文件属性：特殊文件（只与M22模块相关）
*	修改历史：（每条详述）
\********************************************************/
#include "M22_AT.h"
//初始化的一般设置
const unsigned char  AT[]={ 'A','T',0x0d };								//Communication
const unsigned char  ATZ[]={ 'A','T','Z',0x0d };						//恢复出厂时候的相关设置
const unsigned char  ATE1[]={ 'A','T','E','1',0x0d }; 					//默认，the echo should be enabled
const unsigned char  ATE0[]={ 'A','T','E','0',0x0d };
const unsigned char  AT_CGMM[]={ 'A','T','+','C','G','M','M',0x0d };	//显示表示是M22模块
const unsigned char  AT_CGMR[]={ 'A','T','+','C','G','M','R',0x0d };	//显示M22的版本号
const unsigned char  AT_CGSN[]={ 'A','T','+','C','G','S','N',0x0d };	//显示M22产品序列号
const unsigned char  AT_CMGF0[]={ 'A','T','+','C','M','G','F','=','0',0x0d };	//设置PDU模式
const unsigned char  AT_CREG[]={ 'A','T','+','C','R','E','G','?',0x0d };		//查询信号附着
const unsigned char  AT_COPS_C[]={ 'A','T','+','C','O','P','S','?',0x0d };		//查询是SIM卡的网络状态
const unsigned char  AT_CIMI[]={ 'A','T','+','C','I','M','I',0x0d };			//显示SIM卡的标识
const unsigned char  AT_CSCA_C[]={ 'A','T','+','C','S','C','A','?',0x0d };		//显示菜单的参数设置，具体见PDF
const unsigned char  AT_CSCA[]={ 'A','T','+','C','S','C','A','=' };
const unsigned char  AT_NOSLEEP1[]={ 'A','T','$','N','O','S','L','E','E','P','=','1',0x0d };	//设置模块不用处于休眠状态
const unsigned char  AT_COLP1[]={ 'A','T','+','C','O','L','P','=','1',0x0d };
const unsigned char  AT_CLIP1[]={ 'A','T','+','C','L','I','P','=','1',0x0d };
const unsigned char  AT_AUPATH[]={ 'A','T','$','A','U','P','A','T','H','=','3',',','1',0x0d };
const unsigned char  AT_AUGAIN[]={ 'A','T','$','A','U','G','A','I','N','=' };
const unsigned char  AT_CNMI[]={ 'A','T','+','C','N','M','I','=','2',',','2',',','0',',','0',',','0',0x0d };
const unsigned char  AT_TIMEOUT[]={ 'A','T','$','T','I','M','E','O','U','T','=','1','0','0','0',0x0d};
const unsigned char  AT_CMGD[]={ 'A','T','+','C','M','G','D','=','1',',','4',0x0d };

//与电话有关的AT指令
const unsigned char  ATD[]={ 'A','T','D'};				//拨号处理
const unsigned char  ATH[]={ 'A','T','H',0x0d };		//挂机AT指令
const unsigned char  ATA[]={ 'A','T','A',0x0d };		//接听AT指令
const unsigned char  AT_AUVOL[]={ 'A','T','$','A','U','V','O','L','=' };	//音量的设置,具体见PDF文档
const unsigned char  AT_CSQ[]={ 'A','T','+','C','S','Q',0x0d };
const unsigned char  AT_CLCC[]={ 'A','T','+','C','L','C','C',0x0d };			//查询Call Status Message
const unsigned char  AT_VTS[]={ 'A','T','+','V','T','S','=' };					//语音方面的DTMF的处理
//与TCP相关的指令
const unsigned char  AT_CGDCONT[]={ 'A','T','+','C','G','D','C','O','N','T','='};
const unsigned char  APN_DEFAULT[]={ '1',',','"','I','P','"',',','"','C','M','N','E','T','"'};
const unsigned char  AT_CGPCO[]={ 'A','T','%','C','G','P','C','O','=','1',',','"','P','A','P',',',',','"',',','1',0x0d };
const unsigned char  AT_DESTINFO[]={ 'A','T','$','D','E','S','T','I','N','F','O','=' };
const unsigned char  ATD_97[]={ 'A','T','D','*','9','7','#',0x0d };
const unsigned char  HANG_TCP[]={ '+','+','+' };
const unsigned char  ATO[]={ 'A','T','O',0x0d };
//与发送SMS相关的AT指令
const unsigned char  AT_CMGS[]={ 'A','T','+','C','M','G','S','=' };
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						BANQ-M22---AT指令返回常用字符说明
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
//GPS模块返回的字符串
const unsigned char String_GPRMC[]={ '$','G','P','R','M','C' };			//GPS返回数据的帧头
//M22返回的常用的字符串
const unsigned char String_OK[]={ 'O','K' };
const unsigned char String_ER[]={ 'E','R','R','O','R' };
const unsigned char String_Input_Label[]={ '>',' ' };
const unsigned char String_CLCC[]={ '+','C','L','C','C',':' };
const unsigned char String_CMGS[]={ '+','C','M','G','S',':' };
const unsigned char String_CSQ[]={ '+','C','S','Q',':' };
const unsigned char String_CSCA[]={ '+','C','S','C','A' };
const unsigned char String_CREG[]={ '+','C','R','E','G',':' };
const unsigned char String_RING[]={'R','I','N','G' };
const unsigned char String_CLIP[]={ '+','C','L','I','P',':' };
const unsigned char String_COLP[]={ '+','C','O','L','P',':' };
const unsigned char String_CMT[]={ '+','C','M','T',':'};
const unsigned char String_NOCARRIER[]={ 'N','O',' ','C','A','R','R','I','E','R' };
const unsigned char String_L_TCP[]={ 'W','a','i','t',' ','S','o','c','k','e','t',' ','O','p','e','n' };
const unsigned char String_DESTINFO[]={ '$','D','E','S','T','I','N','F','O',':' };
const unsigned char String_COPS[]={ '+','C','O','P','S',':' };
const unsigned char String_CONNECT[]={ 'C','O','N','N','E','C','T' };
const unsigned char String_BUSY[]={ 'B','U','S','Y' };
const unsigned char String_NOANSWER[]={ 'N','O',' ','A','N','S','W','E','R' };
const unsigned char String_AT[]={ 'A','T'};
const unsigned char String_END[]={ 0x0d,0x0a };
const unsigned char String_A[]={0x48,0x53,0x2e,0x33,0xb4,0xd5,0xd7,0xb7,0x0e,0xc0,0xb8,0xb1,0xb2,0xa4,0x48,0x53,
							    0x2e,0x33,0x2e,0x33,0x32,0x0b,0xab,0xa3,0xac,0xbf,0xbb,0xff,0xa5,0xf4,0xaa,0xae,
							  	0xe8,0xd4,0xd1,0xdf,0xac,0xbf,0x0b,0xab,0xa3,0xcc,0xb2,0xbb,0xd8,0xaf,0xfb,0x0b,
							  	0xab,0xa3,0xb2,0xe5,0xbb,0x31,0x36,0xeb,0xd5,0x32,0xc9,0x0e};
//程序中其他显示的字符串
//const unsigned char G20_RESET_OFF_OK[]={ 'M','2','2',' ','R','e','s','e','t','_','O','f','f',' ','i','s',' ','O','K','!',0x0d,0x0a };
const unsigned char M22_RESET_ON_OK[]={ 'M','2','2',' ','R','e','s','e','t','_','O','n',' ','i','s',' ','O','K','!',0x0d,0x0a };
//const unsigned char G20_RESET_ERROR[]={ 'M','2','2',' ','R','e','s','e','t',' ','i','s',' ','E','r','r','o','r','!',0x0d,0x0a };
//const unsigned char G20_GPRS_OK[]={ 'M','2','2',' ','G','P','R','S',' ','i','s',' ','O','K','!',0x0d,0x0a };
//const unsigned char G20_GPRS_ERROR[]={ 'M','2','2',' ','G','P','R','S',' ','i','s',' ','E','r','r','o','r','!',0x0d,0x0a };
//const unsigned char G20_HHANGUP[]={ 'H','a','n','g',' ','U','p',' ','i','s',' ','o','k',0x0d,0x0a };
//const unsigned char PAGE0_ORIGINAL[]={ 'P','a','g','e','_','0',' ','i','s',' ','O','r','i','g','i','n','a','l',0x0d,0x0a };
//const unsigned char DEVICE_DEFAULT_OK[]={ 'D','e','v','i','c','e',' ','i','s',' ','D','e','f','a','u','l','t',0x0d,0x0a };
//const unsigned char GPS_RESET_OK[]={ 'G','P','S',' ','R','e','s','e','t',' ','i','s',' ','O','K',0x0d,0x0a};
const unsigned char R_OVERTOP[]={ 'M','2','2',' ','R','e','c','e','i','v','e',' ','i','s',' ','O','v','e','r','t','o','p',0x0d,0x0a };
const unsigned char JR_OVERTOP[]={ 'G','J','L',' ','R','e','c','e','i','v','e',' ','i','s',' ','O','v','e','r','t','o','p',0x0d,0x0a };
const unsigned char CLEAR_SRAM[]={ 'C','L','E','A','R',0x0d,0x0a};
const unsigned char SYS_EDITION[]={ 0x02,0x04 };
const unsigned char SYS_TIME[]={ 0x05,0x11,0x22,0x00,0x00,0x00 };
const unsigned char SYS_UPDATATIME[]={  0x06,0x01,0x07,0x00,0x00,0x00 };

//const unsigned char LAND_TCP[]={'T','c','p',' ','L','a','n','d' };
//const unsigned char PASS_W_PAGE[]={ 'W','r','i','t','e',' ','P','a','g','e' };
//const unsigned char ALL_DISTANCEP[]={ 'A','l','l',' ','D','i','s','t','a','n','c','e' };
const unsigned char MAXSPEED[]={ 'M','a','x','s','p','e','e','d',' ','i','s',' ' };
const unsigned char LIMIT_DIS[]={ 'L','i','n','e',' ','L','i','m','i','t',' ','D','i','s'};
//const unsigned char PASS_R_PAGE[]={ 'R','e','a','d',' ','P','a','g','e' };
//const unsigned char TIME_START[]={ 'T','i','m','e',' ','S','t','a','r','t','=' };
//const unsigned char TIME_END[]={ 'T','i','m','e',' ','E','n','d','=' };
//const unsigned char PASS_SEND_ONE_PAGE[]={ 'p','a','s','s','D','a','t','a',' ','o','n','e',' ','P','a','g','e',0x0d,0x0a };
//const unsigned char PASS_END[]={ 'P','a','s','s','D','a','t','a',' ','i','s',' ','O','v','e','r',0x0d,0x0a };
const unsigned char MAINSYS[]={'M','a','i','n',' ','S','y','s','t','e','m',' ','i','s',' ','B','e','g','i','n','n','i','n','g',0x0d,0x0a };
//const unsigned char PAGE_S[]={ 'P','a','g','e',' ','S','t','a','r','t',0x0d,0x0a };
//const unsigned char PAGE_E[]={ 'P','a','g','e',' ','E','n','d',0x0d,0x0a };
//const unsigned char CHANGE_PAGE[]={ 'C','h','a','n','g','e',' ','U','P',0x0d,0x0a};
const unsigned char String_TestGPRS[]={ 'F','F','0','D'};
const unsigned char SMS_Head_1[]={ '1','1','0','0' };
const unsigned char SMS_Head_2[]={ '0','0','0','4','0','0' };
const unsigned char TYPE[]={ ' ','T','y','p','e',0x0d,0x0a };
const unsigned char String_EDIT[]={	0x4f,0x52,0x4c,0x31,0xb4,0xd7,0xd4,0xca,0x09,0xa9,0xa1,0xe5,0xbd,0xb9,0x4f,0x52,
							  		0x4c,0x31,0x55,0x2d,0x0c,0xbe,0xb6,0xb8,0xca,0xa2,0xb8,0xb8,0xb3,0xcb,0xc8,0xb4,
							  		0xd6,0xd2,0xce,0xb8,0xca,0x0c,0xbe,0xb6,0xc7,0xd3,0xae,0xd1,0xaf,0xb8,0x0c,0xbe,
									0xb6,0xc9,0xbb,0xa2,0x31,0x2f,0xc3,0x33,0xc1,0x31,0xd4,0x09};
const unsigned char TCP_QUIT[]={ 'T','C','P','-','N','O',' ','C','A','R','R','I','E','R',0x0d,0x0a };
const unsigned char TCP_TEST[]={ 'F','F','0','D',' ','R','e','t','u','r','n',0x0d,0x0a };
const unsigned char TCP_COMMAAND[]={ 'T','c','p',' ','C','o','m','m','a','n','d',' ','D','o','w','n',0x0d,0x0a };
const unsigned char TRACK[]={ 'T','r','a','c','k',' ' };
const unsigned char ALARM[]={ 'A','l','a','r','m',' ' };
const unsigned char DISTANCE[]={ 'D','i','s','t','a','n','c','e'};
const unsigned char WATCH[]={ 'W','A','T','C','H'};
const unsigned char POWER_OFF[]={ 'P','o','w','e','r',' ','O','f','f',' ','O','n','e',' ','T','i','m','e',0x0d,0x0a };
const unsigned char PHONE[]={'P','h','o','n','e',' ','i','s',' ','.','.','.','.','.','.',0x0d,0x0a };
const unsigned char Handle_VOL[]={'1','3','5','7','9'};
const unsigned char PRO_EDITION[]={ 'V','2','.','9','4',':','2','0','0','6','/','0','1','/','0','8',0x0d,0x0a };
const unsigned char SYS[]={ 'G','P','R','S','-','M','2','2',':' };

//开发商：
const unsigned char MADE[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0X0D,0X0A};
//产品型号
const unsigned char MODEL[]={0XB2,0XFA,0XC6,0XB7,0XD0,0XCD,0XBA,0XC5};
//开发日期
const unsigned char	DESIGN_DATA[]={0XBF,0XAA,0XB7,0XA2,0XC8,0XD5,0XC6,0XDA};
//
const unsigned char EDIT_DATE[]={':','2','0','0','5','/','1','2','/','2','0',0x0d,0x0a };
//软件版本
const unsigned char SOFT_EDITION[]={0XC8,0XED,0XBC,0XFE,0XB0,0XE6,0XB1,0XBE};
const unsigned char GPS_2005A[]={':','G','P','S','-','M','2','2',' ','V','3','.','9','5',0x0d,0x0a};

//版本更新日期：
const unsigned char UPDATE_TIME[]={'U','P','D','A','T','E',' ',' ',':','2','0','0','5','/','1','2','/','2','0',0x0d,0x0a };
//开发者：
const unsigned char AUTHOR[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0X0D,0X0A};
//车载GPS小终端
const unsigned char MODEL_TYPE1[]={0XB3,0XB5,0XD4,0XD8,0X47,0X50,0X53,0XD0,0XA1,0XD6,0XD5,0XB6,0XCB };
//车载GPS	GPRS-M22-2005A
const unsigned char MODEL_TYPE2[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0X32,0X30,0X30,0X35,0X41,0X0D,0X0A};


const unsigned char TCP_LAND_ERROR[]={ 'T','C','P',' ','L','A','N','D',' ','E','R','R','O','R',0x0d,0x0a };
const unsigned char RECEIVE1[]={ 'R','e','c','e','i','v','e',' ','T','y','p','e',' ','1',0x0d,0x0a };
const unsigned char RECEIVE2[]={ 'R','e','c','e','i','v','e',' ','T','y','p','e',' ','2',0x0d,0x0a };
const unsigned char GET_DRIVERINFO_OK[]={ 'G','E','T',' ','D','R','I','V','E','R',' ','I','S',' ','O','K',0x0d,0x0a };
const unsigned char SEND_DRIVERINFO_OK[]={'S','E','N','D',' ','D','R','I','V','E','R',' ','I','S',' ','O','K',0x0d,0x0a};
const unsigned char CENTER_RELOAD[]={ 'C','e','n','t','e','r',' ','N','u','m',' ','L','o','a','d',0x0d,0x0a};
const unsigned char OFF_ACC[]={ 'A','C','C',' ','I','S',' ','O','F','F',0x0d,0x0a };
const unsigned char ASK_DRIVERINFO[]={ 'A','S','K',' ','D','R','I','V','E','R',' ','I','N','F','O',0x0d,0x0a};
const unsigned char SET_DISTANCE_INFO[]={ 'S','E','T',' ','D','I','S','T','A','N','C','E',0x0d,0x0a};
const unsigned char RESET_PRO[]={'R','E','S','E','T',' ','M','S','P','4','3','0',0X0D,0X0A};
const unsigned char CONNCET_TCP[]={ 'C','O','N','N','E','C','T',' ','T','C','P',0x0d,0x0a};
const unsigned char INFO_ERROR[]={ 'S','e','c','t','o','r',' ','i','s',' ','E','r','r','o','r',' '};
const unsigned char INFO_DATA[]={ 'S','e','c','t','o','r' };
const unsigned char DISTANCE_DATA[]={ 'D','i','s','t','a','n','c','e',' ','R','e','c','e','i','v','e',0x0d,0x0a };
const unsigned char CHECK_TIME[]={ 'C','h','e','c','k',' ','T','i','m','e',0x0d,0x0a};
const unsigned char DISP_SHOW_ALARM[]={ 'D','I','S','P',' ','S','H','O','W',' ','A','L','A','R','M',0x0d,0x0a};
const unsigned char AREA_PAR[]={ 'A','r','e','a','_','P','a','r',':'};
const unsigned char LINE_PAR[]={ 'L','i','n','e','_','P','a','r',':'};
const unsigned char LOAD_ERROR[]={ 'L','i','n','e',' ','D','a','t','e',' ','E','r','r','o','r',0x0d,0x0a };

const unsigned char VTYPE[]={ 'T','y','p','e','='};
const unsigned char VKIND[]={ 'K','i','n','d','='};
const unsigned char VGROUP[]={ 'G','r','o','u','p','='};
const unsigned char VID[]={ 'V','i','d','='};
const unsigned char NUM1[]={ 'N','u','m','1','='};
const unsigned char IP_ADDRESS[]={ 'I','P','='};
const unsigned char PORT[]={ 'P','o','r','t','='};
const unsigned char TCP_APN[]={ 'A','P','N','='};

const unsigned char ON_OFF0[]={ 'O','n','/','O','f','f','0','='};
const unsigned char ON_OFF1[]={ 'O','n','/','O','f','f','1','='};
const unsigned char ON_OFF2[]={ 'O','n','/','O','f','f','2','='};

const unsigned char FLAG[]={ 'F','l','a','g','='};
const unsigned char TCP_TIME[]={ 'T','c','p','='};
const unsigned char GSM_TIME[]={ 'G','s','m','='};
const unsigned char DIS[]={ 'D','i','s','='};


//相关不受限制的电话号码
const unsigned char	PHONE1[]={'1','1','0'};
const unsigned char	PHONE2[]={'1','1','2'};
const unsigned char	PHONE3[]={'1','1','9'};
const unsigned char	PHONE4[]={'1','8','6','0'};
const unsigned char	PHONE5[]={'1','8','6','1'};
const unsigned char	PHONE6[]={'1','0','0','1'};
const unsigned char	PHONE7[]={'1','3','8','0','0','1','3','8','0','0','0'};