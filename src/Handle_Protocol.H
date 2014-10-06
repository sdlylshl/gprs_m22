/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						主控系统与HANDLE之间的协议命令

上行和下行的消息结构一样，消息头和消息尾固定，消息头 '$'0x24 表示为$的ASCII字符，
消息长度表示为整个消息包的长度，包括了消息头和消息尾。消息号即为命令字，
这里的命令字从ASCII字符开始
'1'0x31 一直往后排。

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
//--------------手柄到主控的命令字
#ifndef __Handle_Protocol_H
#define __Handle_Protocol_H

/********************************************************\
手柄到主控的命令
\********************************************************/
//手柄命令
#define			HANDLE_DIAL			0x31			//手柄拨号
#define			HANDLE_ANSWER		0x32			//手柄接听
#define			HANDLE_HANGUP		0x33			//手柄挂机
#define			HANDLE_VOL			0x34			//手柄调节音量
#define			HANDLE_DTMF			0x35			//手柄DTMF
#define			HANDLE_READ_DIAL	0x36			//手柄读最新拨出电话号码
#define			HANDLE_UP_DIAL		0x37			//手柄读前一个拨出电话号码
#define			HANDLE_DOWN_DIAL	0x38			//手柄读后一个拨出电话号码
#define			HANDLE_READ_TALK	0x39			//手柄读最新一次的通话记录
#define			HANDLE_UP_TALK		0x40			//手柄读前一个通话记录
#define			HANDLE_DOWN_TALK	0x41			//手柄读后一个通话记录
#define			HANDLE_SEND_TALK	0x42			//手柄送给主控存储的通话记录
#define			HANDLE_SET_DEFENCE	0x43			//手柄送设防请求命令
#define		HANDLE_ASK_CENTER_NUM	0x44			//手柄提取短消息中心号码（暂时取消）
#define		HANDLE_SET_CENTER_NUM	0x45			//手柄设置短消息中心号码（暂时取消）
#define			HANDLE_DEFAULT_SET	0x46			//手柄对车载终端进行出厂设置(暂时作为测试指令)

#define			HANDLE_STATUS		0x47			//手柄返回是否开机的状态
#define			HANDLE_PASSWORD		0x48			//手柄更改设置密码
#define			HANDLE_FREE_DEFENCE	0x49			//手柄解除设防命令
#define			HANDLE_PHONE_NOTE	0x50			//手柄查询指定位置的电话号码本
#define			HANDLE_ECHO			0x51			//手柄命令的返回

//测试命令
#define		HANDLE_ERASE_PAGEDATA	0x60			//通过手柄(COM3)擦除一个指定扇区页的数据
#define		HANDLE_READ_PAGEDATA	0x61			//通过手柄(COM3)读取一个指定扇区页的数据
#define		HANDLE_SET_PHONE_NUM	0x62			//通过手柄(COM3)设置目标电话号码1的命令
#define		HANDLE_CHECK_FLASH		0x63			//通过手柄(COM3)发送检查整个FLASH的命令
#define		HANDLE_CHECK_GPS		0x64			//通过手柄(COM3)发送检查GPS数据格式有效的命令
#define		HANDLE_CHECK_SMS		0x65			//通过手柄(COM3)使GSM模块自发自收一个SMS的命令
#define		HANDLED_ACTIVE_MONITOR	0x66			//通过手柄(com3)进行监听电路的调试
#define		HANDLE_ERASE_ALLPASS	0x67			//通过手柄(COM3)擦除历史轨迹的数据的记忆页
#define		HANDLE_SET_TCP			0x68			//通过手柄(COM3)设置TCP相关参数
#define		HNADLE_READ_EDITION		0x99

//测试命令1
#define		HANDLE_ERROR_NUM		0X70
#define		HANDLE_ERROR_CENTER		0X71
#define		HANDLE_ERROR_IP			0X72
#define		HANDLE_ERROR_PORT		0X73
#define		HANDLE_SET_ALARM		0X74
#define 	HANDLE_READ_SRAM		0X81
#define		HANDLE_TEST_SRAM		0x7C			//测试外部SRAM


