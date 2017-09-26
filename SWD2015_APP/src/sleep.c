/*
 * sleep.c
 *
 * Created: 2017-08-18 5:10:52 PM
 *  Author: Panmaya
 */ 
#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"
#include "conf_uart_serial.h"
#include "board.h"
#include "pio.h"
#include "pio_handler.h"
#include "pmc.h"
#include "matrix.h"
#include "sam4e8c.h"


extern int rtc_wakeup(uint16_t second);
extern int rtt_wakeup(uint16_t second);

#ifndef PLL_DEFAULT_MUL
#define PLL_DEFAULT_MUL  7
#endif

#ifndef PLL_DEFAULT_DIV
#define PLL_DEFAULT_DIV  1
#endif

#ifndef MCK_DEFAULT_DIV
#define MCK_DEFAULT_DIV  PMC_MCKR_PRES_CLK_4
#endif

#ifndef example_switch_clock
#define example_switch_clock(a, b, c, d) \
	do {                                 \
		pmc_enable_pllack(a, b, c);      \
		pmc_switch_mck_to_pllack(d);     \
	} while (0)
#endif

#ifndef example_disable_pll
#define example_disable_pll()  pmc_disable_pllack()
#endif


/** Current MCK in Hz */
static uint32_t g_ul_current_mck;

/**
 * \brief Set default clock (MCK = 24MHz).
 */
static void set_default_working_clock(void)
{
	
	/* Switch MCK to slow clock  */
	pmc_switch_mck_to_sclk(PMC_MCKR_PRES_CLK_1);

	/* Switch mainck to external xtal */
	pmc_switch_mainck_to_xtal(0, BOARD_OSC_STARTUP_US);

	/*
	 * Configure PLL and switch clock.
	 * MCK = XTAL * (PLL_DEFAULT_MUL+1) / PLL_DEFAULT_DIV / MCK_DEFAULT_DIV
	 *     = 24 MHz
	 */
	example_switch_clock(PLL_DEFAULT_MUL, PLL_COUNT, PLL_DEFAULT_DIV,MCK_DEFAULT_DIV);

	/* Disable unused clock to save power */
	pmc_osc_disable_fastrc();


	/* Save current clock */
	g_ul_current_mck = 24000000; /* 24MHz */

}


/**
 *  Reconfigure UART console for changed MCK and baudrate.
 */
static void reconfigure_console(uint32_t ul_mck, uint32_t ul_baudrate)
{

	const sam_uart_opt_t uart_console_settings =
			{ ul_mck, ul_baudrate, UART_MR_PAR_NO };

	/* Configure PMC */
	pmc_enable_periph_clk(CONSOLE_UART_ID);

	/* Configure PIO */
	pio_configure_pin_group(CONF_UART_PIO, CONF_PINS_UART,
			CONF_PINS_UART_FLAGS);

	/* Configure UART */
	uart_init(CONF_UART, &uart_console_settings);

}

/**
 * \brief Initialize SAM4E_XPRO board for low power test.
 */
static void init_specific_board(void)
{
	/* Disable all Extra functions in matrix except for SWD CLK/IO */
	matrix_set_system_io(0x00001C30);
		
	pio_set_output(PIOA, 0xFFFFFFFF, 0, 0, 0);
	pio_set_output(PIOB, 0xFFFFFFFF, 0, 0, 0);
	pio_set_output(PIOD, 0xFFFFFFFF, 0, 0, 0);
		
	/* Disable USB Clock */
	pmc_disable_udpck();
		
	// WD
	pio_set_input(PIOA, PIO_PA31, PIO_DEFAULT);
		
	// SDA / SCL
	pio_set_input(PIOA, PIO_PA3 | PIO_PA4, PIO_PULLUP);
		
	// URXD1
	pio_set_input(PIOA, PIO_PA5, PIO_PULLUP);
		
	// XIN32
	pio_set_input(PIOA, PIO_PA7, PIO_PULLUP);
		
	// URXD0
	pio_set_input(PIOA, PIO_PA9, PIO_PULLUP);
		
	// #CS DF
	pio_set_input(PIOA, PIO_PA11, PIO_PULLUP);
		
	// SCLK DF
	pio_set_input(PIOA, PIO_PA14, PIO_PULLUP);
		
	// JTAG TDI/SDA, TDI/SCL
	pio_set_input(PIOB, PIO_PB4 | PIO_PB5, PIO_PULLUP);
		
	// IN1 , IN2
	pio_set_input(PIOD, PIO_PD18 | PIO_PD19, PIO_PULLUP);
	
	
	/* Disable pull-up on PHY */
	//pio_pull_up(PIOD, PIO_PD0 | PIO_PD4 | PIO_PD5 | PIO_PD6 | PIO_PD7, 0);
	/* Hold PHY in reset to avoid the clock output switching */
	//pio_set_output(PIOD, PIO_PD31, 0, 0, 0);

	/* Disable pull-up on VBUS */
	//pio_pull_up(PIOE, PIO_PE2, 0);
	
	/* Disable PIO pull-up for PB10(USB_DDM), PB11(USB_DDP) */
	//pio_pull_up(PIOB, PIO_PB10 | PIO_PB11, 0);

	/* Enable the PMC clocks of push button for wakeup */
	//pmc_enable_periph_clk(ID_PIOE);
	//pio_handler_set_priority(PIOE, PIOE_IRQn, IRQ_PRIOR_PIO);
}


