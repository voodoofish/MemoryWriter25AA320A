//Project Memory test for 25AA320A
/*4096 bytes, 128 pages @32 bytes per page.
 *For this project I think that the first byte of each page should contain the info on what data is being used. 
 * The data collected should be linear so this will work. Not sure if I should use 0x00 for blank data or 0xff.
 * Example page data:
 * _________________________________________________
 * |Page	|bytes  | hex 16bit	|Page Ranges 16bit	|
 * |0		|0-31	|0x0000		|0x0000-0x001f		|
 * |1		|32-63	|0x0020		|0x0020-0x003f		|
 * |2		|64-95	|0x0040		|0x0040-0x005f		|
 * --------------------------------------------------
 * 
 * 
 */

#include "msp430g2231.h"
#include "spi.h"
#include "25AA.h"

#define LED1 BIT0
#define CS BIT4
volatile unsigned char  a = 0;
volatile unsigned char membyte = 0;
/*Sequence for read
 * 1. cs goes low
 * 2. 8bit read instruction-->16bit location-->8bit data
 * 3. CS goes high or continue to read data n bytes.
 * 
 *Sequence for write
 * 1. cs goes low
 * 2. 8bit write engable
 * 3. cs goes high, then low to start write
 * 4. 8bit write command 16bit address, 8 bit data.
 * 5. cs goes high or continue to write.
*/
void timer_init(void)//copied from user FJ604 MAX6957 code
{
    TACTL = TASSEL_2 | MC_0;        // SMCLK, stop timer
    TACCTL0 = CCIE;                 // Enable timer compare interrupt
} 

void sleep(unsigned int count)//copied from user FJ604 MAX6957 code
{
    TACCR0 = count;             // Load top value in compare register
    TACTL |= TACLR | MC_1;      // Clear counter, start timer in up mode
    _low_power_mode_0();        // Sleep in LPM0 until interrupt
    TACTL &= ~MC_1;             // Stop timer
}

void main(void){
WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
	//set up pin 3 button to trigger action.
	P1IE = 0x08; // P1.3 interrupt enabled
	P1IES |= 0x08; // P1.3 Hi/lo edge
	P1IFG &= ~0x08; // P1.3 IFG cleared
	P1DIR |= CS; //set chip select to output
	disablePin(CS); //bring chip select high
	//put break point here
	P1OUT &= ~0x1; //Set P1.0.
	P1DIR |= 0x1; // Set P1.0.
	timer_init();
	spiInit(); //get things going
	spiStop();	//set spi to inactive.
	spiStart();
_BIS_SR(LPM0_bits + GIE); // Enter LPM0 w/interrupt
}

//put interupt here.
// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
P1OUT |=0x1; //sets P1.0 high
P1IFG &= ~0x08; // P1.3 IFG cleared
a = readStatusReg(CS, RDSR);
wrtiePageLoc(0, 0x89, CS);
while(readStatusReg(CS, RDSR)&0x01==0x01)
{};
//sleep(10000);
membyte = readPageMemLoc(0,CS);

P1OUT &= ~0x1; //Turn off P1.0
}

#pragma vector=USI_VECTOR
__interrupt void usi_interrupt(void)
{
    USICTL1 &= ~USIIFG;             // Clear interrupt flag
    _low_power_mode_off_on_exit();  // Return from LPM
}

#pragma vector=TIMERA0_VECTOR
__interrupt void timer_A_interrupt(void)
{
    _low_power_mode_off_on_exit();  // Return from LPM
}
