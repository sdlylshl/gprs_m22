/********************************************************\
*	文件名：  Other_Define.H
*	创建时间：2005年3月22日
*	创建人：  
*	版本号：  V2.1
*	功能：	  有关方面的一些定义（具体可看详细细节）

*	文件属性：公共文件
*	修改历史：（每条详述）
	补充说明：根据M22模块相关的一些管脚设置

\********************************************************/


#ifndef __Other_Define_H
#define __Other_Define_H
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			一些控制管脚的定义
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

//版本V3.8
#define		WDI					0x01		//P6.0  out 喂狗操作，翻转外部看门狗的输入电平管脚
#define		POWER_ON_OFF		0x40		//P6.6  out 控制GPS，GSM电源开关
#define		STOP_Q1				0x20		//P6.5  out 执行制动的控制管脚1
#define		STOP_Q2				0x10		//P6.4  out 执行制动的控制管脚2
#define		ACC_TEST			0x01		//P3.0	in	检测车载ACC电平
#define		ARALM				0x40		//P2.6	in  检测是否有被劫报警的管脚
#define		DISP_TEST			0x08		//P6.3	in	检测是否允许发送显示凭数据的管脚
#define		SEL_MIC				0x80		//P6.7  out	选择语音通道
#define		MCU_STATUS			0x10		//P1.4	out
#define		RING_TEST			0x01		//P1.0	in	作为检测在TCP在线的状态下，是否有电话打入
#define		NL_CHK				0x10		//P2.7	in  空载/重载的检测线
#define		CHECK_ON_OFF		0x20		//P2.5	in  制动系统执行结果检测

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				16位针对下行带ACK的SMS，在需要响应上行的SMS中带上ACK的图表
				数据存储在gSMS_ACK[]相应的数据中
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define			TRACK_ASKECHO_ACK		0			//轨迹查询响应--定位数据的ACK
#define			ALARM_ACK				1			//车辆报警ACK
#define		RECORD_EQUIPMENT_ACK		2			//外接部件的ACK命令
#define		SET_PHONE_LIMIT_ECHO_ACK	3			//中心设置电话权限设置的响应的ACK

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			    语音音量的档位分类
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define			ONE						1
#define			TWO						2
#define			THREE					3
#define			FOUR					4

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			其他一些常量的定义
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		DIS_LON				28.67
#define		DIS_LAT				30.92


#define		OFF_TIME			15000		//关断2576的时间（ 毫秒）
#define		DELAY_TIME			10000		//开启2576后的延迟时间（毫秒）

#define		M22_NULECHO_TIME	200			//G20 在200秒内无任何返回，则需要复位G20
#define		M22_LAND_TIME		30			//如果未成功收到下行的登陆信息，则每隔30秒进行登陆发送
#define		TCP_TEST_TIME		60			//在GPRS正常在线状态下，发送一个数据包(会有返回GPRS的数据包)的间隔（秒）
#define		ALARM_CIRCLE_TIME	60			//1分钟的报警周期
#define		GPS_INVALID_COUNT	3600 		//gps数据如果超过个小时仍然是无效点，则需要将GPS模块进行复位操作
#define		NO_OUTPUT_TIME		40			//统计GPS无任何输出的时间量单位为秒
#define		GPS_LOW_TIME		10			//gps的RESET管脚持续低电平的时间（秒）
#define		STEAL_ALARM_COUNT	30			//满足被盗报警有速度大于3.6*4的连续点数的计数
#define		SEARCH_PASS_TIME	200			//在历史轨迹查询的过程中，需要间隔500毫秒进行的下一页的查询
#define		ROB_TIME			3000		//表示按下被劫报警开关的持续时间（单位为毫秒）
#define		HANDLE_OUT_TIME		130			//如果持续计时超过5分钟，无收到手柄的任何命令，则认为手柄处于被摘除状态
#define		DISP_OUT_TIME		130			//如果持续4分钟，无收到显示终端的下行命令，则认为显示终端处于关闭状态
#define		ALLOW_STOP_SPEED	3			//允许进行制动的速度
#define		OVER_TESTSMS_TIME	300			//测试SMS的发送的超过5分钟
#define		OVER_TCP_TIME		200			//超过200秒未接收到下行的握手数据
#define		GSM_CHECKCSQ_TIME	30			//在GSM工作方式下查询CSQ的时间间隔30秒
#define 	TCP_CHECKCSQ_TIME	200			//在TCP工作方式下查询CSQ的时间间隔200秒
#define		TCP_TRACK_MINTIME	1200		//如果TCP的行车轨迹是否长期在线的分水岭
#define		RING_TEST_TIME		220			//在TCP在线的状态下，检测电平的时间
#define		RELAND_TCP_TIME		180			//4分钟的时间间隔进行下一次的TCP的登陆

