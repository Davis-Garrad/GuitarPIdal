// This header provides all the necessary tools, in a fashion that I like, to use the ADCs onboard the Raspberry Pi Pico
// Should implement a way to start a hardware clock interrupt which samples at some defined rate, and provide a function to get the latest reading (inc. the time it was taken at)

#ifndef PICO_ADC_H
#define PICO_ADC_H

#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"

static uint round_robin_mask = 0x0;
#define RING_SHIFT 0xc
#define RING_SIZE (1<<(RING_SHIFT-1))//32768/2
static uint16_t adc_data_ring[RING_SIZE];

void init_repeated_read(uint gpio, float sample_rate) {
	// This function implements a hardware-interrupt system where the ADC at `gpio` will be read from at `rate` samples per second.
	// Max rate: 50000

	if(gpio > 29 || gpio < 26) { // Allowed GPIOs for RP2040
		printf("init_repeated_read: Failed! Choose a valid GPIO for RP2040 (26..29)");
		return;
	}

	adc_init();
	float divisor = 48000000/sample_rate - 1.0;
	adc_set_clkdiv(divisor); // Setting the clock divider to the absolute minimum (0) clamps the period of samples to minimum. For RP2040, that's 96 cycles/sample. A bit too fast for us (~48MHz). Instead, set the total divider to 1000, so we read at ~50kHz.
	adc_gpio_init(gpio); // Disable all digital functions on gpio, enable hi-Z, no pullups
	adc_select_input(gpio-26);
	
	// Add this input to the round-robin mask
	round_robin_mask |= 1<<(gpio-26);
	if(round_robin_mask != 1<<(gpio-26)) {
		// There are other inputs to read!
		adc_set_round_robin(round_robin_mask);
	}
	
	// DMA setup
	// Enable the FIFO, enable DMA requests, allow DMA requests when there is >=1 sample in the FIFO, disable error bits because we're reading 12 bit data (and don't care), and do not shift contents to be one byte in size.
	adc_fifo_setup(true, true, 1, false, false);
	uint dma_channel = dma_claim_unused_channel(true);
	uint dma_channel_cmd = dma_claim_unused_channel(true);

	dma_channel_config chan_cfg = dma_channel_get_default_config(dma_channel);
	dma_channel_config chan_cfg_cmd = dma_channel_get_default_config(dma_channel_cmd);

	channel_config_set_transfer_data_size(&chan_cfg, DMA_SIZE_16);
	channel_config_set_read_increment(&chan_cfg, false);
	channel_config_set_write_increment(&chan_cfg, true);
	channel_config_set_irq_quiet(&chan_cfg, true);
	channel_config_set_dreq(&chan_cfg, DREQ_ADC); // only when ADC says it's okay to take data.
	
	channel_config_set_chain_to(&chan_cfg, dma_channel_cmd);
	
	dma_channel_configure(dma_channel, &chan_cfg, 
			adc_data_ring, // dst
			&adc_hw->fifo, // src
			RING_SIZE, // transfer count
			false); // Start now	
	
	channel_config_set_transfer_data_size(&chan_cfg_cmd, DMA_SIZE_32);
	channel_config_set_read_increment(&chan_cfg_cmd, false);
	channel_config_set_write_increment(&chan_cfg_cmd, false);
	channel_config_set_irq_quiet(&chan_cfg_cmd, true);
	
	
	static uint32_t loc = (uint32_t)&adc_data_ring;
	dma_channel_configure(dma_channel_cmd, &chan_cfg_cmd, 
			&(dma_hw->ch[dma_channel].al2_write_addr_trig),    // Tell the DMA to write to the actual ADC reader's DMA channel's destination register.
			&loc,                                    // pointer to pointer to adc reads data
			1, // do this once.
        		true); // Initial DMA trigger

	adc_run(true); // Starts the free-running sampling mode. Might be unnecessary on subsequent runs of this function.
	return;
}

#endif // PICO_ADC_H
