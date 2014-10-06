
#ifndef __Disp_Protocol_H
#define __Disp_Protocol_H

//显示终端向主控终端发送的消息命令字
#define		DISP_ASK_WATCH_STATUS_UP			0x80			//显示终端请求车辆状态
//#define		DISP_RECEIVE_INFO_ECHO_UP			0x81			//显示终端接收到一个完整的文字信息后的ECHO
#define		DISP_INFO_STATUS_UP					0x0A			//显示终端的上行的请求信息
#define		DISP_ASK_RECORDINFO_UP				0x09			//显示终端需要与记录仪通讯的命令
#define    	DISP_CLRCLESEND_ECHO_UP				0x0B			//显示终端定期的向主控返回的响应
#define		DISP_RECEIVE_IC_INFO_UP				0x11			//显示终端收到IC卡数据后的响应
#define		DISP_SET_TCPSTATUS_UP				0x12			//显示终端向主控请求TCP的工作状态
#define		DISP_ASK_PAR_UP						0x13			//显示终端要求查询终端参数
#define		DISP_SMS_SNED_UP					0xAE			//显示终端预置SMS的上报信息

#define		DISP_SET_ECHO_UP					0xFA			//显示终端的响应(包含二级命令字)
//下面是二级命令字（主要是判断显示终端响应命令包中的响应的那一个命令字）
#define		DISP2_SEND_WORD_ECHO_UP				0x4A			//显示终端接收到中心下行文字信息的响应（如果是多屏信息的时候，收到一屏也响应此命令子）
#define		DISP2_SEND_WORD_UP					0x4B			//显示终端接收到全中心下行的文字信息的响应
#define		DISP2_SET_SMS_ECHO_UP				0xAE			//显示终端接收到中心预先设置短信的响应
#define		DISP2_HSET_SMS_ECHO_UP				0x15			//显示终端接收来自手柄端口设置的短消息预置的响应
#define		DISP2_CLEAR_ECHO_UP					0x03
/////////////////////////////////////////////////////////////////////////////////////////
//主控向显示终端发送的消息命令字
#define		DISP_MAIN_ARALM_DOWN				0x50			//主控返回显示终端设备系统处于的报警信息
#define		DISP_MAIN_STATUS_DOWN				0xF7			//主控向显示终端发送时间（每分钟矫正一次）
#define		DISP_ASK_RECORD_INFO_ECHO_DOWM		0x09			//主控向显示终端转发记录仪的命令字
#define		DISP_MAIN_SYS_STATUS_DOWN			0x01			//主控向显示终端定期发送的系统状态
#define		DISP_MAIN_IC_INFO_DOWN				0x02			//主控向显示终端发送新IC卡插入的数据包

#define		DISP_MAIN_WORD_DOWN					0xFB			//主控向显示终端发送文字信息
#define		DISP_MAIN_INFO_ECHO_DOWN			0x12			//主控向显示终端发送接收处理信息的响应
#define		DISP_MAIN_MAIN_DOWN					0x83			//主控向显示转发动态菜单的命令
#define		DISP_MAIN_SET_SMS_DOWN				0x15			//主控向显示转发终端设置预置SMS的命令
#define		DISP_ASK_PAR_DOWM					0x13			//主控向显示终端发送显示查询参数,参数的返回结果
#define		DISP_RECEIVE_ECHO_DOWN				0x14			//主控收到显示终端的需要转发命令后，返回的响应

#define		DISP_CLEAR_DOWN						0x03			//主控初始化显示终端的指令


#endif