/********************************************************\
*	文件名：  Define_Bit.H
*	创建时间：2004年12月7日
*	创建人：
*	版本号：  1.00
*	功能：	  有关一些BIT位的标志的定义
*	文件属性：公共文件
*	修改历史：（每条详述）
	2004年12月14日：修改

\********************************************************/
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			定义位的操作：
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#ifndef __Define_Bit_H
#define __Define_Bit_H

//一，unsigned char  gON_OFF_Temp0=0
/********************************************************\
说明：存储8个开关量(对应扇区0中的0x00处存储的一个字节的数据)
\********************************************************/
#define		TCP_LAND_ON_OFF_1			BIT0				//远程设置终端进入/退出联网状态的开关量
#define		VEH_RUN_ON_OFF_1			BIT1				//定时上传车辆行驶状态数据的开关量
#define		STOP_ON_OFF_1				BIT2				//车载终端是否处于制动状态，为1默认为未处于制动，为0则表示处于制动
#define		DTMF_ALARM_ON_OFF_1			BIT3				//DTMF电话报警的开关量
#define		OVER_SPEED_ON_OFF_1			BIT4				//超速报警的开关量
#define		ONEAREA_ALARM_ON_OFF_1		BIT5				//单区域进出报警的开关量
#define		PHONE_IN_ON					BIT6				//车载电话呼入权限的开关量	1为默认开启,0表示关闭
#define		PHONE_OUT_ON				BIT7				//车载电话呼出权限的开关量	1为默认开启,0表示关闭

//二，unsigned char  gON_OFF_Temp1=0
/********************************************************\
说明：存储8个开关量(对应扇区0中的0x01处存储的一个字节的数据
\********************************************************/
#define	 	DISTANCE_TCP_ON_1			BIT0				//表示间隔距离传输数据的方式采用常在线的方式
//三，unsigned char  gON_OFF_Temp2=0
/********************************************************\
说明：存储8个开关量(对应扇区0中的0x02处存储的一个字节的数据
\********************************************************/
/********************************************************\
补充说明：前面三个gON_OFF_Temp0，gON_OFF_Temp1，gON_OFF_Temp2
这三个存储开关变量的值，初始值均为0xFF,如果相应位置1则表示
此开关量处于屏蔽状态，如果设置为0则表示此开关量处于开启状态
\********************************************************/
//四，unsigned int gGeneral_Flag
/********************************************************\
说明：
\********************************************************/
#define		RESET_GSM_ON_F_1		BIT0		//复位G20的标志
#define		TCP_STATUS_F_1			BIT1
#define		NUM1_VAILD_F_1			BIT2		//目标号码有效的标志
#define		OFF_M22_F_1				BIT3		//掉电G20的标志
#define		OFF_GPS_F_1				BIT4		//掉电GPS的标志

#define		HANG_M22_TCP_F_1		BIT5		//表示M22需要进行挂起操作
#define		MONITOR_RESUME_F_1		BIT6		//置1表示处理过监听，如果在无电话业务的状态下，判断此标志表示需要进行恢复语音方面的设置
#define		TCP_PHONE_F_1			BIT7		//表示有任何和电话相关的业务，只有在这个电话业务完全完成后，才能恢复
#define		MONITOR_ON_F_1			BIT8		//监听对外拨号状态的标志
#define		DAILING_UP_F_1			BIT9		//手柄的最外拨号状态的标志
#define		RING_F_1				BITA		//正在处于有来电状态的标志

#define	 	LAND_TCP_F_1			BITB		//表示需要进行TCP连接的状态量(能进入连接TCP的子程序)
#define	 	LAND_TCP_SUCCESS_F_1	BITC		//表示TCP连接后发送登陆信息，成功收到下行的登陆响应的状态量。
#define	 	LAND_TCP_ERROR_F_1		BITD		//表示一次登陆TCP的失败，则设置此标志位！，有此标志位，则可以进行时间间隔的计数。，如果此标志复位，则表示可以进行第二次的TCP登陆

#define		TCP_IP_VALID_1			BITE		//为1时候，表示FLASH中存储的IP地址有效，为0时候表示为效，不用登陆GPRS操作
#define		GPS_VALID_F_1			BITF		//GPS数据有效标志,为1的时候表示有效，为0的表示无效



