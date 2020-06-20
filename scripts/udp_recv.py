import socket
import struct

from influxdb import InfluxDBClient

IP = "192.168.1.20"
RECV_PORT = 6367

# Influxdb client setup
client = InfluxDBClient(host='localhost', port=8086)
db = 'threephase'
client.create_database(db)
client.switch_database(db)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((IP, RECV_PORT))

def generate_payload(degrees, val1, val2, val3):
    payload = [
        {
            "measurement": "threephaseEvent",
            "tags": {"user": "Alex"},
            "fields": {
                "Degrees": degrees,
                "Phase_1": val1,
                "Phase_2": val2,
                "Phase_3": val3
            }
        }
    ]
    return payload

while True:
    data, addr = sock.recvfrom(32)
    degrees = struct.unpack('d', data[0:8])[0]
    phase_1 = struct.unpack('d', data[8:16])[0]
    phase_2 = struct.unpack('d', data[16:24])[0]
    phase_3 = struct.unpack('d', data[24:32])[0]

    influx_payload = generate_payload(degrees, phase_1, phase_2, phase_3)
    response = client.write_points(influx_payload)
    print(response)

    print("Received Degrees: {}".format(degrees))
    print("Received Phase 1: {}".format(phase_1))
    print("Received Phase 2: {}".format(phase_2))
    print("Received Phase 3: {}".format(phase_3))
    print("")
    
