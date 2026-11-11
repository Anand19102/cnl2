
import socket

PORT = 12345
MAX = 10

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.bind(("0.0.0.0", PORT))
s.listen(1)

print("Server waiting...")

c, addr = s.accept()

data = b""

while True:
    part = c.recv(1024)
    if not part:
        break
    data += part

values = list(map(int, data.decode().split()))

n = values[0]

if n < 1 or n > MAX or len(values) != n * n + 1:
    result = "Invalid matrix data"
else:
    a = []
    k = 1

    for i in range(n):
        row = values[k:k+n]
        a.append(row)
        k += n

    upper = True
    lower = True
    diagonal = True

    for i in range(n):
        for j in range(n):
            if i > j and a[i][j] != 0:
                upper = False

            if i < j and a[i][j] != 0:
                lower = False

            if i != j and a[i][j] != 0:
                diagonal = False

    if diagonal:
        result = "Diagonal Matrix"
    elif upper:
        result = "Upper Triangular Matrix"
    elif lower:
        result = "Lower Triangular Matrix"
    else:
        result = "Neither"

c.sendall(result.encode())

c.close()
s.close()

