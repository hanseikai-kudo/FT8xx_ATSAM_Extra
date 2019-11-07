#include <atmel_start.h>
#include <samd21g18a.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "user_gpio.h"
#include "user_spi.h"
#include "user_misc.h"

#include "ft8xx.h"
#include "ramcmd.h"
#include "trans_sys.h"









extern unsigned char exit_jpeg_480_272[];
extern unsigned char hansei_logo_220_160[];
extern unsigned char ext_uotewyiog_480_272[];
extern unsigned char maji_ten_480_272[];
extern unsigned char blob_dummy[];
extern unsigned char  jpegtest[];



#define Image_Logo  0x00000000 //0x000C0000
#define Image_Exit  0x00040000 //0x00000000
#define Image_Gear1 0x00080000
#define Image_Gear2 0x000C0000
#define Image_Offset1 0x00080000
#define Image_Offset2 0x000C0000
#define SIZE_OF_EXT_UOTEWYIOG (0x15730)
#define SIZE_OF_HANSEI_LOGO (0x3540)
#define SIZE_OF_EXIT_JPEG (0x4970)
#define SIZE_OF_MAJI_TEN (0x13540)
#define SIZE_OF_FLOG (0xE34)

#define Image_BLOB 0x00000000
#define SIZE_OF_BLOB_DMY (4096)

struct ImageTable  
{
	unsigned long offset;
	unsigned long source;
	unsigned long size;
};

const struct ImageTable table[]=
{
	{Image_Logo,&hansei_logo_220_160[0],SIZE_OF_HANSEI_LOGO},
	{Image_Offset1,&exit_jpeg_480_272[0],SIZE_OF_EXIT_JPEG},	
	{Image_Offset2,&ext_uotewyiog_480_272[0],SIZE_OF_EXT_UOTEWYIOG},
	{Image_Offset1,&maji_ten_480_272[0],SIZE_OF_MAJI_TEN},
	{Image_Logo,&jpegtest[0],SIZE_OF_FLOG},
};






GPIO_INIT_TABLE test2[]=
{
	{ GPIO_PA12, USE_GPIO_FUNC, USE_GPIO_OUTPUT,PA12_USE_SERCOM2_PAD_0 },
	{ GPIO_PA13, USE_GPIO_FUNC, USE_GPIO_OUTPUT,PA13_USE_SERCOM2_PAD_1 },
	{ GPIO_PA14, USE_GPIO_PORT, USE_GPIO_OUTPUT,0 },
	{ GPIO_PA15, USE_GPIO_FUNC, USE_GPIO_INPUT ,PA15_USE_SERCOM2_PAD_3 },
	//{ GPIO_PA14, USE_GPIO_FUNC, USE_GPIO_OUTPUT,PA14_USE_GCLK_IO_0 },
	{ GPIO_ENDMARK,0,0,0},
};


extern unsigned int cmdOffset;
extern unsigned int cmdBufferRd;
extern unsigned int cmdBufferWr;
























void FTDI_Logo(void);
void Raster_Logo(void);
void Puzzle_Demo( void );




void FTDI_Logo(void)
{
	Trans( SetupStart, NULL, NULL );
	Trans( ScreenClean, NULL, NULL );
	Trans( Logo, NULL, NULL );
	Trans( TransStart, NULL, NULL );
}








void Raster_Logo(void)
{
	struct PanelTbl move;
	int i;
	float f_deg;
	unsigned short value;



	JpegLoad( 1,(unsigned char *)table[0].source,    table[0].offset, table[0].size );

	ParamScreen.ImageWx=220;
	ParamScreen.ImageWy=160;
	ParamScreen.ImageOffset = table[0].offset;//Image_Logo;
	ParamScreen.dot_depth=2;

	ParamScreen.div_Wx = 1;
	ParamScreen.div_Wy = 80;

	ParamScreen.blkWx = ParamScreen.ImageWx / ParamScreen.div_Wx;
	ParamScreen.blkWy = ParamScreen.ImageWy / ParamScreen.div_Wy;
	ParamScreen.BGcolor = BLACK;
	ParamScreen.BGClear = (CLR_COL | CLR_STN | CLR_TAG);

	for( i=0;i<300;i++ )raster[i]=0;
	f_deg = 5.0;
	value=50;

	while(f_deg < 320)
	{
		Trans( SetupStart, &ParamScreen, NULL );
		Trans( ScreenClean, &ParamScreen, NULL );

		for( i=0;i<80;i++ )
		{

			//				raster[i] = sin( 2.0f * 3.14f *(value)  ) * 100 * sin( 2.0f * 3.14f * ( (float)( ( (float)(f_deg)/60.0f) - (float)((i*2))/50.0f )) );
			//				raster[i] = sin( 2.0f * 3.14f *(value)  ) * 10 * sin( 2.0f * 3.14f * ( (float)( ( (float)(f_deg)/60.0f) - (float)((i*2))/10.0f )) );
			raster[i] = sin( 0.6f * 3.14f *(value)  ) * 50 * sin( 2.0f * 3.14f * ( (float)( ( (float)(f_deg)/60.0f) - (float)((i*2))/50.0f )) );

			move.x = 130+raster[i];
			move.y = (56+i*2) ;
			move.wx = 220;
			move.wy = 2;
			move.offset = ParamScreen.ImageOffset + (move.wx*2)*(i*2);
			move.flag = fDispOn;

			Trans( SetupPartical, &ParamScreen, &move );

		}
		Trans( TransStart, &ParamScreen, NULL );

		value += 50;
		f_deg += 5.0;
	}
	Trans( SetupStart, &ParamScreen, NULL );
	Trans( ScreenClean, &ParamScreen, NULL );

	move.x = 130;
	move.y = 56 ;
	move.wx = 220;
	move.wy = 160;
	move.offset = ParamScreen.ImageOffset;
	move.flag = fDispOn;

	Trans( SetupPartical, &ParamScreen, &move );
	Trans( TransStart, &ParamScreen, NULL );
}


