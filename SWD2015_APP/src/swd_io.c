/*
 * swd_io.c
 *
 * Created: 2016-10-21 3:11:44 PM
 *  Author: Panmaya
 */ 
#include "adl_global.h"
#include "misc_config.h"
#include "apt_tracking.h"
#include "ini_config.h"
#include "buffer.h"
#include "trans_form.h"
#include "distance_calc.h"
#include "apt_gateway.h"
#include "feature.h"
#include "swd_event.h"
#include "_lib_gps/nmea_gps.h"
#include "_lib_gps/nmea.h"
#include "apt_swd.h"
#include "apt_gps_data.h"
#include "vprinter.h"
#include "swd_io.h"



#define	EXT_IG_IDX				0
#define	EXT_IN1_IDX				1
#define	EXT_IN2_IDX				2
#define	EXT_CLRBUF_IDX			3
#define	EXT_OPT1_IDX			4
#define	LED_OPT_IDX				5
#define	LED_PTO_IDX				6
#define	LED_IG_IDX				7
#define	EXT_OUT1_IDX			8
#define	EXT_OUT2_IDX			9
#define	LAST_IDX				(EXT_OUT2_IDX+1)

static s32	swdio_hio = ERROR;
static adl_ioDefs_t swdio_iodef[LAST_IDX] = {	(ADL_IO_GPIO|ADL_IO_DIR_IN |EXT_IG)/* IN0(IG)   */,
	(ADL_IO_GPIO|ADL_IO_DIR_IN |EXT_IN1)/* IN1/PTO1       */,
	(ADL_IO_GPIO|ADL_IO_DIR_IN |EXT_IN2)/* IN2/PTO2/OD_SPEED */,
	(ADL_IO_GPIO|ADL_IO_DIR_IN |EXT_CLRBUF)/*CLEAR BUFFER*/,
	(ADL_IO_GPIO|ADL_IO_DIR_OUT|ADL_IO_LEV_LOW|EXT_OPT1),
	(ADL_IO_GPIO|ADL_IO_DIR_OUT|ADL_IO_LEV_LOW|LED_OPT),
	(ADL_IO_GPIO|ADL_IO_DIR_OUT|ADL_IO_LEV_LOW|LED_PTO),
	(ADL_IO_GPIO|ADL_IO_DIR_OUT|ADL_IO_LEV_LOW|LED_IG),
	(ADL_IO_GPIO|ADL_IO_DIR_OUT|ADL_IO_LEV_LOW|EXT_OUT1)/* OUTPUT CH1 */,
	(ADL_IO_GPIO|ADL_IO_DIR_OUT|ADL_IO_LEV_LOW|EXT_OUT2)/* OUTPUT CH2 */,
};


void swdio_light_ig_set(bool status) { adl_ioWriteSingle (swdio_hio, &swdio_iodef[LED_IG_IDX] ,status);
}

void swdio_light_pto_set(bool status) { adl_ioWriteSingle (swdio_hio, &swdio_iodef[LED_PTO_IDX] ,status);
}

void swdio_light_opt_set(bool status) { adl_ioWriteSingle (swdio_hio, &swdio_iodef[LED_OPT_IDX] ,status);
}

u8 swdio_get_ig_status(void) { return !adl_ioReadSingle (swdio_hio, &swdio_iodef[EXT_IG_IDX]);
}

u8 swdio_get_pto1_status(void) { return !adl_ioReadSingle (swdio_hio, &swdio_iodef[EXT_IN1_IDX]);
}

u8 swdio_get_pto2_status(void) { return !adl_ioReadSingle (swdio_hio, &swdio_iodef[EXT_IN2_IDX]);
}

uint32_t swdio_get_pto2_freq(void) { return adl_ioReadFreq(swdio_hio, &swdio_iodef[EXT_IN2_IDX]);
}

u8 swdio_get_clrbuf_status(void) { return adl_ioReadSingle (swdio_hio, &swdio_iodef[EXT_CLRBUF_IDX]);
}

void swdio_set_outp1_status(bool status) { adl_ioWriteSingle (swdio_hio, &swdio_iodef[EXT_OUT1_IDX],status);
}

void swdio_set_outp2_status(bool status) { adl_ioWriteSingle (swdio_hio, &swdio_iodef[EXT_OUT2_IDX],status);
}

u8 swdio_get_logic_pto1(void) { return adl_ioReadSingle (swdio_hio, &swdio_iodef[EXT_IN1_IDX]);
}

u8 swdio_get_logic_pto2(void) { return adl_ioReadSingle (swdio_hio, &swdio_iodef[EXT_IN2_IDX]);
}

u8 swdio_get_logic_ig(void) { return adl_ioReadSingle (swdio_hio, &swdio_iodef[EXT_IG_IDX]);
}

void swdio_init(void) {
	swdio_hio = adl_ioSubscribe(LAST_IDX, swdio_iodef, 0, 0, 0);
}