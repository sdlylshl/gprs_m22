
/********************************************************\
*	�ļ�����  SPI45DB041.C
*	����ʱ�䣺2004��12��7��
*	�����ˣ�  
*	�汾�ţ�  1.00
*	���ܣ�	  ʵ����Դ���FLASH��SPI45DB041����ز���������д
*	�ļ����ԣ������ļ�
*	�޸���ʷ����ÿ��������
\********************************************************/
#include <msp430x14x.h>
#include "SPI45DB041.H"

#define NOP() { _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); }

void delay(unsigned int t);//ʱ���ӳ�
void SPI45D041_WriteBits(unsigned int byte,unsigned char num_of_bits);//д
unsigned char SPI45D041_ReadByte(void);//һ���ֽڵĶ�ȡ

//void ReadOnePage(unsigned int PageAddress);//��ͨ�������������Flashָ��ҳ����������
//void WriteOnePage(unsigned int PageAddress);//ͨ��buffer1��һ��ҳ������д��Flashָ��ҳ��

//void SI_To_Buffer1(void);  //дһ��ҳ�����ݵ�buffer1��
//void SO_From_Buffer1(void);//��ȡ����buffer1�е�����

//unsigned char SPI_Flash_Buffer[256];
//void ReadOnePageToBuffer1(unsigned int PageAddress);//�����洢��һ��ҳ�����ݵ�buffer1
//void ReadOnePageToBuffer2(unsigned int PageAddress);//�����洢��һ��ҳ�����ݵ�buffer2

//unsigned char ReadByteFromBuffer1(unsigned int ByteAddress); //��buffer1�е�һ��ָ����ַ���ֽ�
//unsigned char ReadByteFromBuffer2(unsigned int ByteAddress); //��buffer2�е�һ��ָ����ַ���ֽ�

//unsigned char ReadNextFromBuffer1(void);//��buffer1�е���һ����ַ���ֽ�����
//unsigned char ReadNextFromBuffer2(void);//��buffer2�е���һ����ַ���ֽ�����

//void WriteOneByteToBuffer1(unsigned int ByteAddress,unsigned char WriteOneByte );//��buffer1�е�һ��ָ���ĵ�ַдһ���ֽ�
//void WriteOneByteToBuffer2(unsigned int ByteAddress,unsigned char WriteOneByte );//��buffer2�е�һ��ָ���ĵ�ַдһ���ֽ�

//void WriteNextByteToBuffer1(unsigned char WriteNextOne);//дbuffer1�е���һ����ַ
//void WriteNextByteToBuffer2(unsigned char WriteNextOne);//дbuffer2�е���һ����ַ

//void WriteBuffer1ToPage(unsigned int PageAddress);//��buffer1�е�����д�����洢��һ��ָ����ҳ
//void WriteBuffer2ToPage(unsigned int PageAddress);//��buffer2�е�����д�����洢��һ��ָ����ҳ

//void OperateSPIEnd(void);


