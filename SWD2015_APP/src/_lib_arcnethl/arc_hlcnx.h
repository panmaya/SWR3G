/*
 * arc_hlxpdp.h
 *
 * Created: 13/11/2557 11:27:21
 *  Author: pan
 */ 


#ifndef ARC_HLXPDP_H_
#define ARC_HLXPDP_H_

#include <adl_global.h>
#include "ctrl/arc_hlaccess.h"

#define ARC_HLCNX_EV_SIM_WAIT		0
#define ARC_HLCNX_EV_CONFIG_OK		1
#define ARC_HLCNX_EV_CONFIG_KO		2

#define ARC_HLCNX_MAX_SESSION		3

typedef struct ARC_HL_CNXCFG_T {
	char apn[33];
	char login[13];
	char password[13];
	char ip[16];
	char dns1[16];
	char dns2[16];
	bool is_sustained;
	bool is_activated;
}arc_hlcnxcfg_t;

/* The name for each tasks. */
#define ARC_HLCNX_TASK_NAME						(const signed char *const) "arc_hlCnx"
/* The stack sizes allocated to the various tasks. */
#define ARC_HLCNX_TASK_STACK_SIZE				((configMINIMAL_STACK_SIZE) + 590)
/* The priorities at which various tasks will get created. */
#define ARC_HLCNX_TASK_PRIORITY					ARC_HL_ACCESS_TASK_PIORITY

arc_hlcnxcfg_t * arc_hlcnx_GetConfig(uint8_t cnxid);
int8_t arc_hlcnx_SetConfig(uint8_t cnxid, int opt, ...);
void arc_hlcnx_Init(uint32_t*, xTaskHandle *);




#endif /* ARC_HLXPDP_H_ */