import bluetooth
import time
import pickle

#Rpi bluetooth adress
bd_addr = "B8:27:EB:A3:F1:C9"

def connect_to_server(sock, bd_addr, port):
    sock.connect((bd_addr, port))

def close_client(sock):
    sock.close()

def printls(ls):
    for item in ls:
        print(item)

def main():
    sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
    connect_to_server(sock, bd_addr, 6)
    data = sock.recv(1024)
    print("Recieved:")
    print(data)
    close_client(sock)
main()
