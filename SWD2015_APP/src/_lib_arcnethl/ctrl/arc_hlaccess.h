/*
 * arc_hlaccess.h
 *
 * Created: 2016-10-27 2:04:45 PM
 *  Author: Panmaya
 */ 


#ifndef ARC_HLACCESS_H_
#define ARC_HLACCESS_H_


#define ARC_HL_ACCESS_TASK_PIORITY			(tskIDLE_PRIORITY + 3)

typedef bool (*arc_hlCtrl_Cbk)(uint8_t conf_id);

typedef struct _ARC_HL_CTRL_PARA_T  {
	arc_hlCtrl_Cbk func;
	arc_hlCtrl_Cbk delc;
	uint8_t id;
	uint32_t *wd;
}arc_hlctrl_para_t;

extern void arc_hlCtrl_AccessTask (void *pvParameters);



#endif /* ARC_HLACCESS_H_ */