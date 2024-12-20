#include <complex.h>

#include "pico/stdlib.h"

#include "kissfft/kiss_fftr.h"

#include "ADC.h"
#include "I2C.h"

#define KERNEL_SIZE RING_SIZE
#define PI 3.1415926535897932386

#ifndef FOURIER_H
#define FOURIER_H

static int16_t fourier_transform_real[KERNEL_SIZE];
static int16_t fourier_transform_imag[KERNEL_SIZE];

typedef struct fourier_kernel {
	dac_t* dac;
	size_t current_index;
	//double complex fourier_transform[KERNEL_SIZE];
} fourier_kernel_t;


fourier_kernel_t* create_kernel(dac_t* dac) {
	fourier_kernel_t* kernel = malloc(sizeof(fourier_kernel_t));
	
	kernel->current_index = 0;

	for(unsigned int i = 0; i < KERNEL_SIZE; i++) {
		fourier_transform_imag[i] = 1<<11;
		fourier_transform_real[i] = 1<<11;
	}
	//kernel->fourier_transform = malloc(sizeof(double complex)*KERNEL_SIZE);
	return kernel;
}

bool process_input(fourier_kernel_t* kernel){//repeating_timer_t* rt) { // This is a repeating timer callback
	// Gets the input from the ADC (technically this is done by DMA, so no worries)
	// Applies some kernel ((fourier_kernel_t*)(rt->user_data)->kernel to the input
	// Applies the output to the DAC
	// Steps along the kernel ((fourier_kernel_t*)(rt->user_data)->time)
	
	//fourier_kernel_t* kernel = (fourier_kernel_t*)(rt->user_data);

	// Input handled by DMA
	uint16_t oldest_input = adc_data_ring[kernel->current_index];
	kernel->current_index = (kernel->current_index + 1) % KERNEL_SIZE;
	uint16_t latest_input = adc_data_ring[kernel->current_index]; // step along time
	int16_t diff = latest_input - oldest_input;
	printf("%d\n", diff);
	// Compute the new DFT
	for(size_t i = 0; i < KERNEL_SIZE; i++) {
		fourier_transform_real[i] -= diff;
		fourier_transform_real[i] *= sin((2*PI*i) / KERNEL_SIZE);
		fourier_transform_imag[i] *= cos((2*PI*i) / KERNEL_SIZE);
		//double complex ft = cexp(I * (2*PI * i)/KERNEL_SIZE);
		//ft *= fourier_transform[i] - diff;
		//fourier_transform[i] = ft;
	}
	
	
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
