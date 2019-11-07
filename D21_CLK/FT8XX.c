//#include <asf.h>
#include <samd21g18a.h>
#include <stdbool.h>
#include "ft8xx.h"
#include "user_gpio.h"
#include "user_spi.h"
#include "def.h"



/*
unsigned int cmdOffset = 0x0000;
unsigned int cmdBufferRd = 0x0000;
unsigned int cmdBufferWr = 0x0000;
*/






unsigned char inline TransByte( unsigned char byte )
{
	unsigned short rcv;

	rcv = SercomSPI_TransByte( SERCOM_FT8XX, byte );

	return rcv;
}



bool HostCommand( unsigned char cmd, unsigned char p1, unsigned char p2 )
{
	GpioBitOut( FT8XX_nCS,0 );

		TransByte( cmd );
		TransByte( p1 );
		TransByte( p2 );

	GpioBitOut( FT8XX_nCS,1 );

	return true;
}



bool HostMemWriteByte( unsigned long adr, unsigned char p1 )
{
	unsigned long addres = adr | 0x00800000;

	GpioBitOut( FT8XX_nCS,0 );

		TransByte( (unsigned char)((addres & 0x00FF0000)>>16) );
		TransByte( (unsigned char)((addres & 0x0000FF00)>>8) );
		TransByte( (unsigned char)(addres & 0x000000FF) );
		TransByte( (unsigned char)(p1) );

	GpioBitOut( FT8XX_nCS,1 );

	return true;
}



bool HostMemWriteWord( unsigned long adr, unsigned short word )
{
	unsigned long addres = adr | 0x00800000;

	GpioBitOut( FT8XX_nCS,0 );

		TransByte( (unsigned char)((addres & 0x00FF0000)>>16) );
		TransByte( (unsigned char)((addres & 0x0000FF00)>>8) );
		TransByte( (unsigned char)(addres & 0x000000FF) );
		TransByte( (unsigned char)(word&0x00FF) );
		TransByte( (unsigned char)((word&0xFF00)>>8) );

	GpioBitOut( FT8XX_nCS,1 );

	return true;
}



bool HostMemWriteDoubleWord( unsigned long adr, unsigned long dword )
{
	int i;
	unsigned char rx;
	unsigned long addres = adr | 0x00800000;
	unsigned char buf[8];

	buf[0]= (unsigned char)((addres & 0x00FF0000)>>16);
	buf[1]= (unsigned char)((addres & 0x0000FF00)>>8);
	buf[2]= (unsigned char)(addres & 0x000000FF);
	buf[3]= (unsigned char)(dword&0x000000FF);
	buf[4]= (unsigned char)((dword&0x0000FF00)>> 8);
	buf[5]= (unsigned char)((dword&0x00FF0000)>>16);
	buf[6]= (unsigned char)((dword&0xFF000000)>>24);

	GpioBitOut( FT8XX_nCS,0 );

		for( i=0;i<7;i++ )
		{
			SERCOM_FT8XX->SPI.DATA.reg = (buf[i] & SPI_DATA_MASK);
			while( !(SERCOM_FT8XX->SPI.INTFLAG.reg & SPI_INTFLAG_TXC) );
			rx = (SERCOM_FT8XX->SPI.DATA.reg & 0xFF);//SPI_DATA_MASK;
		}

	GpioBitOut( FT8XX_nCS,1 );
	return true;
}



unsigned char HostMemReadByte( unsigned long adr )
{
	unsigned long addres = adr | 0x00000000;
	unsigned char data;

	GpioBitOut( FT8XX_nCS,0 );

		TransByte( (unsigned char)((addres & 0x00FF0000)>>16) );
		TransByte( (unsigned char)((addres & 0x0000FF00)>>8) );
		TransByte( (unsigned char)(addres & 0x000000FF) );

		TransByte( 0x00 );//dummy byte

		data = TransByte( 0x00 );

	GpioBitOut( FT8XX_nCS,1 );

	return data;
}



unsigned short HostMemReadWord( unsigned long adr )
{
	unsigned long addres = adr | 0x00000000;
	unsigned char H_byte;
	unsigned char L_byte;

	GpioBitOut( FT8XX_nCS,0 );

		TransByte( (unsigned char)((addres & 0x00FF0000)>>16) );
		TransByte( (unsigned char)((addres & 0x0000FF00)>>8) );
		TransByte( (unsigned char)(addres & 0x000000FF) );

		TransByte( 0x00 );//dummy byte

		L_byte = TransByte( 0x00 );//Low byte
		H_byte = TransByte( 0x00 );//Hi  byte

	GpioBitOut( FT8XX_nCS,1 );

	return ((unsigned short)(H_byte * 256 + L_byte)) ;
}



