/*
 * adl_flashnew2.c
 *
 * Created: 16/5/2558 16:22:46
 *  Author: pan
 */ 
#include <adl_global.h>

#define ADL_FLASH_HIDE_OK_MSG		1
#define ADL_FLASH_USE_DELAY			0

extern void at45dbx_wait_ready(void);
/*
+-----------------------------------------------------------------------------------------------------------------------------------+
 FA_READ                    Specifies read access to the object. Data can be read from the file.
                                  Combine with FA_WRITE for read-write access.
 FA_WRITE                   Specifies write access to the object. Data can be written to the file.
                                  Combine with FA_READ for read-write access.
 FA_OPEN_EXISTING     Opens the file. The function fails if the file is not existing. (Default)
 FA_OPEN_ALWAYS      Opens the file, if it is existing. If not, the function creates the new file.
 FA_CREATE_NEW        Creates a new file. The function fails if the file is already existing.
 FA_CREATE_ALWAYS   Creates a new file. If the file is existing, it is truncated and overwritten.
 +-----------------------------------------------------------------------------------------------------------------------------------+
*/

static uint8_t ADL_FLASH_LOGICAL_Numb = 0;
static s32 ADL_FLASH_Err = FR_OK;

static ascii* adl_flash_fatfs_sfault(FRESULT res) {
	static ascii buffer[30] = {""};
	ADL_FLASH_Err = res;
	switch(res){
	case FR_OK: strcpy(buffer,"FR_OK"); break;
	case FR_DISK_ERR: strcpy(buffer,"FR_DISK_ERR"); break;
	case FR_INT_ERR: strcpy(buffer,"FR_INT_ERR"); break;
	case FR_NOT_READY: strcpy(buffer,"FR_NOT_READY"); break;
	case FR_NO_FILE: strcpy(buffer,"FR_NO_FILE"); break;
	case FR_NO_PATH: strcpy(buffer,"FR_NO_PATH"); break;
	case FR_INVALID_NAME: strcpy(buffer,"FR_INVALID_NAME"); break;
	case FR_DENIED: strcpy(buffer,"FR_DENIED"); break;
	case FR_EXIST: strcpy(buffer,"FR_EXIST"); break;
	case FR_INVALID_OBJECT: strcpy(buffer,"FR_INVALID_OBJECT"); break;
	case FR_WRITE_PROTECTED: strcpy(buffer,"FR_WRITE_PROTECTED"); break;
	case FR_INVALID_DRIVE: strcpy(buffer,"FR_INVALID_DRIVE"); break;
	case FR_NOT_ENABLED: strcpy(buffer,"FR_NOT_ENABLED"); break;
	case FR_NO_FILESYSTEM: strcpy(buffer,"FR_NO_FILESYSTEM"); break;
	case FR_MKFS_ABORTED: strcpy(buffer,"FR_MKFS_ABORTED"); break;
	case FR_TIMEOUT: strcpy(buffer,"FR_TIMEOUT"); break;
	case FR_LOCKED: strcpy(buffer,"FR_LOCKED"); break;
	case FR_NOT_ENOUGH_CORE: strcpy(buffer,"FR_NOT_ENOUGH_CORE"); break;
	case FR_TOO_MANY_OPEN_FILES: strcpy(buffer,"FR_TOO_MANY_OPEN_FILES"); break;
	case FR_INVALID_PARAMETER: strcpy(buffer,"FR_INVALID_PARAMETER"); break;
	default: strcpy(buffer,"FR_ERROR_UNKNOWN"); break;
	}
	return buffer;
}


s8 adl_flash_init(uint8_t lun_id) {
	if(lun_id < 10) {
		ADL_FLASH_LOGICAL_Numb = lun_id;
	}else return ERROR;
	return ADL_FLASH_LOGICAL_Numb;
}

s32 adl_flash_getErr(void) { return ADL_FLASH_Err; }
	
/**
  * @}
  */

/**
  @defgroup adl_flash_public_IFS_op1 FLASH API access functions
  @ingroup adl_flash_public_IFS
  */

/**
  * @addtogroup adl_flash_public_IFS_op1
  * @{
  */

/** 
 * @brief Flash service subscription function
 * @par Description: This function subscribes to a set of objects identified by the given Handle.
 * @param [in]  Handle 			The Handle of the set of objects to subscribe to.
 * @param [in]  NbObjectRes 	The number of objects related to the given handle. It means that the IDs available for this handle are in the range [ 0 , (NbObjectRes – 1) ].
 * @retval 		OK 								Operation successful
 * @retval 		ADL_RET_ERR_PARAM 				on parameter error
 * @retval 		ADL_RET_ERR_ALREADY_SUBSCRIBED 	if space is already created for this handle
 * @retval 		ADL_FLH_RET_ERR_NO_ENOUGH_IDS 	if there are no more enough object IDs to allocate the handle.
 * @retval 		ADL_RET_ERR_SERVICE_LOCKED 		if the function was called from a low level Interrupt handler (the function is forbidden in this context).
 */
