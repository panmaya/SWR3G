/*
 * adl_sim.c
 *
 * Created: 31/10/2557 10:39:05
 *  Author: pan
 */ 
#include <adl_global.h>

adl_simState_e adl_simGetState ( void ) {
	return ADL_SIM_STATE_REMOVED;
}

s32 adl_simSubscribe ( adl_simHdlr_f SimHandler, ascii * PinCode ) {
	return OK;
}