unsigned long HostMemReadDoubleWord( unsigned long adr )
{
	unsigned long addres = adr | 0x00000000;
	volatile unsigned char ary[4];
	unsigned long value;

	GpioBitOut( FT8XX_nCS,0 );

		TransByte( (unsigned char)((addres & 0x00FF0000)>>16) );
		TransByte( (unsigned char)((addres & 0x0000FF00)>>8) );
		TransByte( (unsigned char)(addres & 0x000000FF) );

		TransByte( 0x00 );//dummy byte

		ary[3]= TransByte( 0x00 );
		ary[2]= TransByte( 0x00 );
		ary[1]= TransByte( 0x00 );
		ary[0]= TransByte( 0x00 );

	GpioBitOut( FT8XX_nCS,1 );

	value = (((unsigned long)(ary[0]))<<24)|(((unsigned long)(ary[1]))<<16)|(((unsigned long)(ary[2]))<<8)|((unsigned long)(ary[3]));

	return value;
}



bool HostMemWrite_Burst( unsigned long adr,unsigned char *ptr,unsigned long size )
{
	unsigned char rx;
	unsigned long addres = adr | 0x00800000;

	GpioBitOut( FT8XX_nCS,0 );

		TransByte( (unsigned char)((addres & 0x00FF0000)>>16) );
		TransByte( (unsigned char)((addres & 0x0000FF00)>>8) );
		TransByte( (unsigned char)(addres & 0x000000FF) );
		while( size )
		{
			SERCOM_FT8XX->SPI.DATA.reg = ((unsigned char)(*ptr++) & SPI_DATA_MASK);
			while( !(SERCOM_FT8XX->SPI.INTFLAG.reg & SPI_INTFLAG_TXC) );
			rx = (SERCOM_FT8XX->SPI.DATA.reg & 0xFF);//SPI_DATA_MASK;
			size--;
		};

	GpioBitOut( FT8XX_nCS,1 );

	return true;
}




bool HostMemWrite_Boxfill_Burst( unsigned long adr,unsigned long size,int mode, unsigned char _r,unsigned char _g, unsigned char _b )
{
	unsigned char rx;
	unsigned long addres = adr | 0x00800000;
	unsigned short rgb=0;

	switch( mode )
	{
//		case RGB332:
		case RGB565:
			rgb = (((_r&0xF8)>>3) * 0x0800) + (((_g&0xFC)>>2) * 0x0020) + (((_b&0xF8)>>3) * 0x0001);
			break;
		case ARGB1555:
			rgb = (((_r&0xF8)>>3) * 0x0400) + (((_g&0xF8)>>3) * 0x0020) + (((_b&0xF8)>>3) * 0x0001);
			break;
		default:
			rgb = 0;
	}



	GpioBitOut( FT8XX_nCS,0 );

		TransByte( (unsigned char)((addres & 0x00FF0000)>>16) );
		TransByte( (unsigned char)((addres & 0x0000FF00)>>8) );
		TransByte( (unsigned char)(addres & 0x000000FF) );
		while( size )
		{
			SERCOM_FT8XX->SPI.DATA.reg = ((unsigned char)(rgb&0x00FF) & SPI_DATA_MASK);
			while( !(SERCOM_FT8XX->SPI.INTFLAG.reg & SPI_INTFLAG_TXC) );
			rx = (SERCOM_FT8XX->SPI.DATA.reg & 0xFF);//SPI_DATA_MASK;

			SERCOM_FT8XX->SPI.DATA.reg = ((unsigned char)((rgb&0xFF00)>>8) & SPI_DATA_MASK);
			while( !(SERCOM_FT8XX->SPI.INTFLAG.reg & SPI_INTFLAG_TXC) );
			rx = (SERCOM_FT8XX->SPI.DATA.reg & 0xFF);//SPI_DATA_MASK;
			size--;
		};

	GpioBitOut( FT8XX_nCS,1 );

	return true;
}


//---------------------------------------------------------------------------------------------------------------------------



#define FT_CMD_FIFO_SIZE     (4*1024L)  //4KB coprocessor Fifo size
#define MAX_CMD_FIFO_TRANSFER(x)   fifo_Freespace(x)


