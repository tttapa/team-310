import socket, time

commands = [0x61,0x50,0x60,0x51]

UDP_IP = "hal-9310.local"
UDP_PORT = 9310
MESSAGE = 0x60

while 1:
    for i in commands:
        print("message:", i)

        sock = socket.socket(socket.AF_INET, # Internet
                            socket.SOCK_DGRAM) # UDP
        sock.sendto(bytes([i]), (UDP_IP, UDP_PORT))

        time.sleep(0.5)