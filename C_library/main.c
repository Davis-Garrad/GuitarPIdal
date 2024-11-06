#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "I2C.h"
#include "ADC.h"
#include "LCD.h"

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
	init_repeated_read(26);
	sleep_ms(100);

	i2c_inst_t* i2c = my2c_init(1, 18, 19, 100000);
	//sleep_ms(250);
	//my2c_scan(i2c);
	lcd_inst_t* lcd = lcd_init(i2c, 0x27);
	lcd_print(lcd, "testing", 7, 0, 0);

	uint time = 0;
	char printstr[40];

	absolute_time_t st = get_absolute_time();

	while(time < 1000) {
		uint16_t v = adc_data_ring[++time % (RING_SIZE)];
		float conversion_factor = 3.3f/(1<<12);

		sprintf(printstr, "%d %fV", time, conversion_factor * v);
		lcd_print(lcd, printstr, strlen(printstr), 0, 0);
	}

	absolute_time_t et = get_absolute_time();
	int64_t us = absolute_time_diff_us(st, et);
	double s = (double)us/1000000.f;
	double tpt = s/1000.0f;
	printf("uSeconds: %lld (1000 trials, so %fs/trial)\n", us, tpt);

	while(1) {
	}

	return 0;
}
