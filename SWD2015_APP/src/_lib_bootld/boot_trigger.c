/*
 * boot_trigger.c
 *
 * Created: 8/6/2558 18:49:11
 *  Author: pan
 */ 

#include <asf.h>
#include <string.h>
#include "boot_trigger.h"
#include "regions.h"


void load_trigger(void) {
	uint32_t ul_test_page_addr = BOOT_TRIGGER_PAGE_ADDRESS;
	uint32_t *pul_test_page = (uint32_t *)ul_test_page_addr;
	uint32_t ul_rc;
	uint32_t ul_idx;
	uint32_t temp_data;
	//uint32_t i;
	/* Page used for storing trigger info */
	uint8_t trigger_page[IFLASH_PAGE_SIZE];

	for (ul_idx = 0; ul_idx < (IFLASH_PAGE_SIZE / 4); ul_idx++) {
		temp_data = pul_test_page[ul_idx];
		trigger_page[ul_idx * 4] = (uint8_t)(temp_data & 0xFF);
		trigger_page[ul_idx * 4 + 1] = (uint8_t)((temp_data >> 8) & 0xFF);
		trigger_page[ul_idx * 4 + 2] = (uint8_t)((temp_data >> 16) & 0xFF);
		trigger_page[ul_idx * 4 + 3] = (uint8_t)((temp_data >> 24) & 0xFF);
	}

	/* Set the trigger = LOAD (0x00000001)*/
	trigger_page[0x14] = 0x01;
	trigger_page[0x15] = 0x00;
	trigger_page[0x16] = 0x00;
	trigger_page[0x17] = 0x00;



	trigger_page[0x18] = 'S';
	trigger_page[0x19] = 'W';
	trigger_page[0x1A] = 'D';
	trigger_page[0x1B] = '2';
	trigger_page[0x1D] = '0';
	trigger_page[0x1E] = '1';
	trigger_page[0x1F] = '5';
	trigger_page[0x20] = '.';
	trigger_page[0x21] = 'b';
	trigger_page[0x22] = 'i';
	trigger_page[0x23] = 'n';

	ul_rc = flash_unlock(ul_test_page_addr,
	ul_test_page_addr + IFLASH_PAGE_SIZE, NULL, NULL);
	if (ul_rc != FLASH_RC_OK) {
		return;
	}


	ul_rc = flash_erase_page(ul_test_page_addr, IFLASH_ERASE_PAGES_8);

	if (ul_rc != FLASH_RC_OK) {
		return;
	}

	ul_rc = flash_write(ul_test_page_addr, trigger_page,
	IFLASH_PAGE_SIZE, 0);
	if (ul_rc != FLASH_RC_OK) {
		return;
	}

	/* Perform the software reset. */
	rstc_start_software_reset(RSTC);
}

void sleep_trigger(void) {
	uint32_t ul_test_page_addr = BOOT_TRIGGER_PAGE_ADDRESS;
	uint32_t *pul_test_page = (uint32_t *)ul_test_page_addr;
	uint32_t ul_rc;
	uint32_t ul_idx;
	uint32_t temp_data;
	//uint32_t i;
	/* Page used for storing trigger info */
	uint8_t trigger_page[IFLASH_PAGE_SIZE];

	for (ul_idx = 0; ul_idx < (IFLASH_PAGE_SIZE / 4); ul_idx++) {
		temp_data = pul_test_page[ul_idx];
		trigger_page[ul_idx * 4] = (uint8_t)(temp_data & 0xFF);
		trigger_page[ul_idx * 4 + 1] = (uint8_t)((temp_data >> 8) & 0xFF);
		trigger_page[ul_idx * 4 + 2] = (uint8_t)((temp_data >> 16) & 0xFF);
		trigger_page[ul_idx * 4 + 3] = (uint8_t)((temp_data >> 24) & 0xFF);
	}

	/* Set the trigger = SLEEP (0x00000005)*/
	trigger_page[0x14] = 0x05;
	trigger_page[0x15] = 0x00;
	trigger_page[0x16] = 0x00;
	trigger_page[0x17] = 0x00;



	trigger_page[0x18] = 'S';
	trigger_page[0x19] = 'W';
	trigger_page[0x1A] = 'D';
	trigger_page[0x1B] = '2';
	trigger_page[0x1D] = '0';
	trigger_page[0x1E] = '1';
	trigger_page[0x1F] = '5';
	trigger_page[0x20] = '.';
	trigger_page[0x21] = 'b';
	trigger_page[0x22] = 'i';
	trigger_page[0x23] = 'n';

	ul_rc = flash_unlock(ul_test_page_addr,
	ul_test_page_addr + IFLASH_PAGE_SIZE, NULL, NULL);
	if (ul_rc != FLASH_RC_OK) {
		return;
	}


	ul_rc = flash_erase_page(ul_test_page_addr, IFLASH_ERASE_PAGES_8);

	if (ul_rc != FLASH_RC_OK) {
		return;
	}

	ul_rc = flash_write(ul_test_page_addr, trigger_page,
	IFLASH_PAGE_SIZE, 0);
	if (ul_rc != FLASH_RC_OK) {
		return;
	}

	/* Perform the software reset. */
	rstc_start_software_reset(RSTC);
}

