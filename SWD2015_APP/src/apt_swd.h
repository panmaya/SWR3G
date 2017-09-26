/*
 * IncFile1.h
 *
 * Created: 3/25/2015 5:12:41 PM
 *  Author: Pan
 */ 

#include "ini_config.h"


#ifndef APT_SWD_H_
#define APT_SWD_H_

void aptSwd_submit_overspeed(bool status);
void aptSwd_submit_licensecard_alert(bool status);
u8 aptSwd_gps_fix_status(void);
ascii * aptSwd_get_sim_ccid(void);
ascii * aptSwd_get_sim_operator(void);

void aptSwd_task(void * pvParameters);
bool aptSwd_task_completed(void);
#endif /* APT_SWD_H_ */