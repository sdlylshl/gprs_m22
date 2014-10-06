#ifndef __Sub_C_H
#define __Sub_C_H

extern unsigned char ASCII( unsigned char Input );
extern unsigned char ASCIITOHEX( unsigned char Input_H,unsigned char Input_L );
extern unsigned char Compare_String(unsigned char *Source,unsigned char *Target,unsigned int Count );
extern unsigned int Check_Allow_Phone(unsigned char nPhone_Length);
void Reset_Variable(void);
void Clear_Exter_WatchDog(void);
void Wait_OK(void);
void Copy_String( unsigned char *PS,unsigned char *PD,unsigned int COUNT );
//void Write_PartData_Buffer1(void);
void Write_PartData_Sram(unsigned int S_Address);
void Page_R_Write_Check(unsigned int Page);
void Send_STOP_DealWay(unsigned char nCommand,unsigned char nACK);
void Disp_Handle_Steal_Alarm_Echo(void);
void Load_Flash_Par_Buffer1(void);
void Load_Buffer1_Echo(unsigned char nCommand,unsigned char nACK);
void Send_Handle_Signal(unsigned char nSignal);

void Load_SMS_CenterNum(unsigned int nLength);
unsigned int Check_Port(void);
unsigned int Check_Vaild(unsigned int Address_S,unsigned int Address_E);
void Judge_SMS_Way(void);
void Load_Info_One(void);
void Load_Info_Two(void);
void Load_Info_Three(void);
void Load_Info_Four(void);
void RFlash_Sector(unsigned int Sector);
void Waiting_Allow_Send(void);
void Do_Par(void);
void Do_Phone_Note(void);
void TA_Init(void);
void Ask_Par(void);
void Re430_Flash(unsigned char InFlag);
unsigned int Check_ParNum(void);
unsigned char Deal_Sector(unsigned int iSector);
void Init_Buffer2(void);
#endif