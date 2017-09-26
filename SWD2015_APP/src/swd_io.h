/*
 * swd_io.h
 *
 * Created: 2016-10-21 3:11:22 PM
 *  Author: Panmaya
 */ 


#ifndef SWD_IO_H_
#define SWD_IO_H_

void swdio_init(void);

void swdio_light_ig_set(bool status);
void swdio_light_pto_set(bool status);
void swdio_light_opt_set(bool status);

u8 swdio_get_ig_status(void);
u8 swdio_get_pto1_status(void);
u8 swdio_get_pto2_status(void);
uint32_t swdio_get_pto2_freq(void);
u8 swdio_get_clrbuf_status(void);

void swdio_set_outp1_status(bool status);
void swdio_set_outp2_status(bool status);

u8 swdio_get_logic_pto1(void);
u8 swdio_get_logic_pto2(void);
u8 swdio_get_logic_ig(void);



#endif /* SWD_IO_H_ */