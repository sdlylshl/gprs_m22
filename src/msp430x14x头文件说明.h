/********************************************************************
*
* Standard register and bit definitions for the Texas Instruments
* MSP430 microcontroller.
*
* This file supports assembler and C development for
* MSP430x14x devices.
*
* Texas Instruments, Version 1.5
*
* Rev. 1.2, Additional Timer B bit definitions.
*           Renamed XTOFF to XT2OFF.
*
* Rev. 1.3, Removed leading 0 to aviod interpretation as octal
*            values under C
*           Included <In430.h> rather than "In430.h"
*
* Rev. 1.4, Corrected LPMx_EXIT to reference new intrinsic _BIC_SR_IRQ
*           Changed TAIV and TBIV to be read-only
*
* Rev. 1.5, Enclose all #define statements with parentheses
*
********************************************************************/

#ifndef __msp430x14x
#define __msp430x14x

/************************************************************
* STANDARD BITS,��׼��λ����
************************************************************/

#define BIT0                (0x0001)
#define BIT1                (0x0002)
#define BIT2                (0x0004)
#define BIT3                (0x0008)
#define BIT4                (0x0010)
#define BIT5                (0x0020)
#define BIT6                (0x0040)
#define BIT7                (0x0080)
#define BIT8                (0x0100)
#define BIT9                (0x0200)
#define BITA                (0x0400)
#define BITB                (0x0800)
#define BITC                (0x1000)
#define BITD                (0x2000)
#define BITE                (0x4000)
#define BITF                (0x8000)

/************************************************************
* STATUS REGISTER BITS
************************************************************/
/*״̬�Ĵ�����ÿ��λ����*/
#define C                   (0x0001)
#define Z                   (0x0002)
#define N                   (0x0004)
#define V                   (0x0100)  /*����״̬λ*/
#define GIE                 (0x0008)  /*ͨ���ж�����λ*/
#define CPUOFF              (0x0010)
#define OSCOFF              (0x0020)
#define SCG0                (0x0040)
#define SCG1                (0x0080)

/* Low Power Modes coded with Bits 4-7 in SR */

#ifndef __IAR_SYSTEMS_ICC /* Begin #defines for assembler *//*�ڻ���µĶ���*/
#define LPM0                (CPUOFF)                   /*�͹���ģʽ0*/
#define LPM1                (SCG0+CPUOFF)              /*�͹���ģʽ1*/
#define LPM2                (SCG1+CPUOFF)              /*�͹���ģʽ2*/
#define LPM3                (SCG1+SCG0+CPUOFF)         /*�͹���ģʽ3*/
#define LPM4                (SCG1+SCG0+OSCOFF+CPUOFF)  /*�͹���ģʽ4*/
/* End #defines for assembler */

#else /* Begin #defines for C */                           /*��C�����µĶ���*/
#define LPM0_bits           (CPUOFF)
#define LPM1_bits           (SCG0+CPUOFF)
#define LPM2_bits           (SCG1+CPUOFF)
#define LPM3_bits           (SCG1+SCG0+CPUOFF)
#define LPM4_bits           (SCG1+SCG0+OSCOFF+CPUOFF)

#include <In430.h>

#define LPM0      _BIS_SR(LPM0_bits)     /* Enter Low Power Mode 0 */
#define LPM0_EXIT _BIC_SR_IRQ(LPM0_bits) /* Exit Low Power Mode 0 */
#define LPM1      _BIS_SR(LPM1_bits)     /* Enter Low Power Mode 1 */
#define LPM1_EXIT _BIC_SR_IRQ(LPM1_bits) /* Exit Low Power Mode 1 */
#define LPM2      _BIS_SR(LPM2_bits)     /* Enter Low Power Mode 2 */
#define LPM2_EXIT _BIC_SR_IRQ(LPM2_bits) /* Exit Low Power Mode 2 */
#define LPM3      _BIS_SR(LPM3_bits)     /* Enter Low Power Mode 3 */
#define LPM3_EXIT _BIC_SR_IRQ(LPM3_bits) /* Exit Low Power Mode 3 */
#define LPM4      _BIS_SR(LPM4_bits)     /* Enter Low Power Mode 4 */
#define LPM4_EXIT _BIC_SR_IRQ(LPM4_bits) /* Exit Low Power Mode 4 */
#endif /* End #defines for C */

/************************************************************
* PERIPHERAL FILE MAP
************************************************************/

/************************************************************
* SPECIAL FUNCTION REGISTER ADDRESSES + CONTROL BITS
  ���⹦�ܼĴ�����ַ������bits
************************************************************/

#define IE1_                 (0x0000)     /* Interrupt Enable 1 */
sfrb    IE1               = IE1_;         /*�ж�����Ĵ���1*/
#define WDTIE               (0x01)  /*WDT����ѡ���Ź�ģʽʱ��Ч*/
#define OFIE                (0x02)  /*���������ж�����*/
#define NMIIE               (0x10)  /*NMI�ж�����*/
#define ACCVIE              (0x20)  /*FLASH�洢���Ƿ������ж�����*/
#define URXIE0              (0x40)  /*URSART0�����ж�����*/
#define UTXIE0              (0x80)  /*URSART0�����ж�����*/

#define IFG1_               (0x0002)     /* Interrupt Flag 1 */
sfrb    IFG1              = IFG1_;       /*�жϱ�־�Ĵ���1*/
#define WDTIFG              (0x01)  /*���Ź�ģʽʱ������ܳײ�����ʱ��λ*/
#define OFIFG               (0x02)  /*����ʧЧʱ��λ*/
#define NMIIFG              (0x10)  /*RST/NMI�����ź�*/
#define URXIFG0             (0x40)  /*USART0���ձ�־*/
#define UTXIFG0             (0x80)  /*USART0���ͱ�־*/

#define ME1_                (0x0004)     /* Module Enable 1 */
sfrb    ME1               = ME1_;        /*ģ������Ĵ���1*/
#define URXE0               (0x40)  /*USART0��������UARTģʽ��*/
#define USPIE0              (0x40)  /*USART0�������������SPIģʽ��*/
#define UTXE0               (0x80)  /*USART0��������UARTģʽ��*/

#define IE2_                (0x0001)     /* Interrupt Enable 2 */
sfrb    IE2               = IE2_;        /*�ж�����Ĵ���2*/
#define URXIE1              (0x10)  /*USART1�����ж�����*/
#define UTXIE1              (0x20)  /*USART1�����ж�����*/

#define IFG2_               (0x0003)     /* Interrupt Flag 2 */
sfrb    IFG2              = IFG2_;       /*�жϱ�־�Ĵ���*/
#define URXIFG1             (0x10)  /*USART1���ձ�־*/
#define UTXIFG1             (0x20)  /*USART1���ͱ�־*/

#define ME2_                (0x0005)     /* Module Enable 2 */
sfrb    ME2               = ME2_;        /*ģ������Ĵ���2*/
#define URXE1               (0x10)  /*USART1��������UARTģʽ��*/
#define USPIE1              (0x10)  /*USART1�������������SPIģʽ��*/
#define UTXE1               (0x20)  /*USART1��������UARTģʽ��*/

/************************************************************
* WATCHDOG TIMER ���ǹ��Ķ�ʱ��
************************************************************/

#define WDTCTL_             (0x0120)  /* Watchdog Timer Control */
sfrw    WDTCTL            = WDTCTL_;  /*WTD���ƼĴ���*/
/* The bit names have been prefixed with "WDT" */
#define WDTIS0              (0x0001)  /**/
#define WDTIS1              (0x0002)  /*ѡ��WDTTCNT��4�����֮һ*/
#define WDTSSEL             (0x0004)  /*ѡ��WDTCNT��ʱ��Դ��Ϊ0--WDTCNT��SMCLKΪʱ��Դ��Ϊ1--WDTCNT��ACLKΪʱ��Դ*/
#define WDTCNTCL            (0x0008)  /*�����������ģʽ�У�д1��ʹWDTCNT��0�������������޶���*/
#define WDTTMSEL            (0x0010)  /*ѡ����ģʽ��Ϊ0---���Ź�ģʽ��1---��ʱ��ģʽ*/
#define WDTNMI              (0x0020)  /*ѡ��RST/NMI���Ź��ܣ���PUC��λ��NMI=0��RST/NMI����Ϊ��λ�ˣ�NMI=1��RST/NMI������Ϊ���ش����ķ������ж�����*/
#define WDTNMIES            (0x0040)  /*�����NMI���ܣ���NMIESѡ��RST/NMI����Ĵ����أ�NMIES=0�������ش���NMI�ж�*/
#define WDTHOLD             (0x0080)  /*����ֹͣWDT������WDTHOLD=0��WDT���ܼ��WDTHOLD=1��ʱ�������ֹ������ֹͣ*/

