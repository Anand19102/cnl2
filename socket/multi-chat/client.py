import socket
import threading

PORT = 9000
NAME_SIZE = 30

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("127.0.0.1", PORT))

name = input("Enter your name: ")
name_data = name.encode()[:NAME_SIZE - 1]
s.sendall(name_data.ljust(NAME_SIZE, b"\0"))

print("Connected to chat server")
print("Start chatting:")

def receive_messages():
    while True:
        try:
            data = s.recv(1024)

            if not data:
                print("\nDisconnected from server")
                break

            print(data.decode(), end="", flush=True)

        except OSError:
            break

receiver = threading.Thread(target=receive_messages)
receiver.daemon = True
receiver.start()

try:
    while True:
        message = input()
        s.sendall((message + "\n").encode())

except (EOFError, KeyboardInterrupt):
    pass

s.close()
