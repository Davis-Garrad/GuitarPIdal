#include <complex.h>

#include "pico/stdlib.h"

#include "kissfft/kiss_fftr.h"

#include "ADC.h"
#include "I2C.h"

#define KERNEL_SIZE (RING_SIZE/2 + 1)
#define PI 3.1415926535897932386

#ifndef FOURIER_H
#define FOURIER_H

typedef struct fourier_kernel {
	int* kernel;
	kiss_fft_cpx* fourier_transform; 
	uint time;
	dac_t* dac;
	kiss_fftr_cfg cfg;
	kiss_fftr_cfg icfg;
	size_t cfg_len;
	size_t icfg_len;
} fourier_kernel_t;


fourier_kernel_t* create_kernel(dac_t* dac) {
	fourier_kernel_t* kern = malloc(sizeof(fourier_kernel_t));
	kern->kernel = malloc(sizeof(kiss_fft_cpx) * KERNEL_SIZE);
	kern->fourier_transform = malloc(sizeof(kiss_fft_cpx) * KERNEL_SIZE);
	kern->time = 0;
	kern->dac = dac;

	kern->cfg = NULL;
	kern->icfg = NULL;
	kern->cfg_len = 0;
	kern->icfg_len = 0;

	kiss_fftr_cfg cfg = kiss_fftr_alloc(RING_SIZE, 0, &kern->cfg, &kern->cfg_len); // returns NULL, but gives us the minimum size cfg_len we need
	kern->cfg = malloc(kern->cfg_len);
	cfg = kiss_fftr_alloc(RING_SIZE, 0, kern->cfg, &kern->cfg_len); // Now it'll work!
	
	kiss_fftr_cfg icfg = kiss_fftr_alloc(RING_SIZE, 1, kern->icfg, &kern->icfg_len);
	kern->icfg = malloc(kern->cfg_len);
	icfg = kiss_fftr_alloc(RING_SIZE, 1, kern->icfg, &kern->icfg_len);
	
	if(cfg) {
		printf("Config good\n");
	} else {
		printf("C BAD\n");
	}
	if(icfg) {
		printf("iConfig good\n");
	} else {
		printf("iC BAD\n");
	}
	printf("C: %zu\n", kern->cfg_len);
	printf("iC: %zu\n", kern->icfg_len);
	if(kern->cfg) {
		printf("Good\n");
	}
	return kern;
}

bool process_input(fourier_kernel_t* kernel){//repeating_timer_t* rt) { // This is a repeating timer callback
	// Gets the input from the ADC (technically this is done by DMA, so no worries)
	// Applies some kernel ((fourier_kernel_t*)(rt->user_data)->kernel to the input
	// Applies the output to the DAC
	// Steps along the kernel ((fourier_kernel_t*)(rt->user_data)->time)
	
	//fourier_kernel_t* kernel = (fourier_kernel_t*)(rt->user_data);

	// Input handled by DMA
	//uint16_t latest_input = adc_data_ring[++kernel->time % RING_SIZE]; // step along time

	// Apply FT (kernel application baked in!)
	kiss_fftr(kernel->cfg, (kiss_fft_scalar*)adc_data_ring, kernel->fourier_transform);
	//for(uint i = 0; i < KERNEL_SIZE; i++) {
	//	double complex tau  = i/REFRESH_RATE;
	//	double complex wave = cexp(I*2*PI*tau);
	//	for(uint j = 0; j < KERNEL_SIZE; j++) {
	//		kernel->fourier_transform[j] = wave * adc_data_ring[(kernel->time - i + RING_SIZE) % RING_SIZE] * kernel->kernel[j];
	//	}
	//}

	// Get current wave strength
	//double complex amp = 0.0;
	//for(uint i = 0; i < KERNEL_SIZE; i++) {
	//	amp += cexp(-I * 2*PI*i * kernel->fourier_transform[i]);
	//}
	//uint16_t actual_amplitude = (uint16_t)((creal(amp)*creal(amp) + cimag(amp)*cimag(amp)) * 4095);

	// output to DAC
	//set_dac_amplitude(kernel->dac, actual_amplitude);
	//my2c_write_free(kernel->dac, kernel->dac_address, bytes, 2);
}

#endif // FOURIER_H
