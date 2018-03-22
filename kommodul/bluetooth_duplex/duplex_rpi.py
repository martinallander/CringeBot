import bluetooth
import time
import pickle
from rasp_data import robot_data as rd

#Device adress found using finddevice.py
ma_bd_addr = "98:5F:D3:35:FC:EA"

def make_robots(amount):
	robots = list()
	i = 1
	while i <= amount:
		if (i == amount):
			robots.append(rd(i,i+1,True))
		else:
			robots.append(rd(i,i+1))
		i += 1
	return robots

def connect_to_server(sock, bd_addr, port):
	sock.connect((bd_addr, port))

def close_server(sock):
	sock.close()

def main():
	sock = bluetooth.BluetoothSocket( bluetooth.RFCOMM )
	my_robots = make_robots(20)
	connect_to_server(sock, ma_bd_addr, 6)
	tic = time.clock()
	while True:
		for robot in my_robots:
			time.sleep(0.05)
			sock.send(pickle.dumps(robot))
			print(robot)

	toc = time.clock()
	print(toc-tic)

main()

	
