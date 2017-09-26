/*
 * apt_gps.c
 *
 * Created: 26/3/2558 17:51:07
 *  Author: pan
 */ 
#include "adl_global.h"
#include "ini_config.h"
#include "distance_calc.h"
#include "_lib_gps/nmea_gps.h"
#include "_lib_gps/nmea.h"
#include "vprinter.h"
#include "apt_gps_data.h"
#include "misc_config.h"
#include "_drv_serial/usart/freertos_usart_serial.h"

#define APT_GPS_DATA_TRACE	8
#define APT_GPS_DATA_BAUD_RATE						9600
#define APT_GPS_DATA_NMEA_BUFFSIZE					400
#define APT_GPS_DATA_RCV_SIZE						(APT_GPS_DATA_NMEA_BUFFSIZE/2)
/* The size of the buffer provided to the UART driver for storage of received
 * bytes. */
#define APT_GPS_DATA_DRV_STACK_SIZE					(APT_GPS_DATA_NMEA_BUFFSIZE*2)
/* The buffer provided to the UART driver to store incoming character in. */
static uint8_t APT_GPS_DATA_DrvStack[APT_GPS_DATA_DRV_STACK_SIZE];
/* The UART instance used for input and output. */
static freertos_usart_if APT_GPS_DATA_ifHandle = NULL;

static gps_pvtInfo_t gpsinfo;
static uint16_t gps_sn = 0;
static uint32_t error_secCount = 0;

static u16 APT_GPS_DATA_tranform_knot2kmh(uint16_t knot,uint16_t knot_frac) {
	double speed_knot = 0.0 ;
	double speed_frac_knot = 0.0 ;
	double speed_kmh = 0.0;
	speed_frac_knot = knot_frac/100 ;
	speed_knot	=  knot+speed_frac_knot ;
	speed_kmh	=  speed_knot*1.852;
	return speed_kmh;

}
static void APT_GPS_DATA_SentenceHandler(struct nmea_data * p) {
	double fraction = 0.0;
	double mention = 0.0;
	
	gpsinfo.tTimeDateInfo.day = p->gprmc.utc_day;
	gpsinfo.tTimeDateInfo.month = p->gprmc.utc_month;
	gpsinfo.tTimeDateInfo.year = p->gprmc.utc_year + 2000;
	gpsinfo.tTimeDateInfo.hours = p->gprmc.utc_hour;
	gpsinfo.tTimeDateInfo.minutes = p->gprmc.utc_minute;
	gpsinfo.tTimeDateInfo.seconds = p->gprmc.utc_second;
	gpsinfo.tPositionInfo.course = p->gprmc.course;
	gpsinfo.tPositionInfo.hDop = p->gpgsa.hdop;
	gpsinfo.tPositionInfo.vDop = p->gpgsa.vdop;

	switch(p->gpgsa.fix_status){
	case 2: gpsinfo.tPositionInfo.eFixState = GPS_FIX_2D;break;
	case 3: gpsinfo.tPositionInfo.eFixState = GPS_FIX_3D;break;
	default:gpsinfo.tPositionInfo.eFixState = GPS_FIX_NO_POS;break;
	}

	gpsinfo.tSatellitesInfo.satsUsed = p->gpgga.satellites;
	gpsinfo.tPositionInfo.speed = APT_GPS_DATA_tranform_knot2kmh(p->gprmc.speed,p->gprmc.speed_frac);
	
	fraction = (double)((p->gpgga.coords.latitude.minutes * NMEA_FRACTIONAL_SCALER) + p->gpgga.coords.latitude.minutes_frac)/(double)NMEA_FRACTIONAL_SCALER;
	mention = (double)p->gpgga.coords.latitude.degrees;
	gpsinfo.tPositionInfo.latitude = mention + (fraction/(double)60);
	
	fraction = (double)((p->gpgga.coords.longitude.minutes * NMEA_FRACTIONAL_SCALER) + p->gpgga.coords.longitude.minutes_frac)/(double)NMEA_FRACTIONAL_SCALER;
	mention = (double)p->gpgga.coords.longitude.degrees;
	gpsinfo.tPositionInfo.longitude = mention + (fraction/(double)60);
	
	#if (CONF_NMEA_GPGSV_ENABLED == true)
	gpsinfo.tSatellitesInfo.satsInView =  p->gpgsv.NbSatInViews;
	for(uint8_t i = 0; i < GPS_SV_NUMBER ; i++) {
		gpsinfo.tSatellitesInfo.satInViewInfo[i].statsInViewSVid = p->gpgsv.gps_view[i].PRN_nb;
		gpsinfo.tSatellitesInfo.satInViewInfo[i].statsInViewSNR = p->gpgsv.gps_view[i].SNR;
	}
	#endif
	
	UP_COUNTER_OVERFLOW_ZERO(gps_sn,true,65535);

	vPrintfTrace((APT_GPS_DATA_TRACE,"%d-%d-%d %d:%d:%d %dD \"%.5f-%.5f\" %.2fKmh",
	gpsinfo.tTimeDateInfo.day,
	gpsinfo.tTimeDateInfo.month,gpsinfo.tTimeDateInfo.year,
	gpsinfo.tTimeDateInfo.hours,
	gpsinfo.tTimeDateInfo.minutes,gpsinfo.tTimeDateInfo.seconds,
	gpsinfo.tPositionInfo.eFixState,
	gpsinfo.tPositionInfo.latitude,gpsinfo.tPositionInfo.longitude,
	gpsinfo.tPositionInfo.speed));
}


