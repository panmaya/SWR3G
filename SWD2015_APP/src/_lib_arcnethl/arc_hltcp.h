/*
 * arc_hlxtcp.h
 *
 * Created: 13/11/2557 14:24:21
 *  Author: pan
 */ 


#ifndef ARC_HLXTCP_H_
#define ARC_HLXTCP_H_

#include <adl_global.h>
#include "ctrl/arc_hlaccess.h"
#include "var/arc_hlnet.h"

#define ARC_HLTCP_MAX_SESSION					4

/* The name for each tasks. */
#define ARC_HLTCP_TASK_NAME						(const signed char *const) "arc_hlTcp"
/* The stack sizes allocated to the various tasks. */
#define ARC_HLTCP_TASK_STACK_SIZE				((configMINIMAL_STACK_SIZE) + 1300)
/* The priorities at which various tasks will get created. */
#define ARC_HLTCP_TASK_PRIORITY					ARC_HL_ACCESS_TASK_PIORITY


typedef enum ARC_HLTCP_MODE_E{
	ARC_HLTCP_MODE_CLIENT,
	ARC_HLTCP_MODE_SERVER,
	ARC_HLTCP_MODE_CHILD,	
}arc_hltcp_mode_e;

typedef struct ARC_HLTCP_CTX_T {
	uint16_t length;
	uint8_t  data[1];
}arc_hltcp_ctx_t;

typedef void (*arc_hltcp_fCbk)(uint8_t conf_id, int8_t event, void *ctx);

bool arc_hltcp_isConnected(uint8_t conf_id);

int8_t arc_hltcp_Send(uint8_t conf_id, uint16_t size, uint8_t * data);
int8_t arc_hltcp_SetConfig(uint8_t conf_id, int opt, ...);
int8_t arc_hltcp_ReStart(uint8_t conf_id);
int8_t arc_hltcp_Start(uint8_t conf_id, uint32_t*, xTaskHandle *);
int8_t arc_hldms_Start(uint8_t conf_id);
int8_t arc_hltcp_Stop(uint8_t conf_id);
int8_t arc_hltcp_Subscribe(arc_hltcp_fCbk);

/* Compatible code with arc_net */
#define ARC_NET_EVENT_RELEASE		ARC_HLNET_EV_RELEASE
#define ARC_NET_EVENT_PEER_CLOSE	ARC_HLNET_EV_PEER_CLOSE

#define ARC_TCPCLIENT_CONNECTED		1
#define ARC_TCPCLIENT_DISCONNECT	0

#endif /* ARC_HLXTCP_H_ */