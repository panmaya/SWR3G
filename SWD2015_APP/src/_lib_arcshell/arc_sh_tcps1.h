/*
 * arc_sh_tcp1.h
 *
 *  Created on: 29 Á.¤. 2555
 *      Author: Pan
 */

#ifndef ARC_SH_TCPS1_H_
#define ARC_SH_TCPS1_H_

void arc_shell_tcps1_InitModule(const char dir[16]);

void arc_shell_tcp_Serve1_AddrHandler(adl_atCmdPreParser_t * Params);
void arc_shell_tcp_Serve1_PortHandler(adl_atCmdPreParser_t * Params);

#endif /* ARC_SH_TCP1_H_ */
