import bluetooth
import time
import pickle

#Rpi bluetooth adress
bd_addr = "B8:27:EB:A3:F1:C9"

def open_server(server_sock, port):
    server_sock.bind(("",port))

def listen_to_socket(server_sock):
    server_sock.listen(1)

def server_accept(server_sock):
    client_sock,address = server_sock.accept()
    print "Accepted connection from ",address
    return client_sock

def close_server(client_sock, server_sock):
    client_sock.close()
    server_sock.close()

def printls(ls):
    for item in ls:
        print(item)


def main():
    server_sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
    #Make sure port is same in server and client, ports 1-30
    open_server(server_sock,6)
    listen_to_socket(server_sock)
    client_sock = server_accept(server_sock)
    
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
    printls(upkl_list)
    close_server(client_sock, server_sock)

main()
#print "received [%s]" % data
#toc = time.clock()
#print(toc-tic)