#define WDTPW               (0x5A00)  /**/

/* WDT-interval times [1ms] coded with Bits 0-2 */
/* WDT is clocked by fMCLK (assumed 1MHz) */
#define WDT_MDLY_32         (WDTPW+WDTTMSEL+WDTCNTCL)                         /* 32ms interval (default) */
#define WDT_MDLY_8          (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS0)                  /* 8ms     " */
#define WDT_MDLY_0_5        (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS1)                  /* 0.5ms   " */
#define WDT_MDLY_0_064      (WDTPW+WDTTMSEL+WDTCNTCL+WDTIS1+WDTIS0)           /* 0.064ms " */
/* WDT is clocked by fACLK (assumed 32KHz) */
#define WDT_ADLY_1000       (WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL)                 /* 1000ms  " */
#define WDT_ADLY_250        (WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL+WDTIS0)          /* 250ms   " */
#define WDT_ADLY_16         (WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL+WDTIS1)          /* 16ms    " */
#define WDT_ADLY_1_9        (WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL+WDTIS1+WDTIS0)   /* 1.9ms   " */
/* Watchdog mode -> reset after expired time */
/* WDT is clocked by fMCLK (assumed 1MHz) */
#define WDT_MRST_32         (WDTPW+WDTCNTCL)                                  /* 32ms interval (default) */
#define WDT_MRST_8          (WDTPW+WDTCNTCL+WDTIS0)                           /* 8ms     " */
#define WDT_MRST_0_5        (WDTPW+WDTCNTCL+WDTIS1)                           /* 0.5ms   " */
#define WDT_MRST_0_064      (WDTPW+WDTCNTCL+WDTIS1+WDTIS0)                    /* 0.064ms " */
/* WDT is clocked by fACLK (assumed 32KHz) */
#define WDT_ARST_1000       (WDTPW+WDTCNTCL+WDTSSEL)                          /* 1000ms  " */
#define WDT_ARST_250        (WDTPW+WDTCNTCL+WDTSSEL+WDTIS0)                   /* 250ms   " */
#define WDT_ARST_16         (WDTPW+WDTCNTCL+WDTSSEL+WDTIS1)                   /* 16ms    " */
#define WDT_ARST_1_9        (WDTPW+WDTCNTCL+WDTSSEL+WDTIS1+WDTIS0)            /* 1.9ms   " */

/* INTERRUPT CONTROL */
/* These two bits are defined in the Special Function Registers */
/* #define WDTIE               0x01 */
/* #define WDTIFG              0x01 */

/************************************************************
* HARDWARE MULTIPLIERӲ���˷���
************************************************************/

#define MPY_                (0x0130)  /* Multiply Unsigned/Operand 1 */
sfrw    MPY               = MPY_;
#define MPYS_               (0x0132)  /* Multiply Signed/Operand 1 */
sfrw    MPYS              = MPYS_;
#define MAC_                (0x0134)  /* Multiply Unsigned and Accumulate/Operand 1 */
sfrw    MAC               = MAC_;
#define MACS_               (0x0136)  /* Multiply Signed and Accumulate/Operand 1 */
sfrw    MACS              = MACS_;
#define OP2_                (0x0138)  /* Operand 2 */
sfrw    OP2               = OP2_;
#define RESLO_              (0x013A)  /* Result Low Word */
sfrw    RESLO             = RESLO_;
#define RESHI_              (0x013C)  /* Result High Word */
sfrw    RESHI             = RESHI_;
#define SUMEXT_             (0x013E)  /* Sum Extend */
const sfrw SUMEXT         = SUMEXT_;

/************************************************************
* DIGITAL I/O Port1/2��������˿�I/0 1��2
************************************************************/

#define P1IN_               (0x0020)  /* Port 1 Input    P1����Ĵ��� */
const sfrb P1IN           = P1IN_;
#define P1OUT_              (0x0021)  /* Port 1 Output   P1����Ĵ��� */
sfrb    P1OUT             = P1OUT_;
#define P1DIR_              (0x0022)  /* Port 1 Direction P1����Ĵ���*/
sfrb    P1DIR             = P1DIR_;
#define P1IFG_              (0x0023)  /* Port 1 Interrupt Flag P1�жϱ�־λ */
sfrb    P1IFG             = P1IFG_;
#define P1IES_              (0x0024)  /* Port 1 Interrupt Edge Select  P1�жϴ�����ѡ��*/
sfrb    P1IES             = P1IES_;
#define P1IE_               (0x0025)  /* Port 1 Interrupt Enable P1�ж�����*/
sfrb    P1IE              = P1IE_;
#define P1SEL_              (0x0026)  /* Port 1 Selection  P1����ѡ��Ĵ���*/
sfrb    P1SEL             = P1SEL_;

#define P2IN_               (0x0028)  /* Port 2 Input */
const sfrb P2IN           = P2IN_;
#define P2OUT_              (0x0029)  /* Port 2 Output */
sfrb    P2OUT             = P2OUT_;
#define P2DIR_              (0x002A)  /* Port 2 Direction */
sfrb    P2DIR             = P2DIR_;
#define P2IFG_              (0x002B)  /* Port 2 Interrupt Flag */
sfrb    P2IFG             = P2IFG_;
#define P2IES_              (0x002C)  /* Port 2 Interrupt Edge Select */
sfrb    P2IES             = P2IES_;
#define P2IE_               (0x002D)  /* Port 2 Interrupt Enable */
sfrb    P2IE              = P2IE_;
#define P2SEL_              (0x002E)  /* Port 2 Selection */
sfrb    P2SEL             = P2SEL_;

/************************************************************
* DIGITAL I/O Port3/4  ��������˿�I/O  3��4
************************************************************/

#define P3IN_               (0x0018)  /* Port 3 Input */
const sfrb P3IN           = P3IN_;    /*P3����Ĵ���*/
#define P3OUT_              (0x0019)  /* Port 3 Output */
sfrb    P3OUT             = P3OUT_;   /*P3����Ĵ���*/
#define P3DIR_              (0x001A)  /* Port 3 Direction */
sfrb    P3DIR             = P3DIR_;   /*P3����Ĵ���*/
#define P3SEL_              (0x001B)  /* Port 3 Selection */
sfrb    P3SEL             = P3SEL_;   /*P3�˿�ѡ��Ĵ���*/

#define P4IN_               (0x001C)  /* Port 4 Input */
const sfrb P4IN           = P4IN_;
#define P4OUT_              (0x001D)  /* Port 4 Output */
sfrb    P4OUT             = P4OUT_;
#define P4DIR_              (0x001E)  /* Port 4 Direction */
sfrb    P4DIR             = P4DIR_;
#define P4SEL_              (0x001F)  /* Port 4 Selection */
sfrb    P4SEL             = P4SEL_;

/************************************************************
* DIGITAL I/O Port5/6  ��������˿�I/O 5��6
************************************************************/

#define P5IN_               (0x0030)  /* Port 5 Input */
const sfrb P5IN           = P5IN_;
#define P5OUT_              (0x0031)  /* Port 5 Output */
sfrb    P5OUT             = P5OUT_;
#define P5DIR_              (0x0032)  /* Port 5 Direction */
sfrb    P5DIR             = P5DIR_;
#define P5SEL_              (0x0033)  /* Port 5 Selection */
sfrb    P5SEL             = P5SEL_;

#define P6IN_               (0x0034)  /* Port 6 Input */
const sfrb P6IN           = P6IN_;
#define P6OUT_              (0x0035)  /* Port 6 Output */
sfrb    P6OUT             = P6OUT_;
#define P6DIR_              (0x0036)  /* Port 6 Direction */
sfrb    P6DIR             = P6DIR_;
#define P6SEL_              (0x0037)  /* Port 6 Selection */
sfrb    P6SEL             = P6SEL_;

