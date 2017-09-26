/*
 * buffer_Q512Flash.c
 *
 * Created: 18/6/2558 14:02:12
 *  Author: pan
 */ 
#include "adl_global.h"
#include "buffer_Q512Flash.h"
#include "buffer.h"

/*
 * First In First Out
 * Architecture
*
*########################################################
*|                                FLASH AT45DB161                                    |
*########################################################
*+---------------------------------------------------------------------------------------------+
*|                                       Data[512]                                         |
*+---------------------------------------------------------------------------------------------+
*
* The header character '$' is identifier that record has existed yet.
*/

#define	_BUFFER_Q512FLASH_SECTOR_SIZE			512
#define	_BUFFER_Q512FLASH_RAWDATA_SIZE			_BUFFER_Q512FLASH_SECTOR_SIZE

#define _BUFFER_Q512FLASH_FLASH_SECTOR_ALL		200
#define _BUFFER_Q512FLASH_FLASH_SECTOR_LAST		(_BUFFER_Q512FLASH_FLASH_SECTOR_ALL - 1)// Begin from 0 ->>
#define _BUFFER_Q512FLASH_FLASH_Q_FIRST_ADDR		(0)
#define _BUFFER_Q512FLASH_FLASH_Q_LAST_ADDR		_BUFFER_Q512FLASH_FLASH_SECTOR_LAST

#define	_BUFFER_Q512FLASH_TOTAL_NUMBER			_BUFFER_Q512FLASH_FLASH_SECTOR_ALL

#define _BUFFER_ALLOW_SKIP_BAD_FILE				1

#define _BUFFER_START_FILE_INDEX_NB				1

typedef struct _INDEX_ATTR_T{
	u32 idnumb;
}index_attr_t;

typedef struct _BUFFER_INDEX_T {
	index_attr_t oldest;
	index_attr_t newest;
	uint16_t	nb_allfile;
}buffer_index_t;

static buffer_index_t BuffIndex;

static xSemaphoreHandle buff_sem = NULL;
static const portTickType sem_block_wait = portMAX_DELAY;//5000UL / portTICK_RATE_MS;
static u8 *pBuff_stack = &buffer_accessCentralStack[0];

extern void at45dbx_wait_ready(void);

static void BUFFER_INIT_INDEX(void) {
	BuffIndex.nb_allfile = 0;
	BuffIndex.newest.idnumb = 0;
	BuffIndex.oldest.idnumb = _BUFFER_START_FILE_INDEX_NB;
}

bool buffer_Q512Flash_isfull(void){
	return ( BuffIndex.nb_allfile == _BUFFER_Q512FLASH_FLASH_SECTOR_ALL) ? TRUE:FALSE;
}

s32 buffer_Q512Flash_size(void) {
	s32 item_size = -1;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		if(BuffIndex.nb_allfile) {
			item_size = adl_flhExist(_BUFFER_Q512FLASH_FLASH_DIR,BuffIndex.oldest.idnumb);
		}
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ512F size Locked\r\n");
	return item_size;
}

u32 buffer_Q512Flash_time(void) {
	u32 item_time = 0;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		if(BuffIndex.nb_allfile) {
			item_time = BuffIndex.oldest.idnumb;
		}
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ512F time Locked\r\n");
	return item_time;
}

s32 buffer_Q512Flash_delete ( void ) {
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		if(BuffIndex.nb_allfile) {
			#if(_BUFFER_ALLOW_SKIP_BAD_FILE)
			adl_flhErase(_BUFFER_Q512FLASH_FLASH_DIR, BuffIndex.oldest.idnumb);
			
			at45dbx_wait_ready();
			
			BuffIndex.oldest.idnumb++;
			BuffIndex.nb_allfile--;
			
			#else
			if(adl_flhErase(_BUFFER_Q512FLASH_FLASH_DIR, BuffIndex.oldest.idnumb)==OK) {
				BuffIndex.oldest.idnumb++;
				BuffIndex.nb_allfile--;
			}
			#endif
		}
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ512F delete Locked\r\n");
	return BuffIndex.nb_allfile;
}