s8 adl_flhSubscribe ( const ascii Handle[6], u32 NbObjectsRes ){
	FRESULT res;
	DIR dirs;
	ascii path[36] = {"0:"};
	s8 retval = ADL_RET_ERR_PARAM;
	
	if((!NbObjectsRes)||(!Handle)||(!Handle[0])||(strlen(Handle)>8)) {
		vPrintf("\r\n adl_flhSubscribe [%s] error parameter\r\n",Handle);
		retval = ADL_RET_ERR_PARAM;
	}
	else {
		path[0] += ADL_FLASH_LOGICAL_Numb;
		snprintf(path+2,sizeof(path)-2, "%s", Handle);//"0:Handle"
		res = f_opendir( &dirs, path);//Open "0:Handle"
		if(res != FR_OK) { // No Directory Found
			vPrintf("\r\n adl_flhSubscribe [%s] %s\r\n", path,adl_flash_fatfs_sfault(res));
			res = f_mkdir(path);// Make Directory "0:Handle"
			if(res == FR_OK) {//Make Directory Success
				#if(!ADL_FLASH_HIDE_OK_MSG)
				vPrintf("\r\n adl_flhSubscribe Dir[%s] Created OK\r\n", path);
				#endif
				#if ADL_FLASH_USE_DELAY
				vTaskDelay(200);
				#endif
				retval = OK;
			}else {//Make Directory UnSuccess
				vPrintf("\r\n adl_flhSubscribe Dir[%s] Created Error- %s\r\n", path,adl_flash_fatfs_sfault(res));
				retval = ADL_RET_ERR_SERVICE_LOCKED;
			}
		}else {// Directory existing
			#if(!ADL_FLASH_HIDE_OK_MSG)
			vPrintf("\r\n adl_flhSubscribe [%s] Already Subscribed\r\n", path);
			#endif
			f_closedir(&dirs);
			retval = ADL_RET_ERR_ALREADY_SUBSCRIBED;
		}
	}
	
	return retval;
}

/** 
 * @brief Test flash object existence function
 * @par Description: This function checks if a flash object exists from the given Handle at the given ID in the flash memory allocated to the ADL developer.
 * @param [in]  Handle 			The Handle of the subscribe set of objects.
 * @param [in]  ID 				The ID of the flash object to investigate (in the range allocated to the provided Handle).
 * @retval 		objectlength						on success the requested Flash object length 
 * @retval 		0K 									if the object does not exist.
 * @retval 		ADL_RET_ERR_UNKNOWN_HDL 			if handle is not subscribed
 * @retval 		ADL_FLH_RET_ERR_ID_OUT_OF_RANGE 	if ID is out of handle range
 * @retval 		ADL_RET_ERR_SERVICE_LOCKED 			if the function was called from a low level Interrupt handler (the function is forbidden in this context).
 */
//s32 adl_flhExist( ascii * Handle, u16 ID );
s32 adl_flhExist(const ascii Handle[6], u32 ID ) {
	FRESULT res;
	DIR dirs;
	ascii path[36] = {"0:"};
	s32 retval = ADL_RET_ERR_PARAM;
	FILINFO fno;
	#if FF_USE_LFN
	char c_lfn[_MAX_LFN + 1]={""};
	fno.lfname = c_lfn;
	fno.lfsize = _MAX_LFN;
	#endif
	
	if((!Handle)||(!Handle[0])||(strlen(Handle)>8)) {
		vPrintf("\r\n adl_flhExist [%s] error parameter\r\n",Handle);
		retval = ADL_RET_ERR_PARAM;
	}
	else {
		path[0] += ADL_FLASH_LOGICAL_Numb;
		snprintf(path+2,sizeof(path)-2, "%s", Handle);//"0:Handle"
		res = f_opendir(&dirs, path);//Open "0:Handle"
		if (res == FR_OK) {//"0:Handle" Directory existing
			uint16_t path_len = strlen(path);
			sprintf(path + path_len, "/%lx",ID);//"0:Handle/ID"
			res = f_stat (path, &fno);//Get Stat file "0:Handle/ID"
			if(res == FR_OK) {
				
				retval = fno.fsize;
				
				#if(!ADL_FLASH_HIDE_OK_MSG)
				vPrintf("\r\n adl_flhExist [%s] size = %ld",path,retval);
				#endif
				
				f_closedir(&dirs);
			}
			else {
				if(res==FR_NO_FILE)vPrintf("\r\n adl_flhExist Check Stat [%s] OK %s\r\n", path,adl_flash_fatfs_sfault(res));
				else vPrintf("\r\n adl_flhExist Check Stat [%s] Error- %s\r\n", path,adl_flash_fatfs_sfault(res));
				retval = ADL_FLH_RET_ERR_ID_OUT_OF_RANGE;
			}
		}
		else {//Directory No Exist
			vPrintf("\r\n adl_flhExist Dir [%s] Error- %s\r\n", path,adl_flash_fatfs_sfault(res));
			retval = ADL_RET_ERR_UNKNOWN_HDL;
		}
	}
	return retval;
}

