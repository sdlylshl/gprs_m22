#ifndef __Do_SRAM_H
#define __Do_SRAM_H

#define RD               	(0x0002)			//P3.1
#define WR                	(0x0004)			//P3.2
#define LE                	(0x0008)			//P3.3

void SRAM_Init(void);
void SRAM_Write(unsigned int ByteAddress,unsigned char WriteOneByte);
extern unsigned char SRAM_Read(unsigned int ByteAddress);
#endif