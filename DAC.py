from machine import SPI, Pin
import time

spi_bus = SPI(1, baudrate=20_000_000, bits=8, sck=Pin(10), mosi=Pin(11))
cs_pin = Pin(13, Pin.OUT, pull=Pin.PULL_UP)
cs_pin.value(1)
max_val = 2**12 - 1

config = 3<<12 # write to DAC_A, -, don't double the ref voltage, and don't shut down
#TODO

def set_duty_cycle(dc: float):
    '''Duty cycle is 0-1. Not technically a duty cycle, but whatever. Not calling it "relative-to-max voltage" or something stupid like that'''
    v = int(max_val * dc + config)
    cs_pin.value(0)
    time.sleep_ms(1)
    spi_bus.write(bytes([v >> 8, v & 0xff]))
    time.sleep_ms(1)
    cs_pin.value(1)

