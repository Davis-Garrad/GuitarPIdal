#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"

#define REFRESH_RATE 40000

#include "I2C.h"
#include "ADC.h"
#include "DAC.h"
#include "LCD.h"
#include "Fourier.h"

void flash() {
	for(unsigned int i = 0; i < 5; i++) {
		gpio_put(25, 1);
		sleep_ms(100);
		gpio_put(25, 0);
		sleep_ms(100);
	}
}

int main() {
	//Initialise I/O
	stdio_init_all(); 

	// initialise GPIO (Green LED connected to pin 25)
	gpio_init(25);
	gpio_set_dir(25, GPIO_OUT);
	sleep_ms(1000);
	flash();

	printf("RING_SIZE: %d\n", RING_SIZE);
	init_repeated_read(26, REFRESH_RATE);
	sleep_ms(100);

	i2c_inst_t* i2c = my2c_init(1, 18, 19, 100000);
	//sleep_ms(250);
	//my2c_scan(i2c);
	lcd_inst_t* lcd = lcd_init(i2c, 0x27);
	lcd_print(lcd, "testing", 7, 0, 0);

	uint time = 0;
	char printstr[40];


	int64_t refresh_period_us = (int64_t)(1000000.0/REFRESH_RATE);
	
	dac_t* dac = init_dac(1, 65*1000*1000, 10, 11, 13);
	fourier_kernel_t* kern = create_kernel(dac);

	absolute_time_t st = get_absolute_time();
	for(uint i = 0; i < (100); i++) {
		process_input(kern);
	}
	absolute_time_t et = get_absolute_time();
	printf("Time: %lld, %f", absolute_time_diff_us(st, et), (double)absolute_time_diff_us(st,et)/(100));
	for(uint i = 0; i < 20; i++) {
		printf("%f + i%f\n", kern->fourier_transform[i].r, kern->fourier_transform[i].i);
	}
	printf("Done!\n");
	kiss_fft_scalar* real_result = malloc(sizeof(kiss_fft_scalar)*RING_SIZE);
	kiss_fftri(kern->icfg, kern->fourier_transform, real_result);
	for(uint i = 0; i < 20; i++) {
		printf("%f\n", real_result[i]);
	}

	set_dac_amplitude(dac, 2048);

	while(true) {
		// define some sort of timekeeping routine
		// read from the ADC, add the result to the rolling buf. (done automatically by DMA!)
		// apply some sort of kernel to the rolling buf
		// Write out to the 
		// use a separate thread for writing to the LCD
		uint16_t v = adc_data_ring[++time % (RING_SIZE)];
		float conversion_factor = 3.3f/(1<<12);

		sprintf(printstr, "%d %fV", time, conversion_factor * v);
		lcd_print(lcd, printstr, strlen(printstr), 0, 0);

		sleep_ms(100);
	}

	while(1) {
	}

	return 0;
}
