import bluetooth
import time
import pickle
from rasp_data import robot_data as rd

robots = list()

i = 1

#pickle.dumps(robots,[2])

stop = 10
while i <= stop:
	if (i == stop):
		robots.append(rd(i,i+1,True))
	else:
		robots.append(rd(i,i+1))
	i += 1

for rob in robots:
	print(rob)


#Device adress found using finddevice.py
bd_addr = "98:5F:D3:35:FC:EA"

#Make sure port is same in server and client, ports 1-30
port = 6

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))

tic = time.clock()

#for robot in robots:
#		print(robot.stop)

while True:
	for robot in robots:
		time.sleep(0.05)
		sock.send(pickle.dumps(robot))
		print(robot)

toc = time.clock()
print(toc-tic)
sock.close()