//五，unsigned int gInternal_Flag=0
/********************************************************\
说明：
\********************************************************/
#define		LOAD_ONEVALID_F_1		BIT0		//表示已经在这个周期内存储了一个有效的监控或者行车轨迹点数据
#define		ALLOW_DATA_F_1			BIT1		//有周期数据，置1则表示可以通过GSM方式发送数据
#define 	STOP_OVERSPEED_F_1		BIT2		//置1表示处于超速报警的标志
#define		RECEIVE_SMS_F_1			BIT3		//表示此时是接收SMS的标志
#define		DISP_ON_F_1				BIT4		//此标志置位则表示主控判断到显示终端处于开机状态，复位则表示判断显示终端处于关机状态
#define		M22_RETURN_OK_1			BIT5		//判断G20是否正确返回OK的标志，先设置为0，如果返回正确了，则设置为1
#define		GSM_ONEAT_SUCCESS_1		BIT6		//判断一个AT指令是否结束（必须有返回的时候才算成功）												//如果开始操作一个AT指令，则将此标志设置为1，等待有相应的GSM返回后，则设置为0；
#define		TCPSEND_ALLOW_F_1		BIT7		//作为允许TCP数据发送的标志
#define		SMS_CENTER_CLASS_1		BIT8		//作为短消息中心号码前发送91还是A1的标志，为0表示发送91，为1时候表示发送A1
#define		SMS_TARGET_CLASS_1		BIT9		//作为短消息目标电话号码前发送91还是A1的标志，为0表示发送91，为1时候表示发送A1
#define		VEHICLE_DEFENCE_F_1		BITA		//车辆设防状态的标志位
#define		SET_CSCA_F_1			BITB    	//收到参数设置后，将用AT+CSCA指令设置短消息中心号的标志
#define		RESET_GPS_F_1			BITC		//作为需要重新设置GPS格式的标志位
#define		CENTER_NUM_VAILD_F_1	BITD		//此标志置位，表示正在中断接收GPRS方面的数据，，复位表示数据接收完毕
#define		SEND_CIRCLE_F_1			BITF		//作为在Circle_Buffer[]中收集满周期需要发送的数据后，需要发送的时候，此标志置位,不管是GSM方式还是GPRS方式均起作用

//六，unsigned int gOther_Flag=0
/********************************************************\
说明：

\*******************************************************/
#define		RESET_PHONE_F_1			BIT0		//当监听的拨号后，将此位置位，此时进行电话方面的业务，一旦电话业务结束，在主程序中判断如果无电话业务，同时此标志置位，则将语音方面的设置恢复
#define		NEED_LADN_TCP_F_1		BIT1		//
#define		RESET_PROGRAME_F_1		BIT2		//为0表示正常喂狗，为1的时候则停止喂狗，这里用来如果连续3次GPRS网络登陆不正常，则通过停止喂狗操作来使系统程序重新启动
#define		ADC_SWITCHING_F_1		BIT3		//置位，表示ADC正在进行ADC转换，复位则表示转换完成。
#define		ALLOW_SEND_RECORD_F_1	BIT4		//设置为1,则表示可以进行记录仪的数据发送
#define 	PASTDATA_F_1			BIT5		//设置为1,表示Circle_Buffer中存储的是要发送的历史轨迹数据
#define		RECORD_DATA_F_1			BIT6		//设置1，表示有外接部件的返回数据需要通过TCP方式发送出去
#define		PASS_DATA_LASE_F_1		BIT7		//置1，表示寻找历史轨迹的最后一页，即和存储页的位置相同
#define		END_PASTDATA_F_1		BIT8		//置位，表示可以发送历史轨迹结束的信息，复位则不需要发送
#define		ALLOW_SENDLAND_F_1		BIT9		//置1  则表示发送登陆信息！
#define		QUIT_TCP_F_1			BITA		//置1，表示可以进行一次退出TCP操作，复位则表示一次退出TCP操作完成
#define		GPS_RESET_F_1			BITB		//置1，则表示需要进行GPS模块的复位操作，置0则表示GPS模块复位结束
#define		TCP_HANG_F_1			BITC		//置1，则表示程序正在擦除所有的历史轨迹的数据
#define		FIND_PASSPAGE_F_1		BITD		//置1，则表示程序已经找到满足条件的首页的第一条记录所在的扇区
#define		FIRST_PASSPAGE_F_1		BITE		//置1，则表示已经找到满足要求的页，但不一定是最开始满足要求的页
#define		SEND_PASSPAGE_F_1		BITF		//置1，则表示已经找到满足要求的最开始的页，可以进行数据的查询和发送

