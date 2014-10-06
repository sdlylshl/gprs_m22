/*
++++++++++++++++++++++++++++++++++++++++++++++
	关于串行存储器AT45DB041B的管脚定义
++++++++++++++++++++++++++++++++++++++++++++++
*/
#ifndef __SPI45DB041_H
#define __SPI45DB041_H

#define SPI45DB041_CS                (0x0001)			//P2.0
#define SPI45DB041_RST               (0x0002)			//P2.1
#define SPI45DB041_SCK               (0x0004)			//P2.2
#define SPI45DB041_SI                (0x0008)			//P2.3
#define SPI45DB041_SO                (0x0010)			//P2.4
#define SPI45DB041_WP                (0x0002)			//P6.1

//#define NOP() { _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); }
//extern unsigned char SPI_Flash_Buffer[256];
//void delay(unsigned int t);//时间延迟
//void SPI45D041_WriteBits(unsigned int byte,unsigned char num_of_bits);//写
//unsigned char SPI45D041_ReadByte(void);//一个字节的读取
//void ReadOnePage(unsigned int PageAddress);//不通过两个缓冲而读Flash指定页的整个数据
//void WriteOnePage(unsigned int PageAddress);//通过buffer1将一个页的数据写入Flash指定页中
//void SI_To_Buffer1(void);  //写一个页的数据到buffer1中
//void SO_From_Buffer1(void);//读取整个buffer1中的数据
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//void ReadOnePageToBuffer1(unsigned int PageAddress);//读主存储区一个页的数据到buffer1
void ReadOnePageToBuffer2(unsigned int PageAddress);//读主存储区一个页的数据到buffer2

//extern unsigned char ReadByteFromBuffer1(unsigned int ByteAddress); //读buffer1中的一个指定地址的字节
extern unsigned char ReadByteFromBuffer2(unsigned int ByteAddress); //读buffer2中的一个指定地址的字节

//unsigned char ReadNextFromBuffer1(void);//读buffer1中的下一个地址的字节数据
unsigned char ReadNextFromBuffer2(void);//读buffer2中的下一个地址的字节数据

//void WriteOneByteToBuffer1(unsigned int ByteAddress,unsigned char WriteOneByte );//在buffer1中的一个指定的地址写一个字节
void WriteOneByteToBuffer2(unsigned int ByteAddress,unsigned char WriteOneByte );//在buffer2中的一个指定的地址写一个字节

//void WriteNextByteToBuffer1(unsigned char WriteNextOne);//写buffer1中的下一个地址
void WriteNextByteToBuffer2(unsigned char WriteNextOne);//写buffer2中的下一个地址

//void WriteBuffer1ToPage(unsigned int PageAddress);//将buffer1中的数据写入主存储区一个指定的页
void WriteBuffer2ToPage(unsigned int PageAddress);//将buffer2中的数据写入主存储区一个指定的页

void EraseOnePage(unsigned int PageAddress);
void OperateSPIEnd(void);

#endif
