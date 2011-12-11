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
//#include "ta_uart.h"

#define LED1 BIT0
#define CS BIT4
volatile unsigned char  a = 0;
volatile unsigned char membyte = 0;
long temp;
long IntDegF;
volatile unsigned char loopVar = 1; //Set to 1 so that we don't start doing mem readings
volatile unsigned int memCounter = 0;
//int callBack( unsigned char c );
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

//Adding the WD timer as we will be using the TimerA for SoftSerial 
void WD_intervalTimerInit(void)
{
  WDTCTL = WDT_ADLY_1000;                    // WDT 250ms, ACLK, interval timer
  IE1 |= WDTIE;                             // Enable WDT interrupt
}
//incomplete
void WD_ITimerStartStop(unsigned char command)//puts wd interval timer into a halted state
{
WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
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
//	timer_init(); //turned off since I commented out the code
	ADC10CTL1 = INCH_10 + ADC10DIV_3;         // Temp Sensor ADC10CLK/4
  	ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE;
	//WD_intervalTimerInit();//give some warmup time of 250ms 
	spiInit(); //get things going
	spiStop();	//set spi to inactive.
	spiStart();
//	TI_initTimer(callBack, _1MHz_SMCLK_02400_Bitime, TASSEL_0);
	while(1){
		if (loopVar==0) {
			ADC10CTL0 |= ENC + ADC10SC;
			loopVar++;
			} // Sampling and conversion start
		_BIS_SR(LPM0_bits + GIE); // Enter LPM0 w/interrupt
    	}
}

//put interupt here.
// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	WD_intervalTimerInit();
	//loopVar = 0;
	P1IFG &= ~0x08; // P1.3 IFG cleared
/*
P1OUT |=0x1; //sets P1.0 high
P1IFG &= ~0x08; // P1.3 IFG cleared
a = readStatusReg(CS, RDSR);
wrtiePageLoc(1, 0x66, CS);
while(readStatusReg(CS, RDSR)&0x01==0x01)
{};
//sleep(10000);
membyte = readPageMemLoc(1,CS);
P1OUT &= ~0x1; //Turn off P1.0
*/}

#pragma vector=USI_VECTOR
__interrupt void usi_interrupt(void)
{
    USICTL1 &= ~USIIFG;             // Clear interrupt flag
    _low_power_mode_off_on_exit();  // Return from LPM
}
/*
#pragma vector=TIMERA0_VECTOR
__interrupt void timer_A_interrupt(void)
{
    _low_power_mode_off_on_exit();  // Return from LPM
}
*/
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{	//WDTCTL = WDT_ADLY_1000;  //now set to 1s interval
 	loopVar = 0;
 	_low_power_mode_off_on_exit();
}

#pragma vector=ADC10_VECTOR
__interrupt void adc10_tempGetter(void)
{if (memCounter > MAXMEM)
	{WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
	}
else{
    temp = ADC10MEM;
    IntDegF = ((temp - 630) * 761) / 1024;
	P1OUT |=0x1; //sets P1.0 high
	P1IFG &= ~0x08; // P1.3 IFG cleared
	wrtiePageLoc(memCounter, IntDegF, CS);
	while(readStatusReg(CS, RDSR)&0x01==0x01)
	{}; //keep looping until register no longer shows write active.
//	membyte = readPageMemLoc(1,CS);
//    TI_TX_Byte( IntDegF );
	P1OUT &= ~0x1; //Turn off P1.0
	memCounter++;
	}
}
/*
int callBack( unsigned char c )
{
TI_TA_UART_StatusFlags &= ~TI_TA_RX_RECEIVED; // allows for RX during TX
TI_TX_Byte( c ); // echo byte
return TA_UART_STAY_LPM; // return to LPM3
}
*/