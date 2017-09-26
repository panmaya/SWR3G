/*
 * arc_hlftp.h
 *
 * Created: 10/6/2558 17:34:41
 *  Author: pan
 */ 


#ifndef ARC_HLFTP_H_
#define ARC_HLFTP_H_


#include <adl_global.h>
#include "ctrl/arc_hlaccess.h"
#include "var/arc_hlnet.h"

#define ARC_HLFTP_MAX_SESSION					2

/* The name for each tasks. */
#define ARC_HLFTP_TASK_NAME						(const signed char *const) "arc_hlFtp"
/* The stack sizes allocated to the various tasks. */
#define ARC_HLFTP_TASK_STACK_SIZE				((configMINIMAL_STACK_SIZE) + 730)
/* The priorities at which various tasks will get created. */
#define ARC_HLFTP_TASK_PRIORITY					ARC_HL_ACCESS_TASK_PIORITY

typedef enum ARC_HLFTP_MODE_E{
	ARC_HLFTP_MODE_CLIENT,
	ARC_HLFTP_MODE_SERVER,
}arc_hlftp_mode_e;

typedef void (*arc_hlftp_fCbk)(uint8_t conf_id, int8_t event, void *ctx);

int8_t arc_hlftp_SetConfig(uint8_t conf_id, int opt, ...);
int8_t arc_hlftp_Dota(uint8_t conf_id, const char* name, const char* path);
int8_t arc_hlftp_Subscribe(arc_hlftp_fCbk);

#endif /* ARC_HLFTP_H_ */