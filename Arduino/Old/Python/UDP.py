import socket, time

commands = [0x00, 0x01, 0x02, 0x03, 0x09, 0x0A, 0x0B, 0x04, 0x05, 0x06, 0x07, 0x08]

UDP_IP = "hal-9310.local"
UDP_PORT = 9310

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
while 1:
#    for i in range(10):
    for i in commands: 
        print("message:", i)
        sock.sendto(bytes([i]), (UDP_IP, UDP_PORT))

        time.sleep(1)