/************************************************************
* USART  USARTͨ��ģ��
************************************************************/
/* UCTL��USART���ƼĴ���UCTL */
#define PENA                (0x80) /*У������λ*/
#define PEV                 (0x40) /*��żУ��λ*/
#define SPB                 (0x20) /*ֹͣλ����Ϊ0��1λֹͣλ��Ϊ1��2λֹͣλ to distinguish from stackpointer SP */
#define CHAR                (0x10) /*�ַ����ȣ�CHAR=0��7λ��CHAR=1��8λ*/
#define LISTEN              (0x08) /*�Ƿ񽫷��͵��������ڲ���������������LISTEN=0���޷���*/
#define SYNC                (0x04) /*ģ�鹦��ѡ��SYNC=0��UARTģʽ��SYNC=1��SPIģʽ*/
#define MM                  (0x02) /*���ģʽ��MM=0����·���ж��Э�飬MM=1����ַλ���Э��*/
#define SWRST               (0x01) /*ϵͳ��λ��ֻ�ж�SWRST��λ��USART���ܴ��±�����*/
/* UTCTL ���Ϳ��ƼĴ���UTCTL*/
#define CKPH                (0x80) /**/
#define CKPL                (0x40) /*ʱ�Ӽ��ԣ�CKPL=0��UCLKI�ź���UCLK������ͬ��Ϊ1���෴*/
#define SSEL1               (0x20) /*ʱ��Դѡ��ȷ�����ڲ����ʷ�������ʱ��Դ*/
#define SSEL0               (0x10) /*SSEL1��0     0--�ⲿʱ��UCLKI��1--����ʱ��ACLK��2��3---ϵͳ��ʱ��SMCLK*/
#define URXSE               (0x08) /*�����жϷ������������΢������ʱ��ϵͳ*/
#define TXWAKE              (0x04) /*���ƶദ���ͨ��ģʽ�ķ�������*/
#define STC                 (0x02)
#define TXEPT               (0x01) /*�������ձ�־���ڷ�����λ�Ĵ�����UTXBUF��ʱ��λ��������д��UTXBUFʱ��λ��Ҳ��SWRST��λʱ��λ*/
/* URCTL ���տ��ƼĴ���URCTL */
#define FE                  (0x80) /*��һ�������ַ���ֹͣλΪ0����װ����ջ���ʱ��֡���־��λ��FE��SWRST��PUC��URXBUF����λ*/
#define PE                  (0x40) /*��һ�������ַ��е�1�ĸ���������У��λ�����ϱ�װ����ջ���ʱ��У����־PE��λ*/
#define OE                  (0x20) /*��һ���ַ�д��URXBUFʱǰһ���ַ���δ���������������־OE��λ*/
#define BRK                 (0x10) /*��һ�δ�Ϸ�������URXEIE��λ����ϼ��λBRK��λ��ֻ��ͨ��SWRST��ϵͳ��λ�Ͷ�ȡURXBUF����λ*/
#define URXEIE              (0x08) /*���ճ����ַ��ж�����λ��URXEIE=0�����յĳ����ַ����ı�URXIFGλ����֮������URXWIEλ�����ã��κ��ַ�����ʹURXIFG��λ*/
#define URXWIE              (0x04) /*���ջ����ж�����λ��Ϊ0��ÿһ�����յ��ַ�����ʹURXIFG��λ��Ϊ1��ֻ�е�ַ�ַ�����ʹURXIFG��λ*/
#define RXWAKE              (0x02) /*���ջ��Ѽ��RXWAKEλ�������յ��ַ���һ��ַ�ַ��ұ�������ջ��棬��λ*/
#define RXERR               (0x01) /*���մ���λRXERR��λ������һ�����������־��FE��PE��OE��BRK����λ*/

/************************************************************
* USART 0  ͨ��ģ��0
************************************************************/

#define U0CTL_              (0x0070)  /* UART 0 Control */
sfrb    U0CTL             = U0CTL_;
#define U0TCTL_             (0x0071)  /* UART 0 Transmit Control */
sfrb    U0TCTL            = U0TCTL_;
#define U0RCTL_             (0x0072)  /* UART 0 Receive Control */
sfrb    U0RCTL            = U0RCTL_;
#define U0MCTL_             (0x0073)  /* UART 0 Modulation Control */
sfrb    U0MCTL            = U0MCTL_;
#define U0BR0_              (0x0074)  /* UART 0 Baud Rate 0 */
sfrb    U0BR0             = U0BR0_;
#define U0BR1_              (0x0075)  /* UART 0 Baud Rate 1 */
sfrb    U0BR1             = U0BR1_;
#define U0RXBUF_            (0x0076)  /* UART 0 Receive Buffer */
const sfrb U0RXBUF        = U0RXBUF_;
#define U0TXBUF_            (0x0077)  /* UART 0 Transmit Buffer */
sfrb    U0TXBUF           = U0TXBUF_;

/* Alternate register names */
/*�Ĵ���������*/
#define UCTL0_              (0x0070)  /* UART 0 Control UART0���ƼĴ���*/
sfrb    UCTL0             = UCTL0_;
#define UTCTL0_             (0x0071)  /* UART 0 Transmit Control���Ϳ��ƼĴ��� */
sfrb    UTCTL0            = UTCTL0_;
#define URCTL0_             (0x0072)  /* UART 0 Receive Control���տ��ƼĴ��� */
sfrb    URCTL0            = URCTL0_;
#define UMCTL0_             (0x0073)  /* UART 0 Modulation Control�������ƼĴ��� */
sfrb    UMCTL0            = UMCTL0_;
#define UBR00_              (0x0074)  /* UART 0 Baud Rate 0 ������ѡ��Ĵ���0*/
sfrb    UBR00             = UBR00_;
#define UBR10_              (0x0075)  /* UART 0 Baud Rate 1 ������ѡ��Ĵ���1*/
sfrb    UBR10             = UBR10_;
#define RXBUF0_             (0x0076)  /* UART 0 Receive Buffer�������ݻ��� */
const sfrb RXBUF0         = RXBUF0_;
#define TXBUF0_             (0x0077)  /* UART 0 Transmit Buffer�������ݻ��� */
sfrb    TXBUF0            = TXBUF0_;

#define UCTL_0_             (0x0070)  /* UART 0 Control */
sfrb    UCTL_0            = UCTL_0_;
#define UTCTL_0_            (0x0071)  /* UART 0 Transmit Control */
sfrb    UTCTL_0           = UTCTL_0_;
#define URCTL_0_            (0x0072)  /* UART 0 Receive Control */
sfrb    URCTL_0           = URCTL_0_;
#define UMCTL_0_            (0x0073)  /* UART 0 Modulation Control */
sfrb    UMCTL_0           = UMCTL_0_;
#define UBR0_0_             (0x0074)  /* UART 0 Baud Rate 0 */
sfrb    UBR0_0            = UBR0_0_;
#define UBR1_0_             (0x0075)  /* UART 0 Baud Rate 1 */
sfrb    UBR1_0            = UBR1_0_;
#define RXBUF_0_            (0x0076)  /* UART 0 Receive Buffer */
const sfrb RXBUF_0        = RXBUF_0_;
#define TXBUF_0_            (0x0077)  /* UART 0 Transmit Buffer */
sfrb    TXBUF_0           = TXBUF_0_;

/************************************************************
* USART 1  ͨ��ģ��1
************************************************************/

#define U1CTL_              (0x0078)  /* UART 1 Control */
sfrb    U1CTL             = U1CTL_;
#define U1TCTL_             (0x0079)  /* UART 1 Transmit Control */
sfrb    U1TCTL            = U1TCTL_;
#define U1RCTL_             (0x007A)  /* UART 1 Receive Control */
sfrb    U1RCTL            = U1RCTL_;
#define U1MCTL_             (0x007B)  /* UART 1 Modulation Control */
sfrb    U1MCTL            = U1MCTL_;
#define U1BR0_              (0x007C)  /* UART 1 Baud Rate 0 */
sfrb    U1BR0             = U1BR0_;
#define U1BR1_              (0x007D)  /* UART 1 Baud Rate 1 */
sfrb    U1BR1             = U1BR1_;
#define U1RXBUF_            (0x007E)  /* UART 1 Receive Buffer */
const sfrb U1RXBUF        = U1RXBUF_;
#define U1TXBUF_            (0x007F)  /* UART 1 Transmit Buffer */
sfrb    U1TXBUF           = U1TXBUF_;

