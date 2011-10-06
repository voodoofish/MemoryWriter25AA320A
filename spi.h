//http://www.dur.ac.uk/peter.baxendale/stuff/msp430/
#ifndef SPI_INCL
#define SPI_INCL
/** \file spi.h
Function prototypes and in-line functions for spi handling.
*/

/*
There's no need for an Rx function, since have to send
something on SPI in order to get something back.
*/
void spiInit(void); // initialises spi port

/** Send a byte to spi and return byte received on spi.
param c - byte to send to SPI.
*/
void spiStart(void);
void spiStop(void);
void spiTx(unsigned char c);
void spiTxINT(unsigned int i);
unsigned char spiRx();
void enablePin( unsigned char bits );
void disablePin( unsigned char bits );
void delay(unsigned int ms);
#endif
