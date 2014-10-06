/********************************************************\
*	文件名：  W_Protocol.H
*	创建时间：2004年12月7日
*	创建人：  
*	版本号：  1.00
*	功能：	  定义车载终端与系统中心之间的无线协议
*	文件属性：公共文件
*	修改历史：（每条详述）

\********************************************************/

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						GPS智能监控调度系统---通讯协议说明书：
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
一，通讯协议的格式定义：
1.1 消息结构
	字节序			字段名			长度(Byte)			说明
	0				SOH				1					消息头
	1				MSG#			1					消息号
	2				CMPL-MSG#		1
	3				Length			1					消息长度
	4				ACK				1					响应符
	5				Class			1					类别
	6				gGroup			1					组别
	7				gVID(MSB)		1
	8				gVID(LSB)		1					车号
	9				Data Block		N					数据块，不定长
	9+N				CHK(MSB)		1
	10+N			CHK(LSB)		1					校验和
	11+N			EOT				1					消息尾

1.2 协议说明
	上行和下行的消息均采用相同的通信格式，数据传输采用GPRS格式和SMS格式，采用SMS方式发送的数据
	包最长140个字节。

	SOH：消息的第一个字节 0xFF;
	ACK: 响应字符，返回的消息中必须包括该命令的ACK；
	Length:数据包长度，自SOH开始到CHK以前的数据的字节数；
	[Class]+[gGroup]+[gVID],共4个字节，表示一个完整的车辆识别码
	CHK：16-bit校验和，将自SOH开始到CHK以前的所有无符号8-bit字符的内容相加而得
	对于消息中于一个字节的数据，均采用“高位在前，低位在后”的顺序存放
	EOT：为消息的最后一个字节0x0D

二，下行消息传输协议
	//////////////////////////////////////////////////////////////////////////
	序号					消息号					消息名称

	1						0x21					即时应召请求
	2						0x23					即时应召派车
	3						0x25					预约应召请求
	4						0x27					预约应召派车
	5						0x41					车辆监控
	6						0x43					定位查询
	7						0x45					诡迹查询
	8						0x47					车辆监听
	9						0x49					报警应答
	10						0x4b					文字信息发布
	11						0x4d					密码提取请求
	12						0x4f					车辆登陆响应
	13						0x61					设置行车区域
	14						0x63					设置城域范围
	15						0x65					电话权限设置
	16						0x67					车辆参数设置
	17						0x81					出城登记响应
	18						0x82					解除被盗报警
	19						0x8a					请求监控应答
	20						0x8e					制动命令
	21						0x8f					解除制动命令
	22						0xa1					设置上传位置时间间隔
	23						0xa5					事故疑点数据查询
	24						0xa9					终端自检报告查询
	25						0xab					带反馈报文
	26						0xaf					设置动态菜单
	27						0xb1					带工号登陆响应
	28						0xb3					带工号签退响应
	29						0xb5					设置车载终端工作方式
	30						0xb7					设置计价器工作方式
	31						0xb9					下载电话号码本
	32						0xbb					设置中心号码
	33						0xbd					通话记录数据反馈
	34						0xc3					设置车辆启动开关
	35						0xc5					设置最大时速
	36						0xc7					中心强制设防状态
	37						0xcb					设置服务器地址，端口号
	38						0xcf					更新车载终端程序
	39						0xd1					车载终端的版本查询
	40						0xd3					设置驾驶员列表
	41						0xd5					车辆里程查询
	42						0xd7					计价器即时营运数据响应
	43						0xd9					计价器参数数据查询
	44						0xdb					计价器历史营运数据查询
	45						0xdd					计价器校时
	46						0xdf					计价器程序更新
	47						0xe1					刷卡机交易数据响应
	48						0xe3					刷卡机历史交易数据查询
	49						0xe5					刷卡机交易打包数据查询
	50						0xe7					下载刷卡机黑名单
	51						0xe9					刷卡机程序更新
	/////////////////////////////////////////////////////////////////////////
