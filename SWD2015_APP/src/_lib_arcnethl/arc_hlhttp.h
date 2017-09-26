/*
 * arc_hlhttp.h
 *
 * Created: 7/1/2016 11:34:13 AM
 *  Author: Panmaya
 */ 


#ifndef ARC_HLHTTP_H_
#define ARC_HLHTTP_H_


#include <adl_global.h>
#include "ctrl/arc_hlaccess.h"
#include "var/arc_hlnet.h"

/* The name for each tasks. */
#define ARC_HLHTTP_TASK_NAME					(const signed char *const) "arc_hlHttp"
/* The stack sizes allocated to the various tasks. */
#define ARC_HLHTTP_TASK_STACK_SIZE				((configMINIMAL_STACK_SIZE) + 730)
/* The priorities at which various tasks will get created. */
#define ARC_HLHTTP_TASK_PRIORITY				ARC_HL_ACCESS_TASK_PIORITY


typedef void (*arc_hlhttp_fCbk)(int8_t event, void *ctx);

int8_t arc_hlhttp_Browse(uint16_t cnxid, const char * url);
int8_t arc_hlhttp_Subscribe(arc_hlhttp_fCbk);



#endif /* ARC_HLHTTP_H_ */