from machine import Pin, PWM, Timer
from time import sleep_ms, time, ticks_ms, ticks_diff
import rp2
from lcd import lcd

import uctypes


screen = lcd(1, 39, Pin(27), Pin(26))
screen.clear()
screen.print('Welcome to DPedal!')
screen.set_line(2)
screen.print('Initializing...')
screen.set_line(3)

base_frequency = 4186*2*2 # highest frequency we can achieve (C10, 16kHz or so, very high)
screen.print(f'f = {base_frequency/1000}kHz')
screen.set_line(3)
waveform_length = base_frequency//2 # gives us one quarter second as the clock runs at double time
waveform_descriptor = {
    'amplitudes': (0 | uctypes.ARRAY, waveform_length | uctypes.UINT8)
}

led = Pin(25, Pin.OUT)
led.on()

signal_out = Pin(15, Pin.OUT)

microtick = 0
def run_waveform(wf):
    def switch(timer):
        global microtick
        if (microtick % 2 == 0) and wf.amplitudes[microtick]:
            signal_out.on()
        else:
            signal_out.off()
        microtick += 1
        if(microtick == waveform_length):
            #timer.deinit()
            microtick = 0

    base_timer = Timer()
    base_timer.init(mode=Timer.PERIODIC, freq=base_frequency * 2, callback=switch)

wf = bytearray([0]*waveform_length)
wf_struct = uctypes.struct(uctypes.addressof(wf), waveform_descriptor)

#import math

freq = [4186/16, 4186/16 + 4186/32]
for i in range(waveform_length//32): # c4, middle c
#    #wf_struct.amplitudes[i] += int(math.cos(2*3.1415 * i * freq[1]/base_frequency)*0.999999999 + 1.0)
    wf_struct.amplitudes[i*32] = 1
for i in range(waveform_length//48): # c4, middle c
    wf_struct.amplitudes[i*48] = 1
for i in range(waveform_length//15): # c4, middle c
    wf_struct.amplitudes[i*15] = 1

screen.set_line(4, col=10-2)
screen.print('Done!')
while(True):
    pass
screen.clear()
run_waveform(wf_struct)
#sleep_ms(1000)
#for i in range(waveform_length//16):
#    wf_struct.amplitudes[i*16] = 0
#for i in range(waveform_length//64):
#    wf_struct.amplitudes[i*64] = 1

