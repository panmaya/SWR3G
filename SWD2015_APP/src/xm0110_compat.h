/*
 * xm0110_compat.h
 *
 * Created: 22/6/2558 15:36:38
 *  Author: pan
 */ 


#ifndef XM0110_COMPAT_H_
#define XM0110_COMPAT_H_

/* Error codes    */
typedef enum
{
	GPS_OK = 0,
	// Generic error codes
	GPS_ERR_BAD_STATE = -1,
	GPS_ERR_STATE_ALREADY = -2,
	GPS_ERR_INVALID_PARAMETER = -3,
	GPS_ERR_NOT_AVAILABLE = -4,
	GPS_ERR_STATE_TRANSITION = -5,
	// Error codes from initialization
	GPS_ERR_PORTING_LAYER_INIT = -10,
	GPS_ERR_INIT = -11,
	GPS_ERR_IO_INIT = -12,
	GPS_ERR_BUS_INIT = -13,
	GPS_ERR_SCHED_INIT = -14,
	GPS_ERR_CORE_INIT = -15,
	GPS_ERR_NV_MEMORY_INIT = -16,
	// Error code
	GPS_ERR_SCHED_TASK = - 20,
	GPS_ERR_BUS = - 21,
	GPS_ERR_IO_MNGT = -22,
	GPS_ERR_CORE_LIB = -23,
	GPS_ERR_NV_DATA_ACCESS = -24,
	GPS_ERR_INTERNAL = -25,
	GPS_ERR_SERVICE = -26,
	// Error code for GPS service
	GPS_ERR_GPS_POS_NOT_FIXED = -30,
	// Error code for GPS abort event
	GPS_ERR_ABORT_INTERNAL = -40,
	GPS_ERR_ABORT_NMEA = -41,
	GPS_ERR_ABORT_RESET = -42
}gps_status_e;

/* Plug-In state machine */
typedef enum
{
	GPS_UNINITIALIZED= 0,
	GPS_INITIALIZED,
	GPS_RUNNING,
	GPS_SLEEP
} gps_state_e;


// The gps_busTypeID_e enumeration defines the bus type identifier on which the GPS companion chip is connected
typedef enum
{
	GPS_BUS_UART = 1,
	GPS_BUS_I2C = 2
} gps_busTypeID_e;

// The gps_busNumberID_e enumeration defines the bus number identifier of the bus on which the GPS companion chip is connected.
typedef enum
{
	GPS_BUS_NUM_1 = 1,
	GPS_BUS_NUM_2 = 2
} gps_busNumberID_e;


// Hardware type on which the GPS Plug-In initialization is applied.
typedef enum
{
	GPS_HWTYPE_XM0110 = 1,
	GPS_HWTYPE_NUMBER
} gps_hwType_e;


// The gps_32kMngt_e enumeration defines the management of 32 KHz clock.
typedef enum
{
	GPS_32K_NONE= 0,
	GPS_32K_INTERNAL,
	GPS_32K_EXTERNAL
} gps_32kMngt_e;

/*In case the application intends to manage GPS Hardware power up/power down on its own (i.e., without using gps_start()
and gps_stop ()), then the GPIO member of the gps_ioConfigXXXX_t structures shall be assigned the value GPS_GPIO_NOT_MANAGED.
The other GPIO members of the gps_ioConfigXXXX_t structures shall continue to be managed by the GPS Plug-In (either using default values or user defined values).
*/
#define GPS_GPIO_NOT_MANAGED (u16)(0xFFFF)

// This type is used for GPS_HWTYPE_XM0110 hardware type(gps_hwType_e type).
// The gps_ioConfigXM0110_t type defines the Plug-In hardware resources configuration as shown below:
typedef struct
{
	gps_busTypeID_e ebusTypeID;
	gps_busNumberID_e ebusNumberID;
	u16 u16OnOffCmdID;
	u16 u16RstCmdID;
	u16 u16GpsRecWakeupCmdID;
	u16 u16HostWakeupExtIntID;
	gps_32kMngt_e e32kMngt;
} gps_ioConfigXM0110_t;



// Defines the starting mode of GPS Plug-In
typedef enum
{
	GPS_HOT_START=0,
	GPS_WARM_START,
	GPS_COLD_START,
	GPS_FACTCOLD_START
} gps_startingMode_e;


// Plug-In library events related to Plug-In state machine
typedef enum
{
	GPS_INIT_EVENT= 0,
	GPS_INIT_ERROR_EVENT,
	GPS_START_EVENT,
	GPS_START_ERROR_EVENT,
	GPS_STOP_EVENT,
	GPS_STOP_ERROR_EVENT,
	GPS_SLEEP_EVENT,
	GPS_SLEEP_ERROR_EVENT,
	GPS_RELEASE_EVENT,
	GPS_RELEASE_ERROR_EVENT,
	GPS_ERROR_EVENT,
	GPS_ABORT_EVENT
} gps_event_e;

