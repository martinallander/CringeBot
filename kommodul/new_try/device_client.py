import bluetooth
import msvcrt
import time
import pickle

#Rpi bluetooth adress
bd_addr = "B8:27:EB:A3:F1:C9"

def connect_to_server(sock, bd_addr, port):
    sock.connect((bd_addr, port))

def close_client(sock):
    sock.close()

def print_ls(ls):
    for item in ls:
        print(item)

def send_input(key):
    if kb_input != "":
        sock.send(key)
    else:
        sock.send("no input")
        
def main():
    sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
    connect_to_server(sock, bd_addr, 15)
    while True:
        data = sock.recv(1024)
        #print("Recieved:")
        print(data)
        if data == "end":
            break
        if msvcrt.kbhit():
            keycode = ord(msvcrt.getch())
            if keycode == 0x1B: #ESC
                sock.send("esc")
            elif keycode == 224: #Special keys (arrows, f keys, ins, del, etc.)
                keycode = ord(msvcrt.getch())
                #print("Special: \n")
                if keycode == 72: #Up arrow
                    sock.send("up")
              #  elif keycode == 0x25: #Left arrow
              #      return "left"
              #  elif keycode == 0x27: #Right arrow
              #      return "right"
                elif keycode == 80: #Down arrow
                    sock.send("down")
                else:
                    sock.send("N/A")
            else:
                sock.send("N/A")
        else:
            sock.send("sensor_data")
    close_client(sock) 

main()
