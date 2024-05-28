from machine import Pin, PWM, Timer
from time import sleep_ms, time, ticks_ms, ticks_diff
import rp2

import uctypes

base_frequency = 4186 # highest frequency we can achieve (C8)
waveform_length = base_frequency*2 # gives us one full second as the clock runs at double time
waveform_descriptor = {
    'amplitudes': (0 | uctypes.ARRAY, waveform_length | uctypes.UINT8)
}

led = Pin(25, Pin.OUT)
led.on()

signal_out = Pin(15, Pin.OUT)


microtick = 0
def run_waveform(wf):
    microtick=0
    def switch(timer):
        global microtick
        if (microtick % 2 == 0) and wf.amplitudes[microtick]>0:
            signal_out.on()
        else:
            signal_out.off()
        microtick += 1
        if(microtick == waveform_length):
            timer.deinit()

    base_timer = Timer()
    base_timer.init(mode=Timer.PERIODIC, freq=base_frequency * 2, callback=switch)

wf = bytearray([1]*waveform_length)
print(len(wf))
wf_struct = uctypes.struct(uctypes.addressof(wf), waveform_descriptor)
print(wf_struct)
print(wf_struct.amplitudes[32])
run_waveform(wf_struct)
