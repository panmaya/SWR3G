/*
 * adl_fcm_Private.h
 *
 * Created: 1/3/2558 17:33:12
 *  Author: ARC-PAN
 */ 


#ifndef ADL_FCM_PRIVATE_H_
#define ADL_FCM_PRIVATE_H_

/* The size of the buffer provided to the UART driver
for storage of received  bytes. */
#define ADL_FCM_BUFFER_SIZE		(512)
#define ADL_FCM_UART_BAUDRATE	9600

/* The name for each tasks. */
#define ADL_FCM_TASK_NAME					(const signed char *const) "adl_fcm"
/* The stack sizes allocated to the various tasks. */
#define ADL_FCM_TASK_STACK_SIZE				(configMINIMAL_STACK_SIZE + 1200)
/* The priorities at which various tasks will get created. */
#define ADL_FCM_TASK_PRIORITY				(tskIDLE_PRIORITY + 1)
#define ADL_FCM_TASK_INTERRUPT_PRIORITY		(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 4)

#endif /* ADL_FCM_PRIVATE_H_ */