三，上行消息传输协议
	//////////////////////////////////////////////////////////////////////////
	序号					消息号					消息名称

	1						0x21					即时应召响应
	2						0x23					接客成功
	3						0x25					预约应召响应
	4						0x41					车辆监控响应--监控数据
	5						0x43					定位查询响应
	6						0x45					诡迹查询响应--定位数据
	7						0x46					诡迹查询响应--数据发送结束
	8						0x49					车辆报警
	9						0x4d					车辆密码提取响应
	10						0x4f					车辆登陆信息
	11						0x61					行车区域设置响应
	12						0x65					电话权限设置响应
	13						0x67					车辆参数设置响应
	14						0x81					出城登记
	15						0x83					车辆退出登陆
	16						0x85					车辆求助
	17						0x87					路阻信息
	18						0x89					事故信息
	19						0x8a					请求监控
	20						0x8b					取消监控
	21						0x8c					请求解除监控
	22						0x8d					报警结束
	23						0x8e					制动命令响应
	24						0x8f					解除制动命令响应
	25						0xa1					行驶状态数据
	26						0xa5					事故疑点数据
	27						0xa9					自检报告
	28						0xab					报文阅读回报
	29						0xaf					动态菜单设置响应
	30						0xb1					带工号登陆
	31						0xb3					带工号签退
	32						0xb5					设置车载终端工作方式响应
	33						0xb7					设置计价器工作方式响应
	34						0xb8					请求下载电话号码本
	35						0xb9					下载电话号码本响应
	36						0xbb					中心号码接收响应
	37						0xbd					通话记录数据
	38						0xc3					车辆启动开关设置响应
	39						0xc5					设置最大时速响应
	40						0xc7					中心强制设防状态响应
	41						0xcb					设置服务器地址，端口号响应
	42						0xd1					车载终端的版本数据
	43						0xd3					车载驾驶员列表设置响应
	44						0xd5					车辆里程数据
	45						0xd7					计价器即时营运数据
	46						0xd9					计价器参数数据
	47						0xdb					计价器历史营运数据
	48						0xdd					计价器校时响应
	49						0xdf					计价器程序更新响应
	50						0xe1					刷卡机交易数据
	51						0xe3					刷卡机历史交易数据
	52						0xe5					刷卡机交易打包数据
	53						0xe7					刷卡机黑名单下载响应
	54						0xe9					刷卡机程序更新响应
	/////////////////////////////////////////////////////////////////////////

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#ifndef __W_Protocol_H
#define __W_Protocol_H

#define			SOH                     0xff		//消息头
#define 		EOT                     0x0d		//消息尾
//下行消息传输协议的定义
#define			MUSTER_ASK_DOWN			0x21		//即时应召请求
#define			MUSTER_SEND_DOWN		0x23		//即时应召派车
#define			PREMUSTER_ASK_DOWN		0x25		//预约应召请求
#define			PREMUSTER_SEND_DOWN		0x27		//预约应召派车
#define			VEHICLE_WATCH_DOWN		0x41		//车辆监控
#define			POSITION_ASK_DOWN		0x43		//定位查询
#define			TRACK_ASK_DOWN			0x45		//轨迹查询
#define			VEHICLE_MONITOR_DOWN	0x47		//车辆监听
#define			ALARM_ECHO_DOWN			0x49		//报警应答
#define			WORD_INFORMATION_DOWN	0x4b		//文字信息发布
#define			PASSWORD_ASK_DOWN		0x4d		//密码提取请求
#define			VEHICLE_ENTRY_ECHO_DOWN	0x4f		//车辆登录响应
#define			SET_ALLOW_AREA_DOWM		0x61		//设置行车区域
#define			SET_DISTANCE_DATA_DOWN	0x68		//设置间隔距离上传数据点的下行命令

