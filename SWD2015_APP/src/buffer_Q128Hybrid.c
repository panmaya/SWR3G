/*
 * buffer_Q128Hybrid.c
 *
 * Created: 23/6/2558 17:56:03
 *  Author: pan
 */ 
#include "adl_global.h"
#include "buffer_Q128Hybrid.h"
#include "buffer.h"

/* First In First Out
 * Architecture
*##########################################################
|                                 F-RAM 24C16                                                |
*##########################################################
*+------------------------------------------------------------------------------------------------+  
*+------------------- +-------------------------------+----------------------+-  --------------+--------------------+   
 Size[1]            | Time Stamp[4]       |   Data[123]   |    
*+-------------------+--------------------------------+-------------------+-------------------+------------------+      
*...................................................................................................................................|
###########################################################
/                              General Purpose F-RAM                                                    /
###########################################################
*+--------------------+--------------------------------------------------+----------------------+   
*| Header = "$"  |              Reserve_0[127]                                      |     
*+-------------------+--------------------------------------------------+----------------------+         
*##############################################################
*|                                FLASH AT45DB161                                    | 
*######################################################## #####
*+------------------- +-------------------------------+----------------------+-  --------------+--------------------+
*+-------------------+--------------------------------+-------------------+-------------------+------------------+
###########################################################
*                                                                              
* The header character '$' is identifier that record has existed yet.
*/

#define _BUFFER_Q128HYBRID_F_RAM_Q_CAPACITY		4096// = (8192 /*8K BYTE for fm24cl16*/ / 2) /*Half reserved for the other purpose*/
#define	_BUFFER_Q128HYBRID_SECTOR_SIZE			128
#define _BUFFER_Q128HYBRID_F_RAM_HEAD_LEN		5
#define	_BUFFER_Q128HYBRID_F_RAM_DATA_LEN		123
#define _BUFFER_Q128HYBRID_F_RAM_Q_NUMBER		31// = (_BUFFER_Q128HYBRID_F_RAM_Q_CAPACITY / _BUFFER_Q128HYBRID_SECTOR_SIZE) - 1

#define	_BUFFER_Q128HYBRID_F_RAM_GPBMEM_ADDR	31//(_BUFFER_Q128HYBRID_F_RAM_SECTOR_ALL - 1)// Begin from 0 ----> 
#define	_BUFFER_Q128HYBRID_F_RAM_GPBMEM_SIZE	127

#define _BUFFER_Q128HYBRID_FLASH_CAPACITY		(2097152 /*2M BYTE for AT45DB161*/)
#define _BUFFER_Q128HYBRID_FLASH_Q_NUMBER		2000//3068

#define _BUFFER_START_FILE_INDEX_NB				1

typedef struct _BUFFER_FIFO_INDEX_T {
	u32 rearFileId;//Oldest-Read
	u32 frontFileId;//Newest-Write
	u16	nb_allfile;
}buffer_fifo_index_t;

typedef struct _BUFFER_FRAM_T {
	u8  size;
	u32 idnumb;
}buffer_fram_t;

static buffer_fifo_index_t fifoIndex;

static xSemaphoreHandle buff_sem = NULL;
static const portTickType sem_block_wait = portMAX_DELAY;//5000UL/ portTICK_RATE_MS;
static u8 *pBuff_stack = &buffer_accessCentralStack[0];
static buffer_fram_t fram_tab[_BUFFER_Q128HYBRID_F_RAM_Q_NUMBER];

extern void at45dbx_wait_ready(void);
extern void i2c_reset(void);

static void BUFFER_INIT_INDEX(void) {
	fifoIndex.nb_allfile = 0;
	fifoIndex.frontFileId = 0;
	fifoIndex.rearFileId = _BUFFER_START_FILE_INDEX_NB;
}

static s8 buf_framExist(u32 fileid) {
	for(uint8_t i = 0; i < _BUFFER_Q128HYBRID_F_RAM_Q_NUMBER; i++) {
		if(fram_tab[i].idnumb == fileid) return fram_tab[i].size;
	}
	return 0;
}

static s8 buf_framErase(u32 fileid) {
	for(uint8_t i = 0; i < _BUFFER_Q128HYBRID_F_RAM_Q_NUMBER; i++) {
		if(fram_tab[i].idnumb == fileid) {
			uint8_t buff[5] = {0,0,0,0,0};
			if(OK==drv_fram24c16_Write(i *_BUFFER_Q128HYBRID_SECTOR_SIZE,sizeof(buff),(u8*)buff)) {
				fram_tab[i].size = 0;
				fram_tab[i].idnumb = 0;
				return OK;
			}else i2c_reset();
		}
	}
	return ERROR;
}

