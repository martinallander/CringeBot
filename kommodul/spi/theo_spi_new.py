import spidev
import time
import RPi.GPIO as GPIO
import keyboard

sensmodul_spi = spidev.SpiDev()
sensmodul_spi.open(0, 0)
sensmodul_spi.max_speed_hz = 2000000
    

styrmodul_spi = spidev.SpiDev()
styrmodul_spi.open(0, 1)
styrmodul_spi.max_speed_hz = 2000000

to_send = [0x01, 0x02, 0x03]

print('Please wait.. \n .... \n ......... \n')

def gpio_setup():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(4, GPIO.IN)

#mode = 0b01, 0b10, 0b11
def set_mode(module, mode_id):
    module.mode = mode_id


def write_sensmodul(input):
    msb = input >> 8
    lsb = input & 0xFF
    sensmodul_spi.xfer([msb, lsb])


def write_styrmodul(input):
    msb = input >> 8
    lsb = input & 0xFF
    styrmodul_spi.xfer([msb, lsb])
    
def test_data_sens():
	sensmodul_spi.writebytes([0xAA])
	resp = sensmodul_spi.readbytes(1)
	if (resp[0] == 0x01):
		print('success')
	else:
		print('fail ')
		print(resp)
	time.sleep(1)
	sensmodul_spi.writebytes([0x05])
	resp = sensmodul_spi.readbytes(1)
	if (resp[0] == 0x02):
		print('success')
	else:
		print('fail ')
		print(resp)
	time.sleep(1)
	sensmodul_spi.writebytes([0x0F])
	resp = sensmodul_spi.readbytes(1)
	if (resp[0] == 0x03):
		print('success')
	else:
		print('fail ')
		print(resp)
	time.sleep(1)
			
def test_data_styr():
	styrmodul_spi.writebytes([0xAA])
	resp = styrmodul_spi.readbytes(1)
	if (resp[0] == 0x01):
		print('success')
	else:
		print('fail ')
		print(resp)
	time.sleep(2)
	styrmodul_spi.writebytes([0x05])
	resp = styrmodul_spi.readbytes(1)
	if (resp[0] == 0x02):
		print('success')
	else:
		print('fail ')
		print(resp)
	time.sleep(1)

def main(input_data):
    set_mode(sensmodul_spi, 0b00)
    set_mode(styrmodul_spi, 0b00)
    try:
	while True:
            print('Please select the use of the program by writing SPITEST or ROBOTTEST or CONTROL')
            inp = raw_input().lower()
            if inp == 'spitest':
                while True:
		    print('Please select module to test by writing SENSORMODUL or STYRMODUL')
		    inp = raw_input().lower()
		    if inp == 'sensormodul':
			test_data_sens()
		    elif inp == 'styrmodul':
			test_data_styr()
		    elif inp == 'end':
                        break
		    else:
			print('wrong input')
		    time.sleep(0.5)
	    elif inp == 'robottest':
		while True:
		    print('Please select robot movement by writing EXTEND or CONSTRICT')
		    inp = raw_input().lower()
		    if inp == 'extend':
			styrmodul_spi.writebytes([0x15])
		    elif inp == 'constrict':
			styrmodul_spi.writebytes([0x10])
		    elif inp == 'end':
                        break
		    else:
			print('wrong input')
		    time.sleep(0.5)
	    elif inp == 'control':
                print('WARNING!! This setting is only for advanced users xD')
                time.sleep(1)
                print('Are you sure you wish to continue? y n')
                inp = raw_input().lower()
                if inp == 'y':
                    while True:
                        styrmodul_spi.writebytes([0x06])
                        print('please insert an angle for servo 6')
                        inp1 = input()
                        styrmodul_spi.writebytes([inp1])
                        print('please insert an angle for servo 7')
                        styrmodul_spi.writebytes([0x07])
                        inp2 = input()
                        styrmodul_spi.writebytes([inp2])
                        print('please insert an angle for servo 8')
                        styrmodul_spi.writebytes([0x08])
                        inp3 = input()
                        styrmodul_spi.writebytes([inp3])
                        break
                else:
                    break
    except KeyboardInterrupt:
        styrmodul_spi.close()
        sensmodul_spi.close()
        print('\n\n...........interrupted..............!\n\n')
        print('See you soon! ...')

def lol():
	while True:
		time.sleep(1)
		GPIO.wait_for_edge(4, GPIO.RISING)
		print('wa wa')
		GPIO.wait_for_edge(4, GPIO.FALLING)
		print('dick suck')
		
def keyboard_test():
    time.sleep(1)
    print('Controls - W to go forward \n S to go backwards \n A to go left \n D to go right')
    while True:
        W = keyboard.is_pressed('w')
        S = keyboard.is_pressed('s')
        A = keyboard.is_pressed('a')
        D = keyboard.is_pressed('d')
        if W:
            #print('going forward..')
            styrmodul_spi.writebytes([0xAA])
            time.sleep(1)
        elif S:
            #print('going backwards')
            styrmodul_spi.writebytes([0x05])
            time.sleep(1)
        elif A:
            print('going left')
            time.sleep(0.1)
        elif D:
            print('going right')
            time.sleep(0.1)
        else:
            time.sleep(0.01)
        time.sleep(0.01)
        
def input_test():
    time.sleep(1)
    print('please write styrmodul or sensormodul')
    inp = raw_input()
    if inp == 'sensormodul':
        styrmodul_spi.writebytes([0xAA])
    elif inp == 'styrmodul':
        styrmodul_spi.writebytes([0x05])
    else:
        print('wrong input')
        
main(to_send)
#keyboard_test()
#input_test()




#Kolla sida 167 i atmega databladet for clockfas/polaritet
