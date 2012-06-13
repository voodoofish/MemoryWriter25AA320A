#ifndef mem25AA_INC
#define mem25AA_INC
/*
 *Instruction Name Instruction Format Description
 *READ 0000 0011 Read data from memory array beginning at selected address
 *WRITE 0000 0010 Write data to memory array beginning at selected address
 *WRDI 0000 0100 Reset the write enable latch (disable write operations)
 *WREN 0000 0110 Set the write enable latch (enable write operations)
 *RDSR 0000 0101 Read STATUS register
 *WRSR 0000 0001 Write STATUS register 
 * 
 */

#define READ 	0x03
#define WRITE	0x02
#define WRDI	0x04
#define WREN	0x06
#define RDSR	0x05
#define WRSR	0x01

//other deinfes for the chip
#define MAXPAGE 127 //128 pages 0-127
#define MAXPAGESIZE 31 //32 bytes 0-31
#define MAXMEM 4095 //4096 bytes total

//these should go into their own header files.
int findPageStart(int page);
int findPageMemLoc(int page, int loc);
void writeEn(void);
void writePageMem(int page);
void wrtiePageLoc(int loc, unsigned char writebyte, unsigned char pin, unsigned char port); //get this from findPageMemLoc() 
int readPageMem(int page);
unsigned char readPageMemLoc(int loc, unsigned char pin, unsigned char port);
unsigned char readStatusReg(unsigned char pin, unsigned char port, unsigned char command);

#endif /*mem25AA*/
