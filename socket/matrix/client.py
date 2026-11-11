
import socket
import random

PORT = 12345
MAX = 10

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect(("127.0.0.1", PORT))

n = int(input("Enter order of matrix (N): "))

if n < 1 or n > MAX:
    print("Invalid matrix size")
    s.close()
    exit()

a = []

print("\nGenerated Matrix:")

for i in range(n):
    row = []

    for j in range(n):
        row.append(random.randint(0, 50))

    a.append(row)
    print(*row)

data = str(n) + " "

for row in a:
    data += " ".join(map(str, row)) + " "

s.sendall(data.encode())
s.shutdown(socket.SHUT_WR)

result = b""

while True:
    part = s.recv(1024)
    if not part:
        break
    result += part

print("\nMatrix Type:", result.decode())

s.close()

