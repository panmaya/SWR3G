/*
 * error.h
 *
 * Created: 2016-10-21 10:14:55 AM
 *  Author: Panmaya
 */ 


#ifndef ERROR_H_
#define ERROR_H_

void error_halt_swReset(const char * file, uint32_t line);
char* error_reset_reason(char sDisplay[60]);
adl_InitType_e error_init_by(void);

#define software_reset()	error_halt_swReset(__FILE__, __LINE__)

#endif /* ERROR_H_ */