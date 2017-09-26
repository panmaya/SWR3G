/*
 * arc_shell_common.h
 *
 * Created: 10/12/2016 11:26:51 AM
 *  Author: Panmaya
 */ 


#ifndef ARC_SHELL_RSP_H_
#define ARC_SHELL_RSP_H_

#define ARC_SHELL_MAX_CMD_LEN			256

extern char arc_shell_common_rspBuffer[ARC_SHELL_MAX_CMD_LEN];

typedef void (*arc_shell_postCtx) (void);

void arc_shell_commonPost(ascii *);
void arc_shell_commonToUpper(char * resp, char * p);
void arc_shell_commonSubscribe(arc_shell_postCtx);

#endif /* ARC_SHELL_RSP_H_ */