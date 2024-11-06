#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/i2c.h"

#ifndef I2C_H
#define I2C_H

bool my2c_reserved_address(uint8_t addr) {
	return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

i2c_inst_t* my2c_init(uint id, uint sda, uint scl, uint scl_rate) {
	
	printf("Opening I2C on pins SDA: %d/SCL: %d, baudrate %d\n", sda, scl, scl_rate);
	i2c_inst_t* i2c_instance = i2c_get_instance(id);

	i2c_init(i2c_instance, scl_rate);
	gpio_set_function(sda, GPIO_FUNC_I2C);
	gpio_set_function(scl, GPIO_FUNC_I2C);
	gpio_pull_up(sda);
	gpio_pull_up(scl);

	return i2c_instance;
}

void my2c_write(i2c_inst_t* bus, uint8_t address, uint8_t* bytes, size_t len) {
	i2c_write_blocking_until(bus, address, bytes, len, false, make_timeout_time_ms(50));
}

void my2c_scan(i2c_inst_t* bus) {
	printf("\nI2C Bus Scan\n");
	printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
	for(uint8_t addr = 0; addr <= (1<<7); addr++) {
		if(addr % 16 == 0)
			printf("%02x ", addr);
		int ret;
		uint8_t rxdata;
		if(my2c_reserved_address(addr)) {
			ret = PICO_ERROR_GENERIC;
		} else {
			ret = i2c_read_blocking_until(bus, addr, &rxdata, 1, false, make_timeout_time_ms(10));
		}
		switch(ret) {
			case PICO_ERROR_GENERIC:
				printf(" ");
				break;
			case PICO_ERROR_TIMEOUT:
				printf("T");
				break;
			default:
				printf("X");
				break;
		}
		printf(addr % 16 == 15 ? "\n" : "  ");
		sleep_ms(10);
	}
}

#endif // I2C_H