#define DIR_S 0
#define DIR_W 1
#define DIR_N 2
#define DIR_E 3


void Puzzle_Demo( void )
{
	int i;//,j,k;
	//	unsigned short value;

	unsigned short position_x=4;
	unsigned short position_y=4;
	unsigned short direction=0;
	unsigned short old_position_x = position_x;
	unsigned short old_position_y = position_y;
	unsigned short old_direction = direction;
	unsigned short rnd_dir = 0;

	struct PanelTbl move;
	signed short move_x,move_y;

	signed short frame_dx = 0;
	signed short frame_dy = 0;
	int frame;
	unsigned short move_table_index = 0;

	int imgnum=0;

	imgnum = 1;
	do
	{
		JpegLoad( 1,table[imgnum].source,    table[imgnum].offset, table[imgnum].size );

		ParamScreen.ImageWx=480;
		ParamScreen.ImageWy=272;
		ParamScreen.ImageOffset = table[imgnum].offset;//imgoffset[imgnum];
		ParamScreen.dot_depth=2;

		ParamScreen.div_Wx = 10;
		ParamScreen.div_Wy = 5;

		ParamScreen.blkWx = ParamScreen.ImageWx / ParamScreen.div_Wx;
		ParamScreen.blkWy = ParamScreen.ImageWy / ParamScreen.div_Wy;
		ParamScreen.BGcolor = BLACK;
		ParamScreen.BGClear = (CLR_COL | CLR_STN | CLR_TAG);
		ParamScreen.Alpha = 255;


		Trans( Init, &ParamScreen, NULL );

		position_x=5;
		position_y=4;
		direction=0;

		old_position_x = position_x;
		old_position_y = position_y;
		old_direction = direction;

		rnd_dir = 0;

		Screen[position_y][position_x].flag &= (~fDispOn);

		for( i=0;i<400;i++ )
		{
			do
			{
				rnd_dir = rand();
				rnd_dir = rand();
				rnd_dir = rand();
				old_position_x = position_x;
				old_position_y = position_y;
				frame_dx = 0;
				frame_dy = 0;
				direction = (rnd_dir) & 0x00000003;
				switch( direction )
				{
					case DIR_N:
					if( old_direction == DIR_S )continue;
					if( position_y > 0 )
					{
						position_y--;
						frame_dx = 0;
						frame_dy = 8;
					}
					break;
					case DIR_E:
					if( old_direction == DIR_W )continue;
					if( position_x < 9 )
					{
						position_x++;
						frame_dx = -6;
						frame_dy = 0;
					}
					break;
					case DIR_S:
					if( old_direction == DIR_N )continue;
					if( position_y < 4 )
					{
						position_y++;
						frame_dx = 0;
						frame_dy = -8;
					}
					break;
					case DIR_W:
					if( old_direction == DIR_E )continue;
					if( position_x > 0 )
					{
						position_x--;
						frame_dx = 6;
						frame_dy = 0;
					}
					break;
					default:
					break;
				}
			} while( frame_dx == 0 && frame_dy == 0 );
			old_direction = direction;
			move = Screen[old_position_y][old_position_x];
			Screen[old_position_y][old_position_x].offset = Screen[position_y][position_x].offset;
			Screen[old_position_y][old_position_x].flag = Screen[position_y][position_x].flag;
			Screen[position_y][position_x].offset = move.offset;
			Screen[position_y][position_x].flag = move.flag;
			moving_panel[i] = direction;
		}
		Trans( SetupStart, &ParamScreen, NULL );
		Trans( ScreenClean, &ParamScreen, NULL );
		Trans( ScreenBuild, &ParamScreen, NULL );
		Trans( TransStart, &ParamScreen, NULL );

		move_table_index=399;
		do
		{
			old_position_x = position_x;
			old_position_y = position_y;
			frame_dx = 0;
			frame_dy = 0;
			direction = moving_panel[move_table_index];
			switch( direction )
			{
				case DIR_S:
				if( position_y > 0 )
				{
					position_y--;
					frame_dx = 0;
					frame_dy = 8;
				}
				break;
				case DIR_W:
				if( position_x < 9 )
				{
					position_x++;
					frame_dx = -6;
					frame_dy = 0;
				}
				break;
				case DIR_N:
				if( position_y < 4 )
				{
					position_y++;
					frame_dx = 0;
					frame_dy = -8;
				}
				break;
				case DIR_E:
				if( position_x > 0 )
				{
					position_x--;
					frame_dx = 6;
					frame_dy = 0;
				}
				break;
				default:
				break;
			}
			if( frame_dx != 0 || frame_dy != 0 )
			{
				old_direction = direction;
			}
			move = Screen[position_y][position_x];
			move_x = move.x;
			move_y = move.y;
			Screen[old_position_y][old_position_x].flag &= (~fDispOn);
			Screen[position_y][position_x].flag &= (~fDispOn);
			for( frame = 1;frame < 6;frame++ )
			{
				Trans( SetupStart, &ParamScreen, NULL );
				Trans( ScreenClean, &ParamScreen, NULL );
				Trans( ScreenBuild, &ParamScreen, NULL );
				if( frame_dx != 0 || frame_dy != 0 )
				{
					move.x = move_x + (frame*frame_dx);
					move.y = move_y + (frame*frame_dy);
					Trans( SetupPartical, &ParamScreen, &move );
				}
				Trans( TransStart, &ParamScreen, NULL );
			}
			Screen[old_position_y][old_position_x].flag |= fDispOn;
			Screen[position_y][position_x].flag |= fDispOn;
			move = Screen[old_position_y][old_position_x];
			Screen[old_position_y][old_position_x].offset = Screen[position_y][position_x].offset;
			Screen[position_y][position_x].offset = move.offset;
		}while(move_table_index--);

		Trans( SetupStart, &ParamScreen, NULL );
		Trans( ScreenClean, &ParamScreen, NULL );
		Trans( ScreenBuild, &ParamScreen, NULL );
		Trans( TransStart, &ParamScreen, NULL );


		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);

		imgnum++;
	}while( imgnum < 4 );
}




















