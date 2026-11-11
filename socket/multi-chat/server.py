import socket
import select

PORT = 9000
MAX_CLIENTS = 10
NAME_SIZE = 30

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

server.bind(("0.0.0.0", PORT))
server.listen(MAX_CLIENTS)

print("Chat Server Started on Port", PORT)

clients = {}
buffers = {}

while True:
    sockets = [server] + list(clients.keys())

    readable, _, _ = select.select(sockets, [], [])

    for sock in readable:

        if sock == server:
            client, addr = server.accept()

            name = b""
            while len(name) < NAME_SIZE:
                part = client.recv(NAME_SIZE - len(name))
                if not part:
                    break
                name += part

            if len(name) != NAME_SIZE or len(clients) >= MAX_CLIENTS:
                client.close()
                continue

            name = name.decode().rstrip("\x00")
            clients[client] = name
            buffers[client] = b""

            print(name, "connected")

        else:
            try:
                data = sock.recv(1024)
            except OSError:
                data = b""

            if not data:
                print(clients[sock], "disconnected")
                del clients[sock]
                del buffers[sock]
                sock.close()
                continue

            buffers[sock] += data

            while b"\n" in buffers[sock]:
                message, buffers[sock] = buffers[sock].split(b"\n", 1)

                message = clients[sock] + ": " + message.decode() + "\n"
                print(message, end="")

                for client in list(clients):
                    if client != sock:
                        try:
                            client.sendall(message.encode())
                        except OSError:
                            print(clients[client], "disconnected")
                            del buffers[client]
                            del clients[client]
                            client.close()
