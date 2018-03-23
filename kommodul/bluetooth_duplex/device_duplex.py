import bluetooth
import time
import pickle

#Rpi bluetooth adress
bd_addr = "B8:27:EB:A3:F1:C9"

def open_rec_server(server_sock, port):
    server_sock.bind(("",port))

def listen_to_server(server_sock):
    server_sock.listen(1)

def rec_server_accept(server_sock):
    client_sock,address = server_sock.accept()
    print "Accepted connection from ",address
    return client_sock

def close_reciever(client_sock, server_sock):
    client_sock.close()
    server_sock.close()

def connect_to_server(sock, bd_addr, port):
	sock.connect((bd_addr, port))

def close_sender(sock):
	sock.close()


def printls(ls):
    for item in ls:
        print(item)

def main():
    server_sock_rec=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
    server_sock_send=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
    #Make sure port is same in server and client, ports 1-30
    open_rec_server(server_sock_rec,6)
    listen_to_server(server_sock_rec)
    client_sock = rec_server_accept(server_sock_rec)
    
    upkl_list = list()
    tic = time.clock()
    i = 0
    while True:
        data = client_sock.recv(1024)
        upkl = pickle.loads(data)
        upkl_list.append(upkl)
        if upkl.stop:
            break
        i += 1
    printls(upkl_list)
    connect_to_server(server_sock_send, bd_addr, 5)
    server_sock_send.send(pickle.dumps(upkl_list[1]))
    close_sender(server_sock_send)
    close_reciever(client_sock, server_sock_send)

main()
#print "received [%s]" % data
#toc = time.clock()
#print(toc-tic)


