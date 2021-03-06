/**
 * \file
 *
 * \brief Implementation of low level disk I/O module skeleton for FatFS.
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

#include "compiler.h"
#include "ffconf.h"
#include "diskio.h"
#include "ctrl_access.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#if (SAM3S || SAM3U || SAM3N || SAM3XA || SAM4S || SAM4N)
# include <rtc.h>
#endif

#if (SAM0)
# include <rtc_calendar.h>
struct rtc_module rtc_instance;

static void configure_rtc_calendar(void)
{
	/* Initialize RTC in calendar mode. */
	struct rtc_calendar_config config_rtc_calendar;

	rtc_calendar_get_config_defaults(&config_rtc_calendar);

	struct rtc_calendar_time init_time;
	rtc_calendar_get_time_defaults(&init_time);
	init_time.year   = 2014;
	init_time.month  = 1;
	init_time.day    = 1;
	init_time.hour   = 0;
	init_time.minute = 0;
	init_time.second = 4;

	config_rtc_calendar.clock_24h     = true;
	config_rtc_calendar.alarm[0].time = init_time;
	config_rtc_calendar.alarm[0].mask = RTC_CALENDAR_ALARM_MASK_YEAR;

	rtc_calendar_init(&rtc_instance, RTC, &config_rtc_calendar);

	rtc_calendar_enable(&rtc_instance);
}
#endif

/**
 * \defgroup thirdparty_fatfs_port_group Port of low level driver for FatFS
 *
 * Low level driver for FatFS. The driver is based on the ctrl access module
 * of the specific MCU device.
 *
 * @{
 */

/** Default sector size */
#define SECTOR_SIZE_DEFAULT 512

/** Supported sector size. These values are based on the LUN function:
 * mem_sector_size(). */
#define SECTOR_SIZE_512  1
#define SECTOR_SIZE_1024 2
#define SECTOR_SIZE_2048 4
#define SECTOR_SIZE_4096 8

/**
 * \brief Initialize a disk.
 *
 * \param drv Physical drive number (0..).
 *
 * \return 0 or disk status in combination of DSTATUS bits
 *         (STA_NOINIT, STA_PROTECT).
 */
DSTATUS disk_initialize(BYTE drv)
{
	int i;
	Ctrl_status mem_status;

#if (SAM3S || SAM3U || SAM3N || SAM3XA || SAM4S)
	/* Default RTC configuration, 24-hour mode */
	rtc_set_hour_mode(RTC, 0);
#endif

#if (SAMD20 || SAMD21 || SAMR21)
	configure_rtc_calendar();
#endif

#if LUN_USB
	/* USB disk with multiple LUNs */
	if (drv > LUN_ID_USB + Lun_usb_get_lun()) {
		return STA_NOINIT;
	}
#else
	if (drv > MAX_LUN) {
		/* At least one of the LUN should be defined */
		return STA_NOINIT;
	}
#endif
	/* Check LUN ready (USB disk report CTRL_BUSY then CTRL_GOOD) */
	for (i = 0; i < 2; i ++) {
		mem_status = mem_test_unit_ready(drv);
		if (CTRL_BUSY != mem_status) {
			break;
		}
	}
	if (mem_status != CTRL_GOOD) {
		return STA_NOINIT;
	}

	/* Check Write Protection Status */
	if (mem_wr_protect(drv)) {
		return STA_PROTECT;
	}

	/* The memory should already be initialized */
	return 0;
}

/**
 * \brief  Return disk status.
 *
 * \param drv Physical drive number (0..).
 *
 * \return 0 or disk status in combination of DSTATUS bits
 *         (STA_NOINIT, STA_NODISK, STA_PROTECT).
 */
DSTATUS disk_status(BYTE drv)
{
	switch (mem_test_unit_ready(drv)) {
	case CTRL_GOOD:
		return 0;
	case CTRL_NO_PRESENT:
		return STA_NOINIT | STA_NODISK;
	default:
		return STA_NOINIT;
	}
}

/**
 * \brief  Read sector(s).
 *
 * \param drv Physical drive number (0..).
 * \param buff Data buffer to store read data.
 * \param sector Sector address (LBA).
 * \param count Number of sectors to read (1..255).
 *
 * \return RES_OK for success, otherwise DRESULT error code.
 */
