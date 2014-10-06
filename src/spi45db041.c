
/********************************************************\
*	文件名：  SPI45DB041.C
*	创建时间：2004年12月7日
*	创建人：  
*	版本号：  1.00
*	功能：	  实现针对串行FLASH，SPI45DB041的相关操作，读，写
*	文件属性：公共文件
*	修改历史：（每条详述）
\********************************************************/
#include <msp430x14x.h>
#include "SPI45DB041.H"

#define NOP() { _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); }

void delay(unsigned int t);//时间延迟
void SPI45D041_WriteBits(unsigned int byte,unsigned char num_of_bits);//写
unsigned char SPI45D041_ReadByte(void);//一个字节的读取

//void ReadOnePage(unsigned int PageAddress);//不通过两个缓冲而读Flash指定页的整个数据
//void WriteOnePage(unsigned int PageAddress);//通过buffer1将一个页的数据写入Flash指定页中

//void SI_To_Buffer1(void);  //写一个页的数据到buffer1中
//void SO_From_Buffer1(void);//读取整个buffer1中的数据

//unsigned char SPI_Flash_Buffer[256];
//void ReadOnePageToBuffer1(unsigned int PageAddress);//读主存储区一个页的数据到buffer1
//void ReadOnePageToBuffer2(unsigned int PageAddress);//读主存储区一个页的数据到buffer2

//unsigned char ReadByteFromBuffer1(unsigned int ByteAddress); //读buffer1中的一个指定地址的字节
//unsigned char ReadByteFromBuffer2(unsigned int ByteAddress); //读buffer2中的一个指定地址的字节

//unsigned char ReadNextFromBuffer1(void);//读buffer1中的下一个地址的字节数据
//unsigned char ReadNextFromBuffer2(void);//读buffer2中的下一个地址的字节数据

//void WriteOneByteToBuffer1(unsigned int ByteAddress,unsigned char WriteOneByte );//在buffer1中的一个指定的地址写一个字节
//void WriteOneByteToBuffer2(unsigned int ByteAddress,unsigned char WriteOneByte );//在buffer2中的一个指定的地址写一个字节

//void WriteNextByteToBuffer1(unsigned char WriteNextOne);//写buffer1中的下一个地址
//void WriteNextByteToBuffer2(unsigned char WriteNextOne);//写buffer2中的下一个地址

//void WriteBuffer1ToPage(unsigned int PageAddress);//将buffer1中的数据写入主存储区一个指定的页
//void WriteBuffer2ToPage(unsigned int PageAddress);//将buffer2中的数据写入主存储区一个指定的页

//void OperateSPIEnd(void);


/********************************************************\
*	函数名：delay
	作用域：本文件调用
*	功能：  时间延迟
*	参数：  t
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void delay(unsigned int t)
{
	while(t--){};
}

/********************************************************\
*	函数名：SPI45D041_WriteBits
	作用域：本文件调用
*	功能：  对一个int型的数据(16位)依次右移,并放在FLASH的输入脚上
*	参数：  byte 		一个int的数
			num_of_bits	针对上面的int型的数据，依次向右移出的bit数
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）

	SPI_SCK上升沿数据有效
	针对bit的数据操作:通过SI口，写入一个字节的多少bit
\********************************************************/
void SPI45D041_WriteBits(unsigned int byte,unsigned char num_of_bits)
{
	unsigned char count;
	unsigned int Temp;
	byte=byte<<(16-num_of_bits);
	for(count=0;count<num_of_bits;count++)
	{
		Temp=byte&0x8000;
		P2OUT&=~SPI45DB041_SCK;
		NOP();
		if(Temp==0x8000)
		{
			P2OUT|=SPI45DB041_SI;
		}
		else
		{
			P2OUT&=~SPI45DB041_SI;
		}
		P2OUT|=SPI45DB041_SCK;
		NOP();
		byte<<=1;
	}
}