#define		TEST_ACC_TIME		5000		//持续检测ACC的检测时间
#define		ACC_LASTOFF_TIME	600//1800    	//检测ACC关闭持续超过1小时则关闭两模块的工作
#define		ACC_CHANGEOFF_TIME	240			//检测ACC关闭超过20分钟，则改变行车轨迹的上传间隔
#define		ACCOFF_TRACK_TIME	1800		//作为ACC关闭后，需要转换行车轨迹上传点的分界点，半个小时一上传数据点
#define		DISTANCE_COMPASTATE	12			//作为暂时的GPS数据无效果，并且ACC处于开启状态下的里程统计的补偿
#define		INTER_ATRA_TEST		2			//作为检测区域报警的时间间隔
#define		ALLOW_PHONE_TIME	1800		//允许进行30分钟的通话
//#define	 GET_DRIVER_INFO_TIME	10			//如果没有成功记录仪的驾驶员信息，则需要周期发送的时间间隔(单位秒)
//#define	 WAIT_RETURNINFO_TIME	30			//向记录仪发送查询驾驶员信息后，等待响应的时间
#define	 READY_SENDINFO_TIME	5			//准备向显示屏发送驾驶员信息的等待时间
#define  WAIT_DISPINFO_TIME		10			//
#define		RSET_GPS_TIME		60			//如果GPS超过60秒，输出格式不正确
#define	 OVER_RECORD_DATA_TIME	300			//超过300秒没有接收到记录仪的时间
#define	 MAX_TCP_DATA_LENGHT	1000		//最大发送TCP的数据的长度
#define		RLAOD_FLASH_TIME	36000		//10个小时不管处于什么状态，需要重新启动程序
#define		MAX_LING_DISTANCE	300			//线路偏离的最大限制距离（米）
#define		ALARM_COUNT			10			//允许通过SMS发送报警数据的次数
#define		HANGUP_COUNT		40			//如果40秒内无任何接听或者挂断命令处理，则主动挂断电话

#define		SMS_DATA_TIME		1800		//如果TCP不在线的，通过SMS方式上报数据的间隔20分钟
#define		CHECK_SYS_PAR_TIME	1800
//与GSM操作有关的步骤变量
#define		GSM_PHONE			1			//与电话有关联的相关处理
#define		GSM_OTHER			2			//与其他如CLCC，CSQ，CSCA相关的处理
#define		GSM_SMS_IN			3			//与GSM接收数据有关的相关处理
#define		GSM_SMS_OUT			4			//与GSM发送数据有关的相关处理
#define		LAND_TCP_OPER		5			//与定期检查GPRS网络有关的相关处理
#define		GPRS_DATA_OPER		6			//与通过GPRS网络发送数据的相关处理
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			手柄状态的定义（M22的相关状态变量）
			unsigned char  gPhone_Status=0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#define 	READY				0			//待机状态
#define 	INCOMING			1			//来电状态
#define 	DAILING				2			//正在拨号状态(包括手柄，和监听拨号状态)
#define 	ACTIVE_HANDLE		3			//手柄正在通话状态
#define 	ACTIVE_MONITOR		4			//监听状态

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			车载GPS与记录仪之间的通讯类型
			unsigned char  gStatus=0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		NORMAL_TYPE			0			//处于正常车载GPS与记录仪通讯方式
#define		DISP_TYPE			2			//处于显示终端与记录仪通讯方式
#define		CENTER_TYPE			1			//处于中心下行与记录仪通讯方式

