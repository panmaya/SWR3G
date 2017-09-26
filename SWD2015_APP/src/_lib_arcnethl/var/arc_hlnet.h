/*
 * arc_hlx.h
 *
 * Created: 13/11/2557 11:25:19
 *  Author: pan
 */ 


#ifndef ARC_HLX_H_
#define ARC_HLX_H_

#include <adl_global.h>

#define ARC_HLNET_EOF_PATTERN				"--EOF--Pattern--"
#define ARC_HLNET_OK_RESP_STR				"\r\nOK\r\n"
#define ARC_HLNET_TCP_EOF_STR				ARC_HLNET_EOF_PATTERN"\r\nOK\r\n"
#define ARC_HLNET_CONNECT_STR				"\r\nCONNECT\r\n"
#define ARC_HLNET_UNS_MAX_STR				"\r\n+KTCP_DATA: xxx,yyyyy\r\n+KTCP_NOTIF: aaa,bbbb\r\n"

#define	ARC_HLNET_EOF_PATTERN_LEN			16
#define	ARC_HLNET_CONNECT_STR_LEN			11
#define	ARC_HLNET_OK_RESP_STR_LEN			6
#define	ARC_HLNET_UNS_MAX_STR_LEN			48
#define	ARC_HLNET_TCP_EOF_STR_LEN			(ARC_HLNET_EOF_PATTERN_LEN + ARC_HLNET_OK_RESP_STR_LEN)


#define ARC_HLNET_TCPRECV_MIN_BUFF			( ARC_HLNET_CONNECT_STR_LEN  + \
											  ARC_HLNET_EOF_PATTERN_LEN  + \
											  ARC_HLNET_OK_RESP_STR_LEN  + \
											  ARC_HLNET_UNS_MAX_STR_LEN )
									
									
#define ARC_HLNET_STR_DEBUG(_s)		do{ if(!vPrintTraceGet(13))break;\
										vPutsTrace(13,_s);\
									}while(0)
									
#define ARC_HLNET_STRN_DEBUG(_s,_n)	do{	if(!vPrintTraceGet(13))break;\
										if(_n) {\
											uint8_t _i = _n > 32 ? 32 : _n;\
											char p[33];\
											char *c = p;\
											memcpy(p,_s,_i);\
											p[ _i ] = 0;\
											for( ;*c;c++) { switch(*c) {case '\r': case '\n': *c='.'; break;} }\
											vPutsTrace(13,p);\
										}\
									}while(0)

#define ARC_HLNET_EV_CONNECTED		1
#define ARC_HLNET_EV_READ			2
#define ARC_HLNET_EV_RELEASE		3
#define ARC_HLNET_EV_ALIVE			4
#define ARC_HLNET_EV_TIMEOUT		5


#define ARC_HLNET_OPT_END			0
#define ARC_HLNET_OPT_CNXID			1
#define ARC_HLNET_OPT_MODE			2
#define ARC_HLNET_OPT_ADDR			3
#define ARC_HLNET_OPT_PORT			4
#define ARC_HLNET_OPT_SRCPORT		5
#define ARC_HLNET_OPT_DATAMODE		6
#define ARC_HLNET_OPT_URCTCP		7
#define ARC_HLNET_OPT_USER			8
#define ARC_HLNET_OPT_PASS			9
#define ARC_HLNET_OPT_APN			10
#define ARC_HLNET_OPT_IP			11
#define ARC_HLNET_OPT_DNS1			12
#define ARC_HLNET_OPT_DNS2			13
#define ARC_HLNET_OPT_SUSTAINED		14
#define ARC_HLNET_OPT_PASSIVE		15
#define ARC_HLNET_OPT_FTYPE			16

#define ARC_HLNET_EV_ERR_BASE		20

#define ARC_HLNET_EV_NETW_ERR		ARC_HLNET_EV_ERR_BASE + 0
#define ARC_HLNET_EV_MAX_SOCK		ARC_HLNET_EV_ERR_BASE + 1
#define ARC_HLNET_EV_MEM_ERR		ARC_HLNET_EV_ERR_BASE + 2
#define ARC_HLNET_EV_DNS_ERR		ARC_HLNET_EV_ERR_BASE + 3
#define ARC_HLNET_EV_PEER_CLOSE		ARC_HLNET_EV_ERR_BASE + 4
#define ARC_HLNET_EV_CONN_ERR		ARC_HLNET_EV_ERR_BASE + 5
#define ARC_HLNET_EV_NORMAL_ERR		ARC_HLNET_EV_ERR_BASE + 6
#define ARC_HLNET_EV_ACCEPT_ERR		ARC_HLNET_EV_ERR_BASE + 7
#define ARC_HLNET_EV_SEND_WAIT		ARC_HLNET_EV_ERR_BASE + 8
#define ARC_HLNET_EV_BAD_SSID		ARC_HLNET_EV_ERR_BASE + 9
#define ARC_HLNET_EV_SSID_RUNING	ARC_HLNET_EV_ERR_BASE + 10
#define ARC_HLNET_EV_SSID_FULLY		ARC_HLNET_EV_ERR_BASE + 11


//Compatible ARC_NET
typedef struct __ARC_NET_SYSEM_T__ {
	bool bDeiveInitOK;
	bool bSimReady;
	bool bGsmRegistered;
	bool bGprsAttached;
	bool bPdpActivated;
	bool b3GRegistered;
	bool bNetworkRoaming;
	uint8_t   modemAppVersion;
	uint16_t  wNbSocketUsed;
	uint16_t signLev;
}arc_net_sys_t;

typedef struct __ARC_NET_STATE_T__ {
	int8_t	sim;
	int8_t	gprs;
	int8_t	bearer;
	int8_t	tcpc;
	int8_t	ftpc;
}arc_netwatcher_t;

// global extern
extern  uint32_t arc_netdev_noresponse;
extern arc_net_sys_t arc_netsys;
extern arc_netwatcher_t	arc_netwatcher;
extern int32_t arc_netbearer_start_staus;
extern char arc_netip[16];
extern char arc_netimei[16];

s32 arc_netInit (u8 nbSockets);

#endif /* ARC_HLX_H_ */