#include <samd21g18a.h>
#include <stdbool.h>

#include "user_clock.h"
#include "user_misc.h"







GCLKGENCTRL GclkCtrlTbl[]=
{
	{	USE_GCLKGEN00,GCLK_SRC_OSC8M,		GCLK_CTRL_ENABLE	,1},
	{	USE_GCLKGEN01,GCLK_SRC_DFLL48M,		GCLK_CTRL_ENABLE	,1},
	{	USE_GCLKGEN02,GCLK_SRC_OSCULP32K,	GCLK_CTRL_ENABLE	,1},
	{	USE_GCLKGEN03,GCLK_SRC_XOSC,		0	,1},
	{	USE_GCLKGEN04,GCLK_SRC_XOSC,		0	,1},
	{	USE_GCLKGEN05,GCLK_SRC_XOSC,		0	,1},
	{	USE_GCLKGEN06,GCLK_SRC_XOSC,		0	,1},
	{	USE_GCLKGEN07,GCLK_SRC_XOSC,		0	,1},
	{	USE_GCLKGEN08,GCLK_SRC_XOSC,		0	,1},
    {   USE_GCLKGEN_ENDMARK        ,        0   ,0},
};








const MduleClock ModuleClkTbl[]=
{
//	{	MODULE_CLK_DFLL48M_REF,	USE_GCLKGEN00,	USE_CLKCTRL_CLKEN	},
//	{	MODULE_CLK_DPLL,		USE_GCLKGEN00,	0	},
//	{	MODULE_CLK_DPLL_32K,	USE_GCLKGEN00,	0	},
	{	MODULE_CLK_WDT,			USE_GCLKGEN02,	0	},
	{	MODULE_CLK_RTC,			USE_GCLKGEN00,	0	},
	{	MODULE_CLK_EIC,			USE_GCLKGEN00,	0	},
	{	MODULE_CLK_USB,			USE_GCLKGEN00,	0	},
	{	MODULE_CLK_EVSYS_CH00,	USE_GCLKGEN00,	0	},
	{	MODULE_CLK_EVSYS_CH01,	USE_GCLKGEN00,	0	},
	{	MODULE_CLK_EVSYS_CH02,	USE_GCLKGEN00,	0	},
	{	MODULE_CLK_EVSYS_CH03,	USE_GCLKGEN00,	0	},
	{	MODULE_CLK_EVSYS_CH04,	USE_GCLKGEN00,	0	},
	{	MODULE_CLK_EVSYS_CH05,	USE_GCLKGEN00,	0	},
	{	MODULE_CLK_SERCOMX,		USE_GCLKGEN00,	USE_CLKCTRL_CLKEN	},
	{	MODULE_CLK_SERCOM0,		USE_GCLKGEN00,	USE_CLKCTRL_CLKEN	},
	{	MODULE_CLK_SERCOM1,		USE_GCLKGEN00,	USE_CLKCTRL_CLKEN	},
	{	MODULE_CLK_SERCOM2,		USE_GCLKGEN01,	USE_CLKCTRL_CLKEN	},
	{	MODULE_CLK_SERCOM3,		USE_GCLKGEN01,	USE_CLKCTRL_CLKEN	},
	{	MODULE_CLK_TCC_0_1,		USE_GCLKGEN00,	0	},
	{	MODULE_CLK_TCC_2_3,		USE_GCLKGEN00,	0	},
	{	MODULE_CLK_ADC,			USE_GCLKGEN00,	0	},
	{	MODULE_CLK_AC_DIG,		USE_GCLKGEN00,	0	},
	{	MODULE_CLK_AC_ANA,		USE_GCLKGEN00,	0	},
	{	MODULE_CLK_DAC,			USE_GCLKGEN00,	0	},
	{	MODULE_CLK_PTC,			USE_GCLKGEN00,	0	},

	{	MODULE_CLK_ENDMARK,			        0,	0	},
    
};



const MduleClock ModuleClkTblSERCOM2=
{
	MODULE_CLK_SERCOM2,		USE_GCLKGEN00,	USE_CLKCTRL_CLKEN,
};







void UserWait( volatile unsigned long time )
{
	while( time-- )
	{
		
	};
}



void Wait100ms( void )
{
	UserWait(100000);
	UserWait(100000);
	UserWait(100000);
	UserWait(70000);
}

void Wait10ms( void )
{
	UserWait(10000);
	UserWait(10000);
	UserWait(10000);
	UserWait(7000);
}

void Wait1ms( void )
{
	UserWait(1000);
	UserWait(1000);
	UserWait(1000);
	UserWait(700);
}








void UserClockInit(void)
{
	int i;

/*
	i=0;
    while( GclkCtrlTbl[i].id != USE_GCLKGEN_ENDMARK )
	{
		GClkGenInit( &GclkCtrlTbl[i] );
	}
*/

	ModuleClkInit( &ModuleClkTblSERCOM2 );
/*
	i=0;
    while( ModuleClkTbl[i].id != MODULE_CLK_ENDMARK )
	{
		ModuleClkInit( &ModuleClkTbl[i] );
        i++;
	}
*/
}