//存储在buffer1的0x30---0x4f相关的关于手柄的AT指令的标志，存储在0x31
#define		H_DIAL				0x01		//ATD指令的标志
#define		H_ANSWER			0x02		//ATA指令的标志
#define		H_HUNGUP			0x03		//ATH指令的标志
#define		H_VOLADJ			0x04		//音量调节的标志
#define		H_DTMF				0x05		//DTMF的标志
#define		M_DIAL				0x06		//监听指令
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			报警类型的定义存储报警类型的变量gFlag 为char型，因为最高位被用做来表示
			GPS数据的有效和无效标志，则能表示报警类型的是后7位，故最多能表示127种类型的报警
			unsigned char gAlarm_Type=0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		NOP_ALARM			0			//无任何报警信息
#define		ROB_ALARM			1			//被劫报警
#define		STEAL_ALARM			2			//被盗报警
#define		OVER_AREA_ALARM		3			//区域报警
#define		OVER_SPEED_ALARM	4			//超速报警
#define		OVER_TIRE_ALARM		10			//疲劳驾驶报警
#define		OVER_LINE_ALARM		5			//线路偏离报警
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			存储在Circle_Buffer[]中需要周期发送的数据的类型
			变量：unsigned char gCycle_Send_Status=0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		NOP_DATA			0			//不属于任何类型的数据
#define		TRACK_DATA_TIME		1			//属于按时间上传行车轨迹点的数据
#define		TRACK_DATA_DISTANCE	2			//属于按间隔距离上传轨迹点的数据
#define		ALARM_DATA			3			//属于报警数据
#define		WATCH_DATA			4			//属于监控数据
#define		PASS_DATA			5			//属于历史轨迹数据
#define		SAVE_DATA			6			//属于盲区补偿数据
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			表示监控的类型
			变量：unsigned char gWatch_Type=0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		WATCH_TYPE_NULL			0			//表示无任何类型的监控
#define		WATCH_TYPE_TIME 		1			//表示定时监控类型
#define		WATCH_TYPE_COUNT		2			//表示定量监控类型

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			表示接收到TCP下行的进入/退出联网状态的下行命令
			变量：unsigned char gTCP_Status_Temp=0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		TCP_STATUS_READY				0			//表示未接收到此命令或者命令处理完之后的
#define		TCP_STATUS_LAND_ECHO			1			//表示接收到此命令后，需要开启登陆TCP的标志
#define		TCP_STATUS_QUIT_ECHO			2			//表示接收到此命令后，需要退出登陆的TCP标志
#define		TCP_STATUS_ALLOW_LAND			3
#define		TCP_STATUS_ALLOW_QUIT			4
#define		TCP_STATUS_QUIT_READY			5

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			表示设防状态的常量
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		DEFENCE_NO						0			//表示车载终端未处于设防状态
#define		DEFENCE_YES						1			//表示车载终端已经处于设防状态

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			表示命令返回ECHO的常量
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		COMMAND_ERROR					0			//表示命令处理返回不成功
#define		COMMAND_OK						1			//表示命令处理返回成功

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			表示M22当前状态的常量  gM22_Status
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		GSM_WORK						0			//M22处理GSM的工作方式下
#define		TCP_HANG_WORK					1			//M22处于TCP已经登陆上，但处于挂起状态
#define		TCP_ONLINE_WORK					2			//M22处于TCP已经登陆上，处于非挂起状态
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			表示M22当前状态的常量  gM22_Stop_Status
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#define		NOP_READY						0			//针对制动处于待机状态
#define		ALLOW_SET_STOP					1			//接收到下行的允许制动的命令后的状态
#define		ASK_SET_STOP					2			//执行制动后，检测制动结果的状态
#define		ALLOW_FREE_STOP					3			//接收到下行的允许解除制动的命令后的状态
#define		ASK_FREE_STOP					4			//执行解除制动后，检测解除制动结果后的状态
#define		STOP_STATUS						5			//车载设备已经处于正常的断油断电的状态

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			表示M22当前状态的常量  gDriver_Info
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#define		READY_READ_DRIVER_INFO			0
#define		WAIT_RETURN_DRIVER_INFO			1
#define		GET_DRIVER_INFO					2
#define		WAIT_SEND_DRIVER_INFO			3
#define		SEND_DRIVER_INFO_OK				4

#endif