// GPS Plug-In sleep mode(s)
typedef enum
{
	GPS_IDLE_MODE = 0,
	GPS_HIBERNATE_MODE = 1,
	GPS_DEEP_MODE = 2
} gps_sleepMode_e;



// NMEA sentences encode mask
typedef enum
{
	GPS_NMEA_NONE_EN =	0,
	GPS_NMEA_GGA_EN  = 	(1 << 0),
	GPS_NMEA_GSA_EN  = 	(1 << 1),
	GPS_NMEA_RMC_EN  = 	(1 << 2),
	GPS_NMEA_VTG_EN  = 	(1 << 3),
	GPS_NMEA_GLL_EN  = 	(1 << 4),
	GPS_NMEA_GST_EN  = 	(1 << 5),
	GPS_NMEA_GSV_EN  = 	(1 << 6),
	GPS_NMEA_ZDA_EN  = 	(1 << 7),
	GPS_NMEA_PROP_EN =  (1 << 15),
	GPS_NMEA_ALL_EN  = 	0xFFFF
} gps_nmeaOptList_e;


// The list must be terminated by the following definition:
#define GPS_OPT_END		(0xFF)

#define GPS_SV_NUMBER	(16)


// Options currently defined for the GPS NMEA Service
typedef enum
{
	GPS_OPT_NMEA_HANDLER,
	GPS_OPT_NMEA_LIST,
	GPS_OPT_NMEA_RATE,
	GPS_OPT_NMEA_RAW_DATA_MODE
}gps_nmeaOpt_e;

// Options currently defined for the GPS PVT Service
typedef enum
{
	GPS_OPT_PVT_HANDLER,
	GPS_OPT_PVT_RATE
}gps_pvtOpt_e;


// Options currently defined for the GPS INFO Service
typedef enum
{
	GPS_OPT_INFO_HANDLER,
	GPS_OPT_INFO_2D_TTFF,
	GPS_OPT_INFO_3D_TTFF
}gps_infoOpt_e;



// Defines the position fix state.
typedef enum
{
	GPS_FIX_MODE_FORCED_UNDEF = 1,
	GPS_FIX_MODE_FORCED_2D,
	GPS_FIX_MODE_FORCED_3D,
} gps_pvtFixMode_e;


// Defines the position fix state.
typedef enum
{
	GPS_FIX_NO_POS= 0,
	GPS_FIX_2D,
	GPS_FIX_DIFF_2D,
	GPS_FIX_3D,
	GPS_FIX_DIFF_3D,
	GPS_FIX_ESTIMATED,
	GPS_FIX_1SV_DEGRADED,
	GPS_FIX_2SV_DEGRADED
} gps_pvtFixState_e;

// Defines the Time and Date information state
typedef enum
{
	GPS_PVT_TIME_DATE_INVALID= 0,
	GPS_PVT_TIME_DATE_HOST,
	GPS_PVT_TIME_DATE_GPS
} gps_pvtTimeDateState_e;

/*************************/
/* GPS Core types        */
/*************************/
// Defines the parameter related to the GPS Core.
typedef enum
{
	GPS_OPT_CORE_DEBUG_HANDLER,
	GPS_OPT_CORE_DEBUG_DATA,
	GPS_OPT_CORE_RUNNING_POWER_MODE ,
	GPS_OPT_CORE_LNA_EXT,
	GPS_OPT_CORE_HW_CAP_GPS_MIN_RATE,
	GPS_OPT_CORE_HW_CAP_NMEA_LIST_AVAILABLE,
	GPS_OPT_CORE_AIDING_MODE,
	GPS_OPT_CORE_DEBUG_HANDLER_INPUT,
	GPS_OPT_CORE_BOOST_MODE_HANDLER,
	GPS_OPT_CORE_WAKEUP,
	GPS_OPT_CORE_BANKING_EE,
	GPS_OPT_CORE_JAMMING,
	GPS_OPT_CORE_INFO_HANDLER,
	GPS_OPT_CORE_INFO_DATA,
	GPS_OPT_CORE_INFO_RATE,
	GPS_OPT_CORE_NAV_RATE_FACTOR,
	GPS_OPT_CORE_BLANKING,
	GPS_OPT_CORE_HOST_RTC_HIDEN=250
}gps_coreOpt_e;

// Defines the debug data output
typedef enum
{
	GPS_CORE_DEBUG_NONE= 0x00,
	GPS_CORE_DEBUG_RAW_DATA = 0x01,
	GPS_CORE_DEBUG_INFO = 0x02,
	GPS_CORE_DEBUG_MAX=0x04
} gps_coreDebugData_e;

