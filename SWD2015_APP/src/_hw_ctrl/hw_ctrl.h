/*
 * hw_ctrl.h
 *
 * Created: 20/3/2558 14:27:57
 *  Author: pan
 */ 


#ifndef HW_CTRL_H_
#define HW_CTRL_H_

void hw_ctrl_jtag_Disable(void);

void hw_ctrl_a2d_ResetBus(void);

void hw_ctrl_fram_ResetBus(void);
void hw_ctrl_fram_Protect(bool bEnable);

void hw_ctrl_3v6_Reg(bool bExtPower);
void hw_ctrl_3v3_Power(bool bEnable);

void hw_ctrl_at45_Reset(bool bEnable);

void hw_ctrl_gps_ModuleOn(void);
void hw_ctrl_gps_Reset(bool bEnable);
void hw_ctrl_gps_Enable(bool bEnable);
void hw_ctrl_gps_AntPower(bool bEnable);
bool hw_ctrl_gps_Wakeup(void);

void hw_ctrl_gsm_Reset(bool bEnable);
void hw_ctrl_gsm_Power(bool bEnable);

void hw_ctrl_p5v_Power(bool bEnable);

void hw_ctrl_wd_Toggle(void);
void hw_ctrl_wd_Disable(void);
void hw_ctrl_wd_Enable(void);




#endif /* HW_CTRL_H_ */