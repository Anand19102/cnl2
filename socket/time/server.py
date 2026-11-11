
import socket
import time

PORT = 9000

sockfd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sockfd.bind(("0.0.0.0", PORT))

print("UDP Time Server Started")
print("Listening on Port", PORT)

while True:
    data, client_addr = sockfd.recvfrom(1024)

    current_time = time.ctime()

    sockfd.sendto(current_time.encode() + b"\0", client_addr)

    print("Time request served.")

