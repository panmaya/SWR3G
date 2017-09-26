/*
 * arc_sh_tools.h
 *
 *  Created on: 22 ¡.¾. 2555
 *      Author: Pan
 */

#ifndef ARC_SH_TOOLS_H_
#define ARC_SH_TOOLS_H_

void arc_shell_tools_InitModule(const char dir[6]);

void arc_shell_tools_DOTAForceHandler(adl_atCmdPreParser_t * Params);
void arc_shell_tools_HttpFileHandler(adl_atCmdPreParser_t * Params);

#endif /* ARC_SH_TOOLS_H_ */
