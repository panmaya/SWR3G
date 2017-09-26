/*
 * fat_system.h
 *
 * Created: 20/3/2558 17:24:20
 *  Author: pan
 */ 


#ifndef FAT_SYSTEM_H_
#define FAT_SYSTEM_H_

uint8_t fat_system_mountdisk(uint32_t disk_dev_num, bool format);
FRESULT fat_system_scan_files(char *path);
void fat_system_printdiskfree(uint32_t disk_dev_num);


#endif /* FAT_SYSTEM_H_ */