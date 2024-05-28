import machine
from time import sleep_ms

Enable = 0x04 #Set Entry Mode
RS = 0x01 # Register select bit
Width = 20

class lcd:
   #initializes objects and lcd
   def __init__(self,id,ADDR,SCLPIN,SDAPIN):
      self.i2c=machine.I2C(id, scl=machine.Pin(SCLPIN), sda=machine.Pin(SDAPIN))
      self.address=ADDR
      self.BKLIGHT=0x08 #Set Backlight ON
      self.write(0x03)
      self.write(0x03)
      self.write(0x03)
      self.write(0x02)

      self.write(0x20 | 0x08 | 0x04 | 0x00) #Set Functions 2Line,5x8,4Bit Mode
      self.write(0x08 | 0x04) #Turn Display On
      self.write(0x01) #Clear Screen
      self.write(0x04 | 0x02) #Set Entry Mode Left -> Right
      sleep_ms(300)

   def strobe(self, data):
      self.i2c.writeto(self.address,bytes([data | Enable | self.BKLIGHT]))
      sleep_ms(1)
      self.i2c.writeto(self.address,bytes([(data & ~Enable) | self.BKLIGHT]))
      sleep_ms(1)

   def write_four_bits(self, data):
      self.i2c.writeto(self.address,bytes([data | self.BKLIGHT]))
      self.strobe(data)

   # write a command to lcd
   def write(self, cmd, RS=0):
      self.write_four_bits(RS | (cmd & 0xF0))
      self.write_four_bits(RS | ((cmd << 4) & 0xF0))

   def set_line(self,line,col=0):
      if line == 1:
         self.write(0x80+col)
      if line == 2:
         self.write(0xC0+col)
      if line == 3:
         self.write(0x94+col)
      if line == 4:
         self.write(0xD4+col)

   def print(self, string, line=0, col=0):
      self.set_line(line,col)
      i = 1
      for char in string:
         if ((i > Width) & (line < 4)):
            line = line + 1
            self.set_line(line,0)
            i = 1
         if ((i > Width) & (line == 4)):
			 break
         self.write(ord(char),RS)
         i = i + 1

   def off(self):
      self.lcd_write(0x08 | 0x00)

   def on(self):
      self.lcd_write(0x08 | 0x04)

   def clear(self):
      self.write(0x01) #Clear Screen
      self.write(0x02) #Set Home

   def backlight(self, on):
      if on:
         self.BKLIGHT=0x08
      else:
         self.BKLIGHT=0x00
         
      self.write(0)
