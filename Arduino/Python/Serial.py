import serial

port = serial.Serial("/dev/ttyUSB0", baudrate=115200, timeout=3.0)

while True:
    val = input("Please enter a value to send to the Arduino: ")
    if val:
        port.write(b"\xF6\x00")
    print(port.read_all())
