import bluetooth
import time


#Device adress found using finddevice.py
bd_addr = "98:5F:D3:35:FC:EA"

#Make sure port is same in server and client, ports 1-30
port = 6

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))

tic = time.clock()
i = 1
while i < 10:
	sock.send("xD\n")
	i = i + 1

toc = time.clock()
print(toc-tic)
sock.close()
