/*
 * fat_system.c
 *
 * Created: 20/3/2558 17:23:56
 *  Author: pan
 */ 

/**
 * \brief Scan files under a certain path.
 *
 * \param path Folder path.
 *
 * \return Scan result, 1: success.
 */
#include <adl_global.h>
#include "vprinter.h"
#include "fat_system.h"
#include "conf_access.h"


FRESULT fat_system_scan_files(char *path)
{
	FRESULT res;
	DIR dir;
	int32_t i;
	FILINFO fno;
	char *pc_fn;
	#if FF_USE_LFN
	char c_lfn[FF_MAX_LFN + 1];
	fno.lfname = c_lfn;
	fno.lfsize = FF_MAX_LFN;
	#endif

	/* Open the directory */
	res = f_opendir(&dir, path);
	if (res == FR_OK) {
		i = strlen(path);
		for (;;) {
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) {
				break;
			}

			#if FF_USE_LFN
			pc_fn = *fno.lfname ? fno.lfname : fno.fname;
			#else
			pc_fn = fno.fname;
			#endif
			
			if (*pc_fn == '.') continue;

			/* Check if it is a directory type */
			if (fno.fattrib & AM_DIR) {
				sprintf(&path[i], "%s", pc_fn);
				res = fat_system_scan_files(path);
				if (res != FR_OK) {
					vPrintf("\r\nFile: Scan Failed - %d\r\n",res);
					break;
				}

				path[i] = 0;
			} else {
				vPrintf("%s/%s\r\n", path, pc_fn);
			}
		}
		
		f_closedir(&dir);
		
	}else vPuts("\r\nFile: Directory Open Failed\r\n");

	return res;
}

void fat_system_printdiskfree(uint32_t disk_dev_num ) {
	for (; disk_dev_num < get_nb_lun(); disk_dev_num++) {
		FRESULT res;
		FATFS *pfs;
		DWORD fre_clust, fre_sect, tot_sect;
		/* Get volume information and free clusters of drive 0 */
		res = f_getfree("0:", &fre_clust, &pfs);
		/* Get total sectors and free sectors */
		tot_sect = (pfs->n_fatent - 2) * pfs->csize;
		fre_sect = fre_clust * pfs->csize;
		/* Print the free space (assuming 512 bytes/sector) */
		vPrintf("\r\nDisk %d: free %lu KB/%lu KB\r\n",(int)disk_dev_num, fre_sect / 2, tot_sect / 2);
	}
}

static uint8_t fat_system_verify(uint32_t disk_dev_num, bool format){	
	FRESULT res;
	DIR dirs;
	TCHAR root_directory[3] = "0:";
	BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */
	
	/* Declare these as static to avoid stack usage.
	 * They each contain an array of maximum sector size.
	 */
	static FATFS fs;
	root_directory[0] = '0' + disk_dev_num;
	
	/* Clear file system object */
	memset(&fs, 0, sizeof(FATFS));
	
	#ifdef FREERTOS_USED
		
	ctrl_access_init();

	#endif
	
	/* Mount disk*/
	res = f_mount (	&fs,			/* Pointer to the file-system object (NULL:unmount)*/
					root_directory,	/* Logical drive number to be mounted/unmounted */
					0			/* Mode option 0:Do not mount (delayed mount), 1:Mount immediately */);
					
	//res = f_mount(disk_dev_num, &fs);
	if (res != FR_OK) {
		vPrintf("\r\nDisk %d: Mount -E- f_mount pb: 0x%X\n\r", (int)disk_dev_num, res);
		configASSERT(0);
	}else vPrintf("\r\n Disk %d: Mounted\r\n", (int)disk_dev_num);
	
	if(format) res = FR_NO_FILESYSTEM;
	else res = f_opendir(&dirs, root_directory);/* Test if the disk is formatted */
	
	switch(res) {
	case FR_OK:
		res = 1;
		f_closedir(&dirs);
		//fat_system_scan_files(root_directory);
		break;
	case FR_NO_FILESYSTEM:
		#if FF_FS_READONLY == 0
		/* Format disk */
		vPrintf("\r\nDisk %d: Formatting, Please Wait a Moment...\n\r", (int)disk_dev_num);

		res = f_mkfs (  root_directory,	/* Logical drive number */
						FM_FAT,			/* Format option Partitioning rule; 0:FDISK, 1:SFD */
						0,			/* Size of allocation unit (cluster) [byte]; 1-128: sector, >128:bytes, 0: auto */
						work,		/* Pointer to working buffer */
						sizeof(work)			/* Size of working buffer */ );

		//res = f_mkfs(	disk_dev_num	/* Logical drive number */
		//				, 0				/* Partitioning rule 0:FDISK, 1:SFD */
		//				, 0	);			/* Allocation unit size of cluster 1-128: sector, >128:bytes, 0: auto*/
			
		if (res != FR_OK) {
			vPrintf("\r\nDisk %d: -E- f_mkfs pb: 0x%X\n\r", (int)disk_dev_num, res);
			res = 0;
		}
		else {
			vPrintf("\r\nDisk %d: Disk Format Finished !\r", (int)disk_dev_num);
			res = 1;
		}
		#else
		vPuts("-I- Please run Full version FAT FS test first\r");
		res = 0;
		#endif
		break;
	default:
		vPrintf("\r\nDisk %d: -E- f_opendir pb: 0x%X\n\r", (int)disk_dev_num, res);
		res = 0;
		break;
	}
	
	return res;	
}

uint8_t fat_system_mountdisk(uint32_t disk_dev_num, bool format ) {
	for (; disk_dev_num < get_nb_lun(); disk_dev_num++) {
		if (fat_system_verify(disk_dev_num,format)) {
			vPrintf("\r\nDisk %d: Install OK\r\n\r\n", (int)disk_dev_num);
		}else vPrintf("\r\nDisk %d: Install Failed\r\n\r\n", (int)disk_dev_num);
	}
	return disk_dev_num;
}