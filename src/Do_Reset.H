#ifndef __Do_Reset_H
#define __Do_Reset_H

extern void Do_OFF_Power(void);
extern void Do_Reset_GSM_On(void);

//下面是通过SMS发送信息的相关子程序
extern void Ask_SMS_Send(unsigned char Input);
extern void Send_SMS_Head(void);
extern unsigned int Send_Echo_GSM_Head(unsigned char nInput,unsigned char nCommand,unsigned char nACK,unsigned char nLength);
extern unsigned int Send_Buffer1_GSM_Data(unsigned int iCheckIn,unsigned char nAddress,unsigned char nLength);
extern void Send_Echo_GSM_End(unsigned int Input);

//下面是通过TCP发送信息的相关子程序
extern unsigned int Send_Echo_TCP_Head(unsigned char nInput,unsigned char nCommand,unsigned char nACK,unsigned char nLength);
extern unsigned int Send_Buffer1_TCP_Data(unsigned int iCheckIn,unsigned char nAddress,unsigned char nLength);
extern void Send_Echo_TCP_End(unsigned int Input);

#endif