// Defines the navigation low power mode of the GPS Plug-In library in GPS_RUNNING state
typedef enum
{
	GPS_PM_FULL= 0,
	GPS_PM_MED,
	GPS_PM_LOW,
	GPS_PM_VERY_LOW
} gps_corePowerMode_e;

// Defines the GPS receiver receive chain configuration.
typedef enum
{
	GPS_LNA_INTERNAL = 0,
	GPS_LNA_EXTERNAL,
	GPS_LNA_INTERNAL_WITH_GPIO_LNA_EN_ALWAYS_OFF,
	GPS_LNA_EXTERNAL_WITH_GPIO_LNA_EN_ALWAYS_OFF
} gps_coreLnaConfig_e;

// Defines the debug data callback event type
typedef enum
{
	GPS_CORE_RAW_DATA_EVT =0x01,
	GPS_CORE_DEBUG_INFO_EVT =0x02,
} gps_coreDebugEvent_e;

// Defines the core information callback event type.
typedef enum
{
	GPS_CORE_INFO_JAMMERS_EVT = 0
} gps_coreInfoEvent_e;

// Defines the core information data output
typedef enum
{
	GPS_CORE_INFO_NONE= 		(0<<0),
	GPS_CORE_INFO_JAMMERS = 	(1<<0)
}gps_coreInfoData_e;

// Provides the presence of at most 8 interferences detected as a result of the most recent CW scan or monitor.
typedef struct
{
	u32 frequency[8];
	u8 level[8];
} gps_coreInfoJam_t;

// Specifies the GPS Aiding mode
// DEPRECATED TYPE: PLEASE USE gps_aidingMode_e and related function
typedef enum
{
	GPS_AIDING_NONE= 0,
	GPS_AIDING_AEE
} gps_coreAidingMode_e;

// Configures the Wakeup software mechanism
typedef enum
{
	GPS_WAKEUP_DISABLED = 0,
	GPS_WAKEUP_ENABLED
} gps_coreWakeup_e;

// Blanking configuration
typedef enum
{
	GPS_CORE_BLANKING_OFF			= 0,
	GPS_CORE_BLANKING_ON			= 1
} gps_coreBlanking_e;

// Jamming CW configuration
typedef enum
{
	GPS_CW_CARRIER_DISABLED			= 0,
	GPS_CW_CARRIER_ENABLED			= 1
} gps_coreCWRemoving_e;


/*************************/
/* Aiding type           */
/*************************/
// Defines options related to Aiding service
typedef enum
{
	GPS_OPT_AIDING_MODE,
	GPS_OPT_AIDING_HANDLER,
	GPS_OPT_AIDING_DEE_DURATION,
	GPS_OPT_AIDING_DEE_CMD,
	GPS_OPT_AIDING_DEE_SERVER_ADD,
	GPS_OPT_AIDING_DEE_SERVER_PORT,
	GPS_OPT_AIDING_DEE_AUTH_CODE,
	GPS_OPT_AIDING_DEE_SOCKET_TYPE,
	GPS_OPT_AIDING_DEE_TIMEOUT,
	GPS_OPT_AIDING_END = GPS_OPT_END
}gps_aidingOpt_e;

// Specifies the GPS Aiding mode encode mask
typedef enum
{
	GPS_AIDING_MODE_NONE= 0,
	GPS_AIDING_MODE_AEE = (1<<0),
	GPS_AIDING_MODE_DEE = (1<<1)
}gps_aidingMode_e;

// Specifies the command controlling the EE downloader
typedef enum
{
	GPS_AIDING_DEE_CMD_NONE,
	GPS_AIDING_DEE_CMD_START,
	GPS_AIDING_DEE_CMD_STOP,
	GPS_AIDING_DEE_CMD_FORCE
} gps_aidingDeeCmd_e;


// Defines the Aiding callback event type
typedef enum
{
	GPS_AIDING_ERROR_EVT,
	GPS_AIDING_AEE_START_EVT,
	GPS_AIDING_AEE_STOP_EVT,
	GPS_AIDING_AEE_INFO_EVT,
	GPS_AIDING_DEE_START_EVT,
	GPS_AIDING_DEE_STOP_EVT,
	GPS_AIDING_DEE_COMPLETED_EVT,
	GPS_AIDING_DEE_ABORT_EVT,
	GPS_AIDING_DEE_INFO_EVT
} gps_aidingEvent_e;