/********************************************************\
*	函数名：SPI45D041_ReadByte
	作用域：本文件调用
*	功能：  SPI_SCK下降沿数据有效,读一个字节数据
*	参数：  无
*	返回值：byte 从FLASH中读取的一个字节
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned char SPI45D041_ReadByte(void)
{
	unsigned char count,byte;
	for(count=0;count<8;count++)
	{
		P2OUT|=SPI45DB041_SCK;
		NOP();
		byte<<=1;
		P2OUT&=~SPI45DB041_SCK;
		NOP();
		if(P2IN&SPI45DB041_SO) byte++;
		NOP();
	}
	return(byte);
}

/********************************************************\
*	函数名：ReadOnePage
	作用域：此文件在此项目中不使用
*	功能：  Main Memory的一个页的读操作：不通过两个buffer，直接读取主存储区一个页的数据
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
void ReadOnePage(unsigned int PageAddress)
{
	unsigned int i;
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();

	SPI45D041_WriteBits(0x52,8);        //送8 bit 的命令字
	SPI45D041_WriteBits(0x00,4);        //送4 bit 的保留位
	SPI45D041_WriteBits(PageAddress,11);//送11位的页地址
	SPI45D041_WriteBits(0x00,9);        //送9位的页内地址
	SPI45D041_WriteBits(0x00,16);
	SPI45D041_WriteBits(0x00,16);       //送32位的NOP操作
	NOP();
	for(i=0;i<256;i++)
	{
	   // SPI_Flash_Buffer[i]=SPI45D041_ReadByte();
	}
	NOP();
	P2OUT|=SPI45DB041_CS;
}
*/
/********************************************************\
*	函数名：SI_To_Buffer1
	作用域：此文件在此项目中不使用
*	功能：  SI 数据写到buffer1中：直接写一个页的数据在buffer1中。
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
void SI_To_Buffer1(void)
{
	unsigned int i;
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();

	SPI45D041_WriteBits(0x84,8);        //送8 bit 的命令字
	SPI45D041_WriteBits(0x00,15);       //送15 bit 的NOP
	SPI45D041_WriteBits(0x00,9);        //送9 bit 的页内地址

	for(i=0;i<256;i++)
	{
//		SPI45D041_WriteBits(SPI_Flash_Buffer[i],8);
	}

	NOP();
	P2OUT|=SPI45DB041_CS;
	NOP();
}
*/
/********************************************************\
*	函数名：SO_From_Buffer1
	作用域：此文件在此项目中不使用
*	功能：  读取整个Buffer1中的数据
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
void SO_From_Buffer1(void)
{
	unsigned int i;
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();

	SPI45D041_WriteBits(0x54,8);        //送8 bit 的命令字
	SPI45D041_WriteBits(0x00,15);       //送15 bit 的NOP
	SPI45D041_WriteBits(0x00,9);        //送9 bit 的页内地址
	SPI45D041_WriteBits(0x00,8);       //送8bit 的NOP
	NOP();
	for(i=0;i<256;i++)
	{
//	    SPI_Flash_Buffer[i]=SPI45D041_ReadByte();
	}
	NOP();
	P2OUT|=SPI45DB041_CS;

}
*/
/********************************************************\
*	函数名：WriteOnePage
	作用域：此文件在此项目中不使用
*	功能：  Main Memory的一个页的写操作：通过buffer1将一个页的数据写入主存储区的一个指定的页中。
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
void WriteOnePage(unsigned int PageAddress)
{
	SI_To_Buffer1();                    //先将数据写到Buffer1中

	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();
	SPI45D041_WriteBits(0x83,8);        //送8 bit 的命令字
	SPI45D041_WriteBits(0x00,4);        //送4 bit 的保留位
	SPI45D041_WriteBits(PageAddress,11);//送11 bit 的页地址
    SPI45D041_WriteBits(0x00,9);        //送9 bit 的NOP
    P2OUT|=SPI45DB041_CS;
    NOP();
    delay(80000);
}
*/
/********************************************************\
*	函数名：ReadOnePageToBuffer1
	作用域：外部文件调用
*	功能：  读主存储区一个页的数据到buffer1。
*	参数：  PageAddress 需要读的主存储区的页地址
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
void ReadOnePageToBuffer1(unsigned int PageAddress)
{
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();

	SPI45D041_WriteBits(0x53,8);        //送8 bit 的命令字
	SPI45D041_WriteBits(0x00,4);        //送4 bit 的保留位
	SPI45D041_WriteBits(PageAddress,11);//送11位的页地址
	SPI45D041_WriteBits(0x00,9);        //送9位的页内地址
	P2OUT|=SPI45DB041_CS;
	NOP();
	delay(10000);
}
*/
/********************************************************\
*	函数名：ReadOnePageToBuffer2
	作用域：外部文件调用
*	功能： 	读主存储区一个页的数据到buffer2
*	参数： 	PageAddress 需要读的主存储区的页地址
*	返回值：无
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void ReadOnePageToBuffer2(unsigned int PageAddress)
{
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();
	SPI45D041_WriteBits(0x55,8);        //送8 bit 的命令字
	SPI45D041_WriteBits(0x00,4);        //送4 bit 的保留位
	SPI45D041_WriteBits(PageAddress,11);//送11位的页地址
	SPI45D041_WriteBits(0x00,9);        //送9位的页内地址
	P2OUT|=SPI45DB041_CS;
	NOP();
	delay(10000);
}

/********************************************************\
*	函数名：ReadByteFromBuffer1
	作用域：外部文件调用
*	功能： 	在buffer1中的一个指定地址读一个字节的数据
*	参数： 	ByteAddress 指定字节在页内的地址
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
unsigned char ReadByteFromBuffer1(unsigned int ByteAddress)
{
	unsigned char Buffer1OneByte;
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();

	SPI45D041_WriteBits(0x54,8);        //送8 bit 的命令字
	SPI45D041_WriteBits(0x00,15);       //送15 bit 的保留位
	SPI45D041_WriteBits(ByteAddress,9);//送11位的页地址
	SPI45D041_WriteBits(0x00,8);        //送8 bit 的保留位
	NOP();
	Buffer1OneByte=SPI45D041_ReadByte();
	return(Buffer1OneByte);
}
*/
/********************************************************\
*	函数名：ReadNextFromBuffer1
	作用域：外部文件调用
*	功能： 	读buffer1中的下一个地址的字节数据(配合的与ReadByteBuffer1一起使用)
*	参数：	无
*	返回值：Buffer1NextByte
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
unsigned char ReadNextFromBuffer1(void)
{
	unsigned char Buffer1NextByte;
	Buffer1NextByte=SPI45D041_ReadByte();
	return(Buffer1NextByte);
}
*/
/********************************************************\
*	函数名：ReadByteFromBuffer2
	作用域：外部文件调用
*	功能： 在buffer2中的一个指定地址读一个字节的数据
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned char ReadByteFromBuffer2(unsigned int ByteAddress)
{
	unsigned char Buffer2OneByte;
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();

	SPI45D041_WriteBits(0x56,8);        //送8 bit 的命令字
	SPI45D041_WriteBits(0x00,15);       //送15 bit 的保留位
	SPI45D041_WriteBits(ByteAddress,9);	//送11位的页地址
	SPI45D041_WriteBits(0x00,8);        //送8 bit 的保留位
	NOP();
	Buffer2OneByte=SPI45D041_ReadByte();
	return(Buffer2OneByte);
}

/********************************************************\
*	函数名：ReadNextFromBuffer2
	作用域：外部文件调用
*	功能： 读buffer2中的下一个地址的字节数据(配合的与ReadByteBuffer2一起使用)
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
unsigned char ReadNextFromBuffer2(void)
{
	unsigned char Buffer2NextByte;
	Buffer2NextByte=SPI45D041_ReadByte();
	return(Buffer2NextByte);
}

/********************************************************\
*	函数名：WriteOneByteToBuffer1
	作用域：外部文件调用
*	功能： 在buffer1中的一个指定的地址写一个字节
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
void WriteOneByteToBuffer1(unsigned int ByteAddress,unsigned char WriteOneByte )
{
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();
	SPI45D041_WriteBits(0x84,8);        	//送8 bit 的命令字
	SPI45D041_WriteBits(0x00,15);       	//送15 bit 的NOP
	SPI45D041_WriteBits(ByteAddress,9); 	//送9 bit 的页内地址
	SPI45D041_WriteBits(WriteOneByte,8);
	NOP();
}
*/
/********************************************************\
*	函数名：WriteNextByteToBuffer1
	作用域：外部文件调用
*	功能： 写buffer1中的下一个地址（必须配合WriteOneByteToBuffer1一起使用）
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
void WriteNextByteToBuffer1(unsigned char WriteNextOne)
{
	SPI45D041_WriteBits(WriteNextOne,8);
}
*/
/********************************************************\
*	函数名：WriteOneByteToBuffer2
	作用域：外部文件调用
*	功能： 在buffer2中的一个指定的地址写一个字节
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void WriteOneByteToBuffer2(unsigned int ByteAddress,unsigned char WriteOneByte )
{
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();
	SPI45D041_WriteBits(0x87,8);        	//送8 bit 的命令字
	SPI45D041_WriteBits(0x00,15);       	//送15 bit 的NOP
	SPI45D041_WriteBits(ByteAddress,9); 	//送9 bit 的页内地址
	SPI45D041_WriteBits(WriteOneByte,8);
	NOP();
}

/********************************************************\
*	函数名：WriteNextByteToBuffer2
	作用域：外部文件调用
*	功能： 写buffer2中的下一个地址（必须配合WriteOneByteToBuffer2一起使用）
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void WriteNextByteToBuffer2(unsigned char WriteNextOne)
{
	SPI45D041_WriteBits(WriteNextOne,8);
}

/********************************************************\
*	函数名：WriteBuffer1ToPage
	作用域：外部文件调用
*	功能： 将buffer1中的数据写入主存储区一个指定的页，前提条件是buffer1中确定是需要写入的数据
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
/*
void WriteBuffer1ToPage(unsigned int PageAddress)
{
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();
	SPI45D041_WriteBits(0x83,8);        //送8 bit 的命令字
	SPI45D041_WriteBits(0x00,4);        //送4 bit 的保留位
	SPI45D041_WriteBits(PageAddress,11);//送11 bit 的页地址
    SPI45D041_WriteBits(0x00,9);        //送9 bit 的NOP
    P2OUT|=SPI45DB041_CS;
    NOP();
    delay(100000);
}
*/
/********************************************************\
*	函数名：WriteBuffer2ToPage
	作用域：外部文件调用
*	功能： 将buffer2中的数据写入主存储区一个指定的页，前提条件是buffer2中确定是需要写入的数据
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void WriteBuffer2ToPage(unsigned int PageAddress)
{
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();
	SPI45D041_WriteBits(0x86,8);        //送8 bit 的命令字
	SPI45D041_WriteBits(0x00,4);        //送4 bit 的保留位
	SPI45D041_WriteBits(PageAddress,11);//送11 bit 的页地址
    SPI45D041_WriteBits(0x00,9);        //送9 bit 的NOP
    P2OUT|=SPI45DB041_CS;
    NOP();
    delay(100000);
}


/********************************************************\
*	函数名：EraseOnePage
	作用域：外部文件调用
*	功能： 指定
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void EraseOnePage(unsigned int PageAddress)
{
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();
	SPI45D041_WriteBits(0x81,8);        //送8 bit 的命令字
	SPI45D041_WriteBits(0x00,4);        //送4 bit 的保留位
	SPI45D041_WriteBits(PageAddress,11);//送11 bit 的页地址
    SPI45D041_WriteBits(0x00,9);        //送9 bit 的NOP
    P2OUT|=SPI45DB041_CS;
    NOP();
    delay(80000);
}

/********************************************************\
*	函数名：OperateSPIEnd
	作用域：外部文件调用
*	功能： 结束SPI的操作
*	参数：
*	返回值：
*	创建人：
*
*	修改历史：（每条详述）
\********************************************************/
void OperateSPIEnd(void)
{
	NOP();
	P2OUT|=SPI45DB041_CS;
	NOP();
}