#ifndef __Uart01_H
#define __Uart01_H

void Send_COM0_Byte(unsigned char Input);
void Send_COM1_Byte(unsigned char Input);
void Send_COM0_String(unsigned char *Point,unsigned int Count);
void Send_COM1_String(unsigned char *Point,unsigned int Count);

#endif
