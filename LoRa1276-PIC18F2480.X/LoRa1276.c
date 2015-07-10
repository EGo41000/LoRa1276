#include <xc.h>         /* XC8 General Include File */
#include "user.h"
#include "LoRa1276.h"
#include <delays.h>
#include<stdlib.h>
//FlagType          Flag;


void SX1276_Reset(void)
{
	LORA_NSS=1;

	LORA_TXEN=0;		// close antenna switch
	LORA_RXEN=0;

	LORA_RESET=0;
	//delay_x10ms(1);		// delay 10ms
        Delay1KTCYx(20); // 20000 @ 8MHz/4 = 10ms
        Delay1KTCYx(20); // 20000 @ 8MHz/4 = 10ms
	LORA_RESET=1;
	//delay_x10ms(2);		// delay 20ms
        Delay1KTCYx(20); // 20000 @ 8MHz/4 = 10ms

}
void SX1276_Config(void)
{
	// In setting mode, RF module should turn to sleep mode
	SPIWriteReg(LR_RegOpMode,0x08);		// low frequency mode?sleep mode
	//nop_10();
        Delay1TCYx(10);

	SPIWriteReg(REG_LR_TCXO,0x09);		// external Crystal
	SPIWriteReg(LR_RegOpMode,0x88);		// lora mode

	SPIWriteReg(LR_RegFrMsb,0x6c);
	SPIWriteReg(LR_RegFrMid,0x80);
	SPIWriteReg(LR_RegFrLsb,0x00);		// frequency?434Mhz

	SPIWriteReg(LR_RegPaConfig,0xff);	// max output power

        // OCP increased
	SPIWriteReg(LR_RegOcp,0x0B);		// close ocp
	// Boost
        SPIWriteReg(LR_RegLna,0b00100011 /*0x23*/);		// enable LNA, G1 highest, Boost on 150%

        // range: haut escalier. estim 22kbps
        /*
	SPIWriteReg(LR_RegModemConfig1,0b10010010);	// signal bandwidth?500kHz,coding rate = 4/5,explicit Header mode
	SPIWriteReg(LR_RegModemConfig2,0b01110111);	// spreading factor?7
	SPIWriteReg(LR_RegModemConfig3,0b00001000);	// LNA
        */

        // 195bps, 6preamble -> 4byte payload: 927ms, 2bytes: 763, tot 1690ms(T0=31000)
        SPIWriteReg(LR_RegModemConfig1,0b00000010);	// signal bandwidth?7.8kHz,coding rate = 4/5,explicit Header mode
	SPIWriteReg(LR_RegModemConfig2,0b10000111);	// spreading factor?8
	SPIWriteReg(LR_RegModemConfig3,0b00001000);	// LNA

        /*
        // 60bps, 6preamble -> 4byte payload: 3000ms, 2bytes: 2400ms, tot 5.4s (T0=)
        SPIWriteReg(LR_RegModemConfig1,0b00000010);	// signal bandwidth?7.8kHz,coding rate = 4/5,explicit Header mode
	SPIWriteReg(LR_RegModemConfig2,0b10100111);	// spreading factor?10
	SPIWriteReg(LR_RegModemConfig3,0b00001000);	// LNA

 */


        // BW 7.8kHz (18bps)
        // SR 12 ?
        // PreambleLength: 6 a 65536

	SPIWriteReg(LR_RegSymbTimeoutLsb,0xFF); // max rx timeout

	SPIWriteReg(LR_RegPreambleMsb,0x00);
	//SPIWriteReg(LR_RegPreambleLsb,16);      // preamble 16 bytes
	SPIWriteReg(LR_RegPreambleLsb,6);      // preamble 16 bytes

	SPIWriteReg(REG_LR_PADAC,0x87);         // tx power 20dBm
	SPIWriteReg(LR_RegHopPeriod,0x00);      // no hopping

	SPIWriteReg(REG_LR_DIOMAPPING2,0x01);   // DIO5=ModeReady,DIO4=CadDetected
	SPIWriteReg(LR_RegOpMode,0x09);         // standby mode
}

