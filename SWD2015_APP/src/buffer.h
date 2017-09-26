/*
 * apt_buffer.h
 *
 * Created: 23/6/2558 11:20:31
 *  Author: pan
 */ 


#ifndef NVMEM_H_
#define NVMEM_H_

#include "buffer_Q512Flash.h"
#include "buffer_Q128Hybrid.h"
#include "buffer_gpbm.h"

extern uint8_t buffer_accessCentralStack[512];

void buffer_init(void);

#define apt_buffer_cpe_add(_p,_l) buffer_Q128Hybrid_put((uint8_t*)_p,_l)
#define apt_buffer_cpe_del() buffer_Q128Hybrid_delete()
#define apt_buffer_cpe_csize() buffer_Q128Hybrid_size()
#define apt_buffer_cpe_ctime() buffer_Q128Hybrid_time()
#define apt_buffer_cpe_get(_p,_l) buffer_Q128Hybrid_get((uint8_t*)_p,_l)
#define apt_buffer_cpe_erase(_opt) buffer_Q128Hybrid_erase(_opt)
#define apt_buffer_cpe_create(_x) buffer_Q128Hybrid_create(_x)
#define apt_buffer_cpe_sort buffer_Q128Hybrid_sort
#define apt_buffer_cpe_nbItem buffer_Q128Hybrid_nbitem
#define apt_buffer_cpe_read_GPBMEM(_p,_l) buffer_Q128Hybrid_ReadGPBMEM((uint8_t*)_p,_l)
#define apt_buffer_cpe_write_GPBMEM(_p,_l) buffer_Q128Hybrid_WriteGPBMEM((uint8_t*)_p,_l)


#define apt_buffer_cpl_add(_p,_l) buffer_Q512Flash_put((uint8_t*)_p,_l)
#define apt_buffer_cpl_del() buffer_Q512Flash_delete()
#define apt_buffer_cpl_csize() buffer_Q512Flash_size()
#define apt_buffer_cpl_ctime() buffer_Q512Flash_time()
#define apt_buffer_cpl_get(_p,_l) buffer_Q512Flash_get((uint8_t*)_p,_l)
#define apt_buffer_cpl_erase buffer_Q512Flash_erase
#define apt_buffer_cpl_create(_x) buffer_Q512Flash_create(_x)
#define apt_buffer_cpl_sort buffer_Q512Flash_sort
#define apt_buffer_cpl_nbItem buffer_Q512Flash_nbitem
#define	apt_buffer_cpl_fully buffer_Q512Flash_isfull

#endif /* NVMEM_H_ */