bool aptGpsData_getError(void) {
	return error_secCount >= _1SecTick_Second(120*2) ? true : false;
}

uint16_t aptGpsData_getInfo(gps_pvtInfo_t * ptrInfo) {
	memcpy(ptrInfo, &gpsinfo, sizeof(gps_pvtInfo_t));
	return gps_sn;
}


/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         Write Serial                             */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
int8_t aptGpsData_send ( uint8_t Handle, uint8_t * Data, uint16_t DataLen ) {
	/* The USART is configured to use a mutex on Tx, so can be safely written to directly. */
	if (APT_GPS_DATA_ifHandle != NULL) {
		freertos_usart_write_packet(APT_GPS_DATA_ifHandle, Data, DataLen, 2000UL / portTICK_RATE_MS);
		return 0;
	}
	return -1;
}


/*-----------------------------------------------------------------------*/
/* Command:                                                       */
/* Properties:         UART RX Task                           */
/*Expect String:                                                   */
/*-----------------------------------------------------------------------*/
void aptGpsData_task(void * pvParameters) {
	static uint8_t *pchar = NULL;
	static uint16_t input_index = 0;
	static portBASE_TYPE returned_value;
	static bool found_start = false;
	static uint8_t read_buff[APT_GPS_DATA_RCV_SIZE + 1];
	static uint8_t nmea_string[APT_GPS_DATA_NMEA_BUFFSIZE + 1];
	static struct nmea_data gps_nmea_data;
	static uint32_t * pWdCount = NULL;
		
	pWdCount = (uint32_t*)pvParameters;
	
	memset(&gpsinfo,0,sizeof(gps_pvtInfo_t));

	freertos_peripheral_options_t driver_options = {
		APT_GPS_DATA_DrvStack,							/* The buffer used internally by the UART driver to store incoming characters. */
		APT_GPS_DATA_DRV_STACK_SIZE,						/* The size of the buffer provided to the UART driver to store incoming characters. */
		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 4,/* The priority used by the UART interrupts. */
		USART_RS232,									/* Configure the UART for RS232 operation. */
		USE_TX_ACCESS_MUTEX		/* Use access mutex on Tx   (as more than one task transmits) but not Rx.*/
	};

	const sam_usart_opt_t usart_settings = {
		APT_GPS_DATA_BAUD_RATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		0 /* Only used in IrDA mode. */
	}; /*_RB_ TODO This is not SAM specific, not a good thing. */

	/* Initialise the UART interface. */
	APT_GPS_DATA_ifHandle = freertos_usart_serial_init(USART0, &usart_settings, &driver_options);
	configASSERT(APT_GPS_DATA_ifHandle);

	for (;;) {
		
		*pWdCount = 0;//Clear watchdog
		
		UP_COUNTER_OVERFLOW_LIMIT(error_secCount,true,_1SecTick_Second(600*2));
		
		returned_value = freertos_usart_serial_read_packet(APT_GPS_DATA_ifHandle,read_buff,APT_GPS_DATA_RCV_SIZE,100UL/ portTICK_RATE_MS);
		if(returned_value > 0) {
			int8_t iret = 0;
			read_buff[returned_value] = 0;
			pchar = &read_buff[0];
			for(;returned_value;returned_value--) {
				switch(*pchar) {
				case 0: //continue;
				case '\n': /* Ignore newline characters */
				break;
				case '\r':
					gps_nmea_data.type = NMEA_TYPE_UNKNOWN;
					nmea_string[input_index] = '\0';
					if((iret = nmea_parse_input((char*)nmea_string,input_index, &gps_nmea_data)) == 1) {
						if(gps_nmea_data.type == NMEA_TYPE_GPRMC) {
							error_secCount = 0;
							APT_GPS_DATA_SentenceHandler(&gps_nmea_data);
						}
					}
					else if(iret == 0) {
						if(gps_nmea_data.type == NMEA_TYPE_GPRMC) {
							error_secCount = 0;
						}
					}
					//else if(iret == -1) vPutsTrace(30,"NMEA: Check Sum Error");
					input_index = 0;
					found_start = false;
				break;
				case '$':
					found_start = true;
				default:
					/* Only start storing the sentence into the buffer when the start
					 * character has been found
					 */
					nmea_string[input_index] = *pchar;
					if (found_start && (input_index < APT_GPS_DATA_NMEA_BUFFSIZE)) input_index++;
				break;
				}
				pchar++;
			}
		}
		
		vTaskDelay(10);//1.2 byte/ms  @9600
	}
}