static s8 buf_framWrite(u32 fileid, uint8_t len, uint8_t *p_uc) {
	for(uint8_t i = 0; i < _BUFFER_Q128HYBRID_F_RAM_Q_NUMBER; i++) {
		if(fram_tab[i].idnumb == 0) {
			uint8_t buff[5] = {len,0,0,0,0};
			buff[1] = (uint8_t)(fileid >> 24)&0xFF;
			buff[2] = (uint8_t)(fileid >> 16)&0xFF;
			buff[3] = (uint8_t)(fileid >> 8)&0xFF;
			buff[4] = (uint8_t)(fileid >> 0)&0xFF;
			//for(u8 i = 1 ; i < 5 ; i++) {
			//	buff[i] = (uint8_t)(fileid << ((i-1)*8))&0xFF;
			//}
			if(OK==drv_fram24c16_Write(i *_BUFFER_Q128HYBRID_SECTOR_SIZE,sizeof(buff),(u8*)buff)) {
				if(OK==drv_fram24c16_Write((i *_BUFFER_Q128HYBRID_SECTOR_SIZE)+_BUFFER_Q128HYBRID_F_RAM_HEAD_LEN,len,(u8*)p_uc)) {
					fram_tab[i].size = len;
					fram_tab[i].idnumb = fileid;
					return OK;
				}else i2c_reset();
			}else i2c_reset();
		}
	}
	return ERROR;
}

static s8 buf_framRead(u32 fileid, uint8_t len, uint8_t *p_uc) {
	for(uint8_t i = 0; i < _BUFFER_Q128HYBRID_F_RAM_Q_NUMBER; i++) {
		if(fram_tab[i].idnumb == fileid) {
			if(OK==drv_fram24c16_Read((i *_BUFFER_Q128HYBRID_SECTOR_SIZE)+_BUFFER_Q128HYBRID_F_RAM_HEAD_LEN,len,(u8*)p_uc))return OK;
			else i2c_reset();
		}
	}
	return ERROR;
}

static s8 buf_framClear(void) {
	uint8_t buff[5] = {0,0,0,0,0};
	for(uint8_t i = 0; i < _BUFFER_Q128HYBRID_F_RAM_Q_NUMBER; i++) {
		if(OK==drv_fram24c16_Write(i *_BUFFER_Q128HYBRID_SECTOR_SIZE,sizeof(buff),(u8*)buff)){
		}else i2c_reset();
		fram_tab[i].size = 0;
		fram_tab[i].idnumb = 0;
	}
	return OK;
}


s32 buffer_Q128Hybrid_size(void) {
	s32 item_size = -1;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		if(fifoIndex.nb_allfile) {
			item_size = buf_framExist(fifoIndex.rearFileId);
			if(item_size==0)item_size = adl_flhExist(_BUFFER_Q128HYBRID_FLASH_DIR,fifoIndex.rearFileId);
		}
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ128H size Locked\r\n");
	return item_size;
}

u32 buffer_Q128Hybrid_time(void) {
	u32 item_time = 0;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		if(fifoIndex.nb_allfile) {
			item_time = fifoIndex.rearFileId;
		}
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ128H time Locked\r\n");
	return item_time;
}

s32 buffer_Q128Hybrid_delete ( void ) {
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		if(fifoIndex.nb_allfile) {
			if(OK!=buf_framErase(fifoIndex.rearFileId)) {
				adl_flhErase(_BUFFER_Q128HYBRID_FLASH_DIR, fifoIndex.rearFileId);
				at45dbx_wait_ready();
			}
			//These Allow To Skip In Case Bad file found
			fifoIndex.rearFileId++;
			fifoIndex.nb_allfile--;
		}
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ128H delete Locked\r\n");
	return fifoIndex.nb_allfile;
}

s32 buffer_Q128Hybrid_erase( uint8_t opt ) {
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		if(opt<=1) { // 0 = erase ALL ,  1 = Erase FRAM only
			buf_framClear();
		}
		if((opt==2)||(opt==0)) { // 0 = erase ALL ,  2 = Erase FLASH only
			adl_flhClear(_BUFFER_Q128HYBRID_FLASH_DIR);
			at45dbx_wait_ready();
		}
		BUFFER_INIT_INDEX();
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ128H erase Locked\r\n");
	return 0;
}

