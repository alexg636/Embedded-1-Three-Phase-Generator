import socket
import struct

IP = "192.168.1.13"
RECV_PORT = 6367

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((IP, RECV_PORT))

while True:
    data, addr = sock.recvfrom(32)
    degrees = struct.unpack('d', data[0:8])[0]
    phase_1 = struct.unpack('d', data[8:16])[0]
    phase_2 = struct.unpack('d', data[16:24])[0]
    phase_3 = struct.unpack('d', data[24:32])[0]
    # dd=struct.unpack('d', degrees)
    # print(dd)
    print("Received Degrees: {}".format(degrees))
    print("Received Phase 1: {}".format(phase_1))
    print("Received Phase 2: {}".format(phase_2))
    print("Received Phase 3: {}".format(phase_3))
    print("")