s32 buffer_Q512Flash_erase(void) {
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		adl_flhClear(_BUFFER_Q512FLASH_FLASH_DIR);
		BUFFER_INIT_INDEX();
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ512F erase Locked\r\n");
	return 0;
}
s32 buffer_Q512Flash_put(uint8_t *p_uc, uint16_t len) {
	u32 item = 0;
	u8 *pWriteBuff = pBuff_stack;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		wm_memcpy(pWriteBuff,p_uc, len );
		item = BuffIndex.newest.idnumb + 1;
		if(adl_flhWrite(_BUFFER_Q512FLASH_FLASH_DIR,item,len, pWriteBuff)==OK) {
			BuffIndex.newest.idnumb = item;
			BuffIndex.nb_allfile++;
			
			at45dbx_wait_ready();
			
		}else vPrintf("\r\nQ512F Write: item %ld Eror\r\n",item);
		
		if(BuffIndex.nb_allfile > _BUFFER_Q512FLASH_FLASH_SECTOR_ALL) {
			adl_flhErase(_BUFFER_Q512FLASH_FLASH_DIR, BuffIndex.oldest.idnumb);
			
			at45dbx_wait_ready();
			
			BuffIndex.oldest.idnumb++;
			BuffIndex.nb_allfile--;

		}
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ512F put Locked\r\n");
	return BuffIndex.nb_allfile;
}

s32 buffer_Q512Flash_get(uint8_t *p_uc, uint16_t len) {
	u8 *pReadBuff = pBuff_stack;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		if(BuffIndex.nb_allfile) {
			if(adl_flhRead(_BUFFER_Q512FLASH_FLASH_DIR,BuffIndex.oldest.idnumb,len, pReadBuff)==OK) {
				wm_memcpy((u8*)p_uc,pReadBuff, len );
			}
		}
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ512F get Locked\r\n");
	return BuffIndex.nb_allfile;
}

s32 buffer_Q512Flash_sort(void) {
	u32 serial = 0;
	FRESULT res;
	DIR dir;
	FILINFO fno;
	char *pc_fn;
	
	//s32 ret = _BUFFER_Q512FLASH_NO_EXIST_DATA;
	
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		res = f_opendir(&dir, _BUFFER_Q512FLASH_FLASH_DIR);                       /* Open the directory */
		if (res == FR_OK) {
			
			BUFFER_INIT_INDEX();
			
			for(;!BuffIndex.nb_allfile;) {
				res = f_readdir(&dir, &fno);             /* Read a directory item */
				pc_fn = &fno.fname[0];
				if (res != FR_OK || *pc_fn == 0) break;  /* Break on error or end of dir */
				if(*pc_fn == '.')continue;
				
				BuffIndex.nb_allfile++;
				serial = wm_hexatoi(pc_fn,strlen(pc_fn));
				BuffIndex.newest.idnumb = serial;
				BuffIndex.oldest.idnumb = serial;
			}
			
			for (;BuffIndex.nb_allfile;) {
				res = f_readdir(&dir, &fno);             /* Read a directory item */
				pc_fn = &fno.fname[0];
				if (res != FR_OK || *pc_fn == 0) break;  /* Break on error or end of dir */
				if(*pc_fn == '.')continue;
				
				BuffIndex.nb_allfile++;
				serial = wm_hexatoi(pc_fn,strlen(pc_fn));
				if(serial >= BuffIndex.newest.idnumb) { // Test Object is Newer than Front Time
					BuffIndex.newest.idnumb = serial; // Update The Newest Object
				}
				if(serial <= BuffIndex.oldest.idnumb) { // Test Object is Older than Rear Time
					BuffIndex.oldest.idnumb = serial; // Update The Oldest Object
				}
			}
			
			printf("\r\nQ512F Sorted = %d file ( %lu -> %lu)",BuffIndex.nb_allfile,BuffIndex.oldest.idnumb,BuffIndex.newest.idnumb);
			
			f_closedir(&dir);
		}
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ512F sort Locked\r\n");
	return BuffIndex.nb_allfile;
}


s32 buffer_Q512Flash_nbitem(void) {
	return BuffIndex.nb_allfile;
}

s32 buffer_Q512Flash_create(xSemaphoreHandle sem) {
	s32 ret = ERROR;
	if(!buff_sem) {
		buff_sem = sem;
		BUFFER_INIT_INDEX();
		ret = drv_fram24c16_Subscribe(0);
		ret = adl_flhSubscribe(_BUFFER_Q512FLASH_FLASH_DIR, _BUFFER_Q512FLASH_TOTAL_NUMBER);
	}
	return ret;
}