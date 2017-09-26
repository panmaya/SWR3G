/*
 * swd_register.h
 *
 * Created: 26/3/2558 11:08:36
 *  Author: pan
 */ 


#ifndef SWD_REGISTER_H_
#define SWD_REGISTER_H_

#define SWD_REGISTER_HARDCODE	0

typedef struct SWD_REGISTER_CFG_T {
	ascii sFwName[4];
	ascii sCompany[20];
	ascii sCarID[16];
	ascii sSIMNumb[20];
	u16 set_over;			/* over speed              */
	u16 set_samp;			/* set sample  of data     */
	u16 set_point;			/* point for sample        */
	u16 set_idle;			/* idle time for moving   */
	u16 set_delay;			/* delay time for over speed   */
	bool bAlertOversp ;	//= 0 ; /*       sound off over speed     */
	ascii recall[32];
	ascii sEmail[32];
	ascii tcprecservall[32];
	ascii apnall[32];
	ascii tcprecservport[6];
	u16	HA;
	u16 HD;
	
	//add for sharp-e
	u16 vehicle_type;
	
	u8 optionmask;
	
	u8 vehicle_attr;
	
	#if(0) //optionmask
	+----------------+-------------+--------------+---------------+-------------+------------+
	|  DLT_CARD Type |  Temp Used  |   SEC Used   |    CAN Used   |  Buzzer En  |   Reserved |
	+----------------+-------------+--------------+---------------+-------------+------------+
	|      XX        |     X       |      X       |      X        |    X        |     XX     |
    +----------------+-------------+--------------+---------------+-------------+------------+
	#endif
	
	#if(0)//vehicle_attr
	+--------------------------+---------------+-------------+---------------+--------------+
	|         Reserved         |  VEH_TRAILER  |  VEH_DANGER |  PASSENGER_NB |  VEH_WEIGHT  |
	+-------------+------------+---------------+-------------+-------------+----------------+
	|           XXXX           |       X       |      X      |       X       |      X       |
	+-------------+------------+---------------+-------------+---------------+--------------+
	#endif
	
	u8 spk_gain;
	u8 mic_gain;
	

}swd_register_cfg_t;

uint8_t swd_register_make_sndframe(char* dst, const char* str_ccid, const char* str_sim);
bool swd_register_cfg_decode(swd_register_cfg_t * cfg, uint8_t* data, uint16_t length);


#endif /* SWD_REGISTER_H_ */