u32 adl_flhExistTime(const ascii Handle[6], u32 ID ) {
	FRESULT res;
	DIR dirs;
	ascii path[36] = {"0:"};
	u32 retval = 0;
	FILINFO fno;
	#if FF_USE_LFN
	char c_lfn[_MAX_LFN + 1]={""};
	fno.lfname = c_lfn;
	fno.lfsize = _MAX_LFN;
	#endif
		
	if((!Handle)||(!Handle[0])||(strlen(Handle)>8)) {
		vPrintf("\r\n adl_flhExistTime [%s] error parameter\r\n",Handle);
		retval = 0;
	}
	else {
		path[0] += ADL_FLASH_LOGICAL_Numb;
		snprintf(path+2,sizeof(path)-2, "%s", Handle);//"0:Handle"
		res = f_opendir(&dirs, path);//Open "0:Handle"
		if (res == FR_OK) {//"0:Handle" Directory existing
			uint16_t path_len = strlen(path);
			sprintf(path + path_len, "/%lx",ID);//"0:Handle/ID"
			res = f_stat (path, &fno);//Get Stat file "0:Handle/ID"
			if(res == FR_OK) {
				retval = fno.fdate;
				retval = (retval << 16) | fno.ftime;
			}
			else {
				if(res==FR_NO_FILE) vPrintf("\r\n adl_flhExistTime Check Stat [%s] OK %s\r\n", path,adl_flash_fatfs_sfault(res));
				else  vPrintf("\r\n adl_flhExistTime Check Stat [%s] Error- %s\r\n", path,adl_flash_fatfs_sfault(res));
				retval = 0;
			}
			
			f_closedir(&dirs);
		}
		else {//Directory No Exist
			TRACE((0,"adl_flhExistTime Dir [%s] Error- %s", path,adl_flash_fatfs_sfault(res)));
			retval = 0;
		}
	}
	return retval;
}

/** 
 * @brief Read flash object function
 * @par Description: This function reads the flash object from the given Handle at the given ID, for the length provided and stores it in a buffer.
 * @param [in]  Handle 			The Handle of the subscribe set of objects.
 * @param [in]  ID 				The ID of the flash object to read.
 * @param [in]  Len 			The length of the flash object to read.
 * @param [in]  ReadData		The string allocated to store the read flash object.
 * @retval 		OK 									on success
 * @retval 		ADL_RET_ERR_PARAM 					if one at least of the parameters has a bad value.
 * @retval 		ADL_RET_ERR_UNKNOWN_HDL 			if handle is not subscribed
 * @retval 		ADL_FLH_RET_ERR_ID_OUT_OF_RANGE 	if ID is out of handle range
 * @retval 		ADL_FLH_RET_ERR_OBJ_NOT_EXIST 		if the object does not exist.
 * @retval 		ADL_RET_ERR_FATAL 					if a fatal error occurred (ADL_ERR_FLH_READ error event will then occur).
 * @retval 		ADL_RET_ERR_SERVICE_LOCKED 			if the function was called from a low level Interrupt handler (the function is forbidden in this context).
 */
