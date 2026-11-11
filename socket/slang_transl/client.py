
import socket

PORT = 12345

sockfd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

server_addr = ("127.0.0.1", PORT)

message = input("Enter sentence:\n")

sockfd.sendto(message.encode() + b"\0", server_addr)

data, server_addr = sockfd.recvfrom(1024)

print("\nTranslated Sentence:\n", data.decode().rstrip("\0"))

sockfd.close()

