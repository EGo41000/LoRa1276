/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

/* TODO Application specific user parameters used in user.c may go here */

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */

void InitApp(void);         /* I/O and Peripheral Initialization */

#define LORA_NSS PORTAbits.RA7
#define LORA_NSStris TRISAbits.TRISA7
#define LORA_RESET PORTCbits.RC2
#define LORA_RESETtris TRISCbits.TRISC2
#define LORA_RXEN PORTBbits.RB0
#define LORA_RXENtris TRISBbits.TRISB0
#define LORA_TXEN PORTBbits.RB1
#define LORA_TXENtris TRISBbits.TRISB1

#define LORA_DIO0 PORTCbits.RC1
#define LORA_DIO0tris TRISCbits.TRISC1
#define LORA_DIO1 PORTCbits.RC0
#define LORA_DIO1tris TRISCbits.TRISC0
#define LORA_DIO2 PORTAbits.RA6
#define LORA_DIO2tris TRISAbits.TRISA6

#define LED PORTBbits.RB7
#define LEDtris TRISBbits.TRISB7

int puts(const char *l);
void putdec(int n);
void puthex(int n);
//void putchar (char c);
unsigned char SpiInOut (unsigned char data);
unsigned char SPIReadReg(unsigned char addr);
void SPIWriteReg(unsigned char addr, unsigned char value);
void SPIBurstRead(unsigned char addr, unsigned char *ptr, unsigned char len);
void SPIBurstWrite(unsigned char addr, unsigned char *ptr, unsigned char len);
