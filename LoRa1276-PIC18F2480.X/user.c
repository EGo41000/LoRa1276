/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
//#include <p18cxxx.h>
//#include <timers.h>
//#include <plib/i2c.h>
#include <stdio.h>
#include <stdlib.h>
#include "user.h"
#include "LoRa1276.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */
/*
void putchar (char c)
 {
   while (!TRMT);
   TXREG = c;
 }
*/

char buffer[SIZE];
char *ptrR=buffer;
char *ptrW=buffer;

int puts(const char *l)
{
    char c;
    while(c = *l++){
        //while (!TRMT);
        //TXREG = c;
        *ptrW++ = c;
        if (ptrW-buffer>=SIZE) ptrW=buffer; // rollover
    }
    TXIE = 1; // Enable transmit
}

char lined[16];
void putdec(int n)
{
    utoa(lined, n, 10);
    puts(lined);
}
void puthex(int n)
{
    utoa(lined, n, 16);
    puts(lined);
}

 // spi read and write one byte
unsigned char SpiInOut(unsigned char data)
{
        //BF = 0;
    SSPIF = 0;
        SSPBUF = data;
        while (!SSPIF); //while (BF==0);
        data = SSPBUF;
	return (data);
}
// SPI read register
unsigned char SPIReadReg(unsigned char addr)
{
	unsigned char data;

	LORA_NSS=0;
	SpiInOut(addr);			// write register address
	data = SpiInOut(0);		// read register value
	LORA_NSS=1;
	return(data);
}

// SPI write register
void SPIWriteReg(unsigned char addr, unsigned char value)
{
	addr |= 0x80;			// write register,MSB=1

	LORA_NSS=0;

	SpiInOut(addr);			// write register address
	SpiInOut(value);		// write register value

	LORA_NSS=1;
}
// spi burst write
void SPIBurstRead(unsigned char addr, unsigned char *ptr, unsigned char len)
{
	unsigned char i;
	if(len<=1)			// length>1,use burst mode
		return;
	else
	{
            LORA_NSS=0;
		SpiInOut(addr);
		for(i=0;i<len;i++)
			ptr[i] = SpiInOut(0);
            LORA_NSS=1;
	}
}
// spi burst read
void SPIBurstWrite(unsigned char addr, unsigned char *ptr, unsigned char len)
{
	unsigned char i;

	addr |= 0x80;

	if(len<=1)			// length>1,use burst mode
		return;
	else
	{
            LORA_NSS=0;
		SpiInOut(addr);
		for(i=0;i<len;i++)
			SpiInOut(ptr[i]);
            LORA_NSS=1;
	}
}


void InitApp(void)
{
    int8_t i, j;

    OSCCON = 0b01110011; // 8MHz internal osc

    LORA_NSStris = 0; // output
    LORA_RESETtris = 0;
    LORA_RXENtris = 0;
    LORA_TXENtris = 0;

    ADCON1 = 0b00001111; // Digital IO

    LORA_DIO0tris = 1; // RC1
    LORA_DIO1tris = 1;
    LORA_DIO2tris = 1;

    LEDtris = 0; // output LED
    LED = 0;

    /* Initialize peripherals */

    // Timer0 (every 1s)
    //T0CON = 0b10000111; // 1:256, Fosc/4, 8MHz/4/256: 7812Hz, FFFF->8.39s
    T0CON = 0b10000110; // 1:128, Fosc/4, 8MHz/4/128: 15kHz, FFFF->4.16s
    //T0CON = 0b10000100; // 1:32, Fosc/4, 8MHz/4/32: 62.5kHz, FFFF->1.04s
    //T0CON = 0b10000010; // 1:8, Fosc/4, 8MHz/4/8: 250kHz, FFFF->0.26s
    TMR0IE = 1;

    // Timer1
    //T1CON = 0b00110001; // 1:8 -> 8MHz/4/8: 250kHz, FFFF->262ms
    T1CON = 0b00000001; // 1:1 -> 3686400/65536/4/1: 71ms
    TMR1IE = 1;
/*
    CCP1CON = 0b1011; //Compare mode, trigger special event
    //CCPR1 = 57600; // 115200Hz/57600: 2Hz 500ms
    CCPR1 = 2304; // 115200Hz/2304: 50Hz
    //CCP1IE = 1;
*/

    // USART (high speed, 8MHz)
    SPBRG = 51; // 9600baud
    //TXSTA = 0b00100000; // Async, BRGH=0:low speed
    //RCSTA = 0b10010000; // SPEN
    SYNC = 0;
    BRGH = 1; // High speed
    BRG16 = 0;
    SPEN = 1; // Serial Port enable
    TXEN = 1; // TX enable
    CREN = 1; // Cont Receive enable
    RCIE = 1; // Rcv interrupt
    // TXIE = 1; => send data
    TRISCbits.RC6 = 0; // TBD

    // SPI (CKP=CPOL=0, CKE=/CPHA=1)
    //CKE = 1;
    //CKP = 0;
    //SSPCON1 = 0b00000010; // SPI master, Fosc/64, CKP=0
    //SSPEN = 1; // SPI enable

    SSPSTAT = 0b01000000; // CKE=1
    SSPCON1 = 0b00100010; // SSPEN, CKP=0, SPI master F/64
    TRISCbits.TRISC5 = 0; // output SDO
    TRISCbits.TRISC4 = 1; // input SDI
    TRISCbits.TRISC3 = 0; // output SCK

    /* Enable interrupts */
    PEIE = 1;
    //ei(); //
    GIE = 1;

    SX1276_Reset();
    SX1276_Config();
    SX1276_RX_INIT(); // Reception default

    puts("initApp\n\r");
}

