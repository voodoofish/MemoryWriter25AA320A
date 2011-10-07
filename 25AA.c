#include "25AA.h"
#include "spi.h"

unsigned char readStatusReg(unsigned char pin, unsigned char command ){
volatile unsigned char Reg = 0;
enablePin(pin);
spiTx(command);
Reg = spiRx();
disablePin(pin);

return Reg;		
}

unsigned char readPageMemLoc(int loc, unsigned char pin){
unsigned char Reg = 0;
enablePin(pin);
spiTx(READ);
spiTxINT(loc);
Reg = spiRx();
disablePin(pin);
return Reg;
}

void wrtiePageLoc(int loc, unsigned char writebyte, unsigned char pin) {
enablePin(pin);
spiTx(WREN); //init the write enable
disablePin(pin);
enablePin(pin);
spiTx(WRITE); //send write command.
spiTxINT(loc); //send location
spiTx(writebyte);//send databyte
disablePin(pin);
}