#define UCTL1_              (0x0078)  /* UART 1 Control */
sfrb    UCTL1             = UCTL1_;
#define UTCTL1_             (0x0079)  /* UART 1 Transmit Control */
sfrb    UTCTL1            = UTCTL1_;
#define URCTL1_             (0x007A)  /* UART 1 Receive Control */
sfrb    URCTL1            = URCTL1_;
#define UMCTL1_             (0x007B)  /* UART 1 Modulation Control */
sfrb    UMCTL1            = UMCTL1_;
#define UBR01_              (0x007C)  /* UART 1 Baud Rate 0 */
sfrb    UBR01             = UBR01_;
#define UBR11_              (0x007D)  /* UART 1 Baud Rate 1 */
sfrb    UBR11             = UBR11_;
#define RXBUF1_             (0x007E)  /* UART 1 Receive Buffer */
const sfrb RXBUF1         = RXBUF1_;
#define TXBUF1_             (0x007F)  /* UART 1 Transmit Buffer */
sfrb    TXBUF1            = TXBUF1_;

#define UCTL_1_             (0x0078)  /* UART 1 Control */
sfrb    UCTL_1            = UCTL_1_;
#define UTCTL_1_            (0x0079)  /* UART 1 Transmit Control */
sfrb    UTCTL_1           = UTCTL_1_;
#define URCTL_1_            (0x007A)  /* UART 1 Receive Control */
sfrb    URCTL_1           = URCTL_1_;
#define UMCTL_1_            (0x007B)  /* UART 1 Modulation Control */
sfrb    UMCTL_1           = UMCTL_1_;
#define UBR0_1_             (0x007C)  /* UART 1 Baud Rate 0 */
sfrb    UBR0_1            = UBR0_1_;
#define UBR1_1_             (0x007D)  /* UART 1 Baud Rate 1 */
sfrb    UBR1_1            = UBR1_1_;
#define RXBUF_1_            (0x007E)  /* UART 1 Receive Buffer */
const sfrb RXBUF_1        = RXBUF_1_;
#define TXBUF_1_            (0x007F)  /* UART 1 Transmit Buffer */
sfrb    TXBUF_1           = TXBUF_1_;

/************************************************************
* Timer A   ��ʱ��A
************************************************************/

#define TAIV_               (0x012E)  /* Timer A Interrupt Vector Word�ж������Ĵ��� */
const sfrw TAIV           = TAIV_;
#define TACTL_              (0x0160)  /* Timer A Control���ƼĴ��� */
sfrw    TACTL             = TACTL_;
#define TACCTL0_            (0x0162)  /* Timer A Capture/Compare Control 0 ����/�ȽϿ��ƼĴ���0*/
sfrw    TACCTL0           = TACCTL0_;
#define TACCTL1_            (0x0164)  /* Timer A Capture/Compare Control 1 ����/�ȽϿ��ƼĴ���1*/
sfrw    TACCTL1           = TACCTL1_;
#define TACCTL2_            (0x0166)  /* Timer A Capture/Compare Control 2 ����/�ȽϿ��ƼĴ���2*/
sfrw    TACCTL2           = TACCTL2_;
#define TAR_                (0x0170)  /* Timer A �Ĵ���*/
sfrw    TAR               = TAR_;
#define TACCR0_             (0x0172)  /* Timer A Capture/Compare 0����/�ȽϼĴ���0 */
sfrw    TACCR0            = TACCR0_;
#define TACCR1_             (0x0174)  /* Timer A Capture/Compare 1����/�ȽϼĴ���1 */
sfrw    TACCR1            = TACCR1_;
#define TACCR2_             (0x0176)  /* Timer A Capture/Compare 2����/�ȽϼĴ���2 */
sfrw    TACCR2            = TACCR2_;

/* Alternate register names ���滻�ļĴ���������*/
#define CCTL0_              (0x0162)  /* Timer A Capture/Compare Control 0 */
sfrw    CCTL0             = CCTL0_;
#define CCTL1_              (0x0164)  /* Timer A Capture/Compare Control 1 */
sfrw    CCTL1             = CCTL1_;
#define CCTL2_              (0x0166)  /* Timer A Capture/Compare Control 2 */
sfrw    CCTL2             = CCTL2_;
#define CCR0_               (0x0172)  /* Timer A Capture/Compare 0 */
sfrw    CCR0              = CCR0_;
#define CCR1_               (0x0174)  /* Timer A Capture/Compare 1 */
sfrw    CCR1              = CCR1_;
#define CCR2_               (0x0176)  /* Timer A Capture/Compare 2 */
sfrw    CCR2              = CCR2_;

#define TASSEL2             (0x0400)  /* unused */        /* to distinguish from UART SSELx */
#define TASSEL1             (0x0200)  /* Timer A clock source select 0 */
#define TASSEL0             (0x0100)  /* Timer A clock source select 1 ���������Ƶ��������ʱ��Դѡ��*/
#define ID1                 (0x0080)  /* Timer A clock input devider 1 */
#define ID0                 (0x0040)  /* Timer A clock input devider 0 �����Ƶ����λ*/
#define MC1                 (0x0020)  /* Timer A mode control 1 */
#define MC0                 (0x0010)  /* Timer A mode control 0ģʽ����λ */
#define TACLR               (0x0004)  /* Timer A counter clear */
#define TAIE                (0x0002)  /* Timer A counter interrupt enable ��ʱ������ж�����λ*/
#define TAIFG               (0x0001)  /* Timer A counter interrupt flag ָ����ʱ�����ʱ��*/

#define MC_0                (0*0x10)  /* Timer A mode control: 0 - Stop */
#define MC_1                (1*0x10)  /* Timer A mode control: 1 - Up to CCR0 */
#define MC_2                (2*0x10)  /* Timer A mode control: 2 - Continous up */
#define MC_3                (3*0x10)  /* Timer A mode control: 3 - Up/Down */
#define ID_0                (0*0x40)  /* Timer A input divider: 0 - /1 */
#define ID_1                (1*0x40)  /* Timer A input divider: 1 - /2 */
#define ID_2                (2*0x40)  /* Timer A input divider: 2 - /4 */
#define ID_3                (3*0x40)  /* Timer A input divider: 3 - /8 */
#define TASSEL_0            (0*0x100) /* Timer A clock source select: 0 - TACLK */
#define TASSEL_1            (1*0x100) /* Timer A clock source select: 1 - ACLK  */
#define TASSEL_2            (2*0x100) /* Timer A clock source select: 2 - SMCLK */
#define TASSEL_3            (3*0x100) /* Timer A clock source select: 3 - INCLK */

#define CM1                 (0x8000)  /* Capture mode 1 */
#define CM0                 (0x4000)  /* Capture mode 0 */
#define CCIS1               (0x2000)  /* Capture input select 1 */
#define CCIS0               (0x1000)  /* Capture input select 0 */
#define SCS                 (0x0800)  /* Capture sychronize */
#define SCCI                (0x0400)  /* Latched capture signal (read) */
#define CAP                 (0x0100)  /* Capture mode: 1 /Compare mode : 0 */
#define OUTMOD2             (0x0080)  /* Output mode 2 */
#define OUTMOD1             (0x0040)  /* Output mode 1 */
#define OUTMOD0             (0x0020)  /* Output mode 0 */
#define CCIE                (0x0010)  /* Capture/compare interrupt enable */
#define CCI                 (0x0008)  /* Capture input signal (read) */
#define OUT                 (0x0004)  /* PWM Output signal if output mode 0 */
#define COV                 (0x0002)  /* Capture/compare overflow flag */
#define CCIFG               (0x0001)  /* Capture/compare interrupt flag */