void SX1276_TX (const char *line, unsigned char len)
{
	unsigned char addr,temp;

	LORA_RXEN=0;
	LORA_TXEN=1; // open tx antenna switch

	//SPIWriteReg(REG_LR_DIOMAPPING1,0x41); 			// DIO0=TxDone,DIO1=RxTimeout,DIO3=ValidHeader
	SPIWriteReg(REG_LR_DIOMAPPING1,0b01000001); 			// DIO0=TxDone,DIO1=RxTimeout,DIO3=ValidHeader

	SPIWriteReg(LR_RegIrqFlags,0xff);			// clear interrupt
	SPIWriteReg(LR_RegIrqFlagsMask,0xf7);			// enable txdone
	SPIWriteReg(LR_RegPayloadLength, len);                  // packet length

	addr = SPIReadReg(LR_RegFifoTxBaseAddr);		// read TxBaseAddr
	SPIWriteReg(LR_RegFifoAddrPtr,addr);			// TxBaseAddr->FifoAddrPtr

	SPIBurstWrite(0x00, line, len);		// fill data into fifo
	SPIWriteReg(LR_RegOpMode,0x0b);					// enter tx mode

	//Flag.is_tx = 1;						// tx flag
	//LED_TX = LED_TX^1;

	//rf_timeout = 0;
	//Flag.rf_reach_timeout = 0;				// clear tx timeout flag

	temp=SPIReadReg(LR_RegIrqFlags);			// read interrupt
	while(!(temp&0x08))					// wait for txdone
	{
		temp=SPIReadReg(LR_RegIrqFlags);
/*
		if(Flag.rf_reach_timeout)			// reset the RF module when tx timeout
		{
			SPIWriteReg(LR_RegIrqFlags,0xff);	// clear interrupt
			SPIWriteReg(LR_RegOpMode,0x09);		// enter Standby mode
			SX1276_Reset();						// reset RF
			SX1276_Config();  			// initialize RF module
			break;
		}
*/
 	}

	LORA_TXEN=0; // close tx antenna switch
	LORA_RXEN=0;
	SPIWriteReg(LR_RegIrqFlags,0xff);			// clear interrupt
	SPIWriteReg(LR_RegOpMode,0x09);  			// enter standby mode

}


void SX1276_RX_INIT(void)
{
	unsigned char addr;

	LORA_TXEN = 0; // open rx antenna switch
	LORA_RXEN = 1;

	//Flag.is_tx = 0;

	//SPIWriteReg(REG_LR_DIOMAPPING1,0x01);			//DIO0=00, DIO1=00, DIO2=00, DIO3=01  DIO0=00--RXDONE
	SPIWriteReg(REG_LR_DIOMAPPING1,0b00000001);			//DIO0=00, DIO1=00, DIO2=00, DIO3=01  DIO0=00--RXDONE

	SPIWriteReg(LR_RegIrqFlagsMask,0x3f);			// enable rxdone and rxtimeout
	SPIWriteReg(LR_RegIrqFlags,0xff);			// clear interrupt

	addr = SPIReadReg(LR_RegFifoRxBaseAddr);		// read RxBaseAddr
	SPIWriteReg(LR_RegFifoAddrPtr,addr);			// RxBaseAddr->FifoAddrPtr
	SPIWriteReg(LR_RegOpMode,0x0d);				// enter rx continuous mode
}

//extern char line2[128];
unsigned char rxbuf[256];
void SX1276_RxDone(void)
{
    unsigned char a, len;
            LED ^= 1; // toggle debug LED
            SPIWriteReg(LR_RegIrqFlags, 0xff);		// clear interrupt
            a = SPIReadReg(LR_RegFifoRxCurrentaddr);	// read RxCurrentaddr
            SPIWriteReg(LR_RegFifoAddrPtr, a);		// RxCurrentaddr -> FiFoAddrPtr
            len = SPIReadReg(LR_RegRxNbBytes);	// read length of packet
            SPIBurstRead(0x00, rxbuf, len);		// read from fifo
            puts("\n\rRCV(");
            putdec(len); //utoa(line2, len, 10); puts(line2);
            puts("):");
            rxbuf[len] = '\0';
            puts(rxbuf);
            puts(", T0:");
            putdec(TMR0); //utoa(line2, TMR0, 10); puts(line2);
            puts("\n\r");
            if (len>2) SX1276_TX("OK", 2); // answer...
            SX1276_RX_INIT();
}

/*

>0200000000

 */