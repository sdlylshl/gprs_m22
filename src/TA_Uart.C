
/********************************************************\
*	ÎÄ¼şÃû£º  TA_Uart.C
*	´´½¨Ê±¼ä£º2004Äê12ÔÂ7ÈÕ
*	´´½¨ÈË£º  
*	°æ±¾ºÅ£º  1.00
*	¹¦ÄÜ£º	  ¸ù¾İMSP430µÄÌØµã£¬Õâ²¿·ÖÊµÏÖMSP430Èı¸öÄ£Äâ´®¿ÚµÄ¹¦ÄÜ
			  °üÀ¨Èı¸öÄ£Äâ´®¿ÚµÄ·¢ËÍºÍ½ÓÊÕ
*	ÎÄ¼şÊôĞÔ£º¹«¹²ÎÄ¼ş
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©

\********************************************************/

#include <msp430x14x.h>
#include "TA_Uart.h"
#include "General.h"
#include "Sub_C.h"
#include "Define_Bit.h"

#define SEND_DELAY_TIME 	3

unsigned char SBUFIN2,SBUFIN3,SBUFIN4;
unsigned char SBUFOUT2,SBUFOUT3,SBUFOUT4;
unsigned char BitCnt2,BitCnt3,BitCnt4;
unsigned char RTI2,RTI3,RTI4;
unsigned char RIE;
unsigned int  RXTXData2,RXTXData3,RXTXData4;

void TX2_Byte(void);
void TX3_Byte(void);
void TX4_Byte(void);

