//#include <io.h>
#include "spi.h"
#include "msp430.h"
/** \file spi.c
SPI functions are either on here or in spi.h.
*/
/** Initialise spi interface on uart0.
Set for 8 bits data, master, 3 pin mode.
Uses SMCLK with divide set to 2 (ie maximum speed). Interrupts disabled.
*/
/*I'm considering giving the spiInit function an option list so that I can turn on each one of the features.
 * I might need to just bite the bullet and keep this as a project specific function as it controls the setup.
 */
void spiInit(void) {
	  USICTL = USIPE7 + USIPE6 + USIPE5 + USIMST + USIOE; //enable spi out, clock enable, MSB(leave unset), data output Enable
	  USICTL1 |= USIIE + USICKPH;             // Counter interrupt, flag remains set
      USICKCTL = USIDIV_4 + USISSEL_2;          // /16 ACLK
  	  USICTL0 &= ~USISWRST;                     // USI released for operation
      USICNT = 8;                   // init-load counter
//      _low_power_mode_0();            
}

void spiStart(void){
//  	  USICTL1 |= USIIE;
  	  USICTL0 &= ~USISWRST;                     // USI released for operation
}

void spiStop(void){
USICTL0 &= ~USISWRST;
//USICNT =1;
USICTL0 |= USISWRST; //turn off USI operation
}

void spiTx(unsigned char c) {
USICNT &= ~USI16B;
USISRL = c;
USICNT = 8;
_low_power_mode_0();
}

void spiTxINT(unsigned int i) {
USICNT |= USI16B;				//set to use 16 bits, this may need to be set every time
USISR = i; //was USISRL
USICNT |= 0x10;	
 _low_power_mode_0();
//I want to send 16 bits worth of data, this sets usicnt4 to 1.
//USICNT =USI16B | 16;		//This is an alt method of setting usi16b and sending data //returns 0x640A ..this is correct!
//delay(1); //this isn't needed...clean up and use the usi interupt.
} 

unsigned char spiRx()
{
USICNT &= ~USI16B;
USICNT = 8;
 _low_power_mode_0();
return 	USISRL;
}

void enablePin(unsigned char bits)
{
	P1OUT &= ~bits;
}
 
void disablePin(unsigned char bits)
{
  P1OUT |= bits;
}
void delay(unsigned int ms)
{
 while (ms--)
    {
        __delay_cycles(1000); // set for 16Mhz change it to 1000 for 1 Mhz
    }
}
