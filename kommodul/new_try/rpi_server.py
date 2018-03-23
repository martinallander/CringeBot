import bluetooth
import time
import pickle

#Rpi bluetooth adress
bd_addr = "B8:27:EB:A3:F1:C9"

def open_server(server_sock, port):
    server_sock.bind(("",port))

def connection_accept(server_sock):
    client_sock,address = server_sock.accept()
    print "Accepted connection from ",address
    return client_sock

def listen_to_client(server_sock):
    server_sock.listen(1)

def close_server(client_sock, server_sock):
    client_sock.close()
    server_sock.close()


def printls(ls):
    for item in ls:
        print(item)

def main():
    server_sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
    open_server(server_sock,6)
    listen_to_client(server_sock)
    client_sock = connection_accept(server_sock)
    time.sleep(10)
    client_sock.send("xD")
    close.server(client_sock, server_sock)
main()


