#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "ADC.h"

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

	uint time = 0;
	while(1) {
		uint16_t v = adc_data_ring[time % (RING_SIZE)];
		float conversion_factor = 3.3f/(1<<12);

		printf("%d: Raw: %d. Converted: %f\n", time%(RING_SIZE), v, conversion_factor * v);

		time++;
	}
	while(1) {
	}

	return 0;
}