#define			SET_PHONE_LIMIT_DOWN	0x63		//电话权限设置
#define			PARAMETER_SET_DOWN		0x67		//车辆参数设置
#define			OUTCITY_ECHO_DOWN		0x81		//出城登记响应
#define			FREE_STRAL_ALARM_DOWN	0x82		//解除被盗报警
#define		ALLOW_TCP_QUIT_ECHO_DOWN	0x83		//车辆退出TCP联网状态
#define			ASK_WATCH_ECHO_DOWN		0x8a		//请求监控应答
#define			SET_STOP_COMMAND_DOWN	0x8e		//制动命令
#define			FREE_STOP_COMMAND_DOWN	0x8f		//解除制动命令
#define			TIME_INTERVAL_DOWN		0xa1		//设置上传位置时间间隔
#define			ACCIDENT_DATA_ASK_DOWN	0xa5		//事故疑点数据查询
#define		DEVICE_CHECK_REPORT_DOWN	0xa9		//终端自检报告查询
#define			MESSAGE_FEEDBACK_DOWN	0xab		//带反馈的报文
#define			SET_MENU_DOWN			0xaf		//设置动态菜单
#define			LAND_ECHO_DOWN			0xb1		//带工号登陆响应
#define			QUIT_ECHO_DOWN			0xb3		//带工号签退响应
#define			SET_DEVICE_WORKING_DOWN	0xb5		//设置车载终端工作方式
#define			SET_TAXI_WORKING_DOWN	0xb7		//设置计价器工作方式
#define			CARRY_PHONE_NUM_DOWN	0xb9		//下载电话号码本
#define			SET_CENTER_NUM_DOWN		0xbb		//设置中心号码
#define			CALL_LOG_ECHO_DOWN		0xbd		//通话记录数据反馈
#define			VEHICLE_STARTUP_DOWN	0xc3		//设置车辆启动开关
#define			SET_MAXSPEED_DOWN		0xc5		//设置最大时速
#define			FORCE_FORTIFY_DOWN		0xc7		//中心强制设防状态
#define			SET_ADDRESS_PORT_DOWN	0xcb		//设置服务器地址，端口
#define			SET_TCP_ONOROFF_DOWN	0xcd		//设置终端进入/退出联网的状态
#define			UPGRADE_SYSTEM_DOWN		0xcf		//更新车载终端程序
#define			QUERY_VERSION_DOWN		0xd1		//车载终端的版本查询
#define			SET_DRIVER_LIST_DOWN	0xd3		//设置驾驶员列表
#define			SET_DISTANCE_INIT_DOWN	0xD5		//设置里程初始化的值
#define			EXTER_EQUIPMENT_DOWN	0xd7		//外接部件数据的命令
#define			TEST_SMS_COMMAND_DOWN	0x10		//作为接收到测试命令字
#define			SET_SMS_COMMAND_DOWN	0xae		//作为短消息固定设置的下行命令，主控转发给显示终端
#define			LINE_DATA_DOWN			0x65		//线路数据的下行命令
#define		LINE_LIMIT_DISTANCE_DOWN	0x66		//线路数据的限制距离的设置
/*
#define			TAXI_DATA_ECHO_DOWN		0xd7		//计价器即时营运数据响应
#define			TAXI_PARAMETER_DOWN		0xd9		//计价器参数数据查询
#define			TAXI_PASTDATA_ECHO_DOWN	0xdb		//计价器历史营运数据查询
#define			TAXI_CHECKOUT_DOWN		0xdd		//计价器校时
#define			TAXI_UPGRADE_DOWN		0xdf		//计价器程序更新
#define			CARD_DATA_ECHO_DOWN		0xe1		//刷卡机交易数据响应
#define			CARD_PASTDATA_ECHO_DOWN	0xe3		//刷卡机历史交易数据查询
#define			CARD_DEALDATA_ECHO_DOWN	0xe5		//刷卡机交易打包数据查询
#define			LOAD_BLACKLIST_DOWN		0xe7		//下载刷卡机黑名单
#define			CARD_UPGRADE_DOWN		0xe9		//刷卡机程序更新
*/

/////////////////////////////////////////////////////////////////////////////
//上行消息传输协议的定义：
#define			MUSTER_ASK_UP			0x21		//即时应召响应:20
#define			MUSTER_SUCCESS_UP		0x23		//接客成功:12
#define			PREMUSTER_ECHO_UP		0x25		//预约应召响应:20

