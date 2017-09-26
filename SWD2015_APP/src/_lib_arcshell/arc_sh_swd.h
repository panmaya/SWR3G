/*
 * IncFile1.h
 *
 * Created: 3/26/2015 3:26:01 PM
 *  Author: Pan
 */ 


#ifndef ARC_SH_SWD_H_
#define ARC_SH_SWD_H_

void arc_shell_swd_InitModule(const char dir[16]);

void arc_shell_swd_CompanyGetHandler(adl_atCmdPreParser_t * Params);
void arc_shell_swd_BoxidGetHandler(adl_atCmdPreParser_t * Params);
void arc_shell_swd_SimnumGetHandler(adl_atCmdPreParser_t * Params);
void arc_shell_swd_PointGetHandler(adl_atCmdPreParser_t * Params);
void arc_shell_swd_OverspwGetHandler(adl_atCmdPreParser_t * Params);
void arc_shell_swd_EmailGetHandler(adl_atCmdPreParser_t * Params);
void arc_shell_swd_HDHAHandler(adl_atCmdPreParser_t * Command);
void arc_shell_swd_HDHAGpsHandler(adl_atCmdPreParser_t * Command);
void arc_shell_swd_HDHACalHandler(adl_atCmdPreParser_t * Command);
void arc_shell_swd_FeatureHandler(adl_atCmdPreParser_t * Command);
void arc_shell_swd_OptionHandler(adl_atCmdPreParser_t * Command);
void arc_shell_swd_VehTypeHandler(adl_atCmdPreParser_t * Command);
void arc_shell_swd_VehAttrHandler(adl_atCmdPreParser_t * Command);

#endif /* ARC_SH_SWD_H_ */