static unsigned short fifo_Freespace(unsigned short cmd_fifo_wp );
static void CheckCmdBuffer( unsigned long now , unsigned short count );
static unsigned short Updatecmdfifo( unsigned short cmd_fifo_wp, unsigned short count);

unsigned long cmdOffset = 0x0000;
//	unsigned int cmdOffset = 0x0000;
//	unsigned int cmdBufferRd = 0x0000;
//	unsigned int cmdBufferWr = 0x0000;


unsigned int incCMDOffset(unsigned int currentOffset, unsigned short commandSize)
{
    unsigned int newOffset;

    newOffset = currentOffset + commandSize;
    if(newOffset > 4095)
    {
        newOffset = (newOffset - 4096);
    }
    return newOffset;
}



static unsigned short fifo_Freespace(unsigned short cmd_fifo_wp )
{
 
	unsigned short fullness,retval;
    fullness = ( cmd_fifo_wp - HostMemReadWord(REG_CMD_READ)) & 4095;
    retval = (FT_CMD_FIFO_SIZE - 4) - fullness;


    return (retval);
}



static void CheckCmdBuffer( unsigned long now , unsigned short count )
{
	unsigned short getfreespace;
	do
	{
		getfreespace = fifo_Freespace( now );
	}while( getfreespace < count );
}



static unsigned short Updatecmdfifo( unsigned short cmd_fifo_wp, unsigned short count )
{
     cmd_fifo_wp = ( cmd_fifo_wp + count) & 4095;
     cmd_fifo_wp = ( cmd_fifo_wp + 3) & 0xffc;

    HostMemWriteWord( REG_CMD_WRITE, cmd_fifo_wp);

	return cmd_fifo_wp;
}



unsigned short WaitCmdfifo_empty( void )
{
	unsigned short cmd_fifo_wp,cmdBufferRd,cmdBufferWr;

	do
	{
		cmdBufferRd = HostMemReadWord(REG_CMD_READ);
		cmdBufferWr = HostMemReadWord(REG_CMD_WRITE);
	}while (cmdBufferWr != cmdBufferRd);

    cmd_fifo_wp = HostMemReadWord( REG_CMD_WRITE);

	return cmd_fifo_wp;
}



unsigned short WrCmdBuf( unsigned long adr, unsigned short cmd_fifo_wp, unsigned char *buffer,unsigned short count )
{
	unsigned char rx;
	unsigned long length =0, SizeTransfered = 0;
	unsigned long addres = adr | 0x00800000;

    do
	{
        length = count;

        if (length > MAX_CMD_FIFO_TRANSFER(cmd_fifo_wp) )
		{
            length = MAX_CMD_FIFO_TRANSFER(cmd_fifo_wp);
        }
		CheckCmdBuffer( cmd_fifo_wp, length);

		GpioBitOut( FT8XX_nCS,0 );

			addres = (RAM_CMD + cmd_fifo_wp) | 0x00800000;
			TransByte( (unsigned char)((addres & 0x00FF0000)>>16) );
			TransByte( (unsigned char)((addres & 0x0000FF00)>>8) );
			TransByte( (unsigned char)(addres & 0x000000FF) );
 
			SizeTransfered = 0;
			while (length--)
			{
				SERCOM_FT8XX->SPI.DATA.reg = ((unsigned char)(*buffer++) & SPI_DATA_MASK);
				while( !(SERCOM_FT8XX->SPI.INTFLAG.reg & SPI_INTFLAG_TXC) );
				rx = (SERCOM_FT8XX->SPI.DATA.reg & 0xFF);//SPI_DATA_MASK;
				SizeTransfered ++;
	        }
			length = SizeTransfered;
 
		GpioBitOut( FT8XX_nCS,1 );
		cmd_fifo_wp = Updatecmdfifo( cmd_fifo_wp, length);
		cmd_fifo_wp = WaitCmdfifo_empty();
 
        count -= length;

    }while (count > 0);

	return cmd_fifo_wp;
}





