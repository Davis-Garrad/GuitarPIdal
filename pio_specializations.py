import rp2

# can give each statemachine a frequency, up to the system clock, this runs at 1/64th of that frequency
highest_frequency = 32_000 # 32kHz
@rp2.asm_pio(set_init=rp2.PIO.OUT_LOW, out_init=rp2.PIO.OUT_LOW, autopull=True, fifo_join=rp2.PIO.JOIN_TX, out_shiftdir=rp2.PIO.SHIFT_RIGHT, pull_thresh=1)
def test():
    wrap_target()
    out(pins, 1) [31]
    set(pins, 0) [31]
    wrap()

from machine import Pin
from array import array

wf1 = array('B', [ 1 if (i%int(highest_frequency/440)==0 or i%int(highest_frequency/263)==0) else 0 for i in range(8000)])

sm1 = rp2.StateMachine(1, test, freq=32_000*64, set_base=Pin(15), out_base=Pin(15))

sm1.active(1)

sm1.put(wf1)