/********************************************************\
*	��������delay
	�����򣺱��ļ�����
*	���ܣ�  ʱ���ӳ�
*	������  t
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void delay(unsigned int t)
{
	while(t--){};
}

/********************************************************\
*	��������SPI45D041_WriteBits
	�����򣺱��ļ�����
*	���ܣ�  ��һ��int�͵�����(16λ)��������,������FLASH���������
*	������  byte 		һ��int����
			num_of_bits	��������int�͵����ݣ����������Ƴ���bit��
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������

	SPI_SCK������������Ч
	���bit�����ݲ���:ͨ��SI�ڣ�д��һ���ֽڵĶ���bit
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
*	��������SPI45D041_ReadByte
	�����򣺱��ļ�����
*	���ܣ�  SPI_SCK�½���������Ч,��һ���ֽ�����
*	������  ��
*	����ֵ��byte ��FLASH�ж�ȡ��һ���ֽ�
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
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
*	��������ReadOnePage
	�����򣺴��ļ��ڴ���Ŀ�в�ʹ��
*	���ܣ�  Main Memory��һ��ҳ�Ķ���������ͨ������buffer��ֱ�Ӷ�ȡ���洢��һ��ҳ������
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
/*
void ReadOnePage(unsigned int PageAddress)
{
	unsigned int i;
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();

	SPI45D041_WriteBits(0x52,8);        //��8 bit ��������
	SPI45D041_WriteBits(0x00,4);        //��4 bit �ı���λ
	SPI45D041_WriteBits(PageAddress,11);//��11λ��ҳ��ַ
	SPI45D041_WriteBits(0x00,9);        //��9λ��ҳ�ڵ�ַ
	SPI45D041_WriteBits(0x00,16);
	SPI45D041_WriteBits(0x00,16);       //��32λ��NOP����
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
*	��������SI_To_Buffer1
	�����򣺴��ļ��ڴ���Ŀ�в�ʹ��
*	���ܣ�  SI ����д��buffer1�У�ֱ��дһ��ҳ��������buffer1�С�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
/*
void SI_To_Buffer1(void)
{
	unsigned int i;
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();

	SPI45D041_WriteBits(0x84,8);        //��8 bit ��������
	SPI45D041_WriteBits(0x00,15);       //��15 bit ��NOP
	SPI45D041_WriteBits(0x00,9);        //��9 bit ��ҳ�ڵ�ַ

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
*	��������SO_From_Buffer1
	�����򣺴��ļ��ڴ���Ŀ�в�ʹ��
*	���ܣ�  ��ȡ����Buffer1�е�����
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
/*
void SO_From_Buffer1(void)
{
	unsigned int i;
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();

	SPI45D041_WriteBits(0x54,8);        //��8 bit ��������
	SPI45D041_WriteBits(0x00,15);       //��15 bit ��NOP
	SPI45D041_WriteBits(0x00,9);        //��9 bit ��ҳ�ڵ�ַ
	SPI45D041_WriteBits(0x00,8);       //��8bit ��NOP
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
*	��������WriteOnePage
	�����򣺴��ļ��ڴ���Ŀ�в�ʹ��
*	���ܣ�  Main Memory��һ��ҳ��д������ͨ��buffer1��һ��ҳ������д�����洢����һ��ָ����ҳ�С�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
/*
void WriteOnePage(unsigned int PageAddress)
{
	SI_To_Buffer1();                    //�Ƚ�����д��Buffer1��

	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();
	SPI45D041_WriteBits(0x83,8);        //��8 bit ��������
	SPI45D041_WriteBits(0x00,4);        //��4 bit �ı���λ
	SPI45D041_WriteBits(PageAddress,11);//��11 bit ��ҳ��ַ
    SPI45D041_WriteBits(0x00,9);        //��9 bit ��NOP
    P2OUT|=SPI45DB041_CS;
    NOP();
    delay(80000);
}
*/
/********************************************************\
*	��������ReadOnePageToBuffer1
	�������ⲿ�ļ�����
*	���ܣ�  �����洢��һ��ҳ�����ݵ�buffer1��
*	������  PageAddress ��Ҫ�������洢����ҳ��ַ
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
/*
void ReadOnePageToBuffer1(unsigned int PageAddress)
{
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();

	SPI45D041_WriteBits(0x53,8);        //��8 bit ��������
	SPI45D041_WriteBits(0x00,4);        //��4 bit �ı���λ
	SPI45D041_WriteBits(PageAddress,11);//��11λ��ҳ��ַ
	SPI45D041_WriteBits(0x00,9);        //��9λ��ҳ�ڵ�ַ
	P2OUT|=SPI45DB041_CS;
	NOP();
	delay(10000);
}
*/
/********************************************************\
*	��������ReadOnePageToBuffer2
	�������ⲿ�ļ�����
*	���ܣ� 	�����洢��һ��ҳ�����ݵ�buffer2
*	������ 	PageAddress ��Ҫ�������洢����ҳ��ַ
*	����ֵ����
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void ReadOnePageToBuffer2(unsigned int PageAddress)
{
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();
	SPI45D041_WriteBits(0x55,8);        //��8 bit ��������
	SPI45D041_WriteBits(0x00,4);        //��4 bit �ı���λ
	SPI45D041_WriteBits(PageAddress,11);//��11λ��ҳ��ַ
	SPI45D041_WriteBits(0x00,9);        //��9λ��ҳ�ڵ�ַ
	P2OUT|=SPI45DB041_CS;
	NOP();
	delay(10000);
}

/********************************************************\
*	��������ReadByteFromBuffer1
	�������ⲿ�ļ�����
*	���ܣ� 	��buffer1�е�һ��ָ����ַ��һ���ֽڵ�����
*	������ 	ByteAddress ָ���ֽ���ҳ�ڵĵ�ַ
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
/*
unsigned char ReadByteFromBuffer1(unsigned int ByteAddress)
{
	unsigned char Buffer1OneByte;
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();

	SPI45D041_WriteBits(0x54,8);        //��8 bit ��������
	SPI45D041_WriteBits(0x00,15);       //��15 bit �ı���λ
	SPI45D041_WriteBits(ByteAddress,9);//��11λ��ҳ��ַ
	SPI45D041_WriteBits(0x00,8);        //��8 bit �ı���λ
	NOP();
	Buffer1OneByte=SPI45D041_ReadByte();
	return(Buffer1OneByte);
}
*/
/********************************************************\
*	��������ReadNextFromBuffer1
	�������ⲿ�ļ�����
*	���ܣ� 	��buffer1�е���һ����ַ���ֽ�����(��ϵ���ReadByteBuffer1һ��ʹ��)
*	������	��
*	����ֵ��Buffer1NextByte
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
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
*	��������ReadByteFromBuffer2
	�������ⲿ�ļ�����
*	���ܣ� ��buffer2�е�һ��ָ����ַ��һ���ֽڵ�����
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
unsigned char ReadByteFromBuffer2(unsigned int ByteAddress)
{
	unsigned char Buffer2OneByte;
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();

	SPI45D041_WriteBits(0x56,8);        //��8 bit ��������
	SPI45D041_WriteBits(0x00,15);       //��15 bit �ı���λ
	SPI45D041_WriteBits(ByteAddress,9);	//��11λ��ҳ��ַ
	SPI45D041_WriteBits(0x00,8);        //��8 bit �ı���λ
	NOP();
	Buffer2OneByte=SPI45D041_ReadByte();
	return(Buffer2OneByte);
}

/********************************************************\
*	��������ReadNextFromBuffer2
	�������ⲿ�ļ�����
*	���ܣ� ��buffer2�е���һ����ַ���ֽ�����(��ϵ���ReadByteBuffer2һ��ʹ��)
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
unsigned char ReadNextFromBuffer2(void)
{
	unsigned char Buffer2NextByte;
	Buffer2NextByte=SPI45D041_ReadByte();
	return(Buffer2NextByte);
}

/********************************************************\
*	��������WriteOneByteToBuffer1
	�������ⲿ�ļ�����
*	���ܣ� ��buffer1�е�һ��ָ���ĵ�ַдһ���ֽ�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
/*
void WriteOneByteToBuffer1(unsigned int ByteAddress,unsigned char WriteOneByte )
{
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();
	SPI45D041_WriteBits(0x84,8);        	//��8 bit ��������
	SPI45D041_WriteBits(0x00,15);       	//��15 bit ��NOP
	SPI45D041_WriteBits(ByteAddress,9); 	//��9 bit ��ҳ�ڵ�ַ
	SPI45D041_WriteBits(WriteOneByte,8);
	NOP();
}
*/
/********************************************************\
*	��������WriteNextByteToBuffer1
	�������ⲿ�ļ�����
*	���ܣ� дbuffer1�е���һ����ַ���������WriteOneByteToBuffer1һ��ʹ�ã�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
/*
void WriteNextByteToBuffer1(unsigned char WriteNextOne)
{
	SPI45D041_WriteBits(WriteNextOne,8);
}
*/
/********************************************************\
*	��������WriteOneByteToBuffer2
	�������ⲿ�ļ�����
*	���ܣ� ��buffer2�е�һ��ָ���ĵ�ַдһ���ֽ�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void WriteOneByteToBuffer2(unsigned int ByteAddress,unsigned char WriteOneByte )
{
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();
	SPI45D041_WriteBits(0x87,8);        	//��8 bit ��������
	SPI45D041_WriteBits(0x00,15);       	//��15 bit ��NOP
	SPI45D041_WriteBits(ByteAddress,9); 	//��9 bit ��ҳ�ڵ�ַ
	SPI45D041_WriteBits(WriteOneByte,8);
	NOP();
}

/********************************************************\
*	��������WriteNextByteToBuffer2
	�������ⲿ�ļ�����
*	���ܣ� дbuffer2�е���һ����ַ���������WriteOneByteToBuffer2һ��ʹ�ã�
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void WriteNextByteToBuffer2(unsigned char WriteNextOne)
{
	SPI45D041_WriteBits(WriteNextOne,8);
}

/********************************************************\
*	��������WriteBuffer1ToPage
	�������ⲿ�ļ�����
*	���ܣ� ��buffer1�е�����д�����洢��һ��ָ����ҳ��ǰ��������buffer1��ȷ������Ҫд�������
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
/*
void WriteBuffer1ToPage(unsigned int PageAddress)
{
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();
	SPI45D041_WriteBits(0x83,8);        //��8 bit ��������
	SPI45D041_WriteBits(0x00,4);        //��4 bit �ı���λ
	SPI45D041_WriteBits(PageAddress,11);//��11 bit ��ҳ��ַ
    SPI45D041_WriteBits(0x00,9);        //��9 bit ��NOP
    P2OUT|=SPI45DB041_CS;
    NOP();
    delay(100000);
}
*/
/********************************************************\
*	��������WriteBuffer2ToPage
	�������ⲿ�ļ�����
*	���ܣ� ��buffer2�е�����д�����洢��һ��ָ����ҳ��ǰ��������buffer2��ȷ������Ҫд�������
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void WriteBuffer2ToPage(unsigned int PageAddress)
{
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();
	SPI45D041_WriteBits(0x86,8);        //��8 bit ��������
	SPI45D041_WriteBits(0x00,4);        //��4 bit �ı���λ
	SPI45D041_WriteBits(PageAddress,11);//��11 bit ��ҳ��ַ
    SPI45D041_WriteBits(0x00,9);        //��9 bit ��NOP
    P2OUT|=SPI45DB041_CS;
    NOP();
    delay(100000);
}


/********************************************************\
*	��������EraseOnePage
	�������ⲿ�ļ�����
*	���ܣ� ָ��
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void EraseOnePage(unsigned int PageAddress)
{
	P2OUT|=SPI45DB041_SCK;
	NOP();
	P2OUT&=~SPI45DB041_CS;
	NOP();
	SPI45D041_WriteBits(0x81,8);        //��8 bit ��������
	SPI45D041_WriteBits(0x00,4);        //��4 bit �ı���λ
	SPI45D041_WriteBits(PageAddress,11);//��11 bit ��ҳ��ַ
    SPI45D041_WriteBits(0x00,9);        //��9 bit ��NOP
    P2OUT|=SPI45DB041_CS;
    NOP();
    delay(80000);
}

/********************************************************\
*	��������OperateSPIEnd
	�������ⲿ�ļ�����
*	���ܣ� ����SPI�Ĳ���
*	������
*	����ֵ��
*	�����ˣ�
*
*	�޸���ʷ����ÿ��������
\********************************************************/
void OperateSPIEnd(void)
{
	NOP();
	P2OUT|=SPI45DB041_CS;
	NOP();
}