int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

	hri_pm_set_APBCMASK_SERCOM2_bit(PM);
	UserClockInit();
	SercomSPI_Init( SERCOM2, (SPI_CTRLA_MODE00|SPI_CTRLA_PINMODE00|SPI_CTRLA_MSB_FIRST|SPI_CTRLA_FORM_SPI|SPI_CTRLA_MASTER), (SPI_CTRLB_CHSIZE8BIT|SPI_CTRLB_RXEN),0 );
	GpioPortInit( &test2[0] );
	GpioPortInit( &test2[1] );
	GpioPortInit( &test2[2] );
	GpioPortInit( &test2[3] );
	SercomSPI_Enable( SERCOM2 );

	Wait100ms();
	Wait100ms();
	Wait100ms();
	Wait100ms();
	Wait100ms();
	Wait100ms();

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

	
	HostCommand(HOST_CLKEXT,0,0);//send command to "CLKEXT" to FT81X 
	HostCommand(HOST_CLKSEL,0,0);//send command to "CLKSEL" to FT81X 
	HostCommand(HOST_CLKINT,0,0);// 
	HostCommand(HOST_ACTIVE,0,0);//send host command "ACTIVE" to FT81X 
	HostMemWriteWord(REG_GPIOX_DIR, 0x8008); 
	HostMemWriteWord(REG_GPIOX, 0x8008); 

	Wait100ms();
	Wait100ms();
	Wait100ms();

	HostMemWriteWord(REG_GPIOX, 0x8000); 


	HostMemWriteByte(REG_CPURESET, 0x01);
	HostMemWriteWord(REG_CMD_READ, 0x0000);
	HostMemWriteWord(REG_CMD_WRITE, 0x0000);
	Wait100ms();
	HostMemWriteByte(REG_CPURESET, 0x00);

	FT8XX_LCD_Init( LCD_480x272 );



	/* write first display list */ 
	HostMemWriteDoubleWord( RAM_DL+0,DL_CLEAR_RGB );
	HostMemWriteDoubleWord( RAM_DL+4,DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG );
	HostMemWriteDoubleWord( RAM_DL+8,DL_DISPLAY );

	HostMemWriteByte(REG_DLSWAP, DLSWAP_FRAME);

	HostMemWriteByte(REG_PWM_DUTY, 128); 

	HostMemWriteByte(REG_PWM_DUTY, 64);

	HostMemWriteByte(REG_PCLK, 7);





	while(1)
	{

		Trans( SetupStart, NULL, NULL );
		Trans( ScreenClean, NULL, NULL );
		Trans( TransStart, NULL, NULL );

		FTDI_Logo();

		Trans( SetupStart, NULL, NULL );
		Trans( ScreenClean, NULL, NULL );
		Trans( TransStart, NULL, NULL );

		Raster_Logo();

		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);

		Trans( SetupStart, NULL, NULL );
		Trans( ScreenClean, NULL, NULL );
		Trans( TransStart, NULL, NULL );

		Puzzle_Demo();

		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);
		UserWait(100000);

	};

}