s32 buffer_Q128Hybrid_put(uint8_t *p_uc, uint8_t len) {
	u32 item = 0;
	u8 *pWriteBuff = pBuff_stack;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {

		if((fifoIndex.nb_allfile+1) > (_BUFFER_Q128HYBRID_FLASH_Q_NUMBER + _BUFFER_Q128HYBRID_F_RAM_Q_NUMBER)) {
			if(0==buf_framExist(fifoIndex.rearFileId)) {
				adl_flhErase(_BUFFER_Q128HYBRID_FLASH_DIR, fifoIndex.rearFileId);
				at45dbx_wait_ready();
			}else buf_framErase(fifoIndex.rearFileId);
			//These Allow To Skip In Case Bad file found
			fifoIndex.rearFileId++;
			fifoIndex.nb_allfile--;

		}
		
		len = len > _BUFFER_Q128HYBRID_F_RAM_DATA_LEN ? _BUFFER_Q128HYBRID_F_RAM_DATA_LEN : len;
		wm_memcpy(pWriteBuff, p_uc, len );
		item = fifoIndex.frontFileId + 1;
		
		if(OK!=buf_framWrite(item, len, pWriteBuff)) {
			if(adl_flhWrite(_BUFFER_Q128HYBRID_FLASH_DIR,item,len, pWriteBuff)==OK) {
				fifoIndex.frontFileId = item;
				fifoIndex.nb_allfile++;
				at45dbx_wait_ready();
				
			}else vPrintf("\r\nQ128H Write: item %ld Eror\r\n",item);
		}
		else {
			fifoIndex.frontFileId = item;
			fifoIndex.nb_allfile++;
		}

		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ128H put Locked\r\n");
	return fifoIndex.nb_allfile;
}

s32 buffer_Q128Hybrid_get(uint8_t *p_uc, uint8_t len) {
	u8 *pReadBuff = pBuff_stack;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		if(fifoIndex.nb_allfile) {
			len = len > _BUFFER_Q128HYBRID_F_RAM_DATA_LEN ? _BUFFER_Q128HYBRID_F_RAM_DATA_LEN : len;
			if(OK!=buf_framRead(fifoIndex.rearFileId,len, pReadBuff)) {
				if(adl_flhRead(_BUFFER_Q128HYBRID_FLASH_DIR,fifoIndex.rearFileId,len, pReadBuff)==OK) {
					wm_memcpy((u8*)p_uc,pReadBuff, len );
				}
			}else wm_memcpy((u8*)p_uc,pReadBuff, len );
		}
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ128H get Locked\r\n");
	return fifoIndex.nb_allfile;
}

s32 buffer_Q128Hybrid_sort(void) {
	u32 serial = 0;
	FRESULT res;
	DIR dir;
	FILINFO fno;
	char *pc_fn;

	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		
		BUFFER_INIT_INDEX();
		
		for(uint8_t i = 0; i < _BUFFER_Q128HYBRID_F_RAM_Q_NUMBER; i++) {
			uint8_t buff[5] = {0,0,0,0,0};
			buffer_fram_t dummy = {0,0};
				
			if(OK==drv_fram24c16_Read(i *_BUFFER_Q128HYBRID_SECTOR_SIZE,sizeof(buff),(u8*)buff)){
				
				dummy.size = buff[0];
				dummy.idnumb = 0;
				for(u8 i = 1 ; i < 5 ; i++) {
					dummy.idnumb <<= 8;
					dummy.idnumb |= buff[i];
				}
				
				if(dummy.idnumb == 0)continue;
				
				fram_tab[i].idnumb = dummy.idnumb;
				fram_tab[i].size = dummy.size;
				
				if(!fifoIndex.nb_allfile) {
					fifoIndex.nb_allfile++;
					serial = fram_tab[i].idnumb;
					fifoIndex.frontFileId = serial;
					fifoIndex.rearFileId = serial;
				}
				else {
					fifoIndex.nb_allfile++;
					serial = fram_tab[i].idnumb;
					if(serial >= fifoIndex.frontFileId) { // Test Object is Newer than Front Time
						fifoIndex.frontFileId = serial; // Update The Newest Object
					}
					if(serial <= fifoIndex.rearFileId) { // Test Object is Older than Rear Time
						fifoIndex.rearFileId = serial; // Update The Oldest Object
					}
				}
			}
		}
		
		res = f_opendir(&dir, _BUFFER_Q128HYBRID_FLASH_DIR);                       /* Open the directory */
		
		if (res == FR_OK) {
			
			for(;!fifoIndex.nb_allfile;) {
				res = f_readdir(&dir, &fno);             /* Read a directory item */
				pc_fn = &fno.fname[0];
				if (res != FR_OK || *pc_fn == 0) break;  /* Break on error or end of dir */
				if(*pc_fn == '.')continue;
				
				fifoIndex.nb_allfile++;
				serial = wm_hexatoi(pc_fn,strlen(pc_fn));
				fifoIndex.frontFileId = serial;
				fifoIndex.rearFileId = serial;
			}
			
			for (;fifoIndex.nb_allfile;) {
				res = f_readdir(&dir, &fno);             /* Read a directory item */
				pc_fn = &fno.fname[0];
				if (res != FR_OK || *pc_fn == 0) break;  /* Break on error or end of dir */
				if(*pc_fn == '.')continue;
				
				fifoIndex.nb_allfile++;
				serial = wm_hexatoi(pc_fn,strlen(pc_fn));
				if(serial >= fifoIndex.frontFileId) { // Test Object is Newer than Front Time
					fifoIndex.frontFileId = serial; // Update The Newest Object
				}
				if(serial <= fifoIndex.rearFileId) { // Test Object is Older than Rear Time
					fifoIndex.rearFileId = serial; // Update The Oldest Object
				}
			}
			
			f_closedir(&dir);
		}
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ128H sort Locked\r\n");
	printf("\r\nQ128H Sorted = %d file ( %lu -> %lu)",fifoIndex.nb_allfile,fifoIndex.rearFileId,fifoIndex.frontFileId);
	return fifoIndex.nb_allfile;
}


s32 buffer_Q128Hybrid_nbitem(void) {
	return fifoIndex.nb_allfile;
}

s32 buffer_Q128Hybrid_create(xSemaphoreHandle sem) {
	s32 ret = ERROR;
	if(!buff_sem) {
		buff_sem = sem;
		BUFFER_INIT_INDEX();
		ret = drv_fram24c16_Subscribe(0);
		ret = adl_flhSubscribe(_BUFFER_Q128HYBRID_FLASH_DIR, _BUFFER_Q128HYBRID_FLASH_Q_NUMBER);
	}
	return ret;
}


s32 buffer_Q128Hybrid_ReadGPBMEM(uint8_t *p_uc, uint8_t len) {
	s32 ret = ERROR;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		u8 *pReadBuff = pBuff_stack;
		
		if(drv_fram24c16_Read((_BUFFER_Q128HYBRID_F_RAM_GPBMEM_ADDR *_BUFFER_Q128HYBRID_SECTOR_SIZE),_BUFFER_Q128HYBRID_SECTOR_SIZE,pReadBuff)>=0) {
			if(pReadBuff[0]== '$') {
				wm_memcpy((u8*)p_uc,pReadBuff+1, len > _BUFFER_Q128HYBRID_F_RAM_GPBMEM_SIZE ? _BUFFER_Q128HYBRID_F_RAM_GPBMEM_SIZE : len);
				ret = 1;
			}else ret = _BUFFER_Q128HYBRID_NO_EXIST_DATA;
		}else {
			i2c_reset();
			ret =  _BUFFER_Q128HYBRID_READ_FAILED;
		}
		
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ128H Read GPFRAM Locked\r\n");
	return ret;
}

s32 buffer_Q128Hybrid_WriteGPBMEM(uint8_t *p_uc, uint8_t len) {
	s32 ret = ERROR;
	if( xSemaphoreTake( buff_sem, sem_block_wait ) == pdTRUE ) {
		u8 *pWriteBuff = pBuff_stack;
		
		pWriteBuff[0] = '$';
		wm_memcpy(pWriteBuff+1,p_uc, len > _BUFFER_Q128HYBRID_F_RAM_GPBMEM_SIZE ? _BUFFER_Q128HYBRID_F_RAM_GPBMEM_SIZE : len);
		if(drv_fram24c16_Write((_BUFFER_Q128HYBRID_F_RAM_GPBMEM_ADDR * _BUFFER_Q128HYBRID_SECTOR_SIZE),_BUFFER_Q128HYBRID_SECTOR_SIZE, pWriteBuff)>=0) {
			ret = 1;
		}else {
			i2c_reset();
			ret = ERROR;
		}
		
		xSemaphoreGive( buff_sem );
	}else vPrintf("\r\nQ128H Write GPFRAM Locked\r\n");
	return ret;
}