//七，unsigned char gStatus1_Flag
/********************************************************\
说明：在发送行车轨迹点的时候的同时上行的车载终端的状态量1

\********************************************************/
#define		DISPLAY_ON_F_1			BIT0		//设置1，则表示显示终端处于开机状态，为0则表示显示终端处于关机状态
#define		HANDLE_ON_0_F_1			BIT1
#define		HANDLE_ON_1_F_1			BIT2		//如果为00，则表示车载手柄处于被摘掉的状态，如果为01，则说明手柄处于关机状态，为10，则说明手柄处于开机状态
#define		DEFENCE_STATUS_F_1		BIT3		//设置1，则表示车载终端处于被盗设防状态，为0则表示未处于被盗设防状态
#define		STOP_CIRCUIT_F_1		BIT4		//设置1，则表示车载终端的制动电路有故障，为0则表示正常
#define		ACC_ON_F_1				BIT5		//设置1，则表示ACC检测线正常开启，为0则表示ACC检测线未开启
#define		STOP_SUCCESS_F_1		BIT6		//设置1，则表示车载终端处于制动状态，为0则表示未处于制动状态
#define		PHONE_ONLINE_F_1		BIT7		//设置1，则表示车载终端的语音通话功能正在使用，为0则表示语音通话功能未使用
//八，unsigned char gStatus2_Flag
/********************************************************\
说明：在发送行车轨迹点的时候的同时上行的车载终端的状态量2
\********************************************************/
#define		HANDLE_PHONE_IN_LIMIT	BIT0		//设置1，则表示车载电话处于呼入限制状态
#define		HANDLE_PHONE_OUT_LIMIT	BIT1		//设置1，则表示车载电话处于呼出限制状态
#define		VEHICLE_LOAD_CARRY		BIT2		//设置1，则表示车辆处于重载状态，为0则表示处于空载状态
#define		ROB_ALARM_F_1			BIT3		//设置1，则表示车辆处于被劫报警状态
#define		TCP_ON_OFF_F_1			BIT4		//设置1，则表示车辆处理TCP的开启方式
#define		IP_VAILD_F_1			BIT5		//设置1，则表示车辆处于IP地址有效方式
#define		AREA_VAILD_F_1			BIT6		//设置1，则表示有区域判断的标志
#define		RECORD_VAILD_F_1		BIT7		//设置1，则表示记录仪超过5分钟无接收到任何信息数据。
//九，unsigned char gStatus3_Flag
/********************************************************\
说明：在发送行车轨迹点的时候的同时上行的车载终端的状态量2
\********************************************************/
#define		DISTANCE_ON_F_1			BIT0		//设置1，则表示间隔距离传输采用TCP常在线的方式
#define		GPS_OUTPUT_WAY_F_1		BIT1		//设置1，则表示GPS的输出格式不正确

#define		LINE_LIMIT_F_1			BIT3		//设置1，则表示有线路判断的标志

#define		M22_WORK_0_F_1			BIT4
#define		M22_WORK_1_F_1			BIT5		//BIT4和BIT5构成系统的工作状态指示，00;表示无周期数传，
												//									01;表示按时间间隔上报数据
												//									10;表示按行使间隔上报数据
												//									11;表示是报警类型的上报数据
//九，unsigned int gPublic_Flag=0
/********************************************************\
说明：
\********************************************************/
#define		TEST_SPEED_F_1			BIT0		//收到下行的制动命令，则设置1，检测10秒内的速度符合要求后则为0
#define		SET_STOP_F_1			BIT1		//设置1，则表示进行制动操作
#define		FREE_STOP_F_1			BIT2		//设置1，则表示进行解除制动操作
#define		CHECK_TIME_F_1			BIT3		//设置1，则表示测试部分的SMS的测试
#define		ACC_OFF_UP_F_1			BIT4		//设置1，则表示需要发送一个关闭ACC的数据
#define		ACCOFF_TRACK_F_1		BIT5		//设置1，则表示过了10分钟开始改变TCP的上传间隔
#define		ACCOFF_OK_F_1			BIT6		//设置1，则表示已经关闭模块电源设置1，则表示需要关闭GPS和GSM模块的标志位
#define		ALLOW_PHONE_IN_F_1		BIT7		//设置1，则表示来电的电话号码与存储在Flash中的中心服务电话号码相同
#define		SET_DISTANCE_DATA_F_1	BIT8		//设置1，则表示向记录仪发送设置间隔距离发送信息点。
#define		SET_DISTANCE_OK_F_1		BIT9		//设置1，则表示向记录仪发送设置间隔距离成功
#define     GPS_NULL_F_1			BITA		//设置1，则表示GPS的速度数据，超过3秒处于无效状态
#define		RECORD_NULL_F_1			BITB		//设置1，则表示记录仪的速度数据，超过3秒处于无效状态
#define		ALLOW_SEND_DISTANCE_F_1	BITC		//设置1，则表示允许发送间隔距离的点
#define		DISP_SEND_INFO_F_1		BITE		//设置1，则表示有正在处理显示屏上行的数据。处理完成后则复位
#define		SET_RECORD_TIME_F_1		BITF		//设置1，则表示需要对记录仪进行时间矫正设置，复位，则不需要进行设置或者设置成功

