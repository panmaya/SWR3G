/*
 * error.c
 *
 * Created: 2016-10-21 10:14:39 AM
 *  Author: Panmaya
 */ 
#include <asf.h>
#include "adl_global.h"
#include "_hw_ctrl/hw_ctrl.h"
#include <string.h>
#include "error.h"

 adl_InitType_e error_init_by(void) {
	 uint32_t info = rstc_get_status(RSTC);
	 adl_InitType_e starttype = ADL_INIT_POWER_ON;
	 switch (info & RSTC_SR_RSTTYP_Msk) {
		 case RSTC_BACKUP_RESET:
		 case RSTC_GENERAL_RESET:
		 case RSTC_USER_RESET:
		 case RSTC_SOFTWARE_RESET:
		 case RSTC_WATCHDOG_RESET:
		 default:
		 starttype = ADL_INIT_REBOOT_FROM_EXCEPTION;
		 break;
	 }
	 return starttype;
}

char* error_reset_reason(char sDisplay[60]) {
	uint32_t info = rstc_get_status(RSTC);
	
	strcpy(sDisplay,"POWER-ON: ");
	switch (info & RSTC_SR_RSTTYP_Msk) {
		case RSTC_BACKUP_RESET:
		strcat(sDisplay,"BACKUP_RESET");
		break;
		case RSTC_GENERAL_RESET:
		strcat(sDisplay,"GENERAL_RESET");
		break;
		case RSTC_USER_RESET:
		strcat(sDisplay,"USER_RESET");
		break;
		case RSTC_SOFTWARE_RESET:
		strcat(sDisplay,"SOFTWARE_RESET");
		break;
		case RSTC_WATCHDOG_RESET:
		strcat(sDisplay,"WATCHDOG_RESET");
		break;
		default:
		strcat(sDisplay,"INVALID_RESET");
		break;
	}
	
	/* NRST level. */
	if (info & RSTC_SR_NRSTL)strcat(sDisplay, " NRST=1,");
	else strcat(sDisplay, " NRST=0,");

	/* User reset status. */
	if (info & RSTC_SR_URSTS) strcat(sDisplay, " User Reset=1\r\n\r\n");
	else strcat(sDisplay, " User Reset=0\r\n\r\n");

	return sDisplay;
}

void error_halt_swReset(const char * file, uint32_t line) {
	taskENTER_CRITICAL();
	//f_mount(0, NULL);//un_mount fatfs
	f_mount(NULL, "0:",0);//un_mount fatfs
	printf("\r\n\r\n Soft-Reset: %s, Ln %lu\r\n",file,line);
	taskEXIT_CRITICAL();
	rstc_start_software_reset(RSTC);
}


