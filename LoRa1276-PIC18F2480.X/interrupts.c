/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include "LoRa1276.h"
#include <string.h>

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/* Baseline devices don't have interrupts. Note that some PIC16's 
 * are baseline devices.  Unfortunately the baseline detection macro is 
 * _PIC12 */
#ifndef _PIC12

char line[80];      // input char from RX & answer TX
char *ptr = line;

//char line2[128];    // used for dump & TMR0 & SX1276_RxDone
//char *ptr2;

//char lineI[8];

int t1;

void dump(int adr)
{
    char i, j, h;
                for(i=0; i<4; i++) {
                    puts("ADR ");
                    if (adr<16) puts("0");
                    puthex(adr);
                    puts(": ");
                    for(j=0; j<16; j++) {
                        LORA_NSS=0;
                        SpiInOut(adr++);
                        h = SpiInOut(0x00);
                        if (h<16) puts("0");
                        puthex(h);
                        puts(" ");
                        LORA_NSS=1;
                    }
                    puts("\n\r");
                }
}

void interrupt isr(void)
{
    char x, h, u, v;
    int i, j;

    if (TXIE) if (TXIF) {
        TXREG = *ptrR++;
        if (ptrR-buffer>=SIZE) ptrR=buffer; // rollover
        if (ptrR == ptrW) TXIE = 0; // buffer empty
    }

    //puts("I");
    if (TMR1IE) if (TMR1IF) { // 71 ms
        x = SPIReadReg(LR_RegIrqFlags);
        if (LORA_DIO0 || (x & 0x40))
            SX1276_RxDone();
        TMR1IF = 0;
    }

    if (TMR0IE) if (TMR0IF) { // 1 sec
        LED ^= 1; // toggle debug LED
        puts("DIO: ");
        LED ^= 1; // toggle debug LED
        puts(LORA_DIO0 ? "1" : "0");
        puts(LORA_DIO1 ? "1" : "0");
        puts(LORA_DIO2 ? "1" : "0");
        puts(" T1: ");
        putdec(++t1);
        //utoa(line2, ++t1, 10); puts(line2);
        puts(" IRQ: ");
        x = SPIReadReg(LR_RegIrqFlags);
        puthex(x);
        //utoa(line2, x, 16); puts(line2);

        puts(" ptr: ");
        putdec(ptr);
        //puts(" RCSTA: ");
        //puthex(RCSTA);

        puts(" \r");

        // Send message
        SX1276_TX("TMR0", 4);
        SX1276_RX_INIT();

        if (OERR) {CREN=0; CREN=1;} // RXSTA, rrcv overwrite

        TMR0IF = 0;
    }

    if (RCIF) {
        x = RCREG;
        *ptr++ = x;
        *ptr = '\0';
        if (x == '\t') { // skip line
            ptr = line;
            puts("Escape\n\r");
        } else if (x == '\n' || x == '\r') { // End of line
            //puts("Line\n\r");
            if (strncmp(line, "RX", 2)==0) {
                LORA_RXEN = (line[2]=='1');
                puts("RX\n\r");
            }else if (strncmp(line, "TX", 2)==0) {
                LORA_TXEN = (line[2]=='1');
                puts("TX\n\r");
            //}else if (strncmp(line, "RESET", 5)==0) {
            }else if (line[0] == 'x') { // Reset
                SX1276_Reset();
                puts("Reset\n\r");
            }else if (line[0] == 'c') { // Config
                SX1276_Config();
                puts("Config\n\r");
            }else if (line[0] == 'g') { // Go Timers change
                TMR0IE = ~TMR0IE;
                TMR1IE = ~TMR1IE;
            }else if (line[0] == 't') { // Tx
                SX1276_TX("Hello, world !", 14);
                puts("Transmit\n\r");
                SX1276_RX_INIT();
                puts("Reception\n\r");
            }else if (line[0] == 'r') { // Rx
                SX1276_RX_INIT();
                puts("Reception\n\r");
            }else if (line[0] == 'd') { // Dump
                dump(0x00);
            }else if (line[0] == 'D') { // Dump
                dump(0x40);

            }else if (line[0] == '>') {
                ptr = line+1;
                puts("<");
                LORA_NSS=0;
                i=0;
                while ((x=*ptr++) >= '0') {
                    // decode hexa
                    u = x-'0';
                    if (u>9) u = x-'A'+10;
                    x = *ptr++;
                    v = x-'0';
                    if (v>9) v = x-'A'+10;
                    h = u*16 + v;
                    //puthex(h);
                    //puts(":");
                    h = SpiInOut(h); // Send Receive SPI
                    if (i==0) {i++; h = u*16 + v;} // restore adr...
                    if (h<16) puts("0");
                    puthex(h);
                    puts(" ");
                }
                LORA_NSS=1;
                puts("\n\r");
                /*
                *ptr++ = '\n';
                *ptr++ = '\r';
                *ptr++ = '\0';
                puts(line);
                */
            }
            /*
            // Output answer
            ptr=line;
            while ((x=*ptr++) != '\0') {
                putchar(x);
            }
            */
            
            // Reset state
            ptr = line;
        }
    }
}
#endif