/********************************************************\
*	º¯ÊıÃû£ºTX2_Byte
	×÷ÓÃÓò£º±¾ÎÄ¼şµ÷ÓÃ
*	¹¦ÄÜ£º  Ä£Äâ´®¿Ú2·¢ËÍÒ»¸ö×Ö½Ú×Ó³ÌĞò
*	²ÎÊı£º  SBUFOUT2
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
void TX2_Byte (void)
{
	RXTXData2=(int)SBUFOUT2;
	BitCnt2=0x0A;    					// Load Bit counter, 8data + ST/SP£¬×°10¸öBITÎ»
	RTI2 |= ~SEND_FLAG;					//Çå³ıÏàÓ¦µÄ·¢ËÍ±êÖ¾Î»
	TACCR0 = TAR;       				// Current state of TA counter£¬Çå¿Õ¼ÇÊıÆ÷
	TACCR0 += Bitime0;        			// Some time till first bit
	RXTXData2 |= 0x100;        			// Add mark stop bit to RXTXData 1ÎªÍ£Ö¹Î»
	RXTXData2 = RXTXData2 << 1;			// Add space start bit  0Îª¿ªÊ¼Î»£¨Î»×óÒÆ1Î»£©
	TACCTL0 = OUTMOD0+CCIE; 			// TXDÏÈÉèÖÃ1£¬ÖĞ¶ÏÔÊĞí
	//while ( TACCTL0 & CCIE ); 		// Wait for TX completion£¬µÈ´ıËùÓĞµÄÎ»¶¼·¢ËÍÍê³É
	while ( (~RTI2)&SEND_FLAG );
	gDelay_Com_Timer=0;
	while(gDelay_Com_Timer<SEND_DELAY_TIME);
}

/********************************************************\
*	º¯ÊıÃû£ºRX2_Ready
	×÷ÓÃÓò£ºÍâ²¿ÎÄ¼şµ÷ÓÃ
*	¹¦ÄÜ£º  Ä£Äâ´®¿Ú2×¼±¸½ÓÊÕÊı¾İ×Ó³ÌĞò£¨²¶»ñÄ£Ê½0£©
*	²ÎÊı£º  ÎŞ
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
void RX2_Ready (void)
{
	TA_Init();
	RTI2 &= ~RECEIVE_FLAG;
	BitCnt2=0x08;                     	//Load Bit counter£¬×°8¸öBITÎ»
	TACCTL0=SCS+OUTMOD0+CM1+CAP+CCIE; 	//Í¬²½²¶»ñSync
					                 	//CCIS0=0,ÔÚ²¶»ñÄ£Ê½ÖĞ£¬¶¨Òå²¶»ñÊÂ¼şµÄÊäÈëÔ´ÎªCCI0A
	                               		//OUTMOD0=1£¬Êä³öÄ£Ê½ÎªÖÃÎ»
	                               		//CM1=1£¬ÉèÖÃ²¶»ñÄ£Ê½1£¬ÏÂ½µÑØ²¶»ñ
                                   		//CAP=1£¬ÉèÖÃÎª²¶»ñÄ£Ê½
                                  		//CCIE=1£¬ÖĞ¶ÏÔÊĞí
}										//SCSÍ¬²½²¶»ñ£¬£¨CCIS0=1£¬CCIS1=0£©¶¨ÒåÌá¹©²¶»ñÊÂ¼şµÄÊäÈëÔ´

/********************************************************\
*	º¯ÊıÃû£ºTX3_Byte
	×÷ÓÃÓò£º±¾ÎÄ¼şµ÷ÓÃ
*	¹¦ÄÜ£º  Ä£Äâ´®¿Ú2·¢ËÍÒ»¸ö×Ö½Ú×Ó³ÌĞò
*	²ÎÊı£º  SBUFOUT3
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
void TX3_Byte (void)
{
	TACCTL0 &=~ CCIE;
	TBCCTL0 &=~ CCIE;
	IE2&=~URXIE1;
	RXTXData3=(int)SBUFOUT3;
	BitCnt3 = 0xA;            			// Load Bit counter, 8data + ST/SP£¬×°10¸öBITÎ»
	RTI3 |= ~SEND_FLAG;					//Çå³ıÏàÓ¦µÄ·¢ËÍ±êÖ¾Î»
	TACCR1 = TAR;             			// Current state of TA counter£¬Çå¿Õ¼ÇÊıÆ÷
	TACCR1 += Bitime1;          		// Some time till first bit
	RXTXData3 |= 0x100;         		// Add mark stop bit to RXTXData 1ÎªÍ£Ö¹Î»
	RXTXData3 = RXTXData3 << 1;  		// Add space start bit    0Îª¿ªÊ¼Î»£¨Î»×óÒÆ1Î»£©
	TACCTL1 = OUTMOD0+CCIE;      		// TXDÏÈÉèÖÃ1£¬ÖĞ¶ÏÔÊĞí,ÉèÖÃCCIFGÄÜ½øÈëÖĞ¶Ï
	//while (TACCTL1 |= CCIE);      	// Wait for TX completion£¬µÈ´ıËùÓĞµÄÎ»¶¼·¢ËÍÍê³É
	while( (~RTI3)&SEND_FLAG  );
	gDelay_Com_Timer=0;
	while(gDelay_Com_Timer<SEND_DELAY_TIME);
}
/********************************************************\
*	º¯ÊıÃû£ºRX3_Ready
	×÷ÓÃÓò£ºÍâ²¿ÎÄ¼şµ÷ÓÃ
*	¹¦ÄÜ£º  Ä£Äâ´®¿Ú3×¼±¸½ÓÊÕÊı¾İ×Ó³ÌĞò£¨²¶»ñÄ£Ê½1£©
*	²ÎÊı£º  ÎŞ
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
void RX3_Ready (void)
{
	TA_Init();
	RTI3 &= ~RECEIVE_FLAG;
	BitCnt3=0x8;                    	//Load Bit counter£¬×°8¸öBITÎ»
	TACCTL1=SCS+OUTMOD0+CM1+CAP+CCIE;  	//Í¬²½²¶»ñSync, £(
	                                    //CCIS0=0,ÔÚ²¶»ñÄ£Ê½ÖĞ£¬¶¨Òå²¶»ñÊÂ¼şµÄÊäÈëÔ´ÎªCCI0A
	                                    //OUTMOD0=1£¬Êä³öÄ£Ê½ÎªÖÃÎ»
	                                    //CM1=1£¬ÉèÖÃ²¶»ñÄ£Ê½1£¬ÏÂ½µÑØ²¶»ñ
	                                    //CAP=1£¬ÉèÖÃÎª²¶»ñÄ£Ê½
	                                 	//CCIE=1£¬ÖĞ¶ÏÔÊĞí
										//SCSÍ¬²½²¶»ñ£¬£¨CCIS0=1£¬CCIS1=0£©¶¨ÒåÌá¹©²¶»ñÊÂ¼şµÄÊäÈëÔ´
	gCommon_Flag|=ALLOW_R232_F_1;
}



/********************************************************\
*	º¯ÊıÃû£ºTX4_Byte
	×÷ÓÃÓò£º±¾ÎÄ¼şµ÷ÓÃ
*	¹¦ÄÜ£º  Ä£Äâ´®¿Ú4·¢ËÍÒ»¸ö×Ö½Ú×Ó³ÌĞò
	(¸ù¾İÕâ¸ö´®¿ÚÊÇÕë¶ÔĞĞÊ¹¼ÇÂ¼ÒÇ·¢ËÍ£¬ÔòĞèÒªÔÚ·¢ËÍÊı¾İÇ°£¬ĞèÒªÓĞÔö¼ÓÆæĞ£Ñé)

*	²ÎÊı£º  SBUFOUT4
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
²¹³äËµÃ÷£º
Odd Parity Ææ Ğ£ Ñé
Êı ¾İ ´« Êä Õı È· ĞÔ µÄ Ò» ÖÖ Ğ£ Ñé ·½ ·¨¡£
ÔÚ Êı ¾İ ´« Êä Ç° ¸½ ¼Ó Ò» Î» Ææ Ğ£ Ñé Î»£¬
ÓÃ À´ ±í Ê¾ ´« Êä µÄ Êı ¾İ ÖĞ"1"µÄ ¸ö Êı ÊÇ Ææ Êı »¹ ÊÇ Å¼ Êı£¬
Îª Ææ Êı Ê±£¬ Ğ£ Ñé Î» ÖÃ Îª"0"£¬ ·ñ Ôò ÖÃ Îª"1"£¬
ÓÃ ÒÔ ±£ ³Ö Êı ¾İ µÄ Ææ Å¼ ĞÔ ²» ±ä¡£ Àı Èç£¬
Ğè Òª ´« Êä"11001110"£¬ Êı ¾İ ÖĞ º¬5¸ö"1"£¬
Ëù ÒÔ Æä Ææ Ğ£ Ñé Î» Îª"0"£¬ Í¬ Ê± °Ñ"110011100"´« Êä ¸ø ½Ó ÊÕ ·½£¬
½Ó ÊÕ ·½ ÊÕ µ½ Êı ¾İ ºó ÔÙ Ò» ´Î ¼Æ Ëã Ææ Å¼ ĞÔ£¬"110011100"ÖĞ ÈÔ È» º¬ ÓĞ5¸ö"1"£¬
Ëù ÒÔ ½Ó ÊÕ ·½ ¼Æ Ëã ³ö µÄ Ææ Ğ£ Ñé Î» »¹ ÊÇ"0"£¬ Óë ·¢ ËÍ ·½ Ò» ÖÂ£¬
±í Ê¾ ÔÚ ´Ë ´Î ´« Êä ¹ı ³Ì ÖĞ Î´ ·¢ Éú ´í Îó¡£

\********************************************************/
void TX4_Byte (void)
{
	TACCTL0 &=~ CCIE;
	TBCCTL0 &=~ CCIE;
	IE2&=~URXIE1;
	RXTXData4=(int)SBUFOUT4;
	BitCnt4 = 0xA;         		    	// Load Bit counter, 8data + ST/SP£¬×°11¸öBITÎ»,(»¹Ôö¼ÓÒ»¸öÆæĞ£ÑéÎ»)
	RTI4 |= ~SEND_FLAG;			    	// Çå³ıÏàÓ¦µÄ·¢ËÍ±êÖ¾Î»
	TACCR2 = TAR;           	    	// Current state of TA counter£¬Çå¿Õ¼ÇÊıÆ÷
	TACCR2 += Bitime2;        	    	// Some time till first bit
	RXTXData4 |= 0x100;         	    // Add mark stop bit to RXTXData 1ÎªÍ£Ö¹Î»
	RXTXData4 = RXTXData4 << 1;   	    // Add space start bit    0Îª¿ªÊ¼Î»£¨Î»×óÒÆ1Î»£©
	TACCTL2 = OUTMOD0+CCIE;       	    // TXDÏÈÉèÖÃ1£¬ÖĞ¶ÏÔÊĞí,ÉèÖÃCCIFGÄÜ½øÈëÖĞ¶Ï
	//while (TACCTL2 |= CCIE);          // Wait for TX completion£¬µÈ´ıËùÓĞµÄÎ»¶¼·¢ËÍÍê³É
	while ( (~RTI4)&SEND_FLAG );		//Ñ­»·µÈ´ı·¢ËÍÍê³É£¨ÔÚÖĞ¶Ï×Ó³ÌĞòÖĞ£¬·¢ËÍÍê³ÉÔò»áÉèÖÃÏàÓ¦µÄ·¢ËÍÍê³É±êÖ¾Î»£©
	gDelay_Com_Timer=0;
	while(gDelay_Com_Timer<SEND_DELAY_TIME);
}
/*
void TX4_Byte (void)
{
	RXTXData4=(int)SBUFOUT4;
	BitCnt4 = 0xB;         		    	// Load Bit counter, 8data + ST/SP£¬×°11¸öBITÎ»,(»¹Ôö¼ÓÒ»¸öÆæĞ£ÑéÎ»)
	RTI4 |= ~SEND_FLAG;			    	// Çå³ıÏàÓ¦µÄ·¢ËÍ±êÖ¾Î»
	TACCR2 = TAR;           	    	// Current state of TA counter£¬Çå¿Õ¼ÇÊıÆ÷
	TACCR2 += Bitime2;        	    	// Some time till first bit
	RXTXData4 |= 0x200;         	    // Add mark stop bit to RXTXData 1ÎªÍ£Ö¹Î»
	//´¦ÀíÔö¼ÓĞ£ÑéÎ»µÄ²¿·Ö£º
	//1£¬ÏÈÒÆ¶¯Ò»Î»£¬½«Ğ£ÑéÎ»Ä¬ÈÏÎª0

	//2£¬¼ÆËãĞ£ÑéÎ»£¬Èç¹ûÊı¾İµÄ1µÄ¸öÊıÎªÅ¼Êı£¬Ôò½«Ğ£ÑéÎ»ÉèÖÃÎª1
	for(i=0;i<8;i++)
	{
		if( (SBUFOUT4&0x01)==0x01 )	j=j^0x01;
		SBUFOUT4 = SBUFOUT4 >> 1;
	}
	//3£¬ĞŞÕıĞ£ÑéÎ»
	if(j==1)	RXTXData4 |=0x100;
	//4£¬ÒÆ¶¯Ò»Î»£¬ÉèÖÃ¿ªÊ¼Î»
	RXTXData4 = RXTXData4 << 1;   	    // Add space start bit    0Îª¿ªÊ¼Î»£¨Î»×óÒÆ1Î»£©
	TACCTL2 = OUTMOD0+CCIE;       	    // TXDÏÈÉèÖÃ1£¬ÖĞ¶ÏÔÊĞí,ÉèÖÃCCIFGÄÜ½øÈëÖĞ¶Ï
	//while (TACCTL2 |= CCIE);          // Wait for TX completion£¬µÈ´ıËùÓĞµÄÎ»¶¼·¢ËÍÍê³É
	while ( (~RTI4)&SEND_FLAG );		//Ñ­»·µÈ´ı·¢ËÍÍê³É£¨ÔÚÖĞ¶Ï×Ó³ÌĞòÖĞ£¬·¢ËÍÍê³ÉÔò»áÉèÖÃÏàÓ¦µÄ·¢ËÍÍê³É±êÖ¾Î»£©
	gDelay_Com_Timer=0;
	while(gDelay_Com_Timer<SEND_DELAY_TIME);
}
*/
/********************************************************\
*	º¯ÊıÃû£ºRX4_Ready
	×÷ÓÃÓò£ºÍâ²¿ÎÄ¼şµ÷ÓÃ
*	¹¦ÄÜ£º  Ä£Äâ´®¿Ú4×¼±¸½ÓÊÕÊı¾İ×Ó³ÌĞò£¨²¶»ñÄ£Ê½1£©
*	²ÎÊı£º  ÎŞ
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
void RX4_Ready (void)
{
	TA_Init();
	RTI4 &= ~RECEIVE_FLAG;
	BitCnt4=0x08;                    	//Load Bit counter£¬×°8¸öBITÎ»
	TACCTL2=SCS+OUTMOD0+CM1+CAP+CCIE;  	//Í¬²½²¶»ñSync,
	                                   	//CCIS0=0,ÔÚ²¶»ñÄ£Ê½ÖĞ£¬¶¨Òå²¶»ñÊÂ¼şµÄÊäÈëÔ´ÎªCCI0A
	                                	//OUTMOD0=1£¬Êä³öÄ£Ê½ÎªÖÃÎ»
	                               		//CM1=1£¬ÉèÖÃ²¶»ñÄ£Ê½1£¬ÏÂ½µÑØ²¶»ñ
	                               		//CAP=1£¬ÉèÖÃÎª²¶»ñÄ£Ê½
	                              		//CCIE=1£¬ÖĞ¶ÏÔÊĞí
										//SCSÍ¬²½²¶»ñ£¬£¨CCIS0=1£¬CCIS1=0£©¶¨ÒåÌá¹©²¶»ñÊÂ¼şµÄÊäÈëÔ´
	gCommon_Flag|=ALLOW_R232_F_1;
}

/********************************************************\
*	º¯ÊıÃû£ºOFF_RXTXCOM2
	×÷ÓÃÓò£º´ËÏîÄ¿ÎÄ¼şÎ´µ÷ÓÃ
*	¹¦ÄÜ£º  ¹Ø±ÕÄ£Äâ´®¿Ú2·¢ËÍºÍ½ÓÊÕÔÊĞí
*	²ÎÊı£º  ÎŞ
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
/*
void OFF_RXTXCOM2(void)
{
	TACCTL0 &= ~ CCIE;
}
*/
/********************************************************\
*	º¯ÊıÃû£ºOFF_RXTXCOM3
	×÷ÓÃÓò£º´ËÏîÄ¿ÎÄ¼şÎ´µ÷ÓÃ
*	¹¦ÄÜ£º  ¹Ø±ÕÄ£Äâ´®¿Ú3·¢ËÍºÍ½ÓÊÕÔÊĞí
*	²ÎÊı£º  ÎŞ
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
/*
void OFF_RXTXCOM3(void)
{
	TACCTL1 &= ~ CCIE;
}
*/
/********************************************************\
*	º¯ÊıÃû£ºOFF_RXTXCOM4
	×÷ÓÃÓò£º´ËÏîÄ¿ÎÄ¼şÎ´µ÷ÓÃ
*	¹¦ÄÜ£º  ¹Ø±ÕÄ£Äâ´®¿Ú4·¢ËÍºÍ½ÓÊÕÔÊĞí
*	²ÎÊı£º  ÎŞ
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
/*
void OFF_RXTXCOM4(void)
{
	TACCTL2 &= ~ CCIE;
}
*/
/********************************************************\
*	º¯ÊıÃû£ºSend_COM2_Byte
	×÷ÓÃÓò£ºÍâ²¿ÎÄ¼şµ÷ÓÃ
*	¹¦ÄÜ£º  Í¨¹ıÄ£Äâ´®¿Ú2·¢ËÍÒ»¸ö×Ö½Ú
*	²ÎÊı£º  Input
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
void Send_COM2_Byte(unsigned char Input)
{
	SBUFOUT2=Input;
	TX2_Byte();
}

/********************************************************\
*	º¯ÊıÃû£ºSend_COM2_String
	×÷ÓÃÓò£ºÍâ²¿ÎÄ¼şµ÷ÓÃ
*	¹¦ÄÜ£º  Í¨¹ıÄ£Äâ´®¿Ú2·¢ËÍÒ»¸ö×Ö·û´®
*	²ÎÊı£º  *Point ×Ö·û´®Ö¸Õë
			 Count ×Ö·û´®³¤¶È
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
void Send_COM2_String(unsigned char *Point,unsigned int Count)
{
	while(Count>0)
	{
		SBUFOUT2=*Point;
		TX2_Byte();
		Point++;
		Count--;
	}
}
/********************************************************\
*	º¯ÊıÃû£ºSend_COM3_Byte
	×÷ÓÃÓò£ºÍâ²¿ÎÄ¼şµ÷ÓÃ
*	¹¦ÄÜ£º  Í¨¹ıÄ£Äâ´®¿Ú3·¢ËÍÒ»¸ö×Ö½Ú
*	²ÎÊı£º  Input
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
void Send_COM3_Byte(unsigned char Input)
{
	SBUFOUT3=Input;
	TX3_Byte();
}
/********************************************************\
*	º¯ÊıÃû£ºSend_COM3_String
	×÷ÓÃÓò£ºÍâ²¿ÎÄ¼şµ÷ÓÃ
*	¹¦ÄÜ£º  Í¨¹ıÄ£Äâ´®¿Ú3·¢ËÍÒ»¸ö×Ö·û´®
*	²ÎÊı£º  *Point ×Ö·û´®Ö¸Õë
			 Count ×Ö·û´®³¤¶È
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
void Send_COM3_String(unsigned char *Point,unsigned int Count)
{
	while(Count>0)
	{
		SBUFOUT3=*Point;
		TX3_Byte();
		Point++;
		Count--;
	}
	RX3_Ready();
}
/********************************************************\
*	º¯ÊıÃû£ºSend_COM4_Byte
	×÷ÓÃÓò£ºÍâ²¿ÎÄ¼şµ÷ÓÃ
*	¹¦ÄÜ£º  Í¨¹ıÄ£Äâ´®¿Ú4·¢ËÍÒ»¸ö×Ö½Ú
*	²ÎÊı£º  Input
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
void Send_COM4_Byte(unsigned char Input)
{
	SBUFOUT4=Input;
	TX4_Byte();
}
/********************************************************\
*	º¯ÊıÃû£ºSend_COM4_String
	×÷ÓÃÓò£ºÍâ²¿ÎÄ¼şµ÷ÓÃ
*	¹¦ÄÜ£º  Í¨¹ıÄ£Äâ´®¿Ú4·¢ËÍÒ»¸ö×Ö·û´®
*	²ÎÊı£º  *Point ×Ö·û´®Ö¸Õë
			 Count ×Ö·û´®³¤¶È
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
void Send_COM4_String(unsigned char *Point,unsigned int Count)
{
	while(Count>0)
	{
		SBUFOUT4=*Point;
		TX4_Byte();
		Point++;
		Count--;
	}
}
/********************************************************\
*	º¯ÊıÃû£ºTimer_A0£¨ÖĞ¶Ï×Ó³ÌĞò£©
	×÷ÓÃÓò£ºÍâ²¿ÎÄ¼şµ÷ÓÃ
*	¹¦ÄÜ£º  ¶¨Ê±Æ÷TAµÄÖĞ¶ÏTimer A CC0 (ÓÃ×öÄ£Äâ´®¿Ú2µÄ·¢ËÍºÍ½ÓÊÕÖĞ¶Ï´¦Àí)
*	²ÎÊı£º  ÎŞ
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
interrupt[TIMERA0_VECTOR] void Timer_A0 (void)
{
	TACCR0 += Bitime0;         				//¶¨Ê±µ½£¬ÔòCCR0µÄÖµÔö¼Ó£¬ÒÔ¼ÌĞøTA¼ÇÊ±
        //RX£¬½ÓÊÕÒ»¸ö×Ö½ÚµÄ³ÌĞò´¦Àí
  	if ( (~RTI2)&RECEIVE_FLAG )     	    //ÅĞ¶ÏÊÇ·ñÌá¹©²¶»ñÊÂ¼şµÄ²¶»ñÔ´ÓĞ²¶»ñÊÂ¼ş·¢Éú
  	{
    	if( TACCTL0 & CAP )     			//ÅĞ¶ÏÊÇ·ñ»¹´¦ÓÚ²¶»ñÄ£Ê½×´Ì¬
    										//Èç¹ûÎª²¶»ñÄ£Ê½£¨±íÊ¾½ÓÊÕµ½ÁË¿ªÊ¼Î»£©
    	{
      		TACCTL0 &= ~ CAP;     			//ÊÇ²¶»ñÄ£Ê½ Ôò½«²¶»ñÄ£Ê½±ä³É±È½ÏÄ£Ê½£¬×¼±¸¿ªÊ¼½ÓÊÕÊı¾İÎ»                                                              		//Ôò½«²¶»ñÄ£Ê½¸ü¸ÄÎª±È½ÏÄ£Ê½
      		TACCR0 += Bitime0_5;			//Ôö¼Ó¼ÆÊıµÄÖµ,Ôò¹²1.5¸öÊı¾İÎ»µÄÊ±¼ä³¤¶È
    	}
    	else	                			//ÅĞ¶Ï´¦ÓÚ±È½ÏÄ£Ê½×´Ì¬£¬Ôò¼ÌĞø½ÓÊÕÎ´½ÓÊÕÍêµÄÊı¾İÎ»
    										//Îª±È½ÏÄ£Ê½£¬Ôò¼ÌĞø½ÓÊÕÊı¾İÎ»
    	{
      		RXTXData2 = RXTXData2 >> 1; 	//Êı¾İÓÒÒÆ1Î»£¨ÏÈ½ÓÊÕÊı¾İµÍÎ»£©
      		if (TACCTL0 & SCCI)       		// ´ÓËø´æÆ÷ÖĞÈ¡³öÊäÈëĞÅºÅ£¬Èç¹ûÎª1£¬Ôò¼ÌĞø£¬·ñÔòÎª0
      		{
        		RXTXData2 |= 0x80;       	//½ÓÊÕĞÅºÅÎª1
      		}
      		BitCnt2 -- ;
      		if ( BitCnt2 == 0 )         	//ÅĞ¶ÏÊı¾İÊÇ·ñ½ÓÊÕÍê£¿
      		{
        		//TACCTL0 &= ~CCIE;       	//ËùÓĞµÄÎ»½ÓÊÕÍê±Ï£¬Ôò½ûÖ¹ÖĞ¶Ï
        		SBUFIN2=(char)RXTXData2;
        		TAUARTCTL|=TAUARTIFG;		//½øÈë±È½ÏÆ÷AµÄÖĞ¶ÏÏòÁ¿µÄ±êÖ¾Î»£¬Ôò½øÈë±È½ÏÆ÷AµÄÖĞ¶Ï
        		RTI2|=RECEIVE_FLAG;			//ÉèÖÃÄ£Äâ´®¿Ú2µÄ½ÓÊÕ±êÖ¾Î»
      		}
    	}
	}
	// TX£¬·¢ËÍÒ»¸ö×Ö½ÚµÄ³ÌĞò´¦Àí
  	else
  	{
    	if ( BitCnt2 == 0 )           		//ÅĞ¶ÏÊÇ·ñËùÓĞÊı¾İ·¢ËÍÍê³É
    	{
    		RTI2 |= SEND_FLAG;
          	//TACCTL0 &= ~ CCIE;          	//ËùÓĞµÄÎ»¶¼·¢ËÍÍê³É£¬Ôò½ûÖ¹ÖĞ¶Ï·¢
        }
    	else                        		//Î»Î´·¢ËÍÍê£¬Ôò¼ÌĞø·¢ËÍ
    	{
      		TACCTL0|=OUTMOD2+OUTMOD0;   	// TX SpaceÖÃµÍ
      		if (RXTXData2 & 0x01)        	//Èç¹û·¢ËÍÎ»Îª1
      		{
        		TACCTL0 &= ~ OUTMOD2;     	// TX Mark ÖÃ¸ß
      		}
      		RXTXData2 = RXTXData2 >> 1;
      		BitCnt2 --;
		}
  	}
}

/*
¶¨Ê±Æ÷TAµÄÖĞ¶Ï
Timer A CC1-2
*/
/********************************************************\
*	º¯ÊıÃû£ºTimer_A1£¨ÖĞ¶Ï×Ó³ÌĞò£©
	×÷ÓÃÓò£ºÍâ²¿ÎÄ¼şµ÷ÓÃ
*	¹¦ÄÜ£º  ¶¨Ê±Æ÷TAµÄÖĞ¶ÏTimer A CC1-2 (ÓÃ×öÄ£Äâ´®¿Ú3£¬4µÄ·¢ËÍºÍ½ÓÊÕÖĞ¶Ï´¦Àí£¬
			Á½¸öÄ£Äâ´®¿Ú·¢ËÍÊı¾İ£¬½ÓÊÕÊı¾İµÄ´¦Àí¾ùÔÚÕâ¸öÖĞ¶ÏÀïÍê³É)
*	²ÎÊı£º  ÎŞ
*	·µ»ØÖµ£ºÎŞ
*	´´½¨ÈË£º
*
*	ĞŞ¸ÄÀúÊ·£º£¨Ã¿ÌõÏêÊö£©
\********************************************************/
interrupt[TIMERA1_VECTOR] void Timer_A1 (void)
{
	if(TACCTL1&CCIFG)						//TACCIFG1ÉèÖÃÎ»ÖÃÎ»ÁË,·ÃÎÊTAIV£¬ÔòÏàÓ¦µÄCCIFG¸´Î»
   	{
		TACCTL1&=0xFFFE;
		TACCR1 += Bitime1;         			// ¶¨Ê±µ½£¬ÔòTACCR1µÄÖµÔö¼Ó£¬ÒÔ¼ÌĞøTA¼ÇÊ±
	 	if ( (~RTI3)&RECEIVE_FLAG )    		//ÅĞ¶ÏÊÇ·ñÌá¹©²¶»ñÊÂ¼şµÄ²¶»ñÔ´ÓĞ²¶»ñÊÂ¼ş·¢Éú£¿
	 	{
	   		if( TACCTL1 & CAP )     		// ÅĞ¶ÏÊÇ·ñ»¹´¦ÓÚ²¶»ñÄ£Ê½×´Ì¬	   																									//Èç¹ûÎª²¶»ñÄ£Ê½£¨±íÊ¾½ÓÊÕµ½ÁË¿ªÊ¼Î»£©
	   		{
	     		TACCTL1 &= ~ CAP;     		//ÊÇ²¶»ñÄ£Ê½ Ôò½«²¶»ñÄ£Ê½±ä³É±È½ÏÄ£Ê½£¬×¼±¸¿ªÊ¼½ÓÊÕÊı¾İÎ»                                                             		//Ôò½«²¶»ñÄ£Ê½¸ü¸ÄÎª±È½ÏÄ£Ê½
	     		TACCR1 += Bitime1_5;		//Ôö¼Ó¼ÆÊıµÄÖµ,Ôò¹²1.5¸öÊı¾İÎ»µÄÊ±¼ä³¤¶È
	     		//Í£Ö¹Ä£Äâ´®¿Ú2µÄÖĞ¶Ï
    			TACCTL0 &=~ CCIE;
    			TBCCTL0 &=~ CCIE;
    			IE2&=~URXIE1;
	   		}
	   		else	                		//ÅĞ¶Ï´¦ÓÚ±È½ÏÄ£Ê½×´Ì¬£¬Ôò¼ÌĞø½ÓÊÕÎ´½ÓÊÕÍêµÄÊı¾İÎ»																								//Îª±È½ÏÄ£Ê½£¬Ôò¼ÌĞø½ÓÊÕÊı¾İÎ»
	   		{
	     		RXTXData3 = RXTXData3 >> 1; //Êı¾İÓÒÒÆ1Î»£¨ÏÈ½ÓÊÕÊı¾İµÍÎ»£©
	     		if (TACCTL1 & SCCI)       	// ´ÓËø´æÆ÷ÖĞÈ¡³öÊäÈëĞÅºÅ£¬Èç¹ûÎª1£¬Ôò¼ÌĞø£¬·ñÔòÎª0
	     		{
	       			RXTXData3 |= 0x80;  	//½ÓÊÕĞÅºÅÎª1
	     		}
	     		BitCnt3 --;
	     		if ( BitCnt3 == 0)			//ÅĞ¶ÏÊı¾İÊÇ·ñ½ÓÊÕÍê£¿
	     		{
	       			SBUFIN3 = (char)RXTXData3;
	       			RTI3 |= RECEIVE_FLAG;
	       			//´¦Àí½ÓÊÕÊÖ±úµÄÊı¾İµÄ²¿·Ö
					gHandle_Buffer[gHandle_Buffer_Point]=SBUFIN3;
					gHandle_Buffer_Point++;
					if(gHandle_Buffer_Point>=sizeof(gHandle_Buffer)) gHandle_Buffer_Point=0;
					gHandle_Receive_Over=0;
					RTI3 &= ~RECEIVE_FLAG;
					BitCnt3=0x8;            //Load Bit counter£¬×°8¸öBITÎ»
					TACCTL1=SCS+OUTMOD0+CM1+CAP+CCIE;
					TACCTL0 |= CCIE;
					TBCCTL0 |= CCIE;
					IE2|=URXIE1;
	     		}
	   		}
	 	}
	 	else                   				//Îª·¢ËÍÖĞ¶Ï
	 	{
	    	if ( BitCnt3 == 0)           	//ÅĞ¶ÏÊÇ·ñËùÓĞÊı¾İ·¢ËÍÍê³É
	    	{
	    		RTI3 |= SEND_FLAG;			//ÉèÖÃÏàÓ¦µÄ·¢ËÍ³É¹¦µÄ±êÖ¾Î»
				TACCTL1 &= ~ CCIE;
				TACCTL0 |= CCIE;
				TBCCTL0 |= CCIE;
				IE2|=URXIE1;
			}
	    	else                        	//Î»Î´·¢ËÍÍê£¬Ôò¼ÌĞø·¢ËÍ
	    	{
	      		TACCTL1|=OUTMOD2+OUTMOD0; 	// TX SpaceÖÃµÍ
	      		if (RXTXData3 & 0x01)      	//Èç¹û·¢ËÍÎ»Îª1
	      		{
	        		TACCTL1 &= ~ OUTMOD2;	// TX Mark ÖÃ¸ß
	      		}
	      		RXTXData3= RXTXData3 >> 1;
	      		BitCnt3 --;
	    	}
	 	}
	}

   	if(TACCTL2&CCIFG)						//TACCIFG2ÉèÖÃÎ»ÖÃÎ»ÁË
   	{
   	 	TACCTL2&=0xFFFE;
	 	TACCR2 += Bitime2;         			// ¶¨Ê±µ½£¬ÔòTACCR2µÄÖµÔö¼Ó£¬ÒÔ¼ÌĞøTA¼ÇÊ±
	 	if ((~RTI4)&RECEIVE_FLAG)     	 	//ÅĞ¶ÏÊÇ·ñÌá¹©²¶»ñÊÂ¼şµÄ²¶»ñÔ´ÓĞ²¶»ñÊÂ¼ş·¢Éú£¿
	 	{
	   		if( TACCTL2 & CAP )     		// ÅĞ¶ÏÊÇ·ñ»¹´¦ÓÚ²¶»ñÄ£Ê½×´Ì¬	   																									//Èç¹ûÎª²¶»ñÄ£Ê½£¨±íÊ¾½ÓÊÕµ½ÁË¿ªÊ¼Î»£©
	   		{
		     	TACCTL2 &= ~ CAP;     		//ÊÇ²¶»ñÄ£Ê½ Ôò½«²¶»ñÄ£Ê½±ä³É±È½ÏÄ£Ê½£¬×¼±¸¿ªÊ¼½ÓÊÕÊı¾İÎ»                                                             		//Ôò½«²¶»ñÄ£Ê½¸ü¸ÄÎª±È½ÏÄ£Ê½
		     	TACCR2 += Bitime2_5;		//Ôö¼Ó¼ÆÊıµÄÖµ,Ôò¹²1.5¸öÊı¾İÎ»µÄÊ±¼ä³¤¶È
	     		//Í£Ö¹Ä£Äâ´®¿Ú2ºÍTBµÄÖĞ¶Ï
				TACCTL0 &=~ CCIE;
				TBCCTL0 &=~ CCIE;
				IE2&=~URXIE1;
	   		}
	   		else	                		//ÅĞ¶Ï´¦ÓÚ±È½ÏÄ£Ê½×´Ì¬£¬Ôò¼ÌĞø½ÓÊÕÎ´½ÓÊÕÍêµÄÊı¾İÎ» 																									//Îª±È½ÏÄ£Ê½£¬Ôò¼ÌĞø½ÓÊÕÊı¾İÎ»
	   		{
	     		RXTXData4 = RXTXData4 >> 1; //Êı¾İÓÒÒÆ1Î»£¨ÏÈ½ÓÊÕÊı¾İµÍÎ»£©
	     		if (TACCTL2 & SCCI)       	// ´ÓËø´æÆ÷ÖĞÈ¡³öÊäÈëĞÅºÅ£¬Èç¹ûÎª1£¬Ôò¼ÌĞø£¬·ñÔòÎª0
	     		{
	      			RXTXData4 |= 0x80;      //½ÓÊÕĞÅºÅÎª1(½ÓÊÕ9¸öbit£¬ÒòÎª»¹ÓĞÒ»¸öÆæĞ£ÑéÎ»)
	     		}
	     		BitCnt4 --;
	     		if ( BitCnt4 == 0)         	//ÅĞ¶ÏÊı¾İÊÇ·ñ½ÓÊÕÍê£¿
	     		{
	       			SBUFIN4 = (char)RXTXData4;
	       			RTI4 |= RECEIVE_FLAG;
					gDisp_Buffer_R[gDisp_Buffer_R_Point]=SBUFIN4;
					gDisp_Buffer_R_Point++;
					if(gDisp_Buffer_R_Point>=sizeof(gDisp_Buffer_R)) gDisp_Buffer_R_Point=0;
					gDisp_Receive_Over=0;
					RTI4 &= ~RECEIVE_FLAG;
					BitCnt4=0x08;                   //Load Bit counter£¬×°8¸öBITÎ»
					TACCTL2=SCS+OUTMOD0+CM1+CAP+CCIE;
					TACCTL0 |= CCIE;
					TBCCTL0 |= CCIE;
					IE2|=URXIE1;
	     		}
	   		}
		}
		else                       			//Îª·¢ËÍÖĞ¶Ï
		{
			if ( BitCnt4 == 0)				//ÅĞ¶ÏÊÇ·ñËùÓĞÊı¾İ·¢ËÍÍê³É£¿
			{
				RTI4 |= SEND_FLAG;
				TACCTL2 &= ~ CCIE;
				TACCTL0 |= CCIE;
				TBCCTL0 |= CCIE;
				IE2|=URXIE1;
			}
			else                     		//Î»Î´·¢ËÍÍê£¬Ôò¼ÌĞø·¢ËÍ
			{
				TACCTL2|=OUTMOD2+OUTMOD0; 	// TX SpaceÖÃµÍ
				if (RXTXData4 & 0x01)     	//Èç¹û·¢ËÍÎ»Îª1
				{
					TACCTL2 &= ~ OUTMOD2;  	// TX Mark ÖÃ¸ß
				}
				RXTXData4= RXTXData4 >> 1;
				BitCnt4 --;
			}
		}
	}
}
