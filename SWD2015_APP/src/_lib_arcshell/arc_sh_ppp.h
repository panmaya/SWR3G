/*
 * arc_sh_apnserv.h
 *
 *  Created on: 29 Á.¤. 2555
 *      Author: Pan
 */

#ifndef ARC_SH_PPP_H_
#define ARC_SH_PPP_H_

void arc_shell_ppp_InitModule(const char dir[16]);

void arc_shell_ppp_apnHandler(adl_atCmdPreParser_t * Params);
void arc_shell_ppp_userHandler(adl_atCmdPreParser_t * Params);
void arc_shell_ppp_passHandler(adl_atCmdPreParser_t * Params);
void arc_shell_ppp_cpinHandler(adl_atCmdPreParser_t * Params);

#endif /* ARC_SH_APNSERV_H_ */
