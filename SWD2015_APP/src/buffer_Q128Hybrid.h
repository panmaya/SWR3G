/*
 * buffer_Q128Hybrid.h
 *
 * Created: 23/6/2558 17:56:25
 *  Author: pan
 */ 


#ifndef BUFFER_Q128HYBRID_H_
#define BUFFER_Q128HYBRID_H_

#define	_BUFFER_Q128HYBRID_TRACE_LEV			23
#define _BUFFER_Q128HYBRID_TRACE_LEV_23		_BUFFER_Q128HYBRID_TRACE_LEV
#define _BUFFER_Q128HYBRID_TRACE_LEV_24		(_BUFFER_Q128HYBRID_TRACE_LEV_23 + 1)


#define	_BUFFER_Q128HYBRID_READ_FAILED		-2
#define	_BUFFER_Q128HYBRID_NO_EXIST_DATA	-255


#define _BUFFER_Q128HYBRID_FLASH_DIR		"Q128H"

s32 buffer_Q128Hybrid_put(uint8_t *p_uc, uint8_t len);
s32 buffer_Q128Hybrid_get(uint8_t *p_uc, uint8_t len);
s32 buffer_Q128Hybrid_delete(void);
s32 buffer_Q128Hybrid_size(void);
u32 buffer_Q128Hybrid_time(void);

s32 buffer_Q128Hybrid_ReadGPBMEM(uint8_t *p_uc, uint8_t len);
s32 buffer_Q128Hybrid_WriteGPBMEM(uint8_t *p_uc, uint8_t len);

s32 buffer_Q128Hybrid_erase(uint8_t opt);
s32 buffer_Q128Hybrid_create(xSemaphoreHandle sem);
s32 buffer_Q128Hybrid_sort(void);
s32 buffer_Q128Hybrid_nbitem(void);



#endif /* BUFFER_Q128HYBRID_H_ */