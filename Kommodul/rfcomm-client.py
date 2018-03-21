import bluetooth

bd_addr = "98:5F:D3:35:FC:EA"

port = 1

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bd_addr, port))

sock.send("hello!!")

sock.close()