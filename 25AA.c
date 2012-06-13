#include "25AA.h"
#include "spi.h"

unsigned char readStatusReg(unsigned char pin, unsigned char port, unsigned char command ){
volatile unsigned char Reg = 0;
enablePin(pin,port);
spiTx(command);
Reg = spiRx();
disablePin(pin,port);

return Reg;		
}

unsigned char readPageMemLoc(int loc, unsigned char pin, unsigned char port){
unsigned char Reg = 0;
enablePin(pin, port);
spiTx(READ);
spiTxINT(loc);
Reg = spiRx();
disablePin(pin, port);
return Reg;
}

void wrtiePageLoc(int loc, unsigned char writebyte, unsigned char pin, unsigned char port) {
enablePin(pin,port);
spiTx(WREN); //init the write enable
disablePin(pin,port);
enablePin(pin,port);
spiTx(WRITE); //send write command.
spiTxINT(loc); //send location
spiTx(writebyte);//send databyte
disablePin(pin,port);
}