unsigned short burst( unsigned char *buffer,unsigned short count )
{
  unsigned char rx;
	unsigned long length =0, SizeTransfered = 0;
	unsigned long addres = 0,spc=0;

//	HostMemWriteWord( REG_CMD_WRITE,cmd_fifo_wp );
//	cmd_fifo_wp = WaitCmdfifo_empty();


	unsigned short cmdBufferRd,cmdBufferWr;

	cmdBufferRd = HostMemReadWord(REG_CMD_READ);



    do
	{
        length = count;
		spc = HostMemReadDoubleWord(REG_CMDB_SPACE);
		if( length > spc )
		{
			length = spc;
		}

		GpioBitOut( FT8XX_nCS,0 );

			addres = (REG_CMDB_WRITE) | 0x00800000;
			TransByte( (unsigned char)((addres & 0x00FF0000)>>16) );
			TransByte( (unsigned char)((addres & 0x0000FF00)>>8) );
			TransByte( (unsigned char)(addres & 0x000000FF) );
 
			SizeTransfered = 0;
			while (length--)
			{
				SERCOM_FT8XX->SPI.DATA.reg = ((unsigned char)(*buffer++) & SPI_DATA_MASK);
				while( !(SERCOM_FT8XX->SPI.INTFLAG.reg & SPI_INTFLAG_TXC) );
				rx = (SERCOM_FT8XX->SPI.DATA.reg & 0xFF);//SPI_DATA_MASK;
				SizeTransfered ++;
	        }
			length = SizeTransfered;
 
		GpioBitOut( FT8XX_nCS,1 );

		spc = HostMemReadDoubleWord(REG_CMDB_SPACE);
	    HostMemWriteWord( REG_CMD_WRITE, cmdBufferRd + length );

		cmdBufferWr = HostMemReadWord(REG_CMD_WRITE);

		WaitCmdfifo_empty();
 
        count -= length;


    }while (count > 0);

	return cmdBufferWr;
}



//---------------------------------------------------------------------------------------------------------------------------



void FT8XX_LCD_Init( int mode )
{

	// Configure display registers - demonstration for WQVGA resolution 
	if( mode == LCD_800x480 )
	{
	// Now start clocking data to the LCD panel
	//	HostMemWriteByte(REG_PCLK, 2);
		HostMemWriteByte(REG_DITHER, 0x00);
	// Configure display registers - demonstration for WQVGA resolution
		HostMemWriteWord(REG_HSIZE, 800); 
		HostMemWriteWord(REG_HCYCLE, 1056); 
		HostMemWriteWord(REG_HOFFSET, 256); //fp+bp
		HostMemWriteWord(REG_HSYNC0, 210); //fp
		HostMemWriteWord(REG_HSYNC1, 212); //fp+hsync
		HostMemWriteWord(REG_VSIZE, 480); 
		HostMemWriteWord(REG_VCYCLE, 525); 
		HostMemWriteWord(REG_VOFFSET, 30); //fp+bp
		HostMemWriteWord(REG_VSYNC0, 7); //fp
		HostMemWriteWord(REG_VSYNC1, 27); //fp+vsync
		HostMemWriteByte(REG_SWIZZLE, 0); 
		HostMemWriteByte(REG_PCLK_POL, 1); 
	}
	else
	{
	// Now start clocking data to the LCD panel
	//	HostMemWriteByte(REG_PCLK, 7);
		HostMemWriteByte(REG_DITHER, 0x00);
	// Configure display registers - demonstration for WQVGA resolution
		HostMemWriteWord(REG_HSIZE, 480); 
		HostMemWriteWord(REG_HCYCLE, 548); 
		HostMemWriteWord(REG_HOFFSET, 43); 
		HostMemWriteWord(REG_HSYNC0, 0); 
		HostMemWriteWord(REG_HSYNC1, 41); 
		HostMemWriteWord(REG_VSIZE, 272); 
		HostMemWriteWord(REG_VCYCLE, 292); 
		HostMemWriteWord(REG_VOFFSET, 12); 
		HostMemWriteWord(REG_VSYNC0, 0); 
		HostMemWriteWord(REG_VSYNC1, 10); 
		HostMemWriteByte(REG_SWIZZLE, 0); 
		HostMemWriteByte(REG_PCLK_POL, 1); 
	}



	/* write first display list */ 
	HostMemWriteDoubleWord( RAM_DL+0,DL_CLEAR_RGB );
	HostMemWriteDoubleWord( RAM_DL+4,DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG );
	HostMemWriteDoubleWord( RAM_DL+8,DL_DISPLAY );
	HostMemWriteByte(REG_DLSWAP, DLSWAP_FRAME);


//	HostMemWriteByte(REG_PWM_DUTY, 128); 
	HostMemWriteByte(REG_PWM_DUTY, 64);

	if( mode )
		HostMemWriteByte(REG_PCLK, 2);
	else
		HostMemWriteByte(REG_PCLK, 7);


}




