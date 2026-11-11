
import socket
import os
import signal

PORT = 9000

signal.signal(signal.SIGCHLD, signal.SIG_IGN)

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

server.bind(("0.0.0.0", PORT))

server.listen(5)

print("TCP Concurrent File Server Started")
print("Listening on Port", PORT)

while True:
    conn, addr = server.accept()

    pid = os.fork()

    if pid == 0:
        server.close()

        filename = b""

        while True:
            data = conn.recv(1024)

            if not data:
                break

            filename += data

        filename = filename.decode()

        try:
            fp = open(filename, "rb")

            response = "Handled by Server PID : " + str(os.getpid()) + "\n\n"

            conn.sendall(response.encode())

            while True:
                data = fp.read(1024)

                if not data:
                    break

                conn.sendall(data)

            fp.close()

        except FileNotFoundError:
            response = "Handled by Server PID : " + str(os.getpid())
            response += "\n\nRequested File Not Found.\n"

            conn.sendall(response.encode())

        conn.close()
        os._exit(0)

    else:
        conn.close()

