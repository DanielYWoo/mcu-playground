import serial
import time

# # Serial port parameters
serial_speed = 9600
serial_port = '/dev/tty.zhamao' # bluetooth shield hc-06


if __name__ == '__main__':
	ser = serial.Serial(serial_port, serial_speed, timeout=1)
	ser.write('?')

	data = ser.readline()

	if (data != ""):
		print ("arduino says: %s" % data)
	else:
		print ("arduino doesnt respond")

	time.sleep(4)
