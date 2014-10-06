#ifndef __Do_Handle_H
#define __Do_Handle_H

void Do_Handle_Module(void);
void Send_COM3_SubProgram(unsigned char DataLength,unsigned char nCommand,unsigned char DataTemp);
void Send_COM3_Load(unsigned char DataLength,unsigned char nCommand,unsigned char DataTemp);
#endif