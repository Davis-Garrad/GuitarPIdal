from machine import Pin, PWM, Timer
from time import sleep_ms, time, ticks_ms, ticks_diff
import rp2
from lcd import lcd

import uctypes

def sleeper(ms):
    st = ticks_ms()
    while(ticks_diff(ticks_ms(), st) < ms):
        pass

screen = lcd(1, 39, Pin(27), Pin(26))
screen.clear()
screen.print('Welcome to DPedal!')
screen.set_line(2)
screen.print('Initializing...')
sleeper(500)
screen.clear()
screen.set_line(1)
screen.print('Init')
screen.set_line(2)

base_frequency = 4186*2*2*2 # highest frequency we can achieve (C11, 32kHz or so, very high)
screen.print(f'f={base_frequency/1000}kHz')
screen.set_line(3)
waveform_length = base_frequency//2 # gives us one quarter second as the clock runs at double time
screen.print(f'buf={waveform_length//1000}kB')
waveform_descriptor = {
    'amplitudes': (0 | uctypes.ARRAY, waveform_length | uctypes.UINT8)
}

led = Pin(25, Pin.OUT)
led.on()
t = Timer()
led.toggle()
t.init(mode=Timer.PERIODIC, freq=2, callback=lambda t: led.toggle())

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
t.deinit()
for i in range(6):
    led.toggle()
    sleeper(1000//6)
led.on()
screen.clear()
screen.set_line(1)
screen.print('DP')
run_waveform(wf_struct)

#sleep_ms(1000)
#for i in range(waveform_length//16):
#    wf_struct.amplitudes[i*16] = 0
#for i in range(waveform_length//64):
#    wf_struct.amplitudes[i*64] = 1

