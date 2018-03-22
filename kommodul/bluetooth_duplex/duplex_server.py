import bluetooth
import time
import pickle

server_sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )

#Make sure port is same in server and client, ports 1-30
port = 6

server_sock.bind(("",port))
server_sock.listen(1)

client_sock,address = server_sock.accept()
print "Accepted connection from ",address

upkl_list = list()

tic = time.clock()
i = 0
while True:
    data = client_sock.recv(1024)
    upkl = pickle.loads(data)
    upkl_list.append(upkl)
    if upkl.stop:
        break
    if i == 20:
        break
    i += 1

client_sock.close()
server_sock.close()

for item in upkl_list:
    print(item)
#print "received [%s]" % data
#toc = time.clock()
#print(toc-tic)


