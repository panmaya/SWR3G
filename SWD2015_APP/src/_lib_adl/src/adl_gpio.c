/*
 * adl_gpio.c
 *
 * Created: 16/2/2558 16:33:24
 *  Author: pan
 */ 
#include <adl_global.h>


#define ADL_GPI_SUPPORT_FREQ_COUNT			1


static volatile uint32_t ADL_GPIO_PulseCnt[PIO_PD31_IDX+1];
static volatile uint32_t ADL_GPIO_FREQHz[PIO_PD31_IDX+1];
static const uint16_t ADL_GPIO_PulseMemSize =  sizeof(uint32_t)*(PIO_PD31_IDX+1);
static uint16_t ADL_GPIO_msTick = 0;

/* IRQ priority for PIO (The lower the value, the greater the priority) */
#define ADL_GPI_INT_IRQ_PRIOR	(7)
#define ADL_GPI_FREQ_Mhz(_f)	(_f*1000000)
#define ADL_GPI_LIMIT_FREQ		ADL_GPI_FREQ_Mhz(100)

#if ADL_GPI_SUPPORT_FREQ_COUNT
/**
 * \brief Handler for SW0 rising edge interrupt.
 * \param id The button ID.
 * \param mask The button mask.
 */

static void ADL_GPI_INT_Handler(uint32_t id, uint32_t mask)
{
	switch(id) {
	case ID_PIOA:
	for(uint8_t portid = 0; portid <= 31; portid++) {
		if((1u << portid) == mask) {
			uint32_t val = ADL_GPIO_PulseCnt[PIO_PA0_IDX + portid];
			if(val < ADL_GPI_LIMIT_FREQ)ADL_GPIO_PulseCnt[PIO_PA0_IDX + portid]++;
			//printf("\r\nISR InputA-%d Count = %lu\r\n\r\n",portid,ADL_GPIO_PulseCnt[PIO_PA0_IDX + portid]);
		}
	}
	break;
	case ID_PIOB:
	for(uint8_t portid = 0; portid <= 14; portid++) {
		if((1u << portid) == mask) {
			uint32_t val = ADL_GPIO_PulseCnt[PIO_PB0_IDX + portid];
			if(val < ADL_GPI_LIMIT_FREQ)ADL_GPIO_PulseCnt[PIO_PB0_IDX + portid]++;
			//printf("\r\nISR InputB-%d Count = %lu\r\n\r\n",portid,ADL_GPIO_PulseCnt[PIO_PB0_IDX + portid]);
		}
	}
	break;
	case ID_PIOD:
	for(uint8_t portid = 0; portid <= 31; portid++) {
		if((1u << portid) == mask) {
			uint32_t val = ADL_GPIO_PulseCnt[PIO_PD0_IDX + portid];
			if(val < ADL_GPI_LIMIT_FREQ)ADL_GPIO_PulseCnt[PIO_PD0_IDX + portid]++;
			//printf("\r\nISR InputD-%d Count = %lu\r\n\r\n",portid,ADL_GPIO_PulseCnt[PIO_PD0_IDX + portid]);
		}
	}
	break;
	}
}
#endif