DRESULT disk_read(BYTE drv, BYTE *buff, DWORD sector, UINT count)
{
#if ACCESS_MEM_TO_RAM
	uint8_t uc_sector_size = mem_sector_size(drv);
	uint32_t i;
	uint32_t ul_last_sector_num;

	if (uc_sector_size == 0) {
		return RES_ERROR;
	}

	/* Check valid address */
	mem_read_capacity(drv, &ul_last_sector_num);
	if ((sector + count * uc_sector_size) >
			(ul_last_sector_num + 1) * uc_sector_size) {
		return RES_PARERR;
	}

	/* Read the data */
	for (i = 0; i < count; i++) {
		if (memory_2_ram(drv, sector + uc_sector_size * i,
				buff + uc_sector_size * SECTOR_SIZE_DEFAULT * i) !=
				CTRL_GOOD) {
			return RES_ERROR;
		}
	}

	return RES_OK;

#else
	return RES_ERROR;
#endif
}

/**
 * \brief  Write sector(s).
 *
 * The FatFs module will issue multiple sector transfer request (count > 1) to
 * the disk I/O layer. The disk function should process the multiple sector
 * transfer properly. Do not translate it into multiple sector transfers to the
 * media, or the data read/write performance may be drastically decreased.
 *
 * \param drv Physical drive number (0..).
 * \param buff Data buffer to store read data.
 * \param sector Sector address (LBA).
 * \param count Number of sectors to read (1..255).
 *
 * \return RES_OK for success, otherwise DRESULT error code.
 */
#if FF_FS_READONLY == 0
DRESULT disk_write(BYTE drv, BYTE const *buff, DWORD sector, UINT count)
{
#if ACCESS_MEM_TO_RAM
	uint8_t uc_sector_size = mem_sector_size(drv);
	uint32_t i;
	uint32_t ul_last_sector_num;

	if (uc_sector_size == 0) {
		return RES_ERROR;
	}

	/* Check valid address */
	mem_read_capacity(drv, &ul_last_sector_num);
	if ((sector + count * uc_sector_size) >
			(ul_last_sector_num + 1) * uc_sector_size) {
		return RES_PARERR;
	}

	/* Write the data */
	for (i = 0; i < count; i++) {
		if (ram_2_memory(drv, sector + uc_sector_size * i,
				buff + uc_sector_size * SECTOR_SIZE_DEFAULT * i) !=
				CTRL_GOOD) {
			return RES_ERROR;
		}
	}

	return RES_OK;

#else
	return RES_ERROR;
#endif
}

#endif /* _READONLY */

/**
 * \brief  Miscellaneous functions, which support the following commands:
 *
 * CTRL_SYNC    Make sure that the disk drive has finished pending write
 * process. When the disk I/O module has a write back cache, flush the
 * dirty sector immediately.
 * In read-only configuration, this command is not needed.
 *
 * GET_SECTOR_COUNT    Return total sectors on the drive into the DWORD variable
 * pointed by buffer.
 * This command is used only in f_mkfs function.
 *
 * GET_BLOCK_SIZE    Return erase block size of the memory array in unit
 * of sector into the DWORD variable pointed by Buffer.
 * When the erase block size is unknown or magnetic disk device, return 1.
 * This command is used only in f_mkfs function.
 *
 * GET_SECTOR_SIZE    Return sector size of the memory array.
 *
 * \param drv Physical drive number (0..).
 * \param ctrl Control code.
 * \param buff Buffer to send/receive control data.
 *
 * \return RES_OK for success, otherwise DRESULT error code.
 */
DRESULT disk_ioctl(BYTE drv, BYTE ctrl, void *buff)
{
	DRESULT res = RES_PARERR;

	switch (ctrl) {
	case GET_BLOCK_SIZE:
		*(DWORD *)buff = 1;
		res = RES_OK;
		break;

	/* Get the number of sectors on the disk (DWORD) */
	case GET_SECTOR_COUNT:
	{
		uint32_t ul_last_sector_num;

		/* Check valid address */
		mem_read_capacity(drv, &ul_last_sector_num);

		*(DWORD *)buff = ul_last_sector_num + 1;

		res = RES_OK;
	}
	break;

	/* Get sectors on the disk (WORD) */
	case GET_SECTOR_SIZE:
	{
		uint8_t uc_sector_size = mem_sector_size(drv);

		if ((uc_sector_size != SECTOR_SIZE_512) &&
				(uc_sector_size != SECTOR_SIZE_1024) &&
				(uc_sector_size != SECTOR_SIZE_2048) &&
				(uc_sector_size != SECTOR_SIZE_4096)) {
			/* The sector size is not supported by the FatFS */
			return RES_ERROR;
		}

		*(U8 *)buff = uc_sector_size * SECTOR_SIZE_DEFAULT;

		res = RES_OK;
	}
	break;

	/* Make sure that data has been written */
	case CTRL_SYNC:
		if (mem_test_unit_ready(drv) == CTRL_GOOD) {
			res = RES_OK;
		} else {
			res = RES_NOTRDY;
		}
		break;

	default:
		res = RES_PARERR;
	}

	return res;
}

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
