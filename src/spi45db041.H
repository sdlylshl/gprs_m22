/*
++++++++++++++++++++++++++++++++++++++++++++++
	���ڴ��д洢��AT45DB041B�ĹܽŶ���
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
//void delay(unsigned int t);//ʱ���ӳ�
//void SPI45D041_WriteBits(unsigned int byte,unsigned char num_of_bits);//д
//unsigned char SPI45D041_ReadByte(void);//һ���ֽڵĶ�ȡ
//void ReadOnePage(unsigned int PageAddress);//��ͨ�������������Flashָ��ҳ����������
//void WriteOnePage(unsigned int PageAddress);//ͨ��buffer1��һ��ҳ������д��Flashָ��ҳ��
//void SI_To_Buffer1(void);  //дһ��ҳ�����ݵ�buffer1��
//void SO_From_Buffer1(void);//��ȡ����buffer1�е�����
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//void ReadOnePageToBuffer1(unsigned int PageAddress);//�����洢��һ��ҳ�����ݵ�buffer1
void ReadOnePageToBuffer2(unsigned int PageAddress);//�����洢��һ��ҳ�����ݵ�buffer2

//extern unsigned char ReadByteFromBuffer1(unsigned int ByteAddress); //��buffer1�е�һ��ָ����ַ���ֽ�
extern unsigned char ReadByteFromBuffer2(unsigned int ByteAddress); //��buffer2�е�һ��ָ����ַ���ֽ�

//unsigned char ReadNextFromBuffer1(void);//��buffer1�е���һ����ַ���ֽ�����
unsigned char ReadNextFromBuffer2(void);//��buffer2�е���һ����ַ���ֽ�����

//void WriteOneByteToBuffer1(unsigned int ByteAddress,unsigned char WriteOneByte );//��buffer1�е�һ��ָ���ĵ�ַдһ���ֽ�
void WriteOneByteToBuffer2(unsigned int ByteAddress,unsigned char WriteOneByte );//��buffer2�е�һ��ָ���ĵ�ַдһ���ֽ�

//void WriteNextByteToBuffer1(unsigned char WriteNextOne);//дbuffer1�е���һ����ַ
void WriteNextByteToBuffer2(unsigned char WriteNextOne);//дbuffer2�е���һ����ַ

//void WriteBuffer1ToPage(unsigned int PageAddress);//��buffer1�е�����д�����洢��һ��ָ����ҳ
void WriteBuffer2ToPage(unsigned int PageAddress);//��buffer2�е�����д�����洢��һ��ָ����ҳ

void EraseOnePage(unsigned int PageAddress);
void OperateSPIEnd(void);

#endif