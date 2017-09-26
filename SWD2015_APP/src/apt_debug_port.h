/*
 * apt_debug_port.h
 *
 * Created: 20/3/2558 14:59:13
 *  Author: pan
 */ 


#ifndef APT_DBGPORT_H_
#define APT_DBGPORT_H_

#define APT_DBGPORT_CMD_MAX_LEN				256

#define APT_DBGPORT_TASK_INTERRUPT_PRIORITY	(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + /*4*/5)

bool arc_atcmd_exe ( void );
void aptDbgPort_task(void * pvParameters);
void aptDbgPort_puts(const char*);


#endif /* APT_DBGPORT_H_ */