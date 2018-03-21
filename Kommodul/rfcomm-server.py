import bluetooth
import time

server_sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )

port = 6
#server_sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
server_sock.bind(("",port))
server_sock.listen(1)

client_sock,address = server_sock.accept()
print "Accepted connection from ",address

tic = time.clock()

data = client_sock.recv(1024)
#print "received [%s]" % data

toc = time.clock()

print(toc-tic)
client_sock.close()
server_sock.close()
