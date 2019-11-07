//#include <asf.h>
#include <samd21g18a.h>
#include <stdbool.h>
#include "ft8xx.h"
#include "user_gpio.h"
#include "user_spi.h"
#include "def.h"





//---------------------------------------------------------------------------------------------------------------------------
unsigned char ramcmd[8192];
unsigned long ramcmd_index = 0;



unsigned short RamCmdStart(void)
{
	return WaitCmdfifo_empty();
}



void RamCmdFlash(void)
{
	ramcmd_index = 0;
}

extern unsigned short burst( unsigned char *buffer,unsigned short count );


unsigned short RamCmdTrans(void)
{
	unsigned short cmdOffset;
	unsigned long sz;
	unsigned long ofs;
	unsigned long cnt;

	cmdOffset = RamCmdStart();

	sz = ramcmd_index;
	ofs=0;
	while(sz > 0)
	{
		if(sz >= 8192)
		{
			cnt=8192;
		}
		else
		{
			cnt=sz;
		}
		cmdOffset = WrCmdBuf( RAM_CMD + cmdOffset, cmdOffset, &ramcmd[ofs],cnt );
		ofs = ofs + cnt;
		sz -= cnt;
	}

	HostMemWriteWord(REG_CMD_WRITE, (cmdOffset));
	cmdOffset = WaitCmdfifo_empty();

	return cmdOffset;
}



void RamMemWriteDoubleWord( unsigned long value )
{
	ramcmd[ ramcmd_index+0 ]= (unsigned char)( value & 0x000000FF);
	ramcmd[ ramcmd_index+1 ]= (unsigned char)((value & 0x0000FF00)>> 8);
	ramcmd[ ramcmd_index+2 ]= (unsigned char)((value & 0x00FF0000)>>16);
	ramcmd[ ramcmd_index+3 ]= (unsigned char)((value & 0xFF000000)>>24);
	ramcmd_index += 4;
}



void RamMemWriteWord( unsigned short value )
{
	ramcmd[ ramcmd_index+0 ]= (unsigned char)( value & 0x00FF);
	ramcmd[ ramcmd_index+1 ]= (unsigned char)((value & 0xFF00)>> 8);
	ramcmd_index += 2;
}



