/* 
 * File:   LoRa1276.h
 * Author: rz0df9
 *
 * Created on 15 juin 2015, 15:40
 */

#ifndef LORA1276_H
#define	LORA1276_H

#ifdef	__cplusplus
extern "C" {
#endif


//SX1276 Internal registers Addres
#define LR_RegFifo                       0x00
#define LR_RegOpMode                     0x01
#define LR_RegBitrateMsb                 0x02
#define LR_RegBitrateLsb                 0x03
#define LR_RegFdevMsb                    0x04
#define LR_RegFdMsb                      0x05
#define LR_RegFrMsb                      0x06
#define LR_RegFrMid                      0x07
#define LR_RegFrLsb                      0x08
#define LR_RegPaConfig                   0x09
#define LR_RegPaRamp                     0x0A
#define LR_RegOcp                        0x0B
#define LR_RegLna                        0x0C
#define LR_RegFifoAddrPtr                0x0D
#define LR_RegFifoTxBaseAddr             0x0E
#define LR_RegFifoRxBaseAddr             0x0F
#define LR_RegFifoRxCurrentaddr          0x10
#define LR_RegIrqFlagsMask               0x11
#define LR_RegIrqFlags                   0x12
#define LR_RegRxNbBytes                  0x13
#define LR_RegRxHeaderCntValueMsb        0x14
#define LR_RegRxHeaderCntValueLsb        0x15
#define LR_RegRxPacketCntValueMsb        0x16
#define LR_RegRxPacketCntValueLsb        0x17
#define LR_RegModemStat                  0x18
#define LR_RegPktSnrValue                0x19
#define LR_RegPktRssiValue               0x1A
#define LR_RegRssiValue                  0x1B
#define LR_RegHopChannel                 0x1C
#define LR_RegModemConfig1               0x1D
#define LR_RegModemConfig2               0x1E
#define LR_RegSymbTimeoutLsb             0x1F
#define LR_RegPreambleMsb                0x20
#define LR_RegPreambleLsb                0x21
#define LR_RegPayloadLength              0x22
#define LR_RegMaxPayloadLength           0x23
#define LR_RegHopPeriod                  0x24
#define LR_RegFifoRxByteAddr             0x25
#define LR_RegModemConfig3               0x26
#define REG_LR_DIOMAPPING1               0x40
#define REG_LR_DIOMAPPING2               0x41
#define REG_LR_VERSION                   0x42
#define REG_LR_PLLHOP                    0x44
#define REG_LR_TCXO                      0x4B
#define REG_LR_PADAC                     0x4D
#define REG_LR_FORMERTEMP                0x5B
#define REG_LR_AGCREF                    0x61
#define REG_LR_AGCTHRESH1                0x62
#define REG_LR_AGCTHRESH2                0x63
#define REG_LR_AGCTHRESH3                0x64

/*
typedef struct
{
	unsigned char reach_tx			: 1;
	unsigned char is_tx			: 1;
	unsigned char rf_reach_timeout		: 1;
}   FlagType;
*/
void SX1276_Reset(void);
void SX1276_Config(void);
void SX1276_TX (const char *line, unsigned char len);
void SX1276_RX_INIT(void);
void SX1276_RxDone(void);

#ifdef	__cplusplus
}
#endif

#endif	/* LORA1276_H */