//s8 adl_flhRead( ascii * Handle, u16 ID, u16 Len, u8 * ReadData );
s8 adl_flhRead( const ascii Handle[6], u32 ID, u16 Len, u8 * ReadData ) {
	FRESULT res;
	ascii path[36] = {"0:"};
	FIL file_object;
	s8 retval = ADL_RET_ERR_PARAM;
	
	path[0] += ADL_FLASH_LOGICAL_Numb;
	snprintf(path+2,sizeof(path)-2,"%s/%lx",Handle,ID);//"0:Handle/ID"
	
	res = f_open(&file_object, path, FA_OPEN_EXISTING | FA_READ);
	if(res == FR_OK) {
		UINT byte_read = 0;
		#if(!ADL_FLASH_HIDE_OK_MSG)
		vPrintf("\r\n adl_flhRead Open [%s] OK\n\r",path);
		#endif
		res = f_read(&file_object, ReadData, Len, &byte_read);
		
		if(res == FR_OK) {
			#if(!ADL_FLASH_HIDE_OK_MSG)
			vPrintf("\r\n adl_flhRead Read [%s] = %d Byte\r\n",path,byte_read);
			#endif
			f_close(&file_object);
			retval = OK;
		}
		else {
			vPrintf("\r\n adl_flhRead Read [%s] Error- %s\r\n", path,adl_flash_fatfs_sfault(res));
			retval = ADL_RET_ERR_SERVICE_LOCKED;
		}
	}
	else {
		vPrintf("\r\n adl_flhRead Open [%s] Error-  %s\r\n", path,adl_flash_fatfs_sfault(res));
		retval = ADL_FLH_RET_ERR_OBJ_NOT_EXIST;
	}

	return retval;
}



/** 
 * @brief Write flash object function
 * @par Description: This function writes the flash object from the given Handle at the given ID, for the length provided with the buffer provided. A single flash object can use up to 30 Kbytes of memory.
 * @param [in]  Handle 			The Handle of the subscribe set of objects.
 * @param [in]  ID 				The ID of the flash object to read.
 * @param [in]  Len 			The length of the flash object to write.
 * @param [in]  WriteData		The provided string to write in the flash object.
 * @retval 		OK 									on success
 * @retval 		ADL_RET_ERR_PARAM 					if one at least of the parameters has a bad value.
 * @retval 		ADL_RET_ERR_UNKNOWN_HDL 			if handle is not subscribed
 * @retval 		ADL_FLH_RET_ERR_ID_OUT_OF_RANGE 	if ID is out of handle range
 * @retval 		ADL_RET_ERR_FATAL 					if a fatal error occurred (ADL_ERR_FLH_WRITE error event will then occur).
 * @retval 		ADL_FLH_RET_ERR_MEM_FULL 			if flash memory is full.
 * @retval 		ADL_FLH_RET_ERR_NO_ENOUGH_IDS 		if the object can not be created due to the global ID number limitation.
 * @retval 		ADL_RET_ERR_SERVICE_LOCKED 			if the function was called from a low level Interrupt handler (the function is forbidden in this context).
 */
//s8 adl_flhWrite( ascii * Handle, u16 ID, u16 Len, u8 * WriteData );
s8 adl_flhWrite( const ascii Handle[6], u32 ID, u16 Len, u8 * WriteData ) {
	FRESULT res;
	ascii path[36] = {"0:"};
	FIL file_object;
	s8 retval = ADL_RET_ERR_PARAM;
	
	path[0] += ADL_FLASH_LOGICAL_Numb;
	snprintf(path+2,sizeof(path)-2,"%s/%lx",Handle,ID);//"0:Handle/ID"
	
	res = f_open(&file_object, path, FA_OPEN_ALWAYS | FA_WRITE);
	if(res == FR_OK) {
		UINT byte_write = 0;
		#if(!ADL_FLASH_HIDE_OK_MSG)
		vPrintf("\r\nadl_flhWrite Open [%s] OK\n\r",path);
		#endif
		res = f_write(&file_object, WriteData, Len, &byte_write);
		#if ADL_FLASH_USE_DELAY
		f_sync(&file_object);
		vTaskDelay(200);
		#endif
		
		if(res == FR_OK){
			
			f_close(&file_object);
			
			if(byte_write == 0) {
				vPrintf("\r\n adl_flhWrite Write [%s] Error- 0 Byte\r\n",path);
				retval = ADL_FLH_RET_ERR_MEM_FULL;
			}
			else {
				retval = OK;
			}
		}
		else {
			vPrintf("\r\n adl_flhWrite Write [%s] Error- %s\r\n", path,adl_flash_fatfs_sfault(res));
			retval = ADL_RET_ERR_SERVICE_LOCKED;
		}
	}
	else {
		vPrintf("\r\n adl_flhWrite Open [%s] Error- %s\r\n", path,adl_flash_fatfs_sfault(res));
		retval = ADL_FLH_RET_ERR_ID_OUT_OF_RANGE;
	}
	
	return retval;
}