#define OUTMOD_0            (0*0x20)  /* PWM output mode: 0 - output only */
#define OUTMOD_1            (1*0x20)  /* PWM output mode: 1 - set */
#define OUTMOD_2            (2*0x20)  /* PWM output mode: 2 - PWM toggle/reset */
#define OUTMOD_3            (3*0x20)  /* PWM output mode: 3 - PWM set/reset */
#define OUTMOD_4            (4*0x20)  /* PWM output mode: 4 - toggle */
#define OUTMOD_5            (5*0x20)  /* PWM output mode: 5 - Reset */
#define OUTMOD_6            (6*0x20)  /* PWM output mode: 6 - PWM toggle/set */
#define OUTMOD_7            (7*0x20)  /* PWM output mode: 7 - PWM reset/set */
#define CCIS_0              (0*0x1000) /* Capture input select: 0 - CCIxA */
#define CCIS_1              (1*0x1000) /* Capture input select: 1 - CCIxB */
#define CCIS_2              (2*0x1000) /* Capture input select: 2 - GND */
#define CCIS_3              (3*0x1000) /* Capture input select: 3 - Vcc */
#define CM_0                (0*0x4000) /* Capture mode: 0 - disabled */
#define CM_1                (1*0x4000) /* Capture mode: 1 - pos. edge */
#define CM_2                (2*0x4000) /* Capture mode: 1 - neg. edge */
#define CM_3                (3*0x4000) /* Capture mode: 1 - both edges */

/************************************************************
* Timer B  ��ʱ��B
************************************************************/

#define TBIV_               (0x011E)  /* Timer B Interrupt Vector Word �ж������Ĵ���*/
const sfrw TBIV           = TBIV_;
#define TBCTL_              (0x0180)  /* Timer B Control ���ƼĴ���*/
sfrw    TBCTL             = TBCTL_;
#define TBCCTL0_            (0x0182)  /* Timer B Capture/Compare Control 0����/�ȽϿ��ƼĴ���0 */
sfrw    TBCCTL0           = TBCCTL0_;
#define TBCCTL1_            (0x0184)  /* Timer B Capture/Compare Control 1����/�ȽϿ��ƼĴ���1 */
sfrw    TBCCTL1           = TBCCTL1_;
#define TBCCTL2_            (0x0186)  /* Timer B Capture/Compare Control 2����/�ȽϿ��ƼĴ���2 */
sfrw    TBCCTL2           = TBCCTL2_;
#define TBCCTL3_            (0x0188)  /* Timer B Capture/Compare Control 3����/�ȽϿ��ƼĴ���3 */
sfrw    TBCCTL3           = TBCCTL3_;
#define TBCCTL4_            (0x018A)  /* Timer B Capture/Compare Control 4����/�ȽϿ��ƼĴ���4 */
sfrw    TBCCTL4           = TBCCTL4_;
#define TBCCTL5_            (0x018C)  /* Timer B Capture/Compare Control 5����/�ȽϿ��ƼĴ���5 */
sfrw    TBCCTL5           = TBCCTL5_;
#define TBCCTL6_            (0x018E)  /* Timer B Capture/Compare Control 6����/�ȽϿ��ƼĴ���6 */
sfrw    TBCCTL6           = TBCCTL6_;
#define TBR_                (0x0190)  /* Timer B �Ĵ���*/
sfrw    TBR               = TBR_;
#define TBCCR0_             (0x0192)  /* Timer B Capture/Compare 0����/�ȽϼĴ���0 */
sfrw    TBCCR0            = TBCCR0_;
#define TBCCR1_             (0x0194)  /* Timer B Capture/Compare 1����/�ȽϼĴ���1 */
sfrw    TBCCR1            = TBCCR1_;
#define TBCCR2_             (0x0196)  /* Timer B Capture/Compare 2����/�ȽϼĴ���2 */
sfrw    TBCCR2            = TBCCR2_;
#define TBCCR3_             (0x0198)  /* Timer B Capture/Compare 3����/�ȽϼĴ���3 */
sfrw    TBCCR3            = TBCCR3_;
#define TBCCR4_             (0x019A)  /* Timer B Capture/Compare 4����/�ȽϼĴ���4 */
sfrw    TBCCR4            = TBCCR4_;
#define TBCCR5_             (0x019C)  /* Timer B Capture/Compare 5����/�ȽϼĴ���5 */
sfrw    TBCCR5            = TBCCR5_;
#define TBCCR6_             (0x019E)  /* Timer B Capture/Compare 6����/�ȽϼĴ���6 */
sfrw    TBCCR6            = TBCCR6_;

#define SHR1                (0x4000)  /* Timer B Compare latch load group 1 */
#define SHR0                (0x2000)  /* Timer B Compare latch load group 0 */
#define TBCLGRP1            (0x4000)  /* Timer B Compare latch load group 1 */
#define TBCLGRP0            (0x2000)  /* Timer B Compare latch load group 0 ѡ�񵥶��������رȽ�������*/
#define CNTL1               (0x1000)  /* Counter lenght 1 */
#define CNTL0               (0x0800)  /* Counter lenght 0 ����16λ��ʱ����TBR��Ϊ1��10��12��16λ����*/
#define TBSSEL2             (0x0400)  /* unused */
#define TBSSEL1             (0x0200)  /* Clock source 1 */
#define TBSSEL0             (0x0100)  /* Clock source 0 ��ʱ�����������Ƶ������ʱ��ѡ��*/
#define TBCLR               (0x0004)  /* Timer B counter clear ��ʱ�����λ*/
#define TBIE                (0x0002)  /* Timer B interrupt enable ��ʱ������ж�����λ����λʹ��������ж����󣬸�λ��ֹ*/
#define TBIFG               (0x0001)  /* Timer B interrupt flag ָʾ��ʱ�����ʵ��*/

#define TBSSEL_0            (0*0x0100)  /* Clock Source: TBCLK */
#define TBSSEL_1            (1*0x0100)  /* Clock Source: ACLK  */
#define TBSSEL_2            (2*0x0100)  /* Clock Source: SMCLK */
#define TBSSEL_3            (3*0x0100)  /* Clock Source: INCLK */
#define CNTL_0              (0*0x0800)  /* Counter lenght: 16 bit */
#define CNTL_1              (1*0x0800)  /* Counter lenght: 12 bit */
#define CNTL_2              (2*0x0800)  /* Counter lenght: 10 bit */
#define CNTL_3              (3*0x0800)  /* Counter lenght:  8 bit */
#define SHR_0               (0*0x2000)  /* Timer B Group: 0 - individually */
#define SHR_1               (1*0x2000)  /* Timer B Group: 1 - 3 groups (1-2, 3-4, 5-6) */
#define SHR_2               (2*0x2000)  /* Timer B Group: 2 - 2 groups (1-3, 4-6)*/
#define SHR_3               (3*0x2000)  /* Timer B Group: 3 - 1 group (all) */
#define TBCLGRP_0           (0*0x2000)  /* Timer B Group: 0 - individually */
#define TBCLGRP_1           (1*0x2000)  /* Timer B Group: 1 - 3 groups (1-2, 3-4, 5-6) */
#define TBCLGRP_2           (2*0x2000)  /* Timer B Group: 2 - 2 groups (1-3, 4-6)*/
#define TBCLGRP_3           (3*0x2000)  /* Timer B Group: 3 - 1 group (all) */

/* Additional Timer B Control Register bits are defined in Timer A */

#define SLSHR1              (0x0400)  /* Compare latch load source 1 */
#define SLSHR0              (0x0200)  /* Compare latch load source 0 */
#define CLLD1               (0x0400)  /* Compare latch load source 1 */
#define CLLD0               (0x0200)  /* Compare latch load source 0 */

#define SLSHR_0             (0*0x0200)  /* Compare latch load sourec : 0 - immediate */
#define SLSHR_1             (1*0x0200)  /* Compare latch load sourec : 1 - TBR counts to 0 */
#define SLSHR_2             (2*0x0200)  /* Compare latch load sourec : 2 - up/down */
#define SLSHR_3             (3*0x0200)  /* Compare latch load sourec : 3 - TBR counts to TBCTL0 */

