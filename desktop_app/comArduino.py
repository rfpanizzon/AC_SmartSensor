##################################herestart
#!/usr/bin/python
import serial
#import syslog
import time

#The following line is for serial over GPIO
port = 'COM6' # note I'm using Mac OS-X


ard = serial.Serial(port,9600)
time.sleep(2) # wait for Arduino

# Serial write section

setTempCar1 = "5"
print ("Python value sent: ")
print (setTempCar1)
ard.write(setTempCar1.encode())
time.sleep(1) # I shortened this to match the new value in your Arduino code

# Serial read section
msg = ard.read(ard.inWaiting()) # read all characters in buffer
print ("Message from arduino: ")
print (msg)
