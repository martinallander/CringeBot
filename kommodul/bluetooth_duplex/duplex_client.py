import bluetooth
import time
from rasp_data import robot_data as rd

robots[]

while i < 10:
	robots.append(rd(i,i+1))
	i = i + 1


#Device adress found using finddevice.py
bd_addr = "98:5F:D3:35:FC:EA"

#Make sure port is same in server and client, ports 1-30
port = 6

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))

tic = time.clock()
sock.send(robots)

toc = time.clock()
print(toc-tic)
sock.close()