s32 adl_ioSubscribe ( u32 GpioNb, adl_ioDefs_t * GpioConfig, u8 PollingTimerType, u32 PollingTime, s32 GpioEventHandle ) {
	for(;GpioNb;GpioNb--) {
		if(*(GpioConfig + (GpioNb-1)) & ADL_IO_DIR_OUT) {
			ioport_set_pin_dir(*(GpioConfig + (GpioNb-1)) & ADL_IO_NUM_MSK, IOPORT_DIR_OUTPUT);
			if(*(GpioConfig + (GpioNb-1)) & ADL_IO_LEV_LOW) {
				ioport_set_pin_level(*(GpioConfig + (GpioNb-1)) & ADL_IO_NUM_MSK, LED_INACTIVE_LEVEL);
			}else ioport_set_pin_level(*(GpioConfig + (GpioNb-1)) & ADL_IO_NUM_MSK, LED_ACTIVE_LEVEL);
		}
		else if(*(GpioConfig + (GpioNb-1)) & ADL_IO_DIR_IN) {
			uint32_t pinid = 0;
			uint32_t portid = 0;
			ioport_set_pin_dir(*(GpioConfig + (GpioNb-1)) & ADL_IO_NUM_MSK, IOPORT_DIR_INPUT);
			//ioport_set_pin_mode(*(GpioConfig + (GpioNb-1)), IOPORT_MODE_PULLUP);
			//portid = (*(GpioConfig + (GpioNb-1)));
			
			#if ADL_GPI_SUPPORT_FREQ_COUNT
			pinid  = *(GpioConfig + (GpioNb-1)) & ADL_IO_NUM_MSK;
			
			if(pinid<=PIO_PA31_IDX) {
				portid = pinid - PIO_PA0_IDX;
				pmc_enable_periph_clk(ID_PIOA);
				pio_set_debounce_filter(PIOA,  (1u << portid), 10);
				pio_handler_set(PIOA, ID_PIOA, (1u << portid), (PIO_PULLUP |  PIO_DEBOUNCE | PIO_IT_RISE_EDGE), ADL_GPI_INT_Handler);
				NVIC_EnableIRQ((IRQn_Type)ID_PIOA);
				pio_handler_set_priority(PIOA, (IRQn_Type)ID_PIOA, ADL_GPI_INT_IRQ_PRIOR);
				pio_enable_interrupt(PIOA, (1u << portid));
				//printf("\r\nADL GPIO SET INPUT PORTA-%lu\r\n",portid);
				
			}else if(pinid<=PIO_PB14_IDX){
				portid = pinid - PIO_PB0_IDX;
				pmc_enable_periph_clk(ID_PIOB);
				pio_set_debounce_filter(PIOB,  (1u << portid), 10);
				pio_handler_set(PIOB, ID_PIOB, (1u << portid), (PIO_PULLUP  |  PIO_DEBOUNCE | PIO_IT_RISE_EDGE), ADL_GPI_INT_Handler);
				NVIC_EnableIRQ((IRQn_Type)ID_PIOB);
				pio_handler_set_priority(PIOB, (IRQn_Type)ID_PIOB, ADL_GPI_INT_IRQ_PRIOR);
				pio_enable_interrupt(PIOB, (1u << portid));
				//printf("\r\nADL GPIO SET INPUT PORTB-%lu\r\n",portid);
				
			}else if(pinid<=PIO_PD31_IDX) {
				portid = pinid - PIO_PD0_IDX;
				pmc_enable_periph_clk(ID_PIOD);
				pio_set_debounce_filter(PIOD,  (1u << portid), 10);
				pio_handler_set(PIOD, ID_PIOD, (1u << portid), (PIO_PULLUP  |  PIO_DEBOUNCE | PIO_IT_RISE_EDGE), ADL_GPI_INT_Handler);
				NVIC_EnableIRQ((IRQn_Type)ID_PIOD);
				pio_handler_set_priority(PIOD, (IRQn_Type)ID_PIOD, ADL_GPI_INT_IRQ_PRIOR);
				pio_enable_interrupt(PIOD, (1u << portid));
				//printf("\r\nADL GPIO SET INPUT PORTD-%lu\r\n",portid);
				
			}else;
			#endif
			
		}else;
	}
	return 1;
}

s32 adl_ioWriteSingle ( s32 GpioHandle, adl_ioDefs_t * Gpio, bool State ) {
	ioport_set_pin_level(*Gpio & ADL_IO_NUM_MSK, State);
	return OK;
}

s32 adl_ioReadSingle ( s32 GpioHandle, adl_ioDefs_t * Gpio) {
	return ioport_get_pin_level(*Gpio & ADL_IO_NUM_MSK);
}

u32 adl_ioReadFreq ( s32 GpioHandle, adl_ioDefs_t * Gpio) {
	return ADL_GPIO_FREQHz[*Gpio & ADL_IO_NUM_MSK];
}

void update_adl_io_1ms(void) {
	if(++ADL_GPIO_msTick >= 1000) { // 1 second 
		ADL_GPIO_msTick = 0;
		memcpy((uint8_t*)ADL_GPIO_FREQHz, (uint8_t*)ADL_GPIO_PulseCnt, ADL_GPIO_PulseMemSize);
		memset((u8*)ADL_GPIO_PulseCnt,0,ADL_GPIO_PulseMemSize);
	}
}