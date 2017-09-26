/*
 * ini_config.h
 *
 *  Created on: 27 Á.¤. 2555
 *      Author: Pan
 */

#ifndef INI_CONFIG_H_
#define INI_CONFIG_H_


typedef struct INI_CONFIG_NET_T {
	struct {
		ascii		APN[64];
		ascii		User[64];
		ascii		PassW[64];
		ascii		SIMPin[ 5];
	}PPP;
	struct {
		ascii		IP[64];
		u16			Port;
	}TCPServer1;

	struct {
		ascii		IP[64];
		ascii		User[64];
		ascii		PassW[64];
		ascii		FileN[64];
	}DOTAServer;
}ini_config_net_t;

typedef struct INI_CONFIG_UTILITIES_T {

	struct {
		u16		Value;			/* kM/h   *//* Exceed Speed*/
		u16		Delay;			/* Second *//* Delay Protect Swing Speed*/
	}OverSpeed;
	
	struct {
		u16		Legacy;
		u16		DriveMode;		/* Second *//* Keep period while Driving*/
	}Sampling;
	
	struct {
		u8		Logic;
		u16		Delay;			/* Second *//* Delay Protect Swing Input*/
	}Input1;
	
	struct {
		u8		Logic;
		u16		Delay;			/* Second *//* Delay Protect Swing Input*/
	}Input2;
	
	struct {
		u8		Logic;
		u16		Delay;			/* Second *//* Delay Protect Swing Input*/
	}Input3;
	
	struct {
		u8		Logic;
		u16		Delay;			/* Second *//* Delay Protect Swing Input*/
	}Input4;
	
	struct {
		u8		Logic;
		u16		Delay;			/* Second *//* Delay Option To Re-Value*/
	}Output1;
	
	struct {
		u8 		Logic;
		u16 	Delay;			/* Second *//* Delay Option To Re-Value*/
	}Output2;
	
	ascii	AdminPhone[4][30];		/* Super User  phone number to identify*/

	struct {
		u8		bDota;
		u8		bHttpFile;
	}CtrlRequest;

	struct {
		u16		Value;			/* kM/h   *//* Lowest Speed To Determine As Neutral*/
		u16		Delay;			/* Second *//* Delay Protect Swing Speed*/
	}Idling;
	
	struct {
		
		ascii sCompany[20];
		ascii sCarID[16];
		ascii sSIMNumb[20];
		bool bAlertOversp;
		ascii sEmail[32];
		ascii sFwName[4];
		
		struct {
			u16 HA;
			u16 HD;	
		}Harshing;
		
		struct {
			bool already;
			double calValue;
		}HAHDCalibrate;
		
		//add for sharp-e
		u16 vehicle_type;
		
		u8 optionmask;
		#if(0)
		+----------------+-------------+--------------+---------------+-------------+------------+
		|  DLT_CARD Type |  Temp Used  |   SEC Used   |    CAN Used   |  Buzzer En  |   Reserved |
		+----------------+-------------+--------------+---------------+-------------+------------+
		|      XX        |     X       |      X       |      X        |    X        |     XX     |
		+----------------+-------------+--------------+---------------+-------------+------------+
		#endif
		
		u8 vehicle_attr;
		#if(0)
		+--------------------------+---------------+-------------+---------------+--------------+
		|         Reserved         |  VEH_TRAILER  |  VEH_DANGER |  PASSENGER_NB |  VEH_WEIGHT  |
		+-------------+------------+---------------+-------------+-------------+----------------+
		|           XXXX           |       X       |      X      |       X       |      X       |
		+-------------+------------+---------------+-------------+---------------+--------------+
		#endif
		
	}DTCUniq;

}ini_config_utils_t;


extern ini_config_net_t ini_config_net;
extern ini_config_utils_t ini_config_utils;


bool ini_config_isUpdate(void);
void ini_config_setUpdate(void);
u16 ini_config_atol_strParameter(const ascii *,u16);
u16 ini_config_atoi_strParameter(const ascii *,u16);
double ini_config_atof_strParameter(const ascii *cmd, u16 nb);
double ini_config_aDMS2i_strParameter( const ascii *cmd, u16 nb);
ascii* ini_config_strParameter(ascii*,const u16, const ascii *, u16);
bool ini_config_loadRecursive(uint8_t i);


#define INI_CONFIG_LOAD()	for(uint8_t i = 0;ini_config_loadRecursive(i);i++) vTaskDelay(1)

#endif /* INI_CONFIG_H_ */
