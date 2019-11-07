//#include <asf.h>
#include <samd21g18a.h>
#include <atmel_start.h>

#include <stdbool.h>

#include "user_gpio.h"
#include "user_spi.h"
#include "user_clock.h"






















static inline bool system_gclk_is_syncing(void)
{
	if (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY){
		return true;
	}

	return false;
}

/*
typedef struct
{
	GCLKGEN id;
	GCLKSRC src;
	unsigned long flag;
	unsigned short div;
}GCLKGENCTRL;
*/

void GClkGenInit( GCLKGENCTRL *tbl )
{
	unsigned long genctrl = 0;
	unsigned long gendiv = 0;

	genctrl = ((tbl->src << USE_GCLKSRC_SHIFT) | tbl->id) | tbl->flag ;
	gendiv = ((tbl->div<<USE_GCLKDIV_SHIFT)|tbl->id);


	while (system_gclk_is_syncing()) {
		/* Wait for synchronization */
	};

	system_interrupt_enter_critical_section();

	GCLK->GENDIV.reg = gendiv;

	while (system_gclk_is_syncing()) {
		/* Wait for synchronization */
	};
	GCLK->GENCTRL.reg = genctrl;

	system_interrupt_leave_critical_section();
}


/*
typedef struct MODULECLOCK
{		
	MODULE_CLK_ID id;
	GCLKGEN src;
	unsigned short flag;
}MduleClock;
*/


void ModuleClkInit( MduleClock *tbl )
{
	unsigned short clkctrl=0;

	clkctrl = ((tbl->src<<USE_GCLKGEN_SHIFT) | tbl->id) | tbl->flag;

//	system_interrupt_enter_critical_section();

	GCLK->CLKCTRL.reg = clkctrl;

//	system_interrupt_leave_critical_section();
}


