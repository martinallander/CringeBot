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
    server_sock.close	


def printls(ls):
    for item in ls:
        print(item)
        
def send_end(sock):
    sock.send("end")

def main():
    server_sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
    open_server(server_sock,15)
    listen_to_client(server_sock)
    client_sock = connection_accept(server_sock)
    i = 1
    data = "sensor_data"
    while True:
    	time.sleep(0.05)
    	if data == "sensor_data":
    		client_sock.send("my_sensor_data")
    	elif data == "up":
    		client_sock.send("action move up")
    	elif data == "down":
    		client_sock.send("action move down")
    	else:
    		client_sock.send("N/A")
    	
    	data = client_sock.recv(1024)
    	print(data)
    	if data == "esc":
    	     break
    send_end(client_sock)
    close_server(client_sock, server_sock)

main()


