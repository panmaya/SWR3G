/*
 * shell.h
 *
 *  Created on: 29 Á.¤. 2555
 *      Author: Pan
 */

#ifndef ARC_SHELL_H_
#define ARC_SHELL_H_

#include "common/arc_shell_common.h"

ascii* arc_shell_execute(const ascii *command);
ascii* arc_shell_execute_port(const ascii* sCmd, adl_port_e port);
void arc_shell_delete(void);
void arc_shell_Subscribe(arc_shell_postCtx);

#endif /* ARC_SHELL_H_ */
