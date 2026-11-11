
import socket

PORT = 9000

sockfd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

server_addr = ("127.0.0.1", PORT)

message = "TIME"

sockfd.sendto(message.encode() + b"\0", server_addr)

data, server_addr = sockfd.recvfrom(1024)

print("\nCurrent Server Time")
print(data.decode().rstrip("\0"))

sockfd.close()