/** 
 * @brief Erase flash object function
 * @par Description: This function erases the flash object from the given Handle at the given ID.
 * @param [in]  Handle 			The Handle of the subscribe set of objects.
 * @param [in]  ID 				The ID of the flash object to be erased.
 * @note		If ID is set to ADL_FLH_ALL_IDS, all flash objects related to the provided handle will be erased.
 * @retval 		OK 									on success
 * @retval 		ADL_RET_ERR_UNKNOWN_HDL 			if handle is not subscribed
 * @retval 		ADL_FLH_RET_ERR_ID_OUT_OF_RANGE 	if ID is out of handle range
 * @retval 		ADL_FLH_RET_ERR_OBJ_NOT_EXIST 		if the object does not exist
 * @retval 		ADL_RET_ERR_FATAL 					if a fatal error occurred (ADL_ERR_FLH_DELETE error event will then be generated)
 * @retval 		ADL_RET_ERR_SERVICE_LOCKED 			if the function was called from a low level Interrupt handler (the function is forbidden in this context).
 */
s8 adl_flhErase( const ascii Handle[6], u32 ID ){
	FRESULT res;
	ascii path[36] = {"0:"};
	s8 retval = ADL_RET_ERR_PARAM;
	
	path[0] += ADL_FLASH_LOGICAL_Numb;
	snprintf(path+2,sizeof(path)-2,"%s/%lx",Handle,ID);//"0:Handle/ID"
	
	res = f_unlink (path);
	if(res == FR_OK) {
		#if(!ADL_FLASH_HIDE_OK_MSG)
		vPrintf("\r\n adl_flhErase Unlink [%s] Finished\r\n",path);
		#endif
		#if ADL_FLASH_USE_DELAY
		vTaskDelay(200);
		#endif
		retval = OK;
	}
	else {
		vPrintf("\r\n adl_flhErase Unlink [%s] Error- %s\r\n", path,adl_flash_fatfs_sfault(res));
		retval = ADL_RET_ERR_PARAM;
	}
	return retval;
}


s8 adl_flhClear( const ascii Handle[6] ){
	FRESULT res;
	DIR dir;
	int offset = 2;
	s8 retval = ADL_RET_ERR_PARAM;
	ascii path[36] = {"0:"};
	FILINFO fno;
	char *pc_fn;	
	#if FF_USE_LFN
	char c_lfn[_MAX_LFN + 1]={""};
	fno.lfname = c_lfn;
	fno.lfsize = _MAX_LFN;
	#endif
		
	path[0] += ADL_FLASH_LOGICAL_Numb;
	offset += snprintf(path+2,sizeof(path)-2,"%s/",Handle);//"0:Handle/
	
	res = f_opendir(&dir, Handle);  /* Open the directory */
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &fno); /* Read a directory item */

			#if FF_USE_LFN
			pc_fn = *fno.lfname ? fno.lfname : fno.fname;
			#else
			pc_fn = fno.fname;
			#endif
			
			if (res != FR_OK || *pc_fn == 0) break;  /* Break on error or end of dir */
			if(*pc_fn == '.')continue;
			path[offset] = 0;
			strcat(path,pc_fn);//"0:Handle/ID"
			
			at45dbx_wait_ready();
			
			res = f_unlink (path);
			if(res != FR_OK) {
				vPrintf("\r\nf_unlink %s error -%d",path,res);
			}
			vTaskDelay(1);
		}
		f_closedir(&dir);
		retval = OK;
	}
	else {
		vPrintf("\r\n adl_flhClear Open [%s] Error- %s\r\n", path,adl_flash_fatfs_sfault(res));
		retval = ADL_RET_ERR_PARAM;
	}
	return retval;
}


s8 adl_flhReFormat(void){
	FRESULT res;
	s8 retval = ADL_RET_ERR_FATAL;
	TCHAR root_directory[3] = "0:";
	BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */
	
	root_directory[0] = '0' + ADL_FLASH_LOGICAL_Numb;

	res = f_mkfs (  root_directory,	/* Logical drive number */
	FM_FAT,			/* Format option Partitioning rule; 0:FDISK, 1:SFD */
	0,			/* Size of allocation unit (cluster) [byte]; 1-128: sector, >128:bytes, 0: auto */
	work,		/* Pointer to working buffer */
	sizeof(work)			/* Size of working buffer */ );

	//res = f_mkfs(	ADL_FLASH_LOGICAL_Numb	/* Logical drive number */
	//				, 0				/* Partitioning rule 0:FDISK, 1:SFD */
	//				, 0	);			/* Allocation unit size of cluster 1-128: sector, >128:bytes, 0: auto*/

	if (res != FR_OK) {
		vPrintf("\r\n adl_flhReFormat Error- %s\r\n", adl_flash_fatfs_sfault(res));
		retval = OK;
	}
	else {
		#if(!ADL_FLASH_HIDE_OK_MSG)
		vPrintf("\r\n adl_flhReFormat Finished\r\n");
		#endif
	}
	return retval;
}
