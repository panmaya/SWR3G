/*
 * conf_bootloader.h
 *
 * Created: 4/6/2558 16:06:54
 *  Author: pan
 */ 


#ifndef CONF_BOOTLOADER_H_
#define CONF_BOOTLOADER_H_

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//###########################################################

#define DBG_USE_INFO_EDIT

/*
 * Trigger config
 * -> Buttons
 * -> SW flag
 */
//#define TRIGGER_USE_BUTTONS     /**< Define to use button trigger */
//#  define TRIGGER_LOAD_BUTTON        (PIO_PA30_IDX)
//#  define TRIGGER_LOAD_BUTTON_ACTIVE (0)
//#  define TRIGGER_SWITCH_BUTTON /* reserved for dual bank with more btn */
//#  define TRIGGER_SWITCH_BUTTON_ACTIVE (0)
#define TRIGGER_USE_FLAG        /**< Define to use SW flag in INFO area */

/*
 * Memory config
 */
#define MEM_LOCK_BOOT_REGION    /**< Define to lock boot region (optional) */

#define MEM_USE_FLASH           /**< Define to Use internal flash (must) */

/* Memory mapping
=====================================================================+
|<   128K   >|<                                       >|<    4 K    >|
+============+=========================================+=============+
| Bootloader | Application code                        | Information |
+=====================================================================
*/

#ifdef MEM_USE_FLASH

/** memory blank value */
#define MEM_BLANK_VALUE				0xFF
/** memory block size */
#define MEM_BLOCK_SIZE				(IFLASH_PAGE_SIZE)
/** memory erase block size (4K) */
#define MEM_ERASE_SIZE				(IFLASH_PAGE_SIZE * 8)
/* memory aligned (8 pages to erase) */
#define mem_align(addr)				(((addr) % MEM_ERASE_SIZE) ? ((addr) - ((addr) % MEM_ERASE_SIZE)) : (addr))
/** boot region size */
#define BOOT_SIZE					(128*1024)
/** application region size */
#define APP_SIZE					mem_align((IFLASH_SIZE - BOOT_SIZE) / 1)
/* memory address */
#define MEM0_ADDR					IFLASH_ADDR

/* MEM1_ADDR   is reserved for dual flash */

/* Boot region (single) */
#define BOOT_CODE_SIZE				(BOOT_SIZE - MEM_BLOCK_SIZE) /* reserve last page */
#define BOOT0_START					mem_align(MEM0_ADDR)
#define BOOT0_END					(BOOT0_START + BOOT_SIZE - 1)
/* BOOT1_START is reserved for dual flash  */
/* BOOT1_END   is reserved for dual flash  */

/* Application region (single) */
#define APP_CODE_SIZE				(APP_SIZE - MEM_ERASE_SIZE) /* reserve last pages */
#define APP0_START					mem_align(MEM0_ADDR + BOOT_SIZE)
#define APP0_END					(APP0_START + APP_CODE_SIZE - 1)

/* boot: boot region number, always 0 */
#define APP_START(boot)				(APP0_START) /* Always boot from 1st region */

/* Information page (follow app buffer) */
#define INFO_SIZE					(MEM_ERASE_SIZE) /**< Boot information size */
#define INFO0_ADDR					(APP0_END + 1)
/* INFO1_ADDR   is reserved for dual flash */

/* for_boot: purpose, true for boot, false for buffer */
#define INFO_ADDR(for_boot)			(INFO0_ADDR) /* Info always save to last flash page */

#endif /* #ifdef MEM_USE_FLASH */


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//##########################################################
#endif /* CONF_BOOTLOADER_H_ */