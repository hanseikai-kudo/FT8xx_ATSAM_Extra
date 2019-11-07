#ifndef _RAMCMD_H_
#define _RAMCMD_H_



void RamMemWriteWord( unsigned short value );
void RamMemWriteDoubleWord( unsigned long value );
unsigned short RamCmdTrans(void);
void RamCmdFlash(void);
unsigned short RamCmdStart(void);


#endif

