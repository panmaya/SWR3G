/*
 * rtt_wakeup.c
 *
 * Created: 2017-08-18 6:03:16 PM
 *  Author: Panmaya
 */ 
#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"

/**
 * \brief Interrupt handler for the RTT.
 *
 * Display the current time on the terminal.
 */
void RTT_Handler(void)
{
	/* Get RTT status */
	uint32_t ul_status = rtt_get_status(RTT);

}

/**
 * \brief Application entry point for RTT example.
 *
 * Initialize the RTT, display the current time and allow the user to
 * perform several actions: clear the timer, set an alarm, etc.
 *
 * \return Unused (ANSI-C compatibility).
 */
int rtt_wakeup(uint16_t second)
{
	uint32_t ul_previous_time;
	
	/* Configure RTT for a 1/2 second tick interrupt */
	rtt_sel_source(RTT, false);
	rtt_init(RTT, 32768/2);

	ul_previous_time = rtt_read_timer_value(RTT);
	while (ul_previous_time == rtt_read_timer_value(RTT));

	/* Enable RTT interrupt */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 0);
	NVIC_EnableIRQ(RTT_IRQn);
	rtt_enable_interrupt(RTT, /*RTT_MR_RTTINCIEN |*/ RTT_MR_ALMIEN);
		
	/* Set Alarm */
	rtt_write_alarm_time(RTT, second*2);

	return 0;
}
