/*
 * arc_hlx.c
 *
 * Created: 13/11/2557 14:12:30
 *  Author: pan
 */ 
#include "arc_hlnet.h"

/*
- Netstat Variable
- Structure has externed in arc_net.h
*/
/*extern*/arc_net_sys_t 		arc_netsys;
/*extern*/arc_netwatcher_t		arc_netwatcher;
/*extern*/s32					arc_netbearer_start_staus;
/*extern*/char					arc_netip[16] = {"0.0.0.0"};
/*extern*/char					arc_netimei[16]={"000000000000000"};
/*extern*/uint32_t				arc_netdev_noresponse = 0;

s32 arc_netInit (u8 nbSockets)
{
	arc_netsys.bDeiveInitOK		= FALSE;
	arc_netsys.bGprsAttached	= FALSE;
	arc_netsys.bPdpActivated	= FALSE;
	arc_netsys.bSimReady		= FALSE;
	arc_netsys.modemAppVersion	= 0;
	arc_netsys.b3GRegistered	= FALSE;
	arc_netsys.bNetworkRoaming  = FALSE;
	arc_netsys.bGsmRegistered	= FALSE;
	arc_netsys.wNbSocketUsed	= 0;
	arc_netsys.signLev			= 0;
	arc_netwatcher.gprs			= -1;
	arc_netwatcher.bearer		= -1;
	arc_netwatcher.sim			= -1;
	arc_netwatcher.tcpc			= -1;
	arc_netdev_noresponse		= 0;
	
	return 0;
}