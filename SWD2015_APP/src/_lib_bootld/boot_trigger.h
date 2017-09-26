/*
 * boot_trigger.h
 *
 * Created: 8/6/2558 18:49:29
 *  Author: pan
 */ 


#ifndef BOOT_TRIGGER_H_
#define BOOT_TRIGGER_H_

#define BOOT_TRIGGER_PAGE_ADDRESS	(IFLASH_ADDR + IFLASH_SIZE - IFLASH_PAGE_SIZE * 8)

void load_trigger(void);
void sleep_trigger(void);
void update_trigger(void);
void dota_trigger(void);

#endif /* BOOT_TRIGGER_H_ */