void dota_trigger(void) {
	uint32_t ul_test_page_addr = BOOT_TRIGGER_PAGE_ADDRESS;
	uint32_t *pul_test_page = (uint32_t *)ul_test_page_addr;
	uint32_t ul_rc;
	uint32_t ul_idx;
	uint32_t temp_data;
	//uint32_t i;
	/* Page used for storing trigger info */
	uint8_t trigger_page[IFLASH_PAGE_SIZE];

	for (ul_idx = 0; ul_idx < (IFLASH_PAGE_SIZE / 4); ul_idx++) {
		temp_data = pul_test_page[ul_idx];
		trigger_page[ul_idx * 4] = (uint8_t)(temp_data & 0xFF);
		trigger_page[ul_idx * 4 + 1] = (uint8_t)((temp_data >> 8) & 0xFF);
		trigger_page[ul_idx * 4 + 2] = (uint8_t)((temp_data >> 16) & 0xFF);
		trigger_page[ul_idx * 4 + 3] = (uint8_t)((temp_data >> 24) & 0xFF);
	}

	/* Set the trigger = DOTA (0x00000006)*/
	trigger_page[0x14] = 0x06;
	trigger_page[0x15] = 0x00;
	trigger_page[0x16] = 0x00;
	trigger_page[0x17] = 0x00;
	
	trigger_page[0x18] = 'S';
	trigger_page[0x19] = 'W';
	trigger_page[0x1A] = 'D';
	trigger_page[0x1B] = '2';
	trigger_page[0x1D] = '0';
	trigger_page[0x1E] = '1';
	trigger_page[0x1F] = '5';
	trigger_page[0x20] = '.';
	trigger_page[0x21] = 'b';
	trigger_page[0x22] = 'i';
	trigger_page[0x23] = 'n';
	
	ul_rc = flash_unlock(ul_test_page_addr,
	ul_test_page_addr + IFLASH_PAGE_SIZE, NULL, NULL);
	if (ul_rc != FLASH_RC_OK) {
		return;
	}
	
	ul_rc = flash_erase_page(ul_test_page_addr, IFLASH_ERASE_PAGES_8);

	if (ul_rc != FLASH_RC_OK) {
		return;
	}
	
	ul_rc = flash_write(ul_test_page_addr, trigger_page,
	IFLASH_PAGE_SIZE, 0);
	if (ul_rc != FLASH_RC_OK) {
		return;
	}

	/* Perform the software reset. */
	rstc_start_software_reset(RSTC);
}

void update_trigger(void) {
	uint32_t ul_test_page_addr = BOOT_TRIGGER_PAGE_ADDRESS;
	uint32_t *pul_test_page = (uint32_t *)ul_test_page_addr;
	uint32_t ul_rc;
	uint32_t ul_idx;
	uint32_t temp_data;
	//uint32_t i;
	/* Page used for storing trigger info */
	uint8_t trigger_page[IFLASH_PAGE_SIZE];

	for (ul_idx = 0; ul_idx < (IFLASH_PAGE_SIZE / 4); ul_idx++) {
		temp_data = pul_test_page[ul_idx];
		trigger_page[ul_idx * 4] = (uint8_t)(temp_data & 0xFF);
		trigger_page[ul_idx * 4 + 1] = (uint8_t)((temp_data >> 8) & 0xFF);
		trigger_page[ul_idx * 4 + 2] = (uint8_t)((temp_data >> 16) & 0xFF);
		trigger_page[ul_idx * 4 + 3] = (uint8_t)((temp_data >> 24) & 0xFF);
	}

	/* Set the trigger = UPDATE (0x00000003)*/
	trigger_page[0x14] = 0x03;
	trigger_page[0x15] = 0x00;
	trigger_page[0x16] = 0x00;
	trigger_page[0x17] = 0x00;



	trigger_page[0x18] = 'S';
	trigger_page[0x19] = 'W';
	trigger_page[0x1A] = 'D';
	trigger_page[0x1B] = '2';
	trigger_page[0x1D] = '0';
	trigger_page[0x1E] = '1';
	trigger_page[0x1F] = '5';
	trigger_page[0x20] = '.';
	trigger_page[0x21] = 'b';
	trigger_page[0x22] = 'i';
	trigger_page[0x23] = 'n';

	ul_rc = flash_unlock(ul_test_page_addr,
	ul_test_page_addr + IFLASH_PAGE_SIZE, NULL, NULL);
	if (ul_rc != FLASH_RC_OK) {
		return;
	}


	ul_rc = flash_erase_page(ul_test_page_addr, IFLASH_ERASE_PAGES_8);

	if (ul_rc != FLASH_RC_OK) {
		return;
	}

	ul_rc = flash_write(ul_test_page_addr, trigger_page,
	IFLASH_PAGE_SIZE, 0);
	if (ul_rc != FLASH_RC_OK) {
		return;
	}

	/* Perform the software reset. */
	rstc_start_software_reset(RSTC);
}