// Defines the GPS Aiding Service API error type
typedef enum
{
	GPS_AIDING_OK = 0,
	// AEE error codes
	GPS_AIDING_AEE_ERROR = -1,
	// DEE error codes
	GPS_AIDING_DEE_SOCKET_ERROR = -10,
	GPS_AIDING_DEE_WRITE_ERROR = -11,
	GPS_AIDING_DEE_READ_ERROR = -12,
	GPS_AIDING_DEE_SERVER_ERROR = -13,
	GPS_AIDING_DEE_FILE_ERROR = -14,
	GPS_AIDING_DEE_TIMEOUT_ERROR = -15,
	GPS_AIDING_DEE_NACK_ERROR	= -16,
	GPS_AIDING_DEE_ACK_TIMEOUT_ERROR = -17,
	GPS_AIDING_DEE_INTERNAL_ERROR = -18
} gps_aidingStatus_e;

// Includes the information about Aiding error event
typedef struct
{
	gps_aidingStatus_e eAidingStatus;
	void * pExtendedErrorCode;
} gps_aidingErrorEvent_t;

// Defines information about the AEE computation
typedef struct
{
	u16 aeeSvId;
	u16 aeeSvValidity;
} gps_aidingAeeEventInfo_t;

// Defines information of GPS_AIDING_DEE_COMPLETED_EVT events
typedef struct
{
	u16 deeValidity;
	void * reservedForFuturUse;
} gps_aidingDeeEventCompleted_t;

// Defines information about the DEE
typedef struct
{
	void * deeInfoReservedForFuturUse;
} gps_aidingDeeEventInfo_t;

// Defines the generic event data type of Aiding handler callback
typedef union
{
	gps_aidingErrorEvent_t 			tAidingErrorEvent;
	gps_aidingAeeEventInfo_t 		tAeeEventInfo;
	gps_aidingDeeEventCompleted_t 	tDeeEventCompleted;
	gps_aidingDeeEventInfo_t 		tDeeEventInfo;
} gps_aidingEventData_u;

/*************************/
/*  PVT Service          */
/*************************/

// Includes the position information as described below
typedef struct
{
	/* Position information */
	gps_pvtFixState_e 	eFixState;
	bool				valid2DFix ;
	bool				valid3DFix ;
	gps_pvtFixMode_e	eFixMode ;
	bool				validStatus;
	/* Position parameters */
	double		latitude;
	double		longitude;
	float			altitudeEll;
	float			altitudeMsl;
	/* Movement parameters */
	float			speed;
	float			course;
	float			verticalVelocity;
	/* Cartesian coordinates */
	double		xCoord;
	double		yCoord;
	double		zCoord;
	/* Precision parameters */
	float			hDop;
	float			vDop;
	float			pDop;
	float			horizontalPositionError;
	float			verticalPositionError;
	float			horizontalSpeedError;
	float			verticalSpeedError;
} gps_pvtPosition_t;




// Includes the date and time information as described below
typedef struct
{
	/* Structure information */
	gps_pvtTimeDateState_e pvtTimeDateState;
	/* Date parameters*/
	u16 			year;
	u16			month;
	u16			day;
	s16			weekNumber;
	/* Time parameters */
	u16			hours;
	u16			minutes;
	u16			seconds;
	u16			milliseconds;
} gps_pvtTimeDate_t;


// Includes the satellites in View information as described below
typedef struct
{
	bool			statsInViewUsed;
	u8				statsInViewSVid;
	u8				statsInViewSNR;
	u16				statsInViewAzim;
	s8				statsInViewElev;
} gps_pvtSatInViewInfo_t;


// Includes the satellites information as described below
typedef struct
{
	u8			satsInView;
	u8			satsUsed;
	gps_pvtSatInViewInfo_t satInViewInfo[GPS_SV_NUMBER] ;
} gps_pvtSatellites_t;


// Includes the PVT information as described below:
typedef struct
{
	/* Satellites information */
	gps_pvtSatellites_t 	tSatellitesInfo;
	/* Date and Time information*/
	gps_pvtTimeDate_t		tTimeDateInfo;
	/* Position information*/
	gps_pvtPosition_t		tPositionInfo;
} gps_pvtInfo_t;


// Defines the GPS Position Service events.
typedef struct
{
	enum event_type_e
	{
		GPS_POS_FIX_LOST_EVENT= 0,
		GPS_POS_FIX_ESTIMATED_EVENT,
		GPS_POS_FIX_2D_EVENT,
		GPS_POS_FIX_3D_EVENT,
		GPS_POS_FIX_INVALID_EVENT
	} eEventType;
	void* pEventData;
} gps_infoEvent_t;

// Structure including the position information
typedef struct
{
	/* PVT Structure */
	gps_pvtInfo_t        tPvtInfo;
	/* Specific GPS information */
	void  *		pDetailedInfo;
} gps_infoDetailedPos_t;



#endif /* XM0110_COMPAT_H_ */