#define CLLD_0              (0*0x0200)  /* Compare latch load sourec : 0 - immediate */
#define CLLD_1              (1*0x0200)  /* Compare latch load sourec : 1 - TBR counts to 0 */
#define CLLD_2              (2*0x0200)  /* Compare latch load sourec : 2 - up/down */
#define CLLD_3              (3*0x0200)  /* Compare latch load sourec : 3 - TBR counts to TBCTL0 */

/************************************************************
* Basic Clock Module   ����ʱ��ģ��
************************************************************/

#define DCOCTL_             (0x0056)  /* DCO Clock Frequency Control DCOʱ�ӿ��� */
sfrb    DCOCTL            = DCOCTL_;
#define BCSCTL1_            (0x0057)  /* Basic Clock System Control 1 ����ʱ�ӿ���1*/
sfrb    BCSCTL1           = BCSCTL1_;
#define BCSCTL2_            (0x0058)  /* Basic Clock System Control 2 ����ʱ�ӿ���2*/
sfrb    BCSCTL2           = BCSCTL2_;

#define MOD0                (0x01)   /* Modulation Bit 0 */
#define MOD1                (0x02)   /* Modulation Bit 1 */
#define MOD2                (0x04)   /* Modulation Bit 2 */
#define MOD3                (0x08)   /* Modulation Bit 3 */
#define MOD4                (0x10)   /* Modulation Bit 4 *//*MOD����������32��DCO�����в����F(dco+1)���ڣ������µ�DCO������ΪF(dco)���ڡ�*/
#define DCO0                (0x20)   /* DCO Select Bit 0 */
#define DCO1                (0x40)   /* DCO Select Bit 1 */
#define DCO2                (0x80)   /* DCO Select Bit 2 *//*DCO����������ѡ��8��Ƶ��֮һ��Ƶ����ע��DC�������ĵ�������*/

#define RSEL0               (0x01)   /* Resistor Select Bit 0 */
#define RSEL1               (0x02)   /* Resistor Select Bit 1 */
#define RSEL2               (0x04)   /* Resistor Select Bit 2 *//*���ڲ�����ѡ��8��Ƶ��֮һ*/
#define XT5V                (0x08)   /* XT5V should always be reset ���븴λ*/
#define DIVA0               (0x10)   /* ACLK Divider 0 */
#define DIVA1               (0x20)   /* ACLK Divider 1 ��ѡ��ACLK�ķ�Ƶ��*/
#define XTS                 (0x40)   /* LFXTCLK 0:Low Freq. / 1: High Freq.ѡ��LFXT1�����ڵ�Ƶ����ģʽ���Ƶ����ģʽ����������ӵľ������ */
#define XT2OFF              (0x80)   /* Enable XT2CLK ����XT2�����Ŀ����ɹر�*/

#define DIVA_0              (0x00)   /* ACLK Divider 0: /1 */
#define DIVA_1              (0x10)   /* ACLK Divider 1: /2 */
#define DIVA_2              (0x20)   /* ACLK Divider 2: /4 */
#define DIVA_3              (0x30)   /* ACLK Divider 3: /8 */

#define DCOR                (0x01)   /* Enable External Resistor : 1 */
#define DIVS0               (0x02)   /* SMCLK Divider 0 */
#define DIVS1               (0x04)   /* SMCLK Divider 1 */
#define SELS                (0x08)   /* SMCLK Source Select 0:DCOCLK / 1:XT2CLK/LFXTCLK */
#define DIVM0               (0x10)   /* MCLK Divider 0 */
#define DIVM1               (0x20)   /* MCLK Divider 1 */
#define SELM0               (0x40)   /* MCLK Source Select 0 */
#define SELM1               (0x80)   /* MCLK Source Select 1 */

#define DIVS_0              (0x00)   /* SMCLK Divider 0: /1 */
#define DIVS_1              (0x02)   /* SMCLK Divider 1: /2 */
#define DIVS_2              (0x04)   /* SMCLK Divider 2: /4 */
#define DIVS_3              (0x06)   /* SMCLK Divider 3: /8 */

#define DIVM_0              (0x00)   /* MCLK Divider 0: /1 */
#define DIVM_1              (0x10)   /* MCLK Divider 1: /2 */
#define DIVM_2              (0x20)   /* MCLK Divider 2: /4 */
#define DIVM_3              (0x30)   /* MCLK Divider 3: /8 */

#define SELM_0              (0x00)   /* MCLK Source Select 0: DCOCLK */
#define SELM_1              (0x40)   /* MCLK Source Select 1: DCOCLK */
#define SELM_2              (0x80)   /* MCLK Source Select 2: XT2CLK/LFXTCLK */
#define SELM_3              (0xC0)   /* MCLK Source Select 3: LFXTCLK */

/*************************************************************
* Flash Memory   FLASHоƬ
*************************************************************/

#define FCTL1_              (0x0128)  /* FLASH Control 1 *///Flash�洢�����ƼĴ���FCTL1
sfrw    FCTL1             = FCTL1_;
#define FCTL2_              (0x012A)  /* FLASH Control 2 *///Flash�洢�����ƼĴ���FCTL2
sfrw    FCTL2             = FCTL2_;
#define FCTL3_              (0x012C)  /* FLASH Control 3 *///Flash�洢�����ƼĴ���FCTL3
sfrw    FCTL3             = FCTL3_;
//FCTL1�Ĵ���
#define FRKEY               (0x9600)  /* Flash key returned by read */
#define FWKEY               (0xA500)  /* Flash key for write */
#define FXKEY               (0x3300)  /* for use with XOR instruction */

#define ERASE               (0x0002)  /* Enable bit for Flash segment erase *///0����������1���������1�Σ��κ��ɿ�д��ָ��ĵ�ַȷ�����������ʱ��ERASE�Զ���λ
#define MERAS               (0x0004)  /* Enable bit for Flash mass erase *///0����������1������������жΣ��������д��ָ���������������ʱMERAS�Զ���λ��
#define WRT                 (0x0040)  /* Enable bit for Flash write *///��̵�ʱ����뽫����λ
#define BLKWRT              (0x0080)  /* Enable bit for Flash segment write *///�α�̣�0��δѡ��α�̷�ʽ��1�����öα�̣������εĲ���֮�䣬��Ҫ��һ�θ�λ����λ
#define SEGWRT              (0x0080)  /* old definition */ /* Enable bit for Flash segment write *///�α�̣�0��δѡ��α�̷�ʽ��1�����öα�̣������εĲ���֮�䣬��Ҫ��һ�θ�λ����λ
//FCTL2�Ĵ���
#define FN0                 (0x0001)  /* Devide Flash clock by: 2^0 *///
#define FN1                 (0x0002)  /* Devide Flash clock by: 2^1 */
#define FN2                 (0x0004)  /* Devide Flash clock by: 2^2 */
#define FN3                 (0x0008)  /* Devide Flash clock by: 2^3 */
#define FN4                 (0x0010)  /* Devide Flash clock by: 2^4 */
#define FN5                 (0x0020)  /* Devide Flash clock by: 2^5 */   //FN0~~~FN5����ʱ��Դ�źŵķ�Ƶ���ӣ�Ϊ1~64��FNֵ��1
#define FSSEL0              (0x0040)  /* Flash clock select 0 */        /* to distinguish from UART SSELx */
#define FSSEL1              (0x0080)  /* Flash clock select 1 */         //ѡ��ʱ��Դ  0��ACLK��1��MCLK��2��3��SMCLK

#define FSSEL_0             (0x0000)  /* Flash clock select: 0 - ACLK */
#define FSSEL_1             (0x0040)  /* Flash clock select: 1 - MCLK */
#define FSSEL_2             (0x0080)  /* Flash clock select: 2 - SMCLK */
#define FSSEL_3             (0x00C0)  /* Flash clock select: 3 - SMCLK */
//FCTL3�Ĵ���
#define BUSY                (0x0001)  /* Flash busy: 1 *///0��FLASH�洢����æ����POR��ʱ��������λ��1��FLASH�洢��æ
#define KEYV                (0x0002)  /* Flash Key violation flag *///0����ȫ��ֵ��0A5H��1���ԼĴ���1��2��3д��ʱ��ȫ��ֵ����0A5H��ʹ��KEYV��λ������PUC��KEYV�����Զ���λ���������λ
#define ACCVIFG             (0x0004)  /* Flash Access violation flag *///�Ƿ������жϱ�־
#define WAIT                (0x0008)  /* Wait flag for segment write *///0���α���Ѿ���ʼ������ڽ����У�1���α����Ч����ǰ���ݱ����ɣ��ȴ������ı������
#define LOCK                (0x0010)  /* Lock bit: 1 - Flash is locked (read only) *///0��FLASH������������̣���������������Ȳ���
#define EMEX                (0x0020)  /* Flash Emergency Exit *///����FLASH�������߱��ʧ�ص������0�������ã�1������ֹͣ��FLASH�Ĳ���

