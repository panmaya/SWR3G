/*
 * arc_sh_btt.h
 *
 *  Created on: 29 Á.¤. 2555
 *      Author: Pan
 */

#ifndef ARC_SH_APT_H_
#define ARC_SH_APT_H_

void arc_shell_apt_InitModule(const char dir[8]);

void arc_shell_apt_Output1Handler(adl_atCmdPreParser_t * Params);
void arc_shell_apt_Output2Handler(adl_atCmdPreParser_t * Params);
void arc_shell_apt_SamplingHandler(adl_atCmdPreParser_t * Params);
void arc_shell_apt_OverSpeedHandler(adl_atCmdPreParser_t * Params);
void arc_shell_apt_AdminPhoneNb1Handler(adl_atCmdPreParser_t * Params);
void arc_shell_apt_AdminPhoneNb2Handler(adl_atCmdPreParser_t * Params);
void arc_shell_apt_AdminPhoneNb3Handler(adl_atCmdPreParser_t * Params);
void arc_shell_apt_AdminPhoneNb4Handler(adl_atCmdPreParser_t * Params);
void arc_shell_apt_IdlingModeHandler(adl_atCmdPreParser_t * Command);

#endif /* ARC_SH_APT_H_ */