/**
 * \brief Initialize the chip for low power test.
 */
static void init_chip(void)
{
	/* Disable all the peripheral clocks */
	pmc_disable_all_periph_clk();

	/* Disable brownout detector */
	supc_disable_brownout_detector(SUPC);

	/* Initialize the specific board */
	init_specific_board();
}

/**
 * \brief Test wait mode.
 */
static void test_wait_mode(uint16_t waitsec)
{
	/* Configure fast RC oscillator */
	pmc_switch_mck_to_sclk(PMC_MCKR_PRES_CLK_1);

	pmc_switch_mainck_to_fastrc(CKGR_MOR_MOSCRCF_4_MHz);

	pmc_switch_mck_to_mainck(PMC_PCK_PRES_CLK_1);

	g_ul_current_mck = 4000000; /* 4MHz */

	/* Disable unused clock to save power */
	pmc_osc_disable_xtal(0);
	example_disable_pll();

	/* Set wakeup input for fast startup */
	#if(1)
	rtt_wakeup(waitsec);
	pmc_set_fast_startup_input(PMC_FSMR_RTTAL);
	supc_set_wakeup_mode(SUPC, SUPC_WUMR_RTTEN_ENABLE);
	#else
	rtc_wakeup(waitsec);
	pmc_set_fast_startup_input(PMC_FSMR_RTCAL);
	supc_set_wakeup_mode(SUPC, SUPC_WUMR_RTCEN_ENABLE);
	#endif

	/* Enter into wait Mode */
	pmc_enable_waitmode();

	/* Set default clock and re-configure UART */
	set_default_working_clock();
	reconfigure_console(g_ul_current_mck, CONF_UART_BAUDRATE);
}


static void test_backup_mode(uint16_t waitsec)
{
	/* Enable the PIO for wake-up */
	#if(0)
	rtt_wakeup(waitsec);
	pmc_set_fast_startup_input(PMC_FSMR_RTTAL);
	supc_set_wakeup_mode(SUPC, SUPC_WUMR_RTTEN_ENABLE);
	#else
	rtc_wakeup(waitsec);
	pmc_set_fast_startup_input(PMC_FSMR_RTCAL);
	supc_set_wakeup_mode(SUPC, SUPC_WUMR_RTCEN_ENABLE);
	#endif

	/* Switch MCK to slow clock  */
	pmc_switch_mck_to_sclk(PMC_MCKR_PRES_CLK_1);

	/* Disable unused clock to save power */
	pmc_osc_disable_xtal(0);
	example_disable_pll();

	/* Enter into backup mode */
	pmc_enable_backupmode();

	/* Note: The core will reset when exiting from backup mode. */
}



/**
 * \brief Test Core consumption.
 */
static void sleep_core(char mode, uint16_t waitsec) {
	bool flash_access128 = true;
	if ((efc_get_flash_access_mode(EFC) & EEFC_FMR_FAM) == EEFC_FMR_FAM) flash_access128 = false;
	
	switch(mode) {
	case 'w':
	case 'W':
		efc_set_flash_access_mode(EFC, EEFC_FMR_FAM); /* 64-bit */
		test_wait_mode(waitsec);
		if(flash_access128) efc_set_flash_access_mode(EFC, 0); /* 128-bit */
		break;
	case 'b':
	case 'B':
		efc_set_flash_access_mode(EFC, EEFC_FMR_FAM); /* 64-bit */
		test_backup_mode(waitsec);
		break;
	}
}


int sleep_entry(char mode, uint16_t waitsec)
{
	g_ul_current_mck = sysclk_get_cpu_hz();
	
	/* Initialize the chip for the power consumption test */
	init_chip();
	/* Set default clock and re-configure UART */
	set_default_working_clock();
	reconfigure_console(g_ul_current_mck, CONF_UART_BAUDRATE);
	/* sleep core consumption */
	sleep_core(mode,waitsec);
	return 0;
}

