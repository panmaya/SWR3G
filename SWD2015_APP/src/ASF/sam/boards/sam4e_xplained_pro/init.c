/**
 * \file
 *
 * \brief SAM4E-XPRO board init.
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
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

#include "compiler.h"
#include "board.h"
#include "conf_board.h"
#include "ioport.h"

/**
 * \brief Set peripheral mode for IOPORT pins.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param port IOPORT port to configure
 * \param masks IOPORT pin masks to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_port_peripheral_mode(port, masks, mode) \
	do {\
		ioport_set_port_mode(port, masks, mode);\
		ioport_disable_port(port, masks);\
	} while (0)

/**
 * \brief Set peripheral mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do {\
		ioport_set_pin_mode(pin, mode);\
		ioport_disable_pin(pin);\
	} while (0)

/**
 * \brief Set input mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 * \param sense Sense for interrupt detection (\ref ioport_sense)
 */
#define ioport_set_pin_input_mode(pin, mode, sense) \
	do {\
		ioport_set_pin_dir(pin, IOPORT_DIR_INPUT);\
		ioport_set_pin_mode(pin, mode);\
		ioport_set_pin_sense_mode(pin, sense);\
	} while (0)

void board_init(bool Opt)
{
#ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT
	/* Disable the watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;
	//WDT->WDT_MR = WDT_MR_WDRSTEN;
#endif

	/* Initialize IOPORTs */
	ioport_init();
	
	ioport_set_pin_dir(EXT_IG, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(EXT_IN1, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(EXT_IN2, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(EXT_CLRBUF, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(PIN_GPIO_0, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_GPIO_1, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_SIMPRESS, IOPORT_DIR_INPUT);

	ioport_set_pin_dir(EXT_OUT1, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(EXT_OUT1, IOPORT_PIN_LEVEL_LOW);
	
	ioport_set_pin_dir(EXT_OUT2, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(EXT_OPT1, IOPORT_DIR_OUTPUT);
	
	/* Disable External Watchdog */
	//ioport_set_pin_dir(PIN_WD_EXT, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(PIN_WD_EXT, Opt ? IOPORT_DIR_OUTPUT : IOPORT_DIR_INPUT);
	
	ioport_set_pin_dir(PIN_GSM_DWL, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIN_GSM_DWL, IOPORT_MODE_PULLUP);
	
	ioport_set_pin_dir(PIN_BOARD_PWR, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIN_BOARD_PWR, IOPORT_MODE_PULLUP);
	
	/* Configure the pins connected to 5.0v power down control
	 default initial state to low  (OFF). */
	
	ioport_set_pin_dir(PIN_EN_5VPDOWN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_EN_5VPDOWN, IOPORT_PIN_LEVEL_LOW);
	//ioport_set_pin_level(PIN_EN_5VPDOWN, Opt ? IOPORT_PIN_LEVEL_LOW : IOPORT_PIN_LEVEL_HIGH);
	
	/* Configure the pins connected to 3.3v enable power
	 default initial state to high (ON). */
	ioport_set_pin_dir(PIN_EN_3V3PER, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_EN_3V3PER, 1 ? PIN_EN_3V3PER_ON : PIN_EN_3V3PER_OFF);
	
	/* Configure the pins connected to 3.6v enable power
	 default initial state to high (ON).*/
	// Default power from external supply
	ioport_set_pin_dir(PIN_EN_3V6MAN_REG, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_EN_3V6MAN_REG, 1 ? PIN_EN_3V6MAN_ON : PIN_EN_3V6MAN_OFF);
	ioport_set_pin_dir(PIN_EN_3V6MAN_UNREG, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_EN_3V6MAN_UNREG, 1 ? PIN_EN_3V6MAN_OFF : PIN_EN_3V6MAN_ON);
	
	/* Configure the pins connected to AT45DB Flash reset control
	 default initial state to high (ON).*/
	ioport_set_pin_dir(PIN_RST_AT45DB, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_RST_AT45DB, Opt ? PIN_RST_AT45DB_ON : PIN_RST_AT45DB_OFF);
	
	/* Configure the pins connected to FM45clx FRAM Write Protect control
	 default initial state to Low (OFF).*/
	ioport_set_pin_dir(PIN_WP_FM24CL, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_WP_FM24CL, Opt ? PIN_WP_FM24CL_OFF : PIN_WP_FM24CL_ON);
	//ioport_set_pin_level(PIN_WP_FM24CL, PIN_WP_FM24CL_ON);
	
	
	/* Configure the pins connected to GPS  WAKE UP  Signal as input*/
	ioport_set_pin_dir(PIN_WAKEUP_GPS, IOPORT_DIR_INPUT);
	
	/* Configure the pins connected to GPS  ON/OFF control
	 default initial state to LOW(OFF).*/
	ioport_set_pin_dir(PIN_ONOFF_GPS, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_ONOFF_GPS, 1 ? PIN_ONOFF_GPS_OFF : PIN_ONOFF_GPS_ON);
	
	/* Configure the pins connected to GPS  Reset control
	 default initial state to Low (OFF).*/
	ioport_set_pin_dir(PIN_RST_GPS, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_RST_GPS, 1 ? PIN_RST_GPS_OFF : PIN_RST_GPS_ON);


	/* Configure the pins connected to GPS  Enable control
	 default initial state to High (ON).*/
	ioport_set_pin_dir(PIN_EN_GPS, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_EN_GPS, 1 ? PIN_EN_GPS_ON : PIN_EN_GPS_OFF);


	/* Configure the pins connected to GPS  EXT Antenna control
	 default initial state to High (ON).*/
	ioport_set_pin_dir(PIN_EXTANT_GPS, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_EXTANT_GPS, 1 ? PIN_EXTANT_GPS_ON : PIN_EXTANT_GPS_OFF);
	
	/* Configure the pins connected to GSM  Reset control
	default initial state to Low (OFF).*/
	ioport_set_pin_dir(PIN_RST_GSM, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_RST_GSM, Opt ? PIN_RST_GSM_OFF : PIN_RST_GSM_ON);
	
	/* Configure the pins connected to GSM Power On control
	default initial state to Low (OFF).*/
	ioport_set_pin_dir(PIN_PWRON_GSM, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_PWRON_GSM, Opt ? PIN_PWRON_GSM_ON : PIN_PWRON_GSM_OFF);


	/* Configure the pins connected to LED as output and set their
	 default initial state to high (LED off).*/
	ioport_set_pin_dir(LED_MCU, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_MCU, LED_ACTIVE_LEVEL);
	
	ioport_set_pin_dir(LED_GSM, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_GSM, LED_ACTIVE_LEVEL);
	
	ioport_set_pin_dir(LED_GPS, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_GPS, LED_INACTIVE_LEVEL);
	
	ioport_set_pin_dir(LED_SEND, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_SEND, LED_INACTIVE_LEVEL);
	
	ioport_set_pin_dir(LED_IG, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_IG, LED_INACTIVE_LEVEL);
	
	ioport_set_pin_dir(LED_PTO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_PTO, LED_INACTIVE_LEVEL);
	
	ioport_set_pin_dir(LED_OPT, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_OPT, LED_INACTIVE_LEVEL);

	
#ifdef CONF_BOARD_PWM_BUZZER
	/* Configure PWM BUZZER pin */
	ioport_set_pin_peripheral_mode(PIN_PWM_BUZZER_GPIO, PIN_PWM_BUZZER_FLAGS);
#endif

#ifdef CONF_BOARD_UART_CONSOLE
	/* Configure UART pins */
	//ioport_set_port_peripheral_mode(PINS_UART0_PORT, PINS_UART0, PINS_UART0_MASK);
	ioport_set_port_peripheral_mode(PINS_UART0_PORT, PINS_UART0, PINS_UART0_FLAGS);
#endif

#ifdef CONF_BOARD_UART_EXTERNAL
	/* Configure UART pins */
	//ioport_set_port_peripheral_mode(PINS_UART1_PORT, PINS_UART1, PINS_UART1_MASK);
	ioport_set_port_peripheral_mode(PINS_UART1_PORT, PINS_UART1, PINS_UART1_FLAGS);
#endif

#ifdef CONF_BOARD_USART_GPS
	/* Configure USART pins */
	//RXD
	ioport_set_pin_peripheral_mode(PIN_USART0_RXD_IDX, PIN_USART0_RXD_FLAGS);
	//TXD
	ioport_set_pin_peripheral_mode(PIN_USART0_TXD_IDX, PIN_USART0_TXD_FLAGS);
#endif

#ifdef CONF_BOARD_USART_MODEM
	/* Configure USART pins */
	//RXD
	ioport_set_pin_peripheral_mode(PIN_USART1_RXD_IDX, PIN_USART1_RXD_FLAGS);
	//TXD
	ioport_set_pin_peripheral_mode(PIN_USART1_TXD_IDX, PIN_USART1_TXD_FLAGS);
	//CTS
	ioport_set_pin_peripheral_mode(PIN_USART1_CTS_IDX, PIN_USART1_CTS_FLAGS);
	//ioport_set_pin_dir(PIN_USART1_CTS_IDX, IOPORT_DIR_INPUT);
	//RTS
	ioport_set_pin_peripheral_mode(PIN_USART1_RTS_IDX, PIN_USART1_RTS_FLAGS);
	//ioport_set_pin_dir(PIN_USART1_RTS_IDX, IOPORT_DIR_INPUT);
	
	//ioport_set_pin_dir(PIN_USART1_RTS_IDX, IOPORT_DIR_OUTPUT);
	//ioport_set_pin_level(PIN_USART1_RTS_IDX, IOPORT_PIN_LEVEL_HIGH);
	//ioport_set_pin_dir(PIN_USART1_CTS_IDX, IOPORT_DIR_OUTPUT);
	//ioport_set_pin_level(PIN_USART1_CTS_IDX, LED0_INACTIVE_LEVEL);
	//ioport_set_pin_input_mode(PIN_USART1_CTS_IDX, GPIO_PUSH_BUTTON_1_FLAGS,GPIO_PUSH_BUTTON_1_SENSE);
#endif

#ifdef CONF_BOARD_CAN
	/* Configure the CAN1 TX and RX pin. */
	ioport_set_pin_peripheral_mode(PIN_CAN1_RX_IDX, PIN_CAN1_RX_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_CAN1_TX_IDX, PIN_CAN1_TX_FLAGS);
	/* Configure the transiver1 RS & EN pins. */
	ioport_set_pin_dir(PIN_CAN1_TR_RS_IDX, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_CAN1_TR_EN_IDX, IOPORT_DIR_OUTPUT);
#endif

#if defined(CONF_BOARD_USB_PORT)
#  if defined(CONF_BOARD_USB_VBUS_DETECT)
	ioport_set_pin_peripheral_mode(USB_VBUS_PIN, USB_VBUS_FLAGS);
#  endif
#endif

#if defined (CONF_BOARD_SD_MMC_HSMCI)
	/* Configure HSMCI pins */
	ioport_set_pin_peripheral_mode(PIN_HSMCI_MCCDA_GPIO, PIN_HSMCI_MCCDA_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_HSMCI_MCCK_GPIO, PIN_HSMCI_MCCK_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_HSMCI_MCDA0_GPIO, PIN_HSMCI_MCDA0_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_HSMCI_MCDA1_GPIO, PIN_HSMCI_MCDA1_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_HSMCI_MCDA2_GPIO, PIN_HSMCI_MCDA2_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_HSMCI_MCDA3_GPIO, PIN_HSMCI_MCDA3_FLAGS);
#endif

#ifdef CONF_BOARD_KSZ8051MNL
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXC_IDX,
			PIN_KSZ8051MNL_RXC_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXC_IDX,
			PIN_KSZ8051MNL_TXC_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXEN_IDX,
			PIN_KSZ8051MNL_TXEN_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXD3_IDX,
			PIN_KSZ8051MNL_TXD3_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXD2_IDX,
			PIN_KSZ8051MNL_TXD2_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXD1_IDX,
			PIN_KSZ8051MNL_TXD1_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_TXD0_IDX,
			PIN_KSZ8051MNL_TXD0_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXD3_IDX,
			PIN_KSZ8051MNL_RXD3_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXD2_IDX,
			PIN_KSZ8051MNL_RXD2_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXD1_IDX,
			PIN_KSZ8051MNL_RXD1_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXD0_IDX,
			PIN_KSZ8051MNL_RXD0_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXER_IDX,
			PIN_KSZ8051MNL_RXER_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_RXDV_IDX,
			PIN_KSZ8051MNL_RXDV_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_CRS_IDX,
			PIN_KSZ8051MNL_CRS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_COL_IDX,
			PIN_KSZ8051MNL_COL_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_MDC_IDX,
			PIN_KSZ8051MNL_MDC_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_KSZ8051MNL_MDIO_IDX,
			PIN_KSZ8051MNL_MDIO_FLAGS);
	ioport_set_pin_dir(PIN_KSZ8051MNL_INTRP_IDX, IOPORT_DIR_INPUT);
#endif

#ifdef CONF_BOARD_NAND
	ioport_set_pin_peripheral_mode(PIN_EBI_NANDOE,   PIN_EBI_NANDOE_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NANDWE,   PIN_EBI_NANDWE_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NANDCLE,  PIN_EBI_NANDCLE_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NANDALE,  PIN_EBI_NANDALE_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NANDIO_0, PIN_EBI_NANDIO_0_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NANDIO_1, PIN_EBI_NANDIO_1_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NANDIO_2, PIN_EBI_NANDIO_2_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NANDIO_3, PIN_EBI_NANDIO_3_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NANDIO_4, PIN_EBI_NANDIO_4_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NANDIO_5, PIN_EBI_NANDIO_5_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NANDIO_6, PIN_EBI_NANDIO_6_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NANDIO_7, PIN_EBI_NANDIO_7_FLAGS);
	ioport_set_pin_dir(PIN_NF_CE_IDX, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PIN_NF_RB_IDX, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIN_NF_RB_IDX, IOPORT_MODE_PULLUP);
#endif

#ifdef CONF_BOARD_SRAM
	ioport_set_pin_peripheral_mode(PIN_EBI_NCS1, PIN_EBI_NCS1_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NCS3, PIN_EBI_NCS3_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NRD, PIN_EBI_NRD_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_NWE, PIN_EBI_NWE_FLAGS);

	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D0, PIN_EBI_DATA_BUS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D1, PIN_EBI_DATA_BUS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D2, PIN_EBI_DATA_BUS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D3, PIN_EBI_DATA_BUS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D4, PIN_EBI_DATA_BUS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D5, PIN_EBI_DATA_BUS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D6, PIN_EBI_DATA_BUS_FLAGS);
	ioport_set_pin_peripheral_mode(PIN_EBI_DATA_BUS_D7, PIN_EBI_DATA_BUS_FLAGS);

	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A0, PIN_EBI_ADDR_BUS_FLAG1);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A1, PIN_EBI_ADDR_BUS_FLAG1);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A2, PIN_EBI_ADDR_BUS_FLAG1);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A3, PIN_EBI_ADDR_BUS_FLAG1);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A4, PIN_EBI_ADDR_BUS_FLAG1);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A5, PIN_EBI_ADDR_BUS_FLAG1);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A6, PIN_EBI_ADDR_BUS_FLAG1);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A7, PIN_EBI_ADDR_BUS_FLAG1);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A8, PIN_EBI_ADDR_BUS_FLAG1);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A9, PIN_EBI_ADDR_BUS_FLAG1);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A10,
			PIN_EBI_ADDR_BUS_FLAG1);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A11,
			PIN_EBI_ADDR_BUS_FLAG1);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A12,
			PIN_EBI_ADDR_BUS_FLAG1);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A13,
			PIN_EBI_ADDR_BUS_FLAG1);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A14,
			PIN_EBI_ADDR_BUS_FLAG2);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A15,
			PIN_EBI_ADDR_BUS_FLAG2);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A16,
			PIN_EBI_ADDR_BUS_FLAG2);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A17,
			PIN_EBI_ADDR_BUS_FLAG2);
	ioport_set_pin_peripheral_mode(PIN_EBI_ADDR_BUS_A18,
			PIN_EBI_ADDR_BUS_FLAG2);
