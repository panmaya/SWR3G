/*
 * apt_gateway.h
 *
 *  Created on: 24 ¡.¾. 2555
 *      Author: Pan
 */

#ifndef APT_GATEWAY_H_
#define APT_GATEWAY_H_
#include "trans_form.h"

void aptGateway_task(void * pvParameters);
bool aptGateway_serverTimeout(void);
uint16_t aptGateway_uploadDelay(void);

#endif /* APT_GATEWAY_H_ */