/************************************************************
* Comparator A   �Ƚ���A
************************************************************/

#define CACTL1_             (0x0059)  /* Comparator A Control 1 */
sfrb    CACTL1            = CACTL1_;
#define CACTL2_             (0x005A)  /* Comparator A Control 2 */
sfrb    CACTL2            = CACTL2_;
#define CAPD_               (0x005B)  /* Comparator A Port Disable */
sfrb    CAPD              = CAPD_;

#define CAIFG               (0x01)    /* Comp. A Interrupt Flag */
#define CAIE                (0x02)    /* Comp. A Interrupt Enable */
#define CAIES               (0x04)    /* Comp. A Int. Edge Select: 0:rising / 1:falling */
#define CAON                (0x08)    /* Comp. A enable */
#define CAREF0              (0x10)    /* Comp. A Internal Reference Select 0 */
#define CAREF1              (0x20)    /* Comp. A Internal Reference Select 1 */
#define CARSEL              (0x40)    /* Comp. A Internal Reference Enable */
#define CAEX                (0x80)    /* Comp. A Exchange Inputs */

#define CAREF_0             (0x00)    /* Comp. A Int. Ref. Select 0 : Off */
#define CAREF_1             (0x10)    /* Comp. A Int. Ref. Select 1 : 0.25*Vcc */
#define CAREF_2             (0x20)    /* Comp. A Int. Ref. Select 2 : 0.5*Vcc */
#define CAREF_3             (0x30)    /* Comp. A Int. Ref. Select 3 : Vt*/

#define CAOUT               (0x01)    /* Comp. A Output */
#define CAF                 (0x02)    /* Comp. A Enable Output Filter */
#define P2CA0               (0x04)    /* Comp. A Connect External Signal to CA0 : 1 */
#define P2CA1               (0x08)    /* Comp. A Connect External Signal to CA1 : 1 */
#define CACTL24             (0x10)
#define CACTL25             (0x20)
#define CACTL26             (0x40)
#define CACTL27             (0x80)

#define CAPD0               (0x01)    /* Comp. A Disable Input Buffer of Port Register .0 */
#define CAPD1               (0x02)    /* Comp. A Disable Input Buffer of Port Register .1 */
#define CAPD2               (0x04)    /* Comp. A Disable Input Buffer of Port Register .2 */
#define CAPD3               (0x08)    /* Comp. A Disable Input Buffer of Port Register .3 */
#define CAPD4               (0x10)    /* Comp. A Disable Input Buffer of Port Register .4 */
#define CAPD5               (0x20)    /* Comp. A Disable Input Buffer of Port Register .5 */
#define CAPD6               (0x40)    /* Comp. A Disable Input Buffer of Port Register .6 */
#define CAPD7               (0x80)    /* Comp. A Disable Input Buffer of Port Register .7 */

/************************************************************
* ADC12  ģ��ת����ADC12
************************************************************/

#define ADC12CTL0_          (0x01A0)  /* ADC12 Control 0 */
sfrw    ADC12CTL0         = ADC12CTL0_;
#define ADC12CTL1_          (0x01A2)  /* ADC12 Control 1 */
sfrw    ADC12CTL1         = ADC12CTL1_;
#define ADC12IFG_           (0x01A4)  /* ADC12 Interrupt Flag */
sfrw    ADC12IFG          = ADC12IFG_;
#define ADC12IE_            (0x01A6)  /* ADC12 Interrupt Enable */
sfrw    ADC12IE           = ADC12IE_;
#define ADC12IV_            (0x01A8)  /* ADC12 Interrupt Vector Word */
sfrw    ADC12IV           = ADC12IV_;

#define ADC12MEM_           (0x0140)  /* ADC12 Conversion Memory */
#ifndef __IAR_SYSTEMS_ICC
#define ADC12MEM            (ADC12MEM_) /* ADC12 Conversion Memory (for assembler) */
#else
#define ADC12MEM            ((int*) ADC12MEM_) /* ADC12 Conversion Memory (for C) */
#endif
#define ADC12MEM0_          (ADC12MEM_) /* ADC12 Conversion Memory 0 */
sfrw    ADC12MEM0         = ADC12MEM0_;
#define ADC12MEM1_          (0x0142)  /* ADC12 Conversion Memory 1 */
sfrw    ADC12MEM1         = ADC12MEM1_;
#define ADC12MEM2_          (0x0144)  /* ADC12 Conversion Memory 2 */
sfrw    ADC12MEM2         = ADC12MEM2_;
#define ADC12MEM3_          (0x0146)  /* ADC12 Conversion Memory 3 */
sfrw    ADC12MEM3         = ADC12MEM3_;
#define ADC12MEM4_          (0x0148)  /* ADC12 Conversion Memory 4 */
sfrw    ADC12MEM4         = ADC12MEM4_;
#define ADC12MEM5_          (0x014A)  /* ADC12 Conversion Memory 5 */
sfrw    ADC12MEM5         = ADC12MEM5_;
#define ADC12MEM6_          (0x014C)  /* ADC12 Conversion Memory 6 */
sfrw    ADC12MEM6         = ADC12MEM6_;
#define ADC12MEM7_          (0x014E)  /* ADC12 Conversion Memory 7 */
sfrw    ADC12MEM7         = ADC12MEM7_;

#define ADC12MEM8_          (0x0150)  /* ADC12 Conversion Memory 8 */
sfrw    ADC12MEM8         = ADC12MEM8_;
#define ADC12MEM9_          (0x0152)  /* ADC12 Conversion Memory 9 */
sfrw    ADC12MEM9         = ADC12MEM9_;
#define ADC12MEM10_         (0x0154)  /* ADC12 Conversion Memory 10 */
sfrw    ADC12MEM10        = ADC12MEM10_;
#define ADC12MEM11_         (0x0156)  /* ADC12 Conversion Memory 11 */
sfrw    ADC12MEM11        = ADC12MEM11_;
#define ADC12MEM12_         (0x0158)  /* ADC12 Conversion Memory 12 */
sfrw    ADC12MEM12        = ADC12MEM12_;
#define ADC12MEM13_         (0x015A)  /* ADC12 Conversion Memory 13 */
sfrw    ADC12MEM13        = ADC12MEM13_;
#define ADC12MEM14_         (0x015C)  /* ADC12 Conversion Memory 14 */
sfrw    ADC12MEM14        = ADC12MEM14_;
#define ADC12MEM15_         (0x015E)  /* ADC12 Conversion Memory 15 */
sfrw    ADC12MEM15        = ADC12MEM15_;

