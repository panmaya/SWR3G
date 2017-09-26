/*
 * arc_hlgsm.h
 *
 * Created: 20/2/2558 16:37:29
 *  Author: pan
 */ 


#ifndef ARC_HLGSM_H_
#define ARC_HLGSM_H_

#include <adl_global.h>
#include "ctrl/arc_hlaccess.h"

/* The name for each tasks. */
#define ARC_HLGSM_AT_TASK_NAME					(const signed char *const) "arc_hlGsm_AT"
#define ARC_HLGSM_SMS_TASK_NAME					(const signed char *const) "arc_hlGsm_SMS"
/* The stack sizes allocated to the various tasks. */
#define ARC_HLGSM_AT_TASK_STACK_SIZE			((configMINIMAL_STACK_SIZE) + 220 + 256)
#define ARC_HLGSM_SMS_TASK_STACK_SIZE			((configMINIMAL_STACK_SIZE) + 280 + 256 + 64 + 32)
/* The priorities at which various tasks will get created. */
#define ARC_HLGSM_SMSTASK_PRIORITY				ARC_HL_ACCESS_TASK_PIORITY
#define ARC_HLGSM_CMDTASK_PRIORITY				ARC_HL_ACCESS_TASK_PIORITY

void arc_hlgsm_cmdInit(uint32_t*, xTaskHandle *);
void arc_hlgsm_smsInit(uint32_t*, xTaskHandle *);



#endif /* ARC_HLGSM_H_ */