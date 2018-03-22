import bluetooth
import time
import pickle
from rasp_data import robot_data as rd

#Device adress found using finddevice.py
ma_bd_addr = "98:5F:D3:35:FC:EA"

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

def make_robots(amount):
	robots = list()
	i = 1
	while i <= amount:
		if (i == amount):
			robots.append(rd(i,i+1,True))
		else:
			robots.append(rd(i,i+1))
		i += 1
	return robots       

def main():
	send_sock = bluetooth.BluetoothSocket( bluetooth.RFCOMM )
	server_sock_rec=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
	
	open_rec_server(server_sock_rec,5)
	my_robots = make_robots(20)
	connect_to_server(send_sock, ma_bd_addr, 6)
	#tic = time.clock()
	for robot in my_robots:
		time.sleep(0.05)
		send_sock.send(pickle.dumps(robot))
		print(robot)
        #toc = time.clock()
	listen_to_server(server_sock_rec)
	client_sock = rec_server_accept(server_sock_rec)
	close_sender(send_sock)
	data = client_sock.recv(1024)
        upkl = pickle.loads(data)
        print(upkl)
        close_reciever(client_sock, server_sock_rec)
main()

	