#define ADC12MCTL_          (0x0080)  /* ADC12 Memory Control */
#ifndef __IAR_SYSTEMS_ICC
#define ADC12MCTL           (ADC12MCTL_) /* ADC12 Memory Control (for assembler) */
#else
#define ADC12MCTL           ((char*) ADC12MCTL_) /* ADC12 Memory Control (for C) */
#endif
#define ADC12MCTL0_         (ADC12MCTL_) /* ADC12 Memory Control 0 */
sfrb    ADC12MCTL0        = ADC12MCTL0_;
#define ADC12MCTL1_         (0x0081)  /* ADC12 Memory Control 1 */
sfrb    ADC12MCTL1        = ADC12MCTL1_;
#define ADC12MCTL2_         (0x0082)  /* ADC12 Memory Control 2 */
sfrb    ADC12MCTL2        = ADC12MCTL2_;
#define ADC12MCTL3_         (0x0083)  /* ADC12 Memory Control 3 */
sfrb    ADC12MCTL3        = ADC12MCTL3_;
#define ADC12MCTL4_         (0x0084)  /* ADC12 Memory Control 4 */
sfrb    ADC12MCTL4        = ADC12MCTL4_;
#define ADC12MCTL5_         (0x0085)  /* ADC12 Memory Control 5 */
sfrb    ADC12MCTL5        = ADC12MCTL5_;
#define ADC12MCTL6_         (0x0086)  /* ADC12 Memory Control 6 */
sfrb    ADC12MCTL6        = ADC12MCTL6_;
#define ADC12MCTL7_         (0x0087)  /* ADC12 Memory Control 7 */
sfrb    ADC12MCTL7        = ADC12MCTL7_;
#define ADC12MCTL8_         (0x0088)  /* ADC12 Memory Control 8 */
sfrb    ADC12MCTL8        = ADC12MCTL8_;
#define ADC12MCTL9_         (0x0089)  /* ADC12 Memory Control 9 */
sfrb    ADC12MCTL9        = ADC12MCTL9_;
#define ADC12MCTL10_        (0x008A)  /* ADC12 Memory Control 10 */
sfrb    ADC12MCTL10       = ADC12MCTL10_;
#define ADC12MCTL11_        (0x008B)  /* ADC12 Memory Control 11 */
sfrb    ADC12MCTL11       = ADC12MCTL11_;
#define ADC12MCTL12_        (0x008C)  /* ADC12 Memory Control 12 */
sfrb    ADC12MCTL12       = ADC12MCTL12_;
#define ADC12MCTL13_        (0x008D)  /* ADC12 Memory Control 13 */
sfrb    ADC12MCTL13       = ADC12MCTL13_;
#define ADC12MCTL14_        (0x008E)  /* ADC12 Memory Control 14 */
sfrb    ADC12MCTL14       = ADC12MCTL14_;
#define ADC12MCTL15_        (0x008F)  /* ADC12 Memory Control 15 */
sfrb    ADC12MCTL15       = ADC12MCTL15_;

#define ADC12SC             (0x001) /* ADC12CTL0 */
#define ENC                 (0x002)
#define ADC12TOVIE          (0x004)
#define ADC12OVIE           (0x008)
#define ADC12ON             (0x010)
#define REFON               (0x020)
#define REF2_5V             (0x040)
#define MSH                 (0x080)
#define MSC                 (0x080)

#define SHT0_0               (0*0x100)
#define SHT0_1               (1*0x100)
#define SHT0_2               (2*0x100)
#define SHT0_3               (3*0x100)
#define SHT0_4               (4*0x100)
#define SHT0_5               (5*0x100)
#define SHT0_6               (6*0x100)
#define SHT0_7               (7*0x100)
#define SHT0_8               (8*0x100)
#define SHT0_9               (9*0x100)
#define SHT0_10             (10*0x100)
#define SHT0_11             (11*0x100)
#define SHT0_12             (12*0x100)
#define SHT0_13             (13*0x100)
#define SHT0_14             (14*0x100)
#define SHT0_15             (15*0x100)

#define SHT1_0               (0*0x1000)
#define SHT1_1               (1*0x1000)
#define SHT1_2               (2*0x1000)
#define SHT1_3               (3*0x1000)
#define SHT1_4               (4*0x1000)
#define SHT1_5               (5*0x1000)
#define SHT1_6               (6*0x1000)
#define SHT1_7               (7*0x1000)
#define SHT1_8               (8*0x1000)
#define SHT1_9               (9*0x1000)
#define SHT1_10             (10*0x1000)
#define SHT1_11             (11*0x1000)
#define SHT1_12             (12*0x1000)
#define SHT1_13             (13*0x1000)
#define SHT1_14             (14*0x1000)
#define SHT1_15             (15*0x1000)

#define ADC12BUSY           (0x0001) /* ADC12CTL1 */
#define CONSEQ_0             (0*2)
#define CONSEQ_1             (1*2)
#define CONSEQ_2             (2*2)
#define CONSEQ_3             (3*2)
#define ADC12SSEL_0          (0*8)
#define ADC12SSEL_1          (1*8)
#define ADC12SSEL_2          (2*8)
#define ADC12SSEL_3          (3*8)
#define ADC12DIV_0           (0*0x20)
#define ADC12DIV_1           (1*0x20)
#define ADC12DIV_2           (2*0x20)
#define ADC12DIV_3           (3*0x20)
#define ADC12DIV_4           (4*0x20)
#define ADC12DIV_5           (5*0x20)
#define ADC12DIV_6           (6*0x20)
#define ADC12DIV_7           (7*0x20)
#define ISSH                (0x0100)
#define SHP                 (0x0200)
#define SHS_0                (0*0x400)
#define SHS_1                (1*0x400)
#define SHS_2                (2*0x400)
#define SHS_3                (3*0x400)

#define CSTARTADD_0          (0*0x1000)
#define CSTARTADD_1          (1*0x1000)
#define CSTARTADD_2          (2*0x1000)
#define CSTARTADD_3          (3*0x1000)
#define CSTARTADD_4          (4*0x1000)
#define CSTARTADD_5          (5*0x1000)
#define CSTARTADD_6          (6*0x1000)
#define CSTARTADD_7          (7*0x1000)
#define CSTARTADD_8          (8*0x1000)
#define CSTARTADD_9          (9*0x1000)
#define CSTARTADD_10        (10*0x1000)
#define CSTARTADD_11        (11*0x1000)
#define CSTARTADD_12        (12*0x1000)
#define CSTARTADD_13        (13*0x1000)
#define CSTARTADD_14        (14*0x1000)
#define CSTARTADD_15        (15*0x1000)

#define INCH_0               (0) /* ADC12CTLx */
#define INCH_1               (1)
#define INCH_2               (2)
#define INCH_3               (3)
#define INCH_4               (4)
#define INCH_5               (5)
#define INCH_6               (6)
#define INCH_7               (7)
#define INCH_8               (8)
#define INCH_9               (9)
#define INCH_10             (10)
#define INCH_11             (11)
#define INCH_12             (12)
#define INCH_13             (13)
#define INCH_14             (14)
#define INCH_15             (15)

#define SREF_0               (0*0x10)
#define SREF_1               (1*0x10)
#define SREF_2               (2*0x10)
#define SREF_3               (3*0x10)
#define SREF_4               (4*0x10)
#define SREF_5               (5*0x10)
#define SREF_6               (6*0x10)
#define SREF_7               (7*0x10)
#define EOS                 (0x80)

/************************************************************
* Interrupt Vectors (offset from 0xFFE0)
  �ж�����
************************************************************/

#define PORT2_VECTOR        (1 * 2)  /* 0xFFE2 Port 2 */
#define UART1TX_VECTOR      (2 * 2)  /* 0xFFE4 UART 1 Transmit */
#define UART1RX_VECTOR      (3 * 2)  /* 0xFFE6 UART 1 Receive */
#define PORT1_VECTOR        (4 * 2)  /* 0xFFE8 Port 1 */
#define TIMERA1_VECTOR      (5 * 2)  /* 0xFFEA Timer A CC1-2, TA */
#define TIMERA0_VECTOR      (6 * 2)  /* 0xFFEC Timer A CC0 */
#define ADC_VECTOR          (7 * 2)  /* 0xFFEE ADC */
#define UART0TX_VECTOR      (8 * 2)  /* 0xFFF0 UART 0 Transmit */
#define UART0RX_VECTOR      (9 * 2)  /* 0xFFF2 UART 0 Receive */
#define WDT_VECTOR          (10 * 2) /* 0xFFF4 Watchdog Timer */
#define COMPARATORA_VECTOR  (11 * 2) /* 0xFFF6 Comparator A */
#define TIMERB1_VECTOR      (12 * 2) /* 0xFFF8 Timer B 1-7 */
#define TIMERB0_VECTOR      (13 * 2) /* 0xFFFA Timer B 0 */
#define NMI_VECTOR          (14 * 2) /* 0xFFFC Non-maskable */
#define RESET_VECTOR        (15 * 2) /* 0xFFFE Reset [Highest Priority] */

/************************************************************
* End of Modules  ģ�����
************************************************************/

#endif /* #ifndef __msp430x14x */

