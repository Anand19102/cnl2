
import socket

PORT = 9000

sockfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_addr = ("127.0.0.1", PORT)

sockfd.connect(server_addr)

filename = input("Enter File Name: ")

sockfd.sendall(filename.encode())

sockfd.shutdown(socket.SHUT_WR)

print("\nResponse Received From Server\n")

while True:
    data = sockfd.recv(1024)

    if not data:
        break

    print(data.decode(), end="")

sockfd.close()

