#include <samd21g18a.h>
#include <stdbool.h>
#include "ft8xx.h"
	
    
    
extern unsigned int cmdOffset;
extern unsigned int cmdBufferRd;
extern unsigned int cmdBufferWr;


void TouchPanelCalibration( void )
{
//	unsigned int cmdOffset = 0x0000;
//	unsigned int cmdBufferRd = 0x0000;
//	unsigned int cmdBufferWr = 0x0000;
#if 0
	do
	{
		cmdBufferRd = HostMemReadWord(REG_CMD_READ);
		cmdBufferWr = HostMemReadWord(REG_CMD_WRITE);
	}while (cmdBufferWr != cmdBufferRd);
	cmdOffset = cmdBufferWr;

	HostMemWriteDoubleWord(RAM_CMD + cmdOffset, (CMD_DLSTART));
	cmdOffset = incCMDOffset(cmdOffset, 4);

	HostMemWriteDoubleWord(RAM_CMD + cmdOffset, (DL_CLEAR_RGB | WHITE));
	cmdOffset = incCMDOffset(cmdOffset, 4);

	HostMemWriteDoubleWord(RAM_CMD + cmdOffset, (DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG));
	cmdOffset = incCMDOffset(cmdOffset, 4);

	HostMemWriteDoubleWord(RAM_CMD + cmdOffset, (CMD_CALIBRATE));
	cmdOffset = incCMDOffset(cmdOffset, 4);
	//		HostMemWriteDoubleWord(RAM_CMD + cmdOffset, (0));//result
	//		cmdOffset = incCMDOffset(cmdOffset, 4);

	HostMemWriteDoubleWord(RAM_CMD + cmdOffset, (DL_DISPLAY));
	cmdOffset = incCMDOffset(cmdOffset, 4);

	HostMemWriteDoubleWord(RAM_CMD + cmdOffset, (CMD_SWAP));
	cmdOffset = incCMDOffset(cmdOffset, 4);

	HostMemWriteWord(REG_CMD_WRITE, cmdOffset);

	do
	{
		cmdBufferRd = HostMemReadWord(REG_CMD_READ);
		cmdBufferWr = HostMemReadWord(REG_CMD_WRITE);
	}while (cmdBufferWr != cmdBufferRd);

#else

	RamCmdFlash();
	RamMemWriteDoubleWord( (CMD_DLSTART) );
	RamMemWriteDoubleWord( (DL_CLEAR_RGB | WHITE) );
	RamMemWriteDoubleWord( (DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG));
	RamMemWriteDoubleWord( (CMD_CALIBRATE));
	RamMemWriteDoubleWord( (DL_DISPLAY) );
	RamMemWriteDoubleWord( (CMD_SWAP) );
	RamCmdTrans();
	WaitCmdfifo_empty();

#endif
}













