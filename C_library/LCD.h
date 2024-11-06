#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "I2C.h"

#include <string.h>

#ifndef LCD_H
#define LCD_H

typedef struct lcd_inst {
	i2c_inst_t* i2c_bus;
	uint8_t address;
	uint8_t backlight;

} lcd_inst_t;

#define ENABLE 0x04 // Set entry mode
#define RS 0x01 // Register select bit
#define WIDTH 20 // Actual screen width

void lcd_write_4bits(lcd_inst_t* lcd, uint8_t data) {
	// send 4 bits
	uint8_t bs[1] = { data | lcd->backlight };
	my2c_write(lcd->i2c_bus, lcd->address, bs, 1);

	// strobe
	bs[0] = data | ENABLE | lcd->backlight;
	my2c_write(lcd->i2c_bus, lcd->address, bs, 1);
	//sleep_ms(1);
	bs[0] = (data & !ENABLE) | lcd->backlight;
	my2c_write(lcd->i2c_bus, lcd->address, bs, 1);
	//sleep_ms(1);
}

void lcd_write_cfg(lcd_inst_t* lcd, uint8_t data) {
	lcd_write_4bits(lcd, (data & 0xf0));
	lcd_write_4bits(lcd, ((data << 4) & 0xf0));
}

void lcd_write(lcd_inst_t* lcd, uint8_t data) {
	lcd_write_4bits(lcd, RS | (data & 0xf0));
	lcd_write_4bits(lcd, RS | ((data << 4) & 0xf0));
}

lcd_inst_t* lcd_init(i2c_inst_t* i2c_bus, uint8_t address) {
	lcd_inst_t* lcd = malloc(sizeof(lcd_inst_t));
	lcd->i2c_bus = i2c_bus;
	lcd->address = address;
	lcd->backlight = 0x08;
	lcd_write_cfg(lcd, 0x03); // black box magick
	lcd_write_cfg(lcd, 0x03);
	lcd_write_cfg(lcd, 0x03);
	lcd_write_cfg(lcd, 0x02);
	
	// Actual config
	lcd_write_cfg(lcd, 0x20 | 0x08 | 0x04 | 0x00); // 2line, 5x8, 4bit mode
	lcd_write_cfg(lcd, 0x08 | 0x04); // Display on
	lcd_write_cfg(lcd, 0x01); // Clear scrn
	lcd_write_cfg(lcd, 0x04 | 0x02); // Left->Right

	//sleep_ms(250); // For safety!

	return lcd;
}

void lcd_set_line(lcd_inst_t* lcd, uint8_t line, uint8_t col) {
	uint8_t add = 0;
	switch(line) {
		case 1:
			add = 0x80;
			break;
		case 2:
			add = 0xc0;
			break;
		case 3:
			add = 0x94;
			break;
		case 4: 
			add = 0xd4;
			break;
		default:
			add = 0x80;
			break;
	}
	add += col;
	lcd_write_cfg(lcd, add);
	//my2c_write(lcd->i2c_bus, lcd->address, add, 1);
}

void lcd_print(lcd_inst_t* lcd, char* string, size_t string_len, uint8_t line, uint8_t col) {
	uint i = col;
	uint8_t cur_line = line;
	lcd_set_line(lcd, cur_line, i);
	for(unsigned int j = 0; j < string_len; j++) {
		if(i > WIDTH && cur_line < 4) {
			cur_line++;
			lcd_set_line(lcd, cur_line, 0);
			i = 0;
		} else if(i > WIDTH && cur_line >= 4) {
			break;
		}
		lcd_write(lcd, string[j]);
		i++;
	}
}

void lcd_set_power(lcd_inst_t* lcd, bool on) {
	lcd_write_cfg(lcd, 0x08 | (0x04 * on));
}

void lcd_clear(lcd_inst_t* lcd) {
	lcd_write_cfg(lcd, 0x01); // Clear
	lcd_write_cfg(lcd, 0x02); // Set home
}

void lcd_backlight(lcd_inst_t* lcd, bool state) {
	lcd->backlight = 0x08 * state;
	lcd_write_cfg(lcd, 0x0);
}

#endif // LCD_H