//十，unsigned int gCommon_Flag=0;
#define		GPSNO_RESET_F_1			BIT0		//设置1，则表示因为5次因为GPS输出格式不正确而引起的掉电操作不在进行掉电操作
#define		LINE_OUT_F_1			BIT1		//设置1，则表示当前点的垂线不与当前计算的线段相交
#define		STOP_OVERLINE_F_1		BIT2		//设置1，则表示需要停止偏移线路报警
#define		ALLOW_LINE_DIS_F_1		BIT3		//设置1，则表示线路距离设置的值满足要求
#define		ALLOW_LINE_DATA_F_1		BIT4		//设置1，则表示线路的参数设置正?
#define		ALLOW_HANGUP_PHONE_F_1	BIT5		//设置1，则
#define		ALLOW_R232_F_1			BIT6		//设置1，则表示不允许进行模拟串口3，4的发送
#define		ALLOW_OUT_DISP_F_1		BIT7		//设置1，则表示有需要重发的数据包，则在发送时候，则不允许先丢弃此包
#define		DISP_SEND_OK_F_1		BIT8
#define		CHECK_RINGPHONE_F_1		BIT9		//设置1，则表示已经进行过一次来电电话号码的检验
#define		ALLOW_SEND_DISP_F_1		BITA		//设置1，则表示收到过一次驾驶员的代码
#define		ALLOW_RESET_MCU_F_1		BITB

//十一，unsigned char gArea_Par=0
/********************************************************\
说明：相关区域设置的参数说明
\********************************************************/
#define		AREA1_ON				BIT0		//设置1，表示区域1有设置参数，为0，则此区域无设置参数
#define		AREA2_ON				BIT1		//设置1，表示区域2有设置参数，为0，则此区域无设置参数
#define		AREA3_ON				BIT2		//设置1，表示区域3有设置参数，为0，则此区域无设置参数
#define		AREA4_ON				BIT3		//设置1，表示区域4有设置参数，为0，则此区域无设置参数
#define		AREA1_DIR				BIT4		//设置1，表示此区域属于禁入方式，为0，表示属于禁出方式
#define		AREA2_DIR				BIT5		//设置1，表示此区域属于禁入方式，为0，表示属于禁出方式
#define		AREA3_DIR				BIT6		//设置1，表示此区域属于禁入方式，为0，表示属于禁出方式
#define		AREA4_DIR				BIT7		//设置1，表示此区域属于禁入方式，为0，表示属于禁出方式

//十二， unsigned char gRecord_Status
/********************************************************\
gRecord_Status，属于记录仪返回的当前记录仪的统计状态数据
\********************************************************/
#define		J_DISTANCE_DATA_F_1			BIT0		//设置1，则表示统计需要上传数据点的距离到
#define		J_TIRE_NOTICE_F_1			BIT1		//设置1，则表示属于疲劳驾驶的提示作用
#define		J_TIRE_ALARM_F_1			BIT2		//设置1，则表示属于疲劳驾驶报警状态
#define		J_PRINTER_STATUS_F_1		BIT3		//设置1，则表示当前记录仪的打印机处于正在工作状态
#define		J_USB_STATUS_F_1			BIT4		//设置1，则表示当前记录仪的USB正在处于工作状态
//十三， unsigned char gDisp_Status
/********************************************************\
gDisp_Status，属于主控返回显示终端显示的当前系统状态数据
\********************************************************/
#define		S_TIRE_NOTICE_F_1			BIT0		//设置1，则表示当前处于疲劳驾驶的提示状态
#define		S_PRINTER_STATUS_F_1		BIT1		//设置1，则表示当前记录仪的打印机处于正在工作状态
#define		S_USB_STATUS_F_1			BIT2		//设置1，则表示当前记录仪的USB正在处于工作状态
/********************************************************\

\********************************************************/
 //char gAlarm_Type=0的8个位的标志设置含义
#define		FLAG_GPSVAILD_1			BIT7		//如果为0表示GPS的数据无效，为1则表示GPS数据有效
#define		FLAG_OVERSPEED_2		BIT6		//如果设置1，则表示有超速报警的标志，可以与被盗和被劫报警共存，但被盗报警和被劫报警不能共存
#endif