void TouchPanelTestDisplay( void )
{

	volatile unsigned long raw_touch_xy;
	volatile unsigned long raw_touch_z;
	volatile unsigned long touch_x;
	volatile unsigned long touch_y;
	volatile unsigned long touch_z;

    while(1)
	{
		do
		{
			cmdBufferRd = HostMemReadWord(REG_CMD_READ);
			cmdBufferWr = HostMemReadWord(REG_CMD_WRITE);
		}while (cmdBufferWr != cmdBufferRd);
		cmdOffset = cmdBufferWr;

		HostMemWriteDoubleWord(RAM_CMD + cmdOffset, (CMD_DLSTART));
		cmdOffset = incCMDOffset(cmdOffset, 4);

		HostMemWriteDoubleWord(RAM_CMD + cmdOffset, (DL_CLEAR_RGB | BLUE));
		cmdOffset = incCMDOffset(cmdOffset, 4);

		HostMemWriteDoubleWord(RAM_CMD + cmdOffset, (DL_COLOR_RGB | WHITE));
		cmdOffset = incCMDOffset(cmdOffset, 4);

		HostMemWriteDoubleWord(RAM_CMD + cmdOffset, (DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG));
		cmdOffset = incCMDOffset(cmdOffset, 4);


		raw_touch_xy = HostMemReadDoubleWord(REG_TOUCH_SCREEN_XY);//REG_TOUCH_RAW_XY);
		touch_x = ((raw_touch_xy & 0xFFFF0000)>>16);
		touch_y = (raw_touch_xy & 0x0000FFFF);

		raw_touch_z = HostMemReadWord(REG_TOUCH_RZ);//REG_TOUCH_RZTHRESH);
		touch_z = (raw_touch_z & 0x0000FFFF);



		HostMemWriteDoubleWord( RAM_CMD + cmdOffset, (CMD_NUMBER));
		cmdOffset = incCMDOffset(cmdOffset, 4);
		
		HostMemWriteWord( RAM_CMD + cmdOffset, (0));//x
		cmdOffset = incCMDOffset(cmdOffset, 2);
		HostMemWriteWord( RAM_CMD + cmdOffset, (0));//y
		cmdOffset = incCMDOffset(cmdOffset, 2);
		HostMemWriteWord( RAM_CMD + cmdOffset, (31));//font
		cmdOffset = incCMDOffset(cmdOffset, 2);
		HostMemWriteWord( RAM_CMD + cmdOffset, (0));//option
		cmdOffset = incCMDOffset(cmdOffset, 2);
		HostMemWriteDoubleWord( RAM_CMD + cmdOffset, (touch_x));
		cmdOffset = incCMDOffset(cmdOffset, 4);

		HostMemWriteDoubleWord( RAM_CMD + cmdOffset, (CMD_NUMBER));
		cmdOffset = incCMDOffset(cmdOffset, 4);

		HostMemWriteWord( RAM_CMD + cmdOffset, (0));//x
		cmdOffset = incCMDOffset(cmdOffset, 2);
		HostMemWriteWord( RAM_CMD + cmdOffset, (32));//y
		cmdOffset = incCMDOffset(cmdOffset, 2);
		HostMemWriteWord( RAM_CMD + cmdOffset, (31));//font
		cmdOffset = incCMDOffset(cmdOffset, 2);
		HostMemWriteWord( RAM_CMD + cmdOffset, (0));//option
		cmdOffset = incCMDOffset(cmdOffset, 2);
		HostMemWriteDoubleWord( RAM_CMD + cmdOffset, (touch_y));
		cmdOffset = incCMDOffset(cmdOffset, 4);

		HostMemWriteDoubleWord( RAM_CMD + cmdOffset, (CMD_NUMBER));
		cmdOffset = incCMDOffset(cmdOffset, 4);

		HostMemWriteWord( RAM_CMD + cmdOffset, (0));//x
		cmdOffset = incCMDOffset(cmdOffset, 2);
		HostMemWriteWord( RAM_CMD + cmdOffset, (64));//y
		cmdOffset = incCMDOffset(cmdOffset, 2);
		HostMemWriteWord( RAM_CMD + cmdOffset, (31));//font
		cmdOffset = incCMDOffset(cmdOffset, 2);
		HostMemWriteWord( RAM_CMD + cmdOffset, (0));//option
		cmdOffset = incCMDOffset(cmdOffset, 2);
		HostMemWriteDoubleWord( RAM_CMD + cmdOffset, (touch_z));
		cmdOffset = incCMDOffset(cmdOffset, 4);


		HostMemWriteDoubleWord(RAM_CMD + cmdOffset, (DL_DISPLAY));
		cmdOffset = incCMDOffset(cmdOffset, 4);

		HostMemWriteDoubleWord(RAM_CMD + cmdOffset, (CMD_SWAP));
		cmdOffset = incCMDOffset(cmdOffset, 4);

		HostMemWriteWord(REG_CMD_WRITE, cmdOffset);

		do
		{
			cmdBufferRd = HostMemReadWord(REG_CMD_READ);
			cmdBufferWr = HostMemReadWord(REG_CMD_WRITE);
		}while (cmdBufferWr != cmdBufferRd);
		cmdOffset = cmdBufferWr;
	}
  
}