#define			VEHICLE_WATCHECHO_UP	0x41		//车辆监控响应--定位数据:不定
#define			POSITION_ASKECHO_UP		0x43		//定位查询响应:25
#define			TRACK_ASKECHO_UP		0x45		//轨迹查询响应--定位数据:不定
#define			TRACK_ENDECHO_UP		0x46		//轨迹查询响应--数据发送结束:8
#define			ALARM_UP				0x49		//车辆报警:不定
#define			PASSWORD_ASKECHO_UP		0x4d		//车辆密码提取响应:16
#define			VEHICLE_ENTRY_UP		0x4f		//车辆登录信息:8
#define			ALLOW_AREA_ECHO_UP		0x61		//行车区域设置响应:8
#define			PHONE_LIMIT_ECHO_UP		0x65		//电话权限设置响应:8
#define			PARAMETER_SETECHO_UP	0x67		//车辆参数设置响应:8
#define			OUTCITY_UP				0x81		//出城登记:30
#define			VEHICLE_QUIT_UP			0x83		//车辆退出登录:8
#define			VEHICLE_HELP_UP			0x85		//车辆求助:22
#define			VEHICLE_ROAD_UP			0x87		//路阻信息:22
#define			VEHICLE_ACCIDENT_UP		0x89		//事故信息:22
#define			ASK_WATCH_UP			0x8a		//请求监控:不定
#define			ASK_FREEWATCH_UP		0x8b		//取消监控:8
#define			ASK_WATCHEND_UP			0x8c		//请求解除监控:8
#define			ALARM_END_UP			0x8d		//报警结束:8
#define		SET_STOP_COMMAND_ECHO_UP	0x8e		//制动命令响应:15
#define		FREE_STOP_COMMAND_ECHO_UP	0x8f		//解除制动命令响应:15
#define			VEHICLE_RUNDATA_UP		0xa1		//行驶状态数据:不定
#define			TIME_INTERVAL_ECHO_UP	0xa3		//设置时间上传间隔的响应：30
#define			ACCIDENT_DATA_UP		0xa5		//事故疑点数据:不定
#define		    DEVICE_CHECK_REPORT_UP	0xa9		//自检报告:12
#define			MESSAGE_FEEDBACK_UP		0xab		//报文阅读回报:19
#define			SET_MENU_ECHO_UP		0xaf		//动态菜单设置响应:8
#define			LAND_UP					0xb1		//带工号登陆:15
#define			QUIT_UP					0xb3		//带工号签退:15
#define			DEVICE_WORKINGECHO_UP	0xb5		//设置车载终端工作方式响应:9
#define			TAXI_WORKINGECHO_UP		0xb7		//设置计价器工作方式响应:9
#define			ASK_PHONE_NUM_UP		0xb8		//请求下载电话号码本:8
#define			ASK_PHONE_NUMECHO_UP	0xb9		//下载电话号码本响应:8
#define			CENTER_NUMECHO_UP		0xbb		//中心号码接收响应:8
#define			CALL_LOG_UP				0xbd		//通话记录数据:12
#define			VEHICLE_STARTUP_ECHO_UP	0xc3		//车辆启动开关设置响应:9
#define			SET_MAXSPEED_ECHO_UP	0xc5		//设置最大时速响应:8
#define			FORCE_FORTIFY_ECHO_UP	0xc7		//中心强制设防状态响应:12
#define			ADDRESS_PORT_ECHO_UP	0xcb		//设置服务器地址，端口响应：不定
#define			SET_TCP_ONOROFF_ECHO_UP	0xcd		//针对下行的设置终端进入/退出联网的响应
#define			QUERY_VERSION_ECHO_UP	0xd1		//车载终端的版本数据:22
#define			DRIVER_LIST_ECHO_UP		0xd3		//车载驾驶员列表设置响应:8
#define			DISTANCE_INIT_ECHO_UP	0xD5		//设置里程初始化的值
#define			EXTER_EQUIPMENT_UP		0xd7		//外接部件数据的命令

#define			DISP_ASK_INFO_UP		0x84		//显示终端的信息上报请求
#define			DISP_SMS_INFO_UP		0x86
#define			DISP_INFO_ECHO_UP		0x4b		//显示终端的信息上报请求响应
#define			SET_PHONE_LIMIT_ECHO_UP 0x63
#define			LINE_DATA_ECHO_UP		0x65
#define	 LINE_LIMIT_DISTANCE_ECHO_UP	0x66


/*
*/
#define			SET_DISTANCE_DATA_UP	0x68		//设置间隔距离的响应
#define			ASK_PAR_SET_UP			0xF0

#define			TEST_SMS_COMMAND_UP		0x10		//在测试SMS中，固定作为测试数据的命令字
#endif
