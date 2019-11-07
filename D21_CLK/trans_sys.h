#ifndef TRANS_SYS_H_
#define TRANS_SYS_H_


#define fDispOn 0x0001
#define fDispOff 0x0000


struct PanelTbl
{
	signed short x;
	signed short y;
	unsigned long wx;
	unsigned long wy;
	unsigned long offset;
	unsigned short flag;
};

struct ScreenConfig
{
	unsigned short ImageWx;
	unsigned short ImageWy;
	unsigned long ImageOffset;
	unsigned char dot_depth;

	unsigned short div_Wx;
	unsigned short div_Wy;
	unsigned short blkWx;
	unsigned short blkWy;

	unsigned long BGcolor;
	unsigned long BGClear;
	unsigned long Alpha;
};

typedef enum 
{
	Init=0,
	SetupStart,
	ScreenClean,
	Logo,
	ScreenBuild,
	SetupPartical,
	TransStart,
	TransSync,
}TransCmd;



extern struct PanelTbl Screen[17][30];
extern struct ScreenConfig ParamScreen;

extern unsigned char moving_panel[];
extern signed short raster[];



#endif