#endif

#ifdef CONF_BOARD_AT86RFX
	ioport_set_pin_peripheral_mode(AT86RFX_SPI_MISO, SPI_MISO_FLAGS);
	ioport_set_pin_peripheral_mode(AT86RFX_SPI_MOSI, SPI_MOSI_FLAGS);
	ioport_set_pin_peripheral_mode(AT86RFX_SPI_SCK,  SPI_SPCK_FLAGS);
	ioport_set_pin_peripheral_mode(AT86RFX_SPI_CS_PIN, AT86RFX_SPI_CS_FLAGS);

	/* Initialize TRX_RST and SLP_TR as GPIO. */
	ioport_set_pin_dir(AT86RFX_RST_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(AT86RFX_RST_PIN, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_dir(AT86RFX_SLP_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(AT86RFX_SLP_PIN, IOPORT_PIN_LEVEL_HIGH);
#endif  

#if defined(CONF_BOARD_TWI0) || defined(CONF_BOARD_EDBG_TWI) || defined(CONF_BOARD_FM24CLX)
	ioport_set_pin_peripheral_mode(TWI0_DATA_GPIO, TWI0_DATA_FLAGS);
	ioport_set_pin_peripheral_mode(TWI0_CLK_GPIO, TWI0_CLK_FLAGS);
#endif

#if defined(CONF_BOARD_TWI1)
	ioport_set_pin_peripheral_mode(TWI1_DATA_GPIO, TWI1_DATA_FLAGS);
	ioport_set_pin_peripheral_mode(TWI1_CLK_GPIO, TWI1_CLK_FLAGS);
#endif

#if defined(CONF_BOARD_SPI) || defined(CONF_BOARD_SD_MMC_SPI) || defined(CONF_BOARD_AT45DBX)
	ioport_set_pin_peripheral_mode(SPI_MISO_GPIO, SPI_MISO_FLAGS);
	ioport_set_pin_peripheral_mode(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
	ioport_set_pin_peripheral_mode(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);

#ifdef CONF_BOARD_SD_MMC_SPI
	/* Setting SD detection pin */
	ioport_set_pin_dir(SD_MMC_0_CD_GPIO, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(SD_MMC_0_CD_GPIO, IOPORT_MODE_PULLUP);
#endif

#ifdef CONF_BOARD_SPI_NPCS0
	ioport_set_pin_peripheral_mode(SPI_NPCS0_GPIO, SPI_NPCS0_FLAGS);
#endif

#ifdef CONF_BOARD_SPI_NPCS1
	ioport_set_pin_peripheral_mode(SPI_NPCS1_PB14_GPIO, SPI_NPCS1_PB14_FLAGS);
#endif

#ifdef CONF_BOARD_SPI_NPCS2
	ioport_set_pin_peripheral_mode(SPI_NPCS2_GPIO, SPI_NPCS2_FLAGS);
#endif

#endif
}