//设置命令
#define		HANDLE_SET_PAR			0x75			//通过手柄设置基本参数
#define		HANDLE_SET_TCPIP		0x76			//通过手柄设置TCP相关的参数
#define		HANDLE_SET_TIME			0x77			//通过手柄设置时间间隔参数
#define		HANDLE_ASK_PAR			0x78			//通过手柄查询各个设置参数
#define		HANDLE_SET_SMS			0x79			//通过手柄设置预先设定的短消息
#define		HANDLE_SET_PHONE_NOTE	0x7A			//通过手柄设置存储在主控端的电话号码本
#define		HANDLE_SET_DISTANCE		0x7B

#define		HANDLE_CLEAR_DISP		0x80			//通过手柄初始化显示终端

/********************************************************\
主控到手柄的命令字极其协议包
\********************************************************/

//主控到手柄命令返回
#define			MAIN_HELLO				0X31					//握手信令
#define			MAIN_TALKING			0x32					//对方接听
#define			MAIN_HANG_UP			0x33					//对方挂机
#define			MAIN_PHONE_IN			0x34					//对方来电
#define			MAIN_BUSY				0x35					//电话正忙(占线)
#define			MAIN_STATUS				0x36					//状态数，场强，音量，设防状态，GPS的状态
#define			MAIN_TIME				0x37					//一分钟时间的定时矫正
#define			MAIN_TALKRECORD			0x38					//通话记录的返回
#define			MAIN_DIALRECORD			0x39					//拨出电话号码的返回
#define			MAIN_RETURN_NO_CALLIN	0X40					//主控返回电话禁止接听信令
#define			MAIN_RETURN_SETPASSWORD	0x41					//主控返回密码更改设置是否成功
#define			MAIN_RETURN_NO_CALLOUT	0x42					//主控返回电话禁止呼出信令
#define			MAIN_RETURN_NUM			0x43					//主控返回短消息中心号码（暂时取消）
#define			MAIN_RETURN_SETNUM		0x44					//主控返回短消息中心号码是否设置成功（暂时取消）
#define			MAIN_RETURN_INIT		0x45					//主控返回手柄初始化系统是否成功（暂时作为测试指令）
#define			MAIN_FREE_DEFENCE		0x46					//返回手柄解除设防是否成功
#define			MAIN_RETURN_PHONE_NOTE	0x47					//主控返回当前指定位置的号码本
#define			MAIN_RETURN_PHONE_ECHO	0x48					//主控返回接听，拨号，挂断的响应

//测试命令返回
#define			MAIN_ERASEPAGE_ECHO		0x60					//擦除系统指定页的ECHO
#define			MAIN_READPAGE_ECHO		0x61					//读取系统指定页的ECHO
#define			MAIN_SETNUM_ECHO		0x62					//设置目标电话号码1的ECHO
#define			MAIN_CHECKFLASH_ECHO	0x63					//检查整个FLASH的命令的ECHO
#define			MAIN_CHECHGPS_ECHO		0x64					//检查GPS数据格式的有效性的ECHO
#define			MAIN_CHECKSMS_ECHO		0x65					//检查GSM的SMS的自发自收的ECHO
#define			MAIN_ERASEPASS_ECHO		0x67					//擦除历史轨迹的记忆页的ECHO
#define			MAIN_SET_TCP_ECHO		0x68					//设置TCP相关参数的响应
#define			MAIN_SEND_STATUS		0x69
#define			HANDLE_TEST_SRAM_ECHO	0x7C
#define			HANDLE_CLEAR_DISP_ECHO	0x80


//设置命令的返回
#define		HANDLE_SET_PAR_ECHO			0x75			//通过手柄设置基本参数
#define		HANDLE_SET_TCPIP_ECHO		0x76			//通过手柄设置TCP相关的参数
#define		HANDLE_SET_TIME_ECHO		0x77			//通过手柄设置时间间隔参数
#define		HANDLE_ASK_PAR_ECHO			0x78			//通过手柄查询设置参数的返回
#define		HANDLE_SET_SMS_ECHO			0x79			//通过手柄设置预先设定的短消息的响应
#define		HANDLE_SET_PHONE_NOTE_ECHO	0x7A			//通过手柄设置存储在主控端的电话号码本的响应
#define		HANDLE_SET_DISTANCE_ECHO	0x7B

#endif