#include "hardware/spi.h"
#include "hardware/gpio.h"

typedef struct dac {
	spi_inst_t* spi_bus;
	uint cs_pin;
} dac_t;

// config bits: write to DAC_A, -, don't double ref voltage, and don't shut down
#define DAC_CONFIG (0b0011<<12)

// SP(my)
spi_inst_t* spmi_init(uint number, uint baudrate) {
	spi_inst_t* inst = SPI_INSTANCE(number);
	spi_init(inst, baudrate);
	spi_set_format(inst, 8, SPI_CPHA_0, SPI_CPOL_0, SPI_MSB_FIRST);
	spi_set_slave(inst, false);
	return inst;
}

dac_t* init_dac(uint spi_id, uint baudrate, uint sck_pin, uint mosi_pin, uint cs_pin) {
	dac_t* dac = malloc(sizeof(dac_t));
	dac->cs_pin = cs_pin;
	dac->spi_bus = spmi_init(spi_id, baudrate);

	gpio_init(cs_pin);
	gpio_pull_up(cs_pin);
	gpio_set_dir(cs_pin, true);
	gpio_put(cs_pin, 1);

	gpio_init(sck_pin);
	gpio_set_function(sck_pin, GPIO_FUNC_SPI);
	gpio_set_dir(sck_pin, true);	

	gpio_init(mosi_pin);
	gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
	gpio_set_dir(sck_pin, true);	

	return dac;
}

void set_dac_amplitude(dac_t* dac, uint16_t amp) {
	uint16_t amp_real = amp + DAC_CONFIG;
	uint8_t amps[2] = {amp_real >> 8, amp_real & 0xf };
	gpio_put(dac->cs_pin, 0);
	spi_write_blocking(dac->spi_bus, amps, 2);
	gpio_put(dac->cs_pin, 1);
}
