/*
 * rtc_wakeup.c
 *
 * Created: 2017-08-18 6:02:06 PM
 *  Author: Panmaya
 */ 
/**
 * \file
 *
 * \brief Real-Time Clock (RTC) example for SAM.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

/**
 * \mainpage RTC Example
 *
 * \section Purpose
 *
 * This basic example shows how to use the Real-Time Clock (RTC) peripheral
 * available on the SAM  microcontrollers. The RTC enables easy
 * time and date management and allows the user to monitor events like a
 * configurable alarm, second change, calendar change, and so on.
 *
 * \section Requirements
 *
 * This example can be used with SAM evaluation kits.
 *
 * \section Description
 *
 * Upon startup, the program displays the currently set time and date
 * and a menu to perform the following:
 *     \code
	Menu:
	   t - Set time
	   d - Set date
	   i - Set time alarm
	   m - Set date alarm
	   c - Clear the alarm notification (only if it has been triggered)
	   w - Generate Waveform
\endcode
 *
 * "w" is an additional option for SAM3S8, SAM3SD8, SAM4S, SAM4C, SAM4CP, SAM4CM
 * SAMV71, SAMV70, SAME70 and SAMS70.
 * An RTC output can be programmed to generate several waveforms, including a
 * prescaled clock derived from slow clock.
 *
 * Setting the time, date and time alarm is done by using Menu option, and
 * the display is updated accordingly.
 *
 * The time alarm is triggered only when the second, minute and hour match the 
 * preset values; the date alarm is triggered only when the month and date
 * match the preset values. 
 *
 * Generating waveform is done by using Menu option "w" and a menu to perform
 * the following:
 *     \code
	Menu:
	0 - No Waveform
	1 - 1 Hz square wave
	2 - 32 Hz square wave
	3 - 64 Hz square wave
	4 - 512 Hz square wave
	5 - Toggles when alarm flag rise
	6 - Copy of the alarm flag
	7 - Duty cycle programmable pulse
	8 - Quit
\endcode
 *
 * \note The example is using RC oscillator by default. This would generate an
 * accuracy problem for the RTC if not calibrated. It is recommended to use an
 * external 32KHz crystal to get high accuracy. How to initialize RTC with
 * external 32KHz crystal can be referred at \ref sam_rtc_quickstart.
 *
 * \note In sam4c_ek board, please use SWD instead of JTAG because RTCOUT share pin with
 * JTAG interface. Otherwise there is a debug issue when enable wave output.
 *
 * \section Usage
 *
 * -# Build the program and download it into the evaluation board.
 * -# On the computer, open and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *   - 115200 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# Start the application.
 * -# In the terminal window, the following text should appear:
 *    \code
	     -- RTC Example xxx --
	     -- xxxxxx-xx
	     -- Compiled: xxx xx xxxx xx:xx:xx --

	     Menu:
	     t - Set time
	     d - Set date
	     i - Set time alarm
	     m - Set date alarm
\endcode
 * -# Press one of the keys listed in the menu to perform the corresponding
 * action.
 * 
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "asf.h"
#include "stdio_serial.h"
#include "conf_clock.h"
#include "conf_board.h"


/**
 * \brief Interrupt handler for the RTC. Refresh the display.
 */
void RTC_Handler(void)
{
	uint32_t ul_status = rtc_get_status(RTC);
	/* Disable RTC interrupt */
	rtc_disable_interrupt(RTC, RTC_IDR_ALRDIS);
	rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
}

/**
 * \brief Application entry point for RTC example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int rtc_wakeup(uint16_t second)
{
	rtc_set_hour_mode(RTC, 0);

	/* Configure RTC interrupts */
	NVIC_DisableIRQ(RTC_IRQn);
	NVIC_ClearPendingIRQ(RTC_IRQn);
	NVIC_SetPriority(RTC_IRQn, 0);
	NVIC_EnableIRQ(RTC_IRQn);
	rtc_enable_interrupt(RTC, /*RTC_IER_SECEN | */ RTC_IER_ALREN);
	
	/* Set time */
	rtc_set_time(RTC, 0, 0,0);
	
	rtc_clear_time_alarm(RTC);
	
	/* Set time alarm */
	rtc_set_time_alarm(RTC, 1, 0,1, 0, 1, second);
	
	return 0;
}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

