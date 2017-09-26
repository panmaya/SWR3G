/*
 * buffer_Q512Flash.h
 *
 * Created: 18/6/2558 14:03:07
 *  Author: pan
 */ 


#ifndef BUFFER_Q512FLASH_H_
#define BUFFER_Q512FLASH_H_

#define	_BUFFER_Q512FLASH_TRACE_LEV			23
#define _BUFFER_Q512FLASH_TRACE_LEV_23		_BUFFER_Q512FLASH_TRACE_LEV
#define _BUFFER_Q512FLASH_TRACE_LEV_24		(_BUFFER_Q512FLASH_TRACE_LEV_23 + 1)

#define	_BUFFER_Q512FLASH_MEM_SPACE_FULL	-4101

#define _BUFFER_Q512FLASH_FLASH_DIR			"Q512F"

s32 buffer_Q512Flash_put(uint8_t *p_uc, uint16_t len);
s32 buffer_Q512Flash_get(uint8_t *p_uc, uint16_t len);
s32 buffer_Q512Flash_delete(void);
s32 buffer_Q512Flash_size(void);
u32 buffer_Q512Flash_time(void);
bool buffer_Q512Flash_isfull(void);

s32 buffer_Q512Flash_erase(void);
s32 buffer_Q512Flash_create(xSemaphoreHandle sem);
s32 buffer_Q512Flash_sort(void);
s32 buffer_Q512Flash_nbitem(void);


#endif /* BUFFER_Q512FLASH_H_ */