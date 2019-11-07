
#include <samd21g18a.h>
#include <stdbool.h>
//#include "user_gpio.h"
//#include "user_spi.h"
#include "ft8xx.h"
#include "trans_sys.h"

#include <atmel_start.h>
//#include <samd21g18a.h>




struct PanelTbl Screen[17][30];
struct ScreenConfig ParamScreen;

unsigned char moving_panel[400];
signed short raster[300];




int Trans( int cmd, struct ScreenConfig *param, struct PanelTbl *partical )
{
	int i,j;
	struct PanelTbl *panel;

	switch( cmd )
	{
		case Init:
			for( i=0;i<param->div_Wy;i++ )
			{
				for( j=0;j<param->div_Wx;j++ )
				{
					Screen[i][j].x = j*param->blkWx;
					Screen[i][j].y = i*param->blkWy;
					Screen[i][j].wx = param->blkWx;
					Screen[i][j].wy = param->blkWy;
					Screen[i][j].offset = param->ImageOffset + i*(param->ImageWx*param->dot_depth)*param->blkWy +(j*param->blkWx*param->dot_depth);
					Screen[i][j].flag = fDispOn;
				}
			}
			break;
		case SetupStart:
			RamCmdFlash();
			RamMemWriteDoubleWord( (CMD_DLSTART) );
			break;
		case ScreenClean:
			if( param != NULL )
			{
				RamMemWriteDoubleWord( (DL_CLEAR_RGB | param->BGcolor) );
				RamMemWriteDoubleWord( (DL_CLEAR | param->BGClear));
			}
			else
			{
				RamMemWriteDoubleWord( (DL_CLEAR_RGB | BLACK) );
				RamMemWriteDoubleWord( ( DL_CLEAR | (CLR_COL | CLR_STN | CLR_TAG) ) );
			}

			break;
		case Logo:
			RamMemWriteDoubleWord( (CMD_LOGO) );
			break;
		case ScreenBuild:
			for( i=0;i<param->div_Wy;i++ )
			{
				for( j=0;j<param->div_Wx;j++ )
				{
					panel = &Screen[i][j];
					if( !(panel->flag & fDispOn) )continue;

					RamMemWriteDoubleWord( BITMAP_HANDLE(7) );
					RamMemWriteDoubleWord( BITMAP_SOURCE( RAM_G+panel->offset ) );
					RamMemWriteDoubleWord( BITMAP_LAYOUT(RGB565, param->ImageWx*param->dot_depth, param->ImageWy) );
					RamMemWriteDoubleWord( BITMAP_SIZE(NEAREST, 0, 0, panel->wx, panel->wy) );
					RamMemWriteDoubleWord( BEGIN_BITMAPS );
					RamMemWriteDoubleWord( VERTEX2F( j*panel->wx*16 , i*panel->wy*16 ) );
					RamMemWriteDoubleWord( DL_END );
				}
			}
			break;
		case SetupPartical:
			RamMemWriteDoubleWord( BITMAP_HANDLE(7) );
			RamMemWriteDoubleWord( BITMAP_SOURCE( RAM_G + partical->offset ) );
			RamMemWriteDoubleWord( BITMAP_LAYOUT(RGB565, param->ImageWx*param->dot_depth, param->ImageWy) );
			RamMemWriteDoubleWord( BITMAP_SIZE(NEAREST, 0, 0, partical->wx, partical->wy) );
			RamMemWriteDoubleWord( BEGIN_BITMAPS );
			RamMemWriteDoubleWord( VERTEX2F( partical->x*16 , partical->y*16 ) );
			RamMemWriteDoubleWord( DL_END );
			break;
		case TransStart:
			RamMemWriteDoubleWord( (DL_DISPLAY) );
			RamMemWriteDoubleWord( (CMD_SWAP) );
			RamCmdTrans();
			break;
		case TransSync:
			WaitCmdfifo_empty();
			break;
	}